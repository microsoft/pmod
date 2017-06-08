/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Command.cpp
****/

#include "pch.h"

#include "Command.h"
#include "CreateFoundationHelper.h"
#include "PropertyTypeInfo.h"
// from model API
#include "../model/ModelTypeInfoStatics.h"

#include <foundation/library/async_operation_class_ptr.h>
#include <foundation/library/critical_section.h>
#include <pmod/library/library_util.h>
#include <pmod/library/can_execute_changed_event_args_impl.h>
#include <pmod/errors.h>

#include <foundation/library/perf_counter_util.h>
#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#endif

#include <foundation/library/com_ptr_singleton.h>

// {A6CF6FF9-2AD8-4417-ABD6-BFB7179784A1}
const GUID pmod::library::IID_ICommandClassFactory =
{ 0xa6cf6ff9, 0x2ad8, 0x4417,{ 0xab, 0xd6, 0xbf, 0xb7, 0x17, 0x97, 0x84, 0xa1 } };

using namespace pmod;
using namespace pmod::library;
using namespace foundation;
using namespace foundation::library;

const CLogCategory*CCommandLogFactory::GetLogCategory()
{
    static _ComPtrSingleton<const CLogCategory, foundation::_NoRefCountPtrImpl> _commandModelLogCategoryInstance;

    return _commandModelLogCategoryInstance._GetInstance([]() {
        return CLogger::GetCategory(
            Foundation_CategoryEnum::TypeId,
            Object_CategoryEnum::TypeId,
            ObjectNode_CategoryEnum::TypeId,
            Command_CategoryEnum::TypeId,
            -1);
    });
}

void CCommand::OnFinalRelease()
{
    _CommandBase_Type::OnFinalRelease();
}


HRESULT CCommand::GetState(UINT32* pVal)
{
    * pVal = m_CommandState;
    return S_OK;
}

HRESULT CCommand::VerifyCommandParameterTypeInfo(_Inout_ foundation::IInspectable **ppParameter)
{
    foundation::ComPtr<foundation::IPropertyTypeInfo> spParameterTypeInfo;
    IFR_ASSERT(this->m_pTypeInfo->GetParameterTypeInfo(spParameterTypeInfo.GetAddressOf()));
    _IFR_(CModelTypeInfoStatics::_VerifyPropertyTypeInfo(
        spParameterTypeInfo,
        ppParameter));
    return S_OK;
}

HRESULT CCommand::VerifyCommandResultTypeInfo(_Inout_ foundation::IInspectable **ppResult)
{
    foundation_assert(ppResult);

    foundation::ComPtr<foundation::IPropertyTypeInfo> spResultTypeInfo;
    IFR_ASSERT(this->m_pTypeInfo->GetResultTypeInfo(spResultTypeInfo.GetAddressOf()));

    // Note: we will allow returning an Async Operation when the expected return type is Empty
    foundation::PropertyType propertType; 
    spResultTypeInfo->GetPropertyType(&propertType);

    if(propertType == foundation::PropertyType_Empty &&
        *ppResult != nullptr &&
        foundation::IsTypeOf<foundation::IAsyncOperation>(*ppResult))
    {
        return S_OK;
    }

    IFR(CModelTypeInfoStatics::_VerifyPropertyTypeInfo(
        spResultTypeInfo,
        ppResult));
    return S_OK;
}

HRESULT CCommand::CanExecute(_In_ foundation::IInspectable *parameter,_Outptr_ bool *pValue)
{
    IFCPTR(pValue);

    PERF_CONTEXT(Core_PerfId_CommandModel_CanExecute_Id)

    if(m_CommandState == CommandStateUnknown)
    {
        HRESULT hr = S_FALSE;

        foundation::InspectablePtr _parameter(parameter);
        if (FAILED(VerifyCommandParameterTypeInfo(_parameter.GetAddressOfPtr())))
        {
            *pValue = false;
            return hr;
        }
        // if delegate is set we call if the call returns S_OK the main CanExecute is not called. 
        // if the call returns S_FALSE is used only to spy the calls.
        // logging support for 'Execute Call' 
        
        if (m_CanExecuteDelegate != nullptr)
        {
            hr = m_CanExecuteDelegate->Invoke(_parameter, pValue);
            IFHR_OK(hr);
        }
        if(hr == S_FALSE && m_pDelegate != nullptr)
        {
            hr = m_pDelegate->OnCanExecute(_parameter, pValue);
            IFHR_OK(hr);
        }
        else
        {
            // default value in case no Delegate and no override
            *pValue = true;
        }

        //check for the right log level before logging.
        if (IsLevelEnabled(foundation::LoggingLevel::Info,
                Command_Category_CanExecute_Id))
        {
            LogCanExecute((UINT32)this->m_UniqueId, _parameter, *pValue);
        }

    }
    else
    {
        *pValue = m_CommandState == CommandStateEnabled;
    }
    
    return S_OK;
}

