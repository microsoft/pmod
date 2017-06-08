/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TBaseObjectProxy.h
****/
#pragma once

#include "BaseObjectProxy.h"
#include "ProxyObjectFactory.h"

#include "BaseModelSource.h"
#include "EventHandler.h"
#include "CriticalSection.h"

//#include <foundation/foundation_logger_type_info.h>
#include <foundation/delegate_weak_ref_library.h>

//------------------------------------------------------------------------------
// Class:   TBaseObjectProxy Template
//
// Purpose: Define a base class for all the Model based proxy we will support
//
//------------------------------------------------------------------------------
template
<
    class Interface,
    class TEventSource,
    class TEventHandler,
    class TEventArgs,
    class TProxyModelDelegate,
    class TProxyEventSourceImpl = TNotifyEventSourceImpl<TEventSource, TEventHandler, foundation::IUnknown, TEventArgs, BaseObjectProxy, foundation::library::ICriticalSection>,
    class TSourceType = Interface
>
class TBaseObjectProxy :
    public BaseObjectProxy,
    public Interface,
    public pmod::ISourceModel,
    public SingleBaseModelSourceAndEventSource<TSourceType, TEventSource, TEventHandler, TEventArgs>
//    protected INotifyEventHandlerDelegate<TEventHandler>
{
private:
    typedef TBaseObjectProxy<Interface, TEventSource, TEventHandler, TEventArgs, TProxyModelDelegate, TProxyEventSourceImpl, TSourceType> _ThisType;

public:
    static bool _IsProxyFreeThreaded(CProxyObjectFactory *pProxyObjectFactory)
    {
        return pProxyObjectFactory->_IsFreeThreaded();
    }

    typedef SingleBaseModelSourceAndEventSource<TSourceType, TEventSource, TEventHandler, TEventArgs> _BaseModelSource;

    HRESULT _Initialize(
        _In_ UINT32 proxyUniqueId,
        _In_ TSourceType *pSourceObject,
        _In_opt_ pmod::proxy::IProxyObjectDelegate *pProxyModelDelegateBase,
        _In_ CProxyObjectFactory *pProxyObjectFactory,
        _In_ bool isFreeThreadedEnabled)
    {
        foundation_assert(pSourceObject);
        this->m_ProxyUniqueId = proxyUniqueId;
        if (pProxyModelDelegateBase)
        {
            IFR(foundation::QueryInterface(pProxyModelDelegateBase, this->m_spProxyDelegate.GetAddressOf()));
        }

        IFR_ASSERT(BaseObjectProxy::_Initialize(pProxyObjectFactory, isFreeThreadedEnabled));
        IFR_ASSERT(_BaseModelSource::_InitializeSourceEventHandler(isFreeThreadedEnabled));
        IFR_ASSERT(TProxyEventSourceImpl::CreateInstance(this, this, this->m_spEventHandler.GetAddressOf()));
        // during initialize we want to avoid QI and Delegate invocation
        IFR_ASSERT(this->Attach(pSourceObject));
        IFR_ASSERT(this->on_source_attached_internal(pSourceObject));

        LogInitialize();

        return S_OK;
    }

protected:
    TBaseObjectProxy()
    {
    }

    virtual ~TBaseObjectProxy()
    {
    }

    // Overrides from BaseModelProxy
    foundation::IObject *GetObjectSource() override
    {
        return this->m_spObjectSource;
    }

    REFIID GetModelIId() const override
    {
        return Interface::GetIID();
    }

    size_t GetProxyCountEventHandlers() override
    {
        return m_spEventHandler->GetEventHandlers().size();
    }

    void OnIsObservedChanged(bool newValue) override
    {
        if (newValue)
        {
            IGNOREHR(this->EnsureAddSourceHandler());
        }
        else
        {
            IGNOREHR(this->EnsureRemoveSourceHandler());
        }
    }

    // Overrides from ComBase
    void FinalRelease() override
    {
        LogFinalRelease();
        // Enforce detach from the Source Object
        SetSource(nullptr);
        BaseObjectProxy::FinalRelease();
    }

    Interface *AsInterface()
    {
        return static_cast<Interface *>(this);
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == Interface::GetIID())
        {
            *ppInterface = static_cast<Interface *>(this);
        }
        else if (iid == TSourceType::GetIID())
        {
            *ppInterface = static_cast<TSourceType *>(this);
        }
        else if (iid == pmod::ISourceModelBase::GetIID())
        {
            *ppInterface = static_cast<pmod::ISourceModelBase *>(this);
        }
        else if (iid == pmod::ISourceModel::GetIID())
        {
            *ppInterface = static_cast<pmod::ISourceModel *>(this);
        }
        else if (iid == _IBaseModelSource::GetIID())
        {
            *ppInterface = static_cast<_IBaseModelSource *>(this);
        }
        else
        {
            return BaseObjectProxy::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // IObject Interface
    STDMETHOD(GetUniqueId)(UINT32 *uniqueId) override
    {
        return BaseObjectProxy::GetUniqueId(uniqueId);
    }

    STDMETHOD(GetProcessId)(UINT32 *processId) override
    {
        return BaseObjectProxy::GetProcessId(processId);
    }

    STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue)
    {
        return BaseObjectProxy::GetTypeInfo(ppValue);
    }

protected:

    // =============== Interface ISourceModel =======================
    STDMETHOD(GetSource)(foundation::IObject **ppObject) override
    {
        if (this->m_spObjectSource)
        {
            *ppObject = this->m_spObjectSource;
            (*ppObject)->AddRef();
        }
        else
        {
            *ppObject = nullptr;
        }
        return S_OK;
    }

    STDMETHOD(SetSource)(foundation::IObject *pObject) override
    {
        if (pObject)
        {
            foundation::ComPtr<TSourceType> spSource;
            IFR(foundation::QueryInterface(pObject, spSource.GetAddressOf()));
            return this->SetSourceInternal(spSource);
        }
        else
        {
            return this->SetSourceInternal(nullptr);
        }
    }

    STDMETHOD(GetIsAttached)(bool *value) override
    {
        *value = _BaseModelSource::IsAttached();
        return S_OK;
    }

    STDMETHOD(GetForceAttached)(bool *value) override
    {
        *value = _BaseModelSource::IsForceAttached();
        return S_OK;
    }

    STDMETHOD(SetForceAttached)(bool newValue) override
    {
        return _BaseModelSource::SetForceAttachedInternal(newValue);
    }

    STDMETHOD(GetAttachEnabled)(bool *value) override
    {
        *value = _BaseModelSource::IsAttachEnabled();
        return S_OK;
    }

    STDMETHOD(SetAttachEnabled)(bool newValue) override
    {
        return _BaseModelSource::SetIsAttachEnabledInternal(newValue);
    }

    // =============== Interface IBaseObjectProxyInternal =======================

    STDMETHOD(IsAttachToEventSourceNeeded)(bool attach, bool& result) override
    {
        result = attach ?
            NeedAttachToEventSource() :
            !NeedDetachFromEventSource();   // please note the negative logic !
        return S_OK;
    }

    STDMETHOD(Dispose()) override
    {
        this->DetachIf();
        IFR(BaseObjectProxy::Dispose());
        // Remove Model Source Reference
        this->m_spObjectSource.Release();
        return S_OK;
    }

    STDMETHOD(ResyncSource)(
        foundation::IInspectable *pSource,
        _SyncSourceInfo_Vector_Type& syncSourceInfo) override
    {
        UNREFERENCED_PARAMETER(syncSourceInfo);

        foundation::ComPtr<TSourceType> spSource;
        if (pSource != nullptr)
        {
            IFR_ASSERT(foundation::QueryInterface(pSource, spSource.GetAddressOf()));
        }

        LogResyncSource(spSource);

        IFR_ASSERT(this->SetSourceInternal(spSource));

        // add if not null
        if (pSource != nullptr)
        {
            IFR_ASSERT(this->AddToCachedInstances());
        }
        return S_OK;
    }

    STDMETHOD(FireResyncEvent)(foundation::IUnknown *pEventArg) override
    {
        UNREFERENCED_PARAMETER(pEventArg);

        foundation_assert(false);
        return E_NOTIMPL;
    }

    // =============== Interface IEventSourceClass =======================
    STDMETHODIMP AddEventHandler(
        _In_ GUID eventSourceType,
        _In_opt_ foundation::IUnknown *pEventHandler,
        _In_opt_ foundation::IInspectable *pOptions,
        _COM_Outptr_ foundation::IInspectable **ppResult) override
    {
        IFR(this->VerifyModelSource());

        if (eventSourceType == TEventSource::GetIID())
        {
            // Add the handler to myself
            foundation::ComPtr<TEventSource> spEventSource;
            IFR_ASSERT(this->GetEventSource(spEventSource.GetAddressOf()));
            foundation::ComPtr<TEventHandler> spEventHandler;
            IFR_ASSERT(foundation::QueryInterface(pEventHandler, spEventHandler.GetAddressOf()));
            IFR_ASSERT(spEventSource->AddHandler(spEventHandler));
            // QI 'Source' object
            foundation::ComPtr<foundation::library::IEventSourceClass> event_source_class_ptr;
            IFR(foundation::QueryInterface(this->m_spObjectSource, event_source_class_ptr.GetAddressOf()));
            // Note: we pass the optional parameter 'pEventHandler' as null since
            // our proxy event handler is already attached after our call to 'AddHandler'
            IFR_ASSERT(event_source_class_ptr->AddEventHandler(eventSourceType,nullptr, pOptions, ppResult));
        }
        else
        {
            return E_NOTIMPL;
        }
        return S_OK;
    }

    STDMETHODIMP RemoveEventHandler(
        _In_ GUID eventSourceType,
        _In_ foundation::IUnknown *pEventHandler) override
    {
        IFR(this->VerifyModelSource());

        if (eventSourceType == TEventSource::GetIID())
        {
            foundation::ComPtr<TEventSource> spEventSource;
            IFR_ASSERT(this->GetEventSource(spEventSource.GetAddressOf()));
            foundation::ComPtr<TEventHandler> spEventHandler;
            IFR_ASSERT(foundation::QueryInterface(pEventHandler, spEventHandler.GetAddressOf()));
            IFR_ASSERT(spEventSource->RemoveHandler(spEventHandler));
        }
        else
        {
            return E_NOTIMPL;
        }
        return S_OK;
    }

    template
        <
            class T,
            class _TEventArgs
        >
    inline HRESULT FireEventSupportInternal(
        _In_ HRESULT(T::*_FireEventDelegate)(foundation::library::IFireEventHandlerContainer *, _TEventArgs *),
        _In_ _TEventArgs *pEventArgs)
    {
        foundation_lib::_FoundationLock csl(*this);
        return foundation::library::_FireEventSupportUtil::_FireEventWithCallback(
			csl, 
			static_cast<T *>(this),
			_FireEventDelegate, 
			pEventArgs);
    }

    HRESULT FireEventInternal(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ TEventArgs *pEventArgs)
    {
        IFCEXPECT(pEventArgs);
        IFR(m_spEventHandler->FireEvent(
			pEventHandlerContainer,
            static_cast<Interface*>(this),
            pEventArgs));
        return S_OK;
    }

    // Overrides from EventSourceBase
    foundation::IUnknown *GetControllerDelegate() override
    {
        return this->AsInspectable();
    }

    // ===============Overrides from BaseModelSource=======================
    template <
        typename _Delegate_Type>
        HRESULT CreateEventHandlerDelegateWeakRefWithType(TEventHandler **ppEventHandlerDelegate)
    {
        return foundation::CreateEventHandlerDelegateWeakRefWithType<_Delegate_Type>(
            this,
            &TBaseObjectProxy::InvokeInternal,
            ppEventHandlerDelegate
            );
    }

    HRESULT CreateSourceEventHandlerDelegate(
        bool isFreeThreaded,
        TEventHandler **ppEventHandlerDelegate) override
    {
        if (isFreeThreaded)
        {
            return foundation::CreateEventHandlerDelegateWeakRef<IBaseObjectProxyInternal>(
                this,
                &TBaseObjectProxy::InvokeInternal,
                ppEventHandlerDelegate);
        }
        else
        {
            return _BaseModelSource::CreateSourceEventHandlerDelegate(isFreeThreaded, ppEventHandlerDelegate);
        }
    }

    HRESULT EnableCallback(
        bool enable,
        foundation::IUnknown *pController) override
    {
        if (this->IsFreeThreadApartment())
        {
            typedef foundation::DelegateWeakRefBase<TEventHandler> _Delegate_Type;
            return _BaseModelSource::template EnableWeakRefCallbackType<_Delegate_Type>(this->m_pEventHandlerDelegate, enable);
        }
        else
        {
            return _BaseModelSource::EnableCallback(enable, pController);
        }
    }

    HRESULT AttachToSource() override
    {
        LogAttachToSource();
        IFR(this->_BaseModelSource::AttachToSource());
        if (m_spProxyDelegate)
        {
            IFR(m_spProxyDelegate->OnSourceAttached());
        }
        return S_OK;
    }

    HRESULT DetachFromSource() override
    {
        LogDetachFromSource();
        IFR(this->_BaseModelSource::DetachFromSource());
        if (m_spProxyDelegate)
        {
            IFR(m_spProxyDelegate->OnSourceDetached());
        }
        return S_OK;
    }

    bool IsAttachEnabled() override
    {
        return this->_BaseModelSource::IsAttachEnabled() && IsAttachEnabledInternal();
    }

    HRESULT SetSourceInternal(TSourceType *pSource) override
    {
        IFR(this->_BaseModelSource::SetSourceInternal(pSource));
        IFR(on_source_attached_internal(pSource));

        return S_OK;
    }

    bool IsSourceAlive() const
    {
        foundation_assert(this->m_spObjectSource);
        // let's probe the Source Object
        UINT32 uniqueId;
        HRESULT hr = this->m_spObjectSource->GetUniqueId(&uniqueId);
        return SUCCEEDED(hr);
    }

    bool IsReferenceAttached() const override
    {
        return this->_BaseModelSource::IsReferenceAttached() && this->IsSourceAlive();
    }

    virtual HRESULT GetEventSource(
        _COM_Outptr_ TEventSource **ppEventSource)
    {
        UNREFERENCED_PARAMETER(ppEventSource);
        foundation_assert(false);
        return E_NOTIMPL;
    }
    virtual  HRESULT on_invoke_internal(foundation::IUnknown* pSender, TEventArgs* pArgs) = 0;

    HRESULT on_invoke_internal_dispatched(foundation::IUnknown* pSender, TEventArgs* pArgs)
    {
        HRESULT hr;
        if (!this->IsAttached())
        {
            // during the dispatch the proxy was disconnected already
            hr = S_FALSE;
        }
        else
        {
            hr = on_invoke_internal(pSender, pArgs);
        }
        // compensate AddRef when dispatch is completed
        this->ReleaseImpl();
        return hr;
    }

    HRESULT InvokeInternal(foundation::IUnknown* pSender, TEventArgs* pArgs) override
    {
        LogSourceEvent(
            foundation::LoggingLevel::Debug, 
            (UINT32)pmod::proxy::BaseProxy_Category::SourceEvent,
            pArgs);

        if (this->IsInvokeDispatcherRequired())
        {
            // add ref myself during the dispatch
            this->AddRefImpl();
            return InvokeLater<
                _ThisType,
                TEventArgs,
                &_ThisType::on_invoke_internal_dispatched
            >(this, pSender, pArgs);
        }
        return on_invoke_internal(pSender, pArgs);
    }

    virtual HRESULT on_source_attached_internal(TSourceType *pSource)
    {
        if (pSource)
        {
            IFR(pSource->GetUniqueId(&m_UniqueId));
            IFR(pSource->GetProcessId(&m_ProcessId));

            // get the 'Source' Type id and cache it
            foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
            IFR(pSource->GetTypeInfo(spModelTypeInfo.GetAddressOf()));
            foundation_assert(spModelTypeInfo != nullptr);
            IFR(spModelTypeInfo->GetType(&m_IIDTypeCache));
        }
        else
        {
            this->ClearFromCachedInstances();
        }
        return S_OK;
    }


    void OnIsAttachedNeededChanged(bool isAttachedNeeded) override
    {
        if (IsPurgeStateEnabledInternal())
        {
            SetPurgeState(EvaluatePurgeState(isAttachedNeeded));
        }
    }

    bool NeedAttachToEventSource() override
    {
        return this->IsReferenceAttached() && this->IsProxyObserved();
    }

    bool NeedDetachFromEventSource() override
    {
        // TODO: should we also confirm 'IsReferenceAttached' ? 
        return !this->IsProxyObserved();
    }


protected:
    foundation::ComPtr<foundation::ctl::ComObject<TProxyEventSourceImpl> > m_spEventHandler;
    foundation::ComPtr<TProxyModelDelegate>                     m_spProxyDelegate;
};
