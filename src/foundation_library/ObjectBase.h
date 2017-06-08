/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectBase.h
****/
#pragma once
#include "FoundationBase.h"
#include "Logger.h"
#include "LogCategory.h"

#include <foundation/type_info_statics_util.h>
#include <foundation/library/logger_util.h>
#include <pal/pal_thread.h>

EXTERN_C FOUNDATION_API UINT32 STDAPICALLTYPE GetNextObjectUniqueId();

extern void OnObjectBaseInitialize(foundation::IObject *pObject);
extern void OnObjectBaseReleased(foundation::IObject *pObject);

template <
    foundation::LoggingLevel _InitializeLevel = foundation::LoggingLevel::Debug,
    foundation::LoggingLevel _FinalReleaseLevel = foundation::LoggingLevel::Debug,
    foundation::LoggingLevel _IsObservedLevel = foundation::LoggingLevel::Trace,
    foundation::LoggingLevel _FireEventLevel = foundation::LoggingLevel::Debug
>
struct _ObjectBaseLogFactory
{
    static inline foundation::LoggingLevel _GetInitializeLevel() { return _InitializeLevel; }
    static inline foundation::LoggingLevel _GetFinalReleaseLevel() { return _FinalReleaseLevel; }
    static inline foundation::LoggingLevel _GetIsObservedLevel() { return _IsObservedLevel; }
    static inline foundation::LoggingLevel _GetFireEventLevel() { return _FireEventLevel; }
};

//------------------------------------------------------------------------------
// Class:   _ObjectBase
//
// Purpose: Template Base class for a Model with IModel support
//
//------------------------------------------------------------------------------
template <
            class Interface,
            class InterfaceClass,
            class TDelegate,
            class TInitializeParameter,
            class TEventSource,
            class TEventHandler,
            class TSender,
            class TEventArgs,
            class TypeInfo,
            class TLogCategoryFactory,
            class TBASE = foundation::ctl::ComposableWeakRefSourceInspectableBase
        >
