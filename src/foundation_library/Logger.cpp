/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Logger.cpp
****/

#include "pch.h"
#include "Logger.h"
#include "LogCategory.h"
#include <foundation/library/event_args_base.h>
#include "CreateFoundationHelper.h"

//#include "../foundation/EnumTypeClassInfoCache.h"

#include <stdarg.h>

#include <foundation/type_info_statics_util.h>
#include <foundation/library/logger_util.h>
#include <foundation/library/logger_library.h>
#include <foundation/library/string_util.h>
#include <foundation/dictionary_ptr.h>

using namespace foundation;

// exports from pmod to allow proper action when EnumType Info is not available
typedef  void (STDAPICALLTYPE *_EnumTypeInfoDisposedCallback)();
EXTERN_C FOUNDATION_API  void STDAPICALLTYPE SetEnumTypeInfoDisposedCallback(_EnumTypeInfoDisposedCallback pCallback);

static void STDAPICALLTYPE OnEnumTypeInfoDisposed()
{
    CLogger::_ForceUnavailable();
}

extern HRESULT register_foundation_logger_type_info(void);

static bool _logger_IsAvailable = true;

class CLogCreatedEventArgs : public foundation::library::_EventArgsBase<ILogCreatedEventArgs>
{
public:
    HRESULT _Initialize(
        const std::pair<IInspectable *, ILogRecord *> &parameters)
    {
        this->_spContext = parameters.first;
        this->_spLogRecord = parameters.second;
        return S_OK;
    }
protected:
    STDMETHOD (GetLogRecord)(ILogRecord **ppLogRecord)
    {
        return _spLogRecord.CopyTo(ppLogRecord);
    }
    STDMETHOD(GetContext)(IInspectable **ppContext)
    {
        return _spContext.CopyTo(ppContext);
    }
private:
    foundation::InspectablePtr _spContext;
    foundation::ComPtr<ILogRecord> _spLogRecord;
};

_Dispatchable_ptr_vector_type& CLogger::GetCallbacks()
{
    static _Dispatchable_ptr_vector_type _callbacks;
    return _callbacks;
}

void CLogger::_ForceUnavailable()
{
    _logger_IsAvailable = false;
}

bool CLogger::IsAvailable()
{
    return _logger_IsAvailable;
}

HRESULT CLogger::register_factory_type_info(foundation::library::IDelegateDispatchable *pCallback)
{
    GetCallbacks().push_back(pCallback);
    return S_OK;
}

HRESULT CLogger::CreateLoggerInstance(CLogger **ppCoreLogger)
{

    library::LoggerCreateParameters coreLoggerParams(Root_CategoryEnum::TypeId);
    coreLoggerParams.m_options = (LoggerOptions)library::FoundationClassOptions_IsFreeThreadApartment;

    foundation::IInspectable *pNewInstance = nullptr;
    IFR_ASSERT(CreateInstance(&coreLoggerParams,nullptr,&pNewInstance));
    *ppCoreLogger = static_cast< CLogger *>(static_cast< CLogger::_ComInspectableBaseType *>(pNewInstance));
    return S_OK;
}

HRESULT CLogger::CreateInstance(
    _In_ const library::LoggerCreateParameters *pCoreLoggerParam,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    static pal::library::PalOnceFlag    _initOnce;
    pal::library::pal_CallOnce_t(
        _initOnce,
        []() {
        // callback when type inof is disposed
        ::SetEnumTypeInfoDisposedCallback(OnEnumTypeInfoDisposed);
        // register foundation logger types
        register_foundation_logger_type_info();

        for (_Dispatchable_ptr_vector_type::const_iterator iter =
            GetCallbacks().begin();
            iter != GetCallbacks().end();
        ++iter)
        {
            (*iter)->Invoke();
        }
        return S_OK;
    });


    return CreateFoundationInstance<CLogger>(pCoreLoggerParam, pOuter, ppNewInstance);
}