HRESULT CCommand::Execute(_In_ foundation::IInspectable *parameter,foundation::IInspectable **ppResult)
{
    PERF_CONTEXT(Core_PerfId_CommandModel_Execute_Id)
    
    // quickly return when no Delegate or Override was specified
    if(m_pDelegate==nullptr && m_ExecuteDelegate==nullptr)
    {
        return E_NOTIMPL;
    }

    foundation::InspectablePtr spParameter(parameter);
    IFR(VerifyCommandParameterTypeInfo(spParameter.GetAddressOfPtr()));

    // First, make sure the command can be executed (calling layer should have verified this
    // already, but it's better to check here than to have the OnExecute do the check).
    bool bCanExecute = false;
    CanExecute(spParameter, &bCanExecute);

    if (bCanExecute)
    {
        bool isAsync = false;
        this->m_pTypeInfo->GetIsAsync(&isAsync);

        foundation::ComPtr<IPropertyTypeInfo> spResultTypeInfo;
        IFR_ASSERT(this->m_pTypeInfo->GetResultTypeInfo(spResultTypeInfo.GetAddressOf()));

        foundation::library::IDispatcher *pWorkingDispatcher = this->GetWorkingDispatcherInternal();
        bool bReturnAsyncOperation = isAsync ||
            (
                m_AllowReturnAsyncOnVoidResult &&
                (pWorkingDispatcher && IsPropertyTypeInfoVoid(spResultTypeInfo))
            );
        
        bool isLogInfoEnabled = IsLevelEnabled(
            LoggingLevel::Info,
                Command_Category_Execute_Id);

        if(bReturnAsyncOperation)
        {
            AsyncOperationClassPtr spAsyncOperationClass;

            // create the Async Operation Class
            IFR_ASSERT(foundation::library::CreateAsyncOperationClass(
                spResultTypeInfo,
                foundation::library::ToFreeThreadApartmentOptions(foundation::AsyncOperationOptions::None),
                spAsyncOperationClass.GetAddressOf()));

            // have the async operation to 'inherit' the current event dispatcher
            IFR_ASSERT(spAsyncOperationClass.GetClassPtr()->SetEventDispatcher(this->_GetEventDispatcher()));

            // set the ppResult to IAsyncOperationModel
            *ppResult = spAsyncOperationClass.Detach();
       }

        if (pWorkingDispatcher != nullptr && !foundation::HasThreadAccess(pWorkingDispatcher))
        {
            IInspectable *pAsyncOperationClass = nullptr;
            if (bReturnAsyncOperation)
            {
                pAsyncOperationClass = (*ppResult);
                // keep alive Async Operation 
                pAsyncOperationClass->AddRef();
            }

            foundation::ComPtr<foundation::IAsyncOperation> spAsyncAction;
            // AddRef to keep us alive during the async invocation
            foundation::IInspectable *pThis = foundation::ctl::inspectable::CastToInspectable(this);
            pThis->AddRef();

            IFR_ASSERT(foundation::RunAsync(pWorkingDispatcher,
                [=]() {
                // attach to the AddRef'ed so we balance the Ref Count
                foundation::InspectablePtr context_ptr;
                context_ptr.Attach(pThis);

                foundation::InspectablePtr spResult;
                if (bReturnAsyncOperation)
                {
                    spResult.Attach(pAsyncOperationClass);
                }
               
                if (isLogInfoEnabled)
                {
                    LogExecute((UINT32)this->m_UniqueId, spParameter);
                }
                HRESULT hr = on_execute_internal(
                    true,
                    true,
                    spParameter,
                    spResult.GetAddressOfPtr());
                if (!bReturnAsyncOperation)
                {
                    spResult.CopyTo(ppResult);
                }
                return hr;
            },
            spAsyncAction.GetAddressOf()));
            // if not Async we will have to Wait for the Worker Dispatcher and copy results
            if (!bReturnAsyncOperation && spAsyncAction != nullptr)
            {
                // Wait sync until command is completed and rely the HRESULT.                
                IFR_ASSERT(spAsyncAction->Wait(foundation::_InfiniteTimeout, nullptr));
                HRESULT hrInvoke = S_OK;
                IFR_ASSERT(spAsyncAction->GetError(&hrInvoke));
                return hrInvoke;
            }
            return S_OK;
        }
        
        if (isLogInfoEnabled)
        {
            LogExecute((UINT32)m_UniqueId, parameter);
        }

        return on_execute_internal(false,isAsync,spParameter,ppResult);
    }

    LOG_TRACE(
        LoggingLevel::Error,
        foundation::Error_Category::Failure, 
        U("Command was not enabled"), 
        pmod::Errors::E_COMMAND_NOT_ENABLED);
    // Command was not enabled
    return pmod::Errors::E_COMMAND_NOT_ENABLED;
}