class _ObjectBase :
    public _FoundationBase<Interface, InterfaceClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, true, TBASE>
{
private:
    // our Base Type
    typedef _FoundationBase<Interface, InterfaceClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, true, TBASE> _Base_Type;

    // this Object Type
    typedef _ObjectBase<Interface, InterfaceClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, TypeInfo, TLogCategoryFactory, TBASE>  _BaseObject_Type;

public:

    HRESULT _Initialize(_In_ const TInitializeParameter *pInitializeParameter)
    {
        foundation_assert(pInitializeParameter);
        IFR_ASSERT(_Base_Type::_Initialize(pInitializeParameter));

        OnObjectBaseInitialize(this);
        IFR_ASSERT(SetTypeInfo(pInitializeParameter->m_iid_type));

        if(this->IsLevelEnabled(
            TLogCategoryFactory::_GetInitializeLevel(),
            foundation::Object_Category_Initialize_Id))
        {
            foundation::logger_util::Log(
                CLogger::GetLogger(),
                this->AsInspectable(),
                foundation::LoggingLevel::Debug,
                // Message Enum Value
                foundation::Object_Category_Initialize_Id,
                // Log Category
                TLogCategoryFactory::GetLogCategory()->AsInterface(),
                foundation::ObjectInitialize_LogRecordEnum::TypeId,
                // UniqueId
                foundation::Object_LogRecord::UniqueId,
                foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
                // TypeId
                foundation::Object_LogRecord::TypeId,
                foundation::pv_util::CreateValue(this->GetTypeInfo()).Get(),
                // Options
                foundation::ObjectInitialize_LogRecord::Options,
                foundation::pv_util::CreateValue((UINT32)pInitializeParameter->m_options).Get(),
                -1);
        }
        return S_OK;
    }

    inline bool IsLocalObjectTypeInfo()
    {
        return IsOptionEnabled(this->GetOptions(), foundation::library::FoundationClassOptions_IsLocalObjectTypeInfo);
    }

    void OnFinalRelease() override
    {
        OnObjectBaseReleased(this);

        if (this->IsLevelEnabled(
            TLogCategoryFactory::_GetFinalReleaseLevel(),
            foundation::Object_Category_FinalRelease_Id))
        {
            foundation::logger_util::Log(
                CLogger::GetLogger(),
                nullptr,
                foundation::LoggingLevel::Debug,
                // Message Enum Value
                foundation::Object_Category_FinalRelease_Id,
                // Log Category
                TLogCategoryFactory::GetLogCategory()->AsInterface(),
                foundation::ObjectFinalRelease_LogRecordEnum::TypeId,
                // UniqueId
                foundation::Object_LogRecord::UniqueId,
                foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
                // TypeId
                foundation::Object_LogRecord::TypeId,
                foundation::pv_util::CreateValue(this->GetTypeInfo()).Get(),
                -1);
        }

        RELEASE_INTERFACE(m_pTypeInfo);
        _Base_Type::OnFinalRelease();
    }

    void OnIsObservedChanged(bool newValue) override
    {
        if (this->IsLevelEnabled(
            TLogCategoryFactory::_GetIsObservedLevel(),
            foundation::Object_Category_IsObserved_Id))
        {
            foundation::logger_util::Log(
                CLogger::GetLogger(),
                this->AsInspectable(),
                foundation::LoggingLevel::Debug,
                // Message Enum Value
                foundation::Object_Category_IsObserved_Id,
                // Log Category
                TLogCategoryFactory::GetLogCategory()->AsInterface(),
                foundation::ObjectIsObserved_LogRecordEnum::TypeId,
                // UniqueId
                foundation::Object_LogRecord::UniqueId,
                foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
                // TypeId
                foundation::Object_LogRecord::TypeId,
                foundation::pv_util::CreateValue(this->GetTypeInfo()).Get(),
                // Observerd
                foundation::ObjectIsObserved_LogRecord::Observed,
                foundation::pv_util::CreateValue(newValue).Get(),
                -1);
        }
        _Base_Type::OnFinalRelease();
    }

protected:
    friend class CFireEventHandlerContainerBase;

    HRESULT LogObject(
        foundation::LoggingLevel level,
        UINT32 messageId)
    {
        if (this->IsLevelEnabled(
            level,
            messageId))
        {
            foundation::logger_util::Log(
                CLogger::GetLogger(),
                this->AsInspectable(),
                level,
                // Message Enum Value
                messageId,
                // Log Category
                TLogCategoryFactory::GetLogCategory()->AsInterface(),
                foundation::Object_LogRecordEnum::TypeId,
                // UniqueId
                foundation::Object_LogRecord::UniqueId,
                foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
                // TypeId
                foundation::Object_LogRecord::TypeId,
                foundation::pv_util::CreateValue(this->GetTypeInfo()).Get(),
                -1);
        }
        return S_OK;
    }

    template <class _TParam>
    struct _ObjectBaseTraceParameter : foundation::create_trace_parameter
    {
        _ObjectBaseTraceParameter(
            _BaseObject_Type *pThis,
            LPCSTR_t parameter_name,
            _TParam& parameter_value
            ) :
            _pThis(*pThis),
            _parameter_name(parameter_name),
            _parameter_value(parameter_value)
        {
        }

        foundation::IInspectable *operator()()
        {
            if (_trace_parameter_ptr == nullptr)
            {
                create_trace_parameter();
            }
            return _trace_parameter_ptr.Get();
        }

        HRESULT create_trace_parameter()
        {
            foundation::logger_util::log_parameter_pair_ptr_vector_type logParameters;
            // ObjectId
            IFR_ASSERT(foundation::logger_util::AppendLogParameter(
                foundation::ObjectTrace_LogRecord_ObjectId_Id,
                _pThis.AsInspectable(),
                logParameters
                ));
            // Parameter Name
            IFR_ASSERT(foundation::logger_util::AppendLogParameter(
                foundation::ObjectTrace_LogRecord_ParameterName_Id,
                foundation::pv_util::CreateValue(_parameter_name),
                logParameters
                ));
            // Parameter Value
            IFR_ASSERT(foundation::logger_util::AppendLogParameter(
                foundation::ObjectTrace_LogRecord_ParameterValue_Id,
                foundation::pv_util::CreateValue(_parameter_value),
                logParameters
                ));
            // store in our trace parameter
            IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
                (UINT32)logParameters.size(),
                (foundation::IInspectable **)logParameters.front().GetAddressOfPtr(),
                _trace_parameter_ptr.GetAddressOf()));
            return S_OK;
        }

        _BaseObject_Type& _pThis;
        LPCSTR_t          _parameter_name;
        _TParam&          _parameter_value;
        foundation::InspectablePtr _trace_parameter_ptr;
    };

    template <class _TParam>
    _ObjectBaseTraceParameter<_TParam> CreateLogTraceCallback(
        LPCSTR_t parameter_name,
        _TParam& parameter_value)
    {
        return _ObjectBaseTraceParameter<_TParam>(this, parameter_name, parameter_value);
    }

    bool IsLevelEnabled(
        foundation::LoggingLevel level,
        UINT32 messageEnumValueId)
    {
        if (!CLogger::IsAvailable() || !foundation::logger_util::IsAvailable())
        {
            return false;
        }
        struct _GetTypeInfo : _GetTypeFunc
        {
            _GetTypeInfo(_BaseObject_Type *pThis):
                _pThis(*pThis)
            {
            }

            bool operator()(IID* pType) const
            {
                *pType = _pThis.GetTypeInfo();
                return true;
            }
            _BaseObject_Type& _pThis;

        };
        return TLogCategoryFactory::GetLogCategory()->IsLevelEnabled(
            level,
            messageEnumValueId,
            _GetTypeInfo(this));
    }

    _ObjectBase() :
          m_pTypeInfo(nullptr)
    {
        m_UniqueId = GetNextObjectUniqueId();
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        HRESULT hr = S_OK;
        if (iid == foundation::IObject::GetIID())
        {
            *ppInterface = static_cast<foundation::IObject *>(this);
        }
        else if (iid == foundation::library::IFoundationClassBase::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IFoundationClassBase *>(this);
        }
        else if (iid == foundation::library::IObjectClass::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IObjectClass *>(this);
        }
        else
        {
            hr = _Base_Type::QueryInterfaceImpl(iid,ppInterface);
        }
        return hr;
    }

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)


    void DumpObjectBase(const CHAR_t *pTypeName)
    {
        ULONG refCnt;
        foundation::IUnknown *pControllingUnk = nullptr;
        if (this->IsComposed())
        {
            foundation::QueryWeakReference(
                foundation::ctl::inspectable::CastToInspectable(this),
                foundation_IID_IUnknown,
                &pControllingUnk);
            foundation_assert(pControllingUnk);
            pControllingUnk->AddRef();
            refCnt = pControllingUnk->Release();
        }
        else
        {
            refCnt = this->GetRefCount();
        }

        _traceMessage(U("Type:%s UniqueId:%d this:0x%04x pControllingUnk:0x%04x RefCount:%d"), pTypeName, m_UniqueId, this, pControllingUnk, refCnt);
    }