HRESULT CLogger::_Initialize(const library::LoggerCreateParameters *pCoreLoggerParam)
{
    foundation_assert(pCoreLoggerParam);

    UINT32 rootCategoryId = pCoreLoggerParam->m_root_category_id;
    CLogCategory *pCoreRootCategory = nullptr;
    IFR_ASSERT(CLogCategory::GetOrCreateCategory(
        nullptr,
        (UINT32)1,
        &rootCategoryId,
        &pCoreRootCategory));
    this->_rootCategory = pCoreRootCategory;

    IFR_ASSERT(_LoggerBase::_Initialize(pCoreLoggerParam));
    
#if defined(DEBUG)
    TraceLogListener *pTraceLogListener = nullptr;
    TextLogListenerBase::CreateInstance(&pTraceLogListener);
    foundation_assert(pTraceLogListener != nullptr);
    pTraceLogListener->Attach(this);
    ctl::inspectable::ReleaseInterface(pTraceLogListener);
#endif
    
    return S_OK;
}

CLogger *CLogger::GetLogger()
{
    return library::_GetFactoryInstance<CLogger>([]() {
        CLogger *pCoreLogger = nullptr;
        CreateLoggerInstance(&pCoreLogger);
        foundation_assert(pCoreLogger != nullptr);
        return pCoreLogger;
    });
}

const CLogCategory *CLogger::GetCategory(_In_ UINT32 categoryTypeId,...)
{
    va_list args;
    va_start( args, categoryTypeId );
    std::vector<UINT32> categoryTypeIdVector;
    while(categoryTypeId != (UINT32)-1 )
    {
        categoryTypeIdVector.push_back(categoryTypeId);
        // next category id
        categoryTypeId = va_arg(args, UINT32);
    }
    CLogCategory *pLogCategory = nullptr;
    HRESULT hr = GetLogger()->GetOrCreateCategory(
        (UINT32)categoryTypeIdVector.size(),
        &(categoryTypeIdVector.front()),
        &pLogCategory);
    foundation_assert(SUCCEEDED(hr));
    return pLogCategory;
}

HRESULT CLogger::GetOrCreateCategory(UINT32 size,UINT32 *pCategories,CLogCategory **ppLogFactory)
{
    return CLogCategory::GetOrCreateCategory(
        CLogCategory::ToLogCategoryClass(this->_rootCategory),
        size,
        pCategories,
        ppLogFactory
        );
}

STDMETHODIMP CLogger::GetLogCreatedEventSource(_Outptr_ foundation::ILogCreatedEventSource** ppEventSource)
{
    IFR_ASSERT(EnsureEventSourceModel());

    *ppEventSource = this->GetEventSourceImpl();
    (*ppEventSource)->AddRef();
    return S_OK;
}


STDMETHODIMP CLogger::ConfigureCategories(IDictionary *pConfigParameters)
{
    IFCPTR_ASSERT(pConfigParameters);

    DictionaryPtr config_parameters_ptr(pConfigParameters);
    for (DictionaryPtr::keys_iterator iter = config_parameters_ptr.keys_begin();
        iter != config_parameters_ptr.keys_end();
        ++iter)
    {
        LPCSTR_t key = _pal_GetStringRawBuffer(*iter, nullptr);
        foundation::ComPtr<IDictionary> category_config_parameters;
        _IFR_(config_parameters_ptr.GetValue(key,category_config_parameters.GetAddressOf()));

        foundation::ComPtr<ILogCategory> spLogCategory;

        if (_pal_strcmp(key, U("Root")) == 0)
        {
            IFR_ASSERT(this->GetRootCategory(spLogCategory.GetAddressOf()));
        }
        else
        {
            // attempt to locate or create the Log Category
            _IFR_(this->GetCategoryByName(*iter, spLogCategory.GetAddressOf()));
        }
        _IFR_(spLogCategory->Configure(category_config_parameters));
    }

    return S_OK;
}

STDMETHODIMP CLogger::GetRootCategory(ILogCategory **ppResult)
{
    IFCPTR_ASSERT(ppResult);
    return _rootCategory.CopyTo(ppResult);
}

STDMETHODIMP CLogger::GetCategory(UINT32 size,UINT32 *pCategories,ILogCategory **ppLogCategory)
{
    IFCPTR_ASSERT(ppLogCategory);

    CLogCategory *pLogFactory = nullptr;
    HRESULT hr = this->GetOrCreateCategory(
        size,
        pCategories,
        &pLogFactory
        );
    if(SUCCEEDED(hr))
    {
        *ppLogCategory = pLogFactory;
        (*ppLogCategory)->AddRef();
    }
    return hr;
}

