/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationAdapterBase.h
****/

#pragma once

#include <foundation/library/adapter_base.h>
#include <foundation/pv_util.h>
#include <foundation/library/critical_section.h>
#include <foundation/async_operation_ptr.h>
#include <foundation/library/logger_macros.h>

//------------------------------------------------------------------------------
// Class:   TMoAsyncOperationAdapterBase
//
// Purpose: Define a template base class for the AsyncOperation generated files
//
//------------------------------------------------------------------------------
template <class TInterface,class TCompletedHandler, const IID* piid = &__uuidof(TInterface)>
class TMoAsyncOperationAdapterBase :
    public foundation::AdapterBase
    <
    foundation::IAsyncOperation,
    TInterface,
    piid
    >,
    public ABI::Windows::Foundation::IAsyncInfo
{
protected:
    TMoAsyncOperationAdapterBase() :
        _asyncOperationCompletedInvoked(false)
    {
    }
    // Interface IInterfaceAdapter
    STDMETHOD(HasEventHandlers)(_Outptr_ bool *pValue) override
    {
        *pValue = m_spCompletedHandler != nullptr;
        return S_OK;
    }
    STDMETHOD(FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		_In_ IUnknown* pEventArgs) override
    {
        foundation::library::FoundationCriticalSection lock(this->CastToInspectable());

		UNREFERENCED_PARAMETER(pFireEventHandlerContainer);
		UNREFERENCED_PARAMETER(pEventArgs);

        if (m_spCompletedHandler != nullptr && !_asyncOperationCompletedInvoked)
        {
            // compensate the 'artificial' AddRef we made when the Completed Handler was set
            this->m_pOuter->Release();

            // prevent further invocations
            _asyncOperationCompletedInvoked = true;

            // Class : CCompletedFireEventSupport
            // Description: Implements IFireEventSupport to fire the winRT  async operation completion
            // event trough the supported event handler container.
            class CCompletedFireEventSupport : public
                foundation::ctl::Implements<
                foundation::library::IFireEventSupport,
                &foundation::library::IID_IFireEventSupport>
            {
            public:
                static HRESULT CreateInstance(
                    TCompletedHandler *pCompletedHandler,
                    TInterface *pSender,
                    AsyncStatus resultStatus,
                    foundation::library::IFireEventSupport **ppFireEventSupport)
                {
                    CCompletedFireEventSupport *pCompletedFireEventSupport = nullptr;
                    foundation::ctl::ComObject<CCompletedFireEventSupport>::CreateInstance(&pCompletedFireEventSupport);

                    pCompletedFireEventSupport->_completedHandler = pCompletedHandler;
                    pCompletedFireEventSupport->_sender = pSender;
                    pCompletedFireEventSupport->_resultStatus = resultStatus;

                    *ppFireEventSupport = static_cast<foundation::library::IFireEventSupport *>(pCompletedFireEventSupport);
                    return S_OK;
                }
                STDMETHOD(GetType)(_Out_ IID *pType)
                {
                    *pType = __uuidof(TCompletedHandler);
                    return S_OK;
                }

                STDMETHOD(FireEventHandlers)(_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer) override
                {
                    UNREFERENCED_PARAMETER(pFireEventHandlerContainer);
                    return _completedHandler->Invoke(_sender, _resultStatus);
                }
                STDMETHOD(GetEventHandlers)(_Out_ UINT32 *pSize, foundation::IUnknown ***ppHandlers)
                {
                    *pSize = 1;
                    *ppHandlers = (foundation::IUnknown **)_pal_MemAlloc(sizeof(foundation::IUnknown *));
                    (**ppHandlers) = _completedHandler.Get();
                    (**ppHandlers)->AddRef();
                    return S_OK;
                }
                STDMETHOD(FireEventHandler)(_In_ foundation::library::IFireEventHandlerContainer *pContainer, _In_ foundation::IUnknown *pEventHandler)
                {
                    UNREFERENCED_PARAMETER(pContainer);
                    UNREFERENCED_PARAMETER(pEventHandler);
                    
                    return _completedHandler->Invoke(_sender, _resultStatus);
                }
            private:
                foundation::ComPtr<TCompletedHandler> _completedHandler;
                foundation::ComPtr<TInterface> _sender;
                AsyncStatus _resultStatus;
            };
            foundation::ResultStatus resultStatus;
            IGNOREHR(m_pInner->GetStatus(&resultStatus));

            foundation::library::IFireEventHandlerContainer *pEventHandlerContainer = nullptr;
            foundation::QueryWeakReference(this->CastToInspectable(), &pEventHandlerContainer);
            foundation_assert(pEventHandlerContainer);

            bool isEventSupportEnabled = false;
            pEventHandlerContainer->IsEventSupportEnabled(&isEventSupportEnabled);

            if (isEventSupportEnabled)
            {
                foundation::ComPtr<foundation::library::IFireEventSupport> spCompletedFireEventSupport;
                IFR_ASSERT(CCompletedFireEventSupport::CreateInstance(
                    m_spCompletedHandler,
                    this,
                    (AsyncStatus)resultStatus,
                    spCompletedFireEventSupport.GetAddressOf()));
                IFR_ASSERT(pEventHandlerContainer->AddFireEventSupport(spCompletedFireEventSupport));
            }
            else
            {
                IFR(m_spCompletedHandler->Invoke(this, (AsyncStatus)resultStatus));
            }
        }
        return S_OK;
    }
    STDMETHOD(QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface)  override
    {
        if (iid == __uuidof(ABI::Windows::Foundation::IAsyncInfo))
        {
            *ppInterface = static_cast<ABI::Windows::Foundation::IAsyncInfo *>(this);
        }
        else
        {
            return __super::QueryAdapterInterface(iid, ppInterface);
        }
        return S_OK;
    }

    // Interface IAsyncOperation<TInterface>
    HRESULT STDMETHODCALLTYPE put_Completed(TCompletedHandler *handler) override
    {
        // Lock/Unlock context to protect use of 'CompletedHandler' property
        foundation::library::FoundationCriticalSection lock(this->CastToInspectable());

        if (m_spCompletedHandler != nullptr)
        {
            IFR(this->_OnAdapterRemoveEventHandler(m_spCompletedHandler));
        }
        m_spCompletedHandler = handler;
        if (m_spCompletedHandler != nullptr)
        {
            foundation::ResultStatus resultStatus;
            IGNOREHR(this->m_pInner->GetStatus(&resultStatus));

            if (resultStatus != foundation::ResultStatus::Started)
            {
                // prevent further invocations
                _asyncOperationCompletedInvoked = true;

                IFR(m_spCompletedHandler->Invoke(this, (AsyncStatus)resultStatus));
            }
            else
            {
                // Note: we will force an AddRef to facilitate the consumption from managed
                // languages where the CLR will early release the original Async Operation
                // class and only care about the Completed Event Handler
                this->m_pOuter->AddRef();
            }
            // next line will cause the 'host' adapter to adjust its IsObserved
            // when hosting is a Proxy it will cause a source connection to start happen and maybe
            // firing an event right away into our 'FireAdapterEvent' callback
            IFR(this->_OnAdapterAddEventHandler(m_spCompletedHandler));
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_Completed(TCompletedHandler **handler) override
    {
        return m_spCompletedHandler.CopyTo(handler);
    }

    HRESULT STDMETHODCALLTYPE GetResults(typename TInterface::TResult_complex *results) override
    {
        return foundation::GetResult(this->m_pInner, results);
    }

    // Interface IAsyncInfo
    HRESULT STDMETHODCALLTYPE get_Id(unsigned __int32 *id) override
    {
        foundation::IObject *pInnerObject;
        IFR_ASSERT(foundation::QueryWeakReference(this->m_pInner, &pInnerObject));

        UINT32 uniqueId;
        IFR_ASSERT(pInnerObject->GetUniqueId(&uniqueId));
        *id = (unsigned __int32)uniqueId;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_Status(AsyncStatus *status) override
    {
        foundation::ResultStatus resultStatus;
        IFR_ASSERT(this->m_pInner->GetStatus(&resultStatus));
        *status = (AsyncStatus)resultStatus;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT *errorCode) override
    {
        return this->m_pInner->GetError(errorCode);
    }
    HRESULT STDMETHODCALLTYPE Cancel(void) override
    {
        return this->m_pInner->Cancel();
    }
    HRESULT STDMETHODCALLTYPE Close(void) override
    {
        return E_NOTIMPL;
    }

private:
    bool    _asyncOperationCompletedInvoked;
    foundation::ComPtr<TCompletedHandler> m_spCompletedHandler;
};