#endif

#if defined(_WINDOWS_RUNTIME)
    HRESULT GetRuntimeClassNameImpl(_Outptr_ HSTRING *pClassName) override
    {
        foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
        IFR_ASSERT(this->GetTypeInfo(spTypeInfo.GetAddressOf()));

        foundation_assert(spTypeInfo != nullptr);
        IID iidType;
        spTypeInfo->GetType(&iidType);

        //if(CCoreModelInfoCache::IsCoreModelType(iidType))
        //{
        //    foundation::HStringPtr typeName;
        //    spTypeInfo->GetName(&typeName);
        //    return GetCoreRuntimeClassName(_pal_GetStringRawBuffer(typeName,nullptr),pClassName);
        //}
        //else
        {
            spTypeInfo->GetName(pClassName);
        }
        return S_OK;
    }
#endif

    // IObject Interface
    STDMETHOD(GetUniqueId)(_Out_ UINT32 *uniqueId)
    {
        *uniqueId = m_UniqueId;
        return S_OK;
    }

    STDMETHOD(GetProcessId)(_Out_ UINT32 *processId)
    {
        *processId = (UINT32)_pal_GetCurrentProcessId();
        return S_OK;
    }

    STDMETHOD(GetTypeInfo)(_COM_Outptr_ foundation::IObjectTypeInfo** ppValue)
    {
        IFCPTR_ASSERT(ppValue);
        IFCPTR_ASSERT(m_pTypeInfo);

        return foundation::QueryInterface(m_pTypeInfo,ppValue);
    }

    // IObjectClass interface
    STDMETHOD(SetTypeInfo)(_In_ foundation::IObjectTypeInfo *pObjectTypeInfo)
    {
        IFCPTR_ASSERT(pObjectTypeInfo);
        RELEASE_INTERFACE(m_pTypeInfo);
        IFR_ASSERT(foundation::QueryInterface(pObjectTypeInfo, &m_pTypeInfo));
        SetOptionEnabled(this->GetOptions(), foundation::library::FoundationClassOptions_IsLocalObjectTypeInfo, true);

        return S_OK;
    }

    virtual HRESULT SetTypeInfo(_In_ const IID& iidType)
    {
        foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
        IFR_ASSERT(foundation::GetTypeInfo(
            IsEqualIID(iidType, foundation_GUID_NULL) ? Interface::GetIID() : iidType,
            spTypeInfo.GetAddressOf()));
        foundation_assert(m_pTypeInfo == nullptr);
        // Type safe to our expected type
        IFR_ASSERT(foundation::QueryInterface(spTypeInfo, &m_pTypeInfo));

        return S_OK;
    }

    template <
    class T,
    class _TEventArgs
    >
    HRESULT FireWithCallback(
		typename _Base_Type::_FoundationBaseLock& csl,
        _In_ HRESULT(T::*_FireEventDelegate)(foundation::library::IFireEventHandlerContainer *, _TEventArgs *),
        _In_ _TEventArgs *pEventArgs)
    {
        if(this->IsLevelEnabled(
            TLogCategoryFactory::_GetFireEventLevel(),
            foundation::Object_Category_FireEvent_Id))
        {
            foundation::logger_util::Log(
                CLogger::GetLogger(),
                this->AsInspectable(),
                foundation::LoggingLevel::Debug,
                // Message Enum Value
                foundation::Object_Category_FireEvent_Id,
                // Log Category
                TLogCategoryFactory::GetLogCategory()->AsInterface(),
                foundation::ObjectFireEvent_LogRecordEnum::TypeId,
                // UniqueId
                foundation::Object_LogRecord::UniqueId,
                foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
                // TypeId
                foundation::Object_LogRecord::TypeId,
                foundation::pv_util::CreateValue(this->GetTypeInfo()).Get(),
                // EventArgs
                foundation::ObjectFireEvent_LogRecord::EventArgs,
                pEventArgs,
                -1);
        }
        return _Base_Type::FireWithCallback(csl,_FireEventDelegate, pEventArgs);
    }

    template <
        class T,
        class _TEventArgs
    >
    inline HRESULT FireWithCallback(
        _In_ HRESULT(T::*_FireEventDelegate)(foundation::library::IFireEventHandlerContainer *,_TEventArgs *),
        _In_ _TEventArgs *pEventArgs)
    {
		typename _Base_Type::_FoundationBaseLock csl(*this);
        return this->FireWithCallback(csl, _FireEventDelegate, pEventArgs);
    }

    GUID GetTypeInfo()
    {
        foundation_assert(m_pTypeInfo);
        if (m_pTypeInfo == nullptr)
        {
            return foundation_GUID_NULL;
        }
        foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
        foundation::QueryInterface(m_pTypeInfo, spTypeInfo.GetAddressOf());
        foundation_assert(spTypeInfo != nullptr);
        IID iidType;
        IGNOREHR(spTypeInfo->GetType(&iidType));
        return iidType;
    }

protected:
	UINT32                   m_UniqueId;
    TypeInfo                *m_pTypeInfo;

};