STDMETHODIMP CLogger::GetCategoryByName(HSTRING categoryPath,ILogCategory **ppResult)
{
    IFCPTR_ASSERT(ppResult);

    string_t categoryPathStr = _pal_GetStringRawBuffer(categoryPath,nullptr);
    if(!categoryPathStr.size())
    {
        return E_UNEXPECTED;
    }

    // next block will split the steps separated as '.'
    std::vector<string_t> steps = library::string_util::split_t(categoryPathStr, U('.'));

    CLogCategory *pLogCategory = CLogCategory::ToLogCategoryClass(_rootCategory);
    for (size_t i = 0; i < steps.size(); ++i)
    {
        CLogCategory *pSubLogCategory = pLogCategory->LookupCategoryByName(steps[i].c_str());
        if (pSubLogCategory == nullptr)
        {
            // next code will attempt to create the category
            ComPtr<IEnumTypeInfo> spEnumTypeInfo;
            GetEnumTypeInfoByNamespaceName(nullptr, HStringRef(steps[i].c_str()), spEnumTypeInfo.GetAddressOf());

            if (spEnumTypeInfo == nullptr &&
                // attempt the '_Category' postfix
                (GetEnumTypeInfoByNamespaceName
                    (nullptr, 
                        HStringRef((steps[i] + CLogCategory::_CategoryPostfix).c_str()), spEnumTypeInfo.GetAddressOf()) != S_OK))
            {
                // we could not find a Category in our Enum
                return foundation::E_INVALID_ENUM_TYPE_INFO;
            }
            UINT32 typeId;
            spEnumTypeInfo->GetTypeId(&typeId);
            IFR_ASSERT(CLogCategory::GetOrCreateCategory(pLogCategory, 1, &typeId, &pSubLogCategory));
        }
        pLogCategory = pSubLogCategory;
    }
    *ppResult = static_cast<ILogCategory *>(pLogCategory);
    (*ppResult)->AddRef();
    return S_OK;
}

STDMETHODIMP CLogger::Log(IInspectable *pContext, ILogRecord *pLogRecord)
{
    IFCPTR_ASSERT(pLogRecord);

    foundation::ComPtr<ILogCreatedEventArgs> spLogCreatedEventArgs;
    IFR_ASSERT(foundation::ctl::inspectable::CreateInstanceWithInitialize<CLogCreatedEventArgs>(
        spLogCreatedEventArgs.GetAddressOf(),
        std::make_pair(pContext, pLogRecord)));

    // Custom class being created
    // on the stack to enable fire logging as soon as
    // our fire dispatching mechanism is finish
    class _LoggerLock
    {
    public:
        _LoggerLock(CLogger& logger):
            _logger(logger)
        {
        }
        HRESULT Lock()
        {
            _logger.Lock();
            return S_OK;
        }
        HRESULT Unlock()
        {
            _logger._is_fire_logging = false;
            _logger.Unlock();
            return S_OK;
        }
    private:
        CLogger& _logger;
    };
    _LoggerLock lockImpl(*this);
    foundation_lib::CFoundationLock<_LoggerLock> csl(lockImpl);

    if (_is_fire_logging)
    {
        // Note: if we arrive here the same thread aquire the lock
        // and is attempting to log during a fire event notification
        return E_UNEXPECTED;
    }
    _is_fire_logging = true;
    HRESULT hr = FireWithCallback(
        csl,
        &_LoggerBase::FireEvent,
        static_cast<ILogCreatedEventArgs *>(spLogCreatedEventArgs));
    return hr;
}

STDMETHODIMP CLogger::OnFireEventError(
    HRESULT hr,
    IUnknown *pEventHandler,
    IUnknown *pEventArgs)
{
    // Logger class will always continue the notification chain and never fail
    // during fire event notification to prevent any recursion in using the logger
    // Note: the base class will attempt to log this event notification and so we 
    // want to prevnt it happen upfront

    UNREFERENCED_PARAMETER(hr);
    UNREFERENCED_PARAMETER(pEventHandler);
    UNREFERENCED_PARAMETER(pEventArgs);
    return S_OK;
}