HRESULT CCommand::on_execute_internal(
    _In_ bool isDispatched,
    _In_ bool isAsync,
    _In_ foundation::IInspectable *parameter,
    foundation::IInspectable **ppResult)
{
    PERF_CONTEXT(Core_PerfId_CommandModel_ExecuteCallback_Id)
    
    foundation::IInspectable **_ppResult = ppResult;
    if(isAsync && *ppResult != nullptr)
    {
        IAsyncOperation *pAsyncOperation = reinterpret_cast<IAsyncOperation *>(*ppResult);
        foundation::QueryWeakReference(
            pAsyncOperation,
            (foundation::library::IAsyncOperationClass **)_ppResult);
        foundation_assert(*_ppResult);

        // return if operation was canceled or completed
        ResultStatus status;
        IFR_ASSERT(pAsyncOperation->GetStatus(&status));
        if (status == ResultStatus::Canceled)
        {
            // no point in invoking any delegate at this point so return
            return S_FALSE;
        }
    }

    HRESULT hr = S_FALSE;
    // if delegate is set we call if the call returns S_OK the main Execute is not called. 
    // if the call returns S_FALSE is used only to spy the calls.
    if (m_ExecuteDelegate != nullptr)
    {
        hr = m_ExecuteDelegate->Invoke(parameter, _ppResult);
        IFHR(hr);
    }
    // there was no delegate try to consume the default delegates
    if (hr == S_FALSE && m_pDelegate != nullptr)
    {
        // we want to return the original execution HRESULT from 'OnExecute' delegate
        hr = m_pDelegate->OnExecute(parameter,_ppResult);
        IFHR(hr);
    }

    if(!isAsync)
    {
        IFR(VerifyCommandResultTypeInfo(_ppResult));
    }
    return hr;
}

HRESULT CCommand::GetCanExecuteChangedEventSource(_Outptr_ ICanExecuteChangedEventSource** ppEventSource)
{
    IFR_ASSERT(EnsureEventSourceModel());

    *ppEventSource = GetEventSourceImpl();
    (*ppEventSource)->AddRef();
    return S_OK;
}

STDMETHODIMP CCommand::GetExecuteDelegate(_COM_Outptr_ pmod::library::IExecuteDelegate **ppExecuteDelegate)
{
    return m_ExecuteDelegate.CopyTo(ppExecuteDelegate);
}

STDMETHODIMP CCommand::SetExecuteDelegate(_In_ pmod::library::IExecuteDelegate *pExecuteDelegate)
{
    m_ExecuteDelegate = pExecuteDelegate;
    return S_OK;
}

STDMETHODIMP CCommand::GetCanExecuteDelegate(_COM_Outptr_ pmod::library::ICanExecuteDelegate **ppCanExecuteDelegate)
{
    return m_CanExecuteDelegate.CopyTo(ppCanExecuteDelegate);
}

STDMETHODIMP CCommand::SetCanExecuteDelegate(_In_ pmod::library::ICanExecuteDelegate *pCanExecuteDelegate)
{
    m_CanExecuteDelegate = pCanExecuteDelegate;
    return S_OK;
}

STDMETHODIMP CCommand::SetCommandState(_In_ UINT32 commandState,_In_ bool fireCanExecute)
{
    UNREFERENCED_PARAMETER(fireCanExecute);

    // Only save state and fire an event if something is changing
    if (commandState==CommandStateUnknown || m_CommandState != commandState)
    {
        int oldState = m_CommandState;

        // The state has changed -- even if the event cannot be created/fired, the underlying
        // action must be set to the new state
        m_CommandState = commandState;
        
        return InternalFireCanExecuteChanged(oldState,commandState);
    }
    return S_OK;
}

STDMETHODIMP CCommand::FireCanExecuteChanged()
{
    return InternalFireCanExecuteChanged(m_CommandState,m_CommandState);
}

HRESULT CCommand::InternalFireCanExecuteChanged(UINT32 oldState,UINT32 newState)
{
    if (this->IsObserved())
    {
        
        foundation::ComPtr<ICanExecuteChangedEventArgs> spCanExecuteChangedEventArgs;

        IFR_ASSERT(CanExecuteChangedEventArgsImpl::CreateInstance(
            oldState,
            newState,
            spCanExecuteChangedEventArgs.GetAddressOf()));
        InternalFireCanExecuteChanged(spCanExecuteChangedEventArgs);
    }
    return S_OK;
}

HRESULT CCommand::InternalFireCanExecuteChanged(_In_ ICanExecuteChangedEventArgs* pEventArgs)
{
    PERF_CONTEXT(Core_PerfId_CommandModel_FireCanExecuteChanged_Id)
    return FireWithCallback(&_CommandBase_Type::FireEvent, pEventArgs);
}

HRESULT CCommand::_Initialize(const pmod::library::CommandCreateParameters *pCommandModelParam)
{
    IFR_ASSERT(_CommandBase_Type::_Initialize(pCommandModelParam));
    m_CommandId = pCommandModelParam->m_command_id;
    m_AllowReturnAsyncOnVoidResult = !IsOptionEnabled(pCommandModelParam->m_options, CommandOptions::NoReturnAsyncOnVoidResult);
    return S_OK;
}

HRESULT CCommand::CreateInstance(
    _In_ const pmod::library::CommandCreateParameters *pCommandModelParam,
    _In_ foundation::IInspectable *pOuter,
    _Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CCommand>(pCommandModelParam, pOuter, ppNewInstance);
}

void CCommand::LogExecute(_In_ UINT32 uniqueId, _In_ foundation::IInspectable *parameter)
{
    foundation::logger_util::Log(
        foundation::logger_util::GetFoundationLogger(),
        this->AsInspectable(),
        foundation::LoggingLevel::Info,
        pmod::Command_Category_Execute_Id,
        CCommandLogFactory::GetLogCategory()->AsInterface(),
        pmod::CommandExecute_LogRecordEnum::TypeId,
        // Unique Id
        pmod::Model_LogRecord::UniqueId,
        foundation::pv_util::CreateValue((UINT32)uniqueId).Get(),
        // Parameters
        pmod::CommandExecute_LogRecord::Parameters,
        parameter,
        -1);
}

void CCommand::LogCanExecute(_In_ UINT32 uniqueId, _In_ foundation::IInspectable *parameter, _In_ bool result)
{
    foundation::logger_util::Log(
        foundation::logger_util::GetFoundationLogger(),
        this->AsInspectable(),
        foundation::LoggingLevel::Info,
        pmod::Command_Category_CanExecute_Id,
        CCommandLogFactory::GetLogCategory()->AsInterface(),
        pmod::CommandCanExecute_LogRecordEnum::TypeId,
        // Unique Id
        pmod::Model_LogRecord::UniqueId,
        foundation::pv_util::CreateValue((UINT32)uniqueId).Get(),
        // CanExecute Result
        pmod::CommandCanExecute_LogRecord::Result,
        foundation::pv_util::CreateValue(result).Get(),
        -1);
}



#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

void CCommand::DumpObject() 
{
    DumpObjectBase(U("CommandModel"));
}

#endif

class CCommandClassFactory :
    public foundation::ctl::Implements<ICommandClassFactory, &IID_ICommandClassFactory>
{
public:
    static ICommandClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CCommandClassFactory, ICommandClassFactory>();
        }
    }

    STDMETHOD(CreateCommandInstance)(
        _In_ const CommandCreateParameters *pCreateParams,
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CCommand::CreateInstance(pCreateParams, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.CommandClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CCommandClassFactory::GetInstance)
);
