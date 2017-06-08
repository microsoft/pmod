/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationProxy.h
****/

#pragma once

#include "TBaseObjectProxy.h"


//------------------------------------------------------------------------------
// Class:   CAsyncOperationProxy
//
// Purpose: Property Model Result Proxy implementation
//
//------------------------------------------------------------------------------
typedef 
TBaseObjectProxy
<
    foundation::IAsyncOperation,
    foundation::ICompletedEventSource,
    foundation::ICompletedEventHandler,
    foundation::ICompletedEventArgs,
    pmod::proxy::IProxyObjectDelegate
>
_AsyncOperationModelProxyBaseType;

class CAsyncOperationProxy:
    public _AsyncOperationModelProxyBaseType
{
protected:
    CAsyncOperationProxy();
public:

    static bool _IsProxyFreeThreaded(CProxyObjectFactory *pProxyObjectFactory)
    {
        UNREFERENCED_PARAMETER(pProxyObjectFactory);
        return true;
    }

    // IAsyncOperation
    STDMETHOD(GetResult)(UINT32 timeout, IInspectable **ppResult);
    STDMETHOD(GetError)(HRESULT* hResult);
    STDMETHOD(GetStatus)(foundation::ResultStatus *status);

    STDMETHOD (GetIsCancelable)(bool* pVal)
    {
        IFR(VerifyModelSource());
        return m_spObjectSource->GetIsCancelable(pVal);
    }

    STDMETHOD(GetCompletedEventSource)(foundation::ICompletedEventSource** ppEventSource)
    {
        * ppEventSource =  m_spEventHandler;
        (*ppEventSource)->AddRef();
        return S_OK;
    }

    STDMETHOD(GetProgressEventSource)(foundation::IProgressEventSource** ppEventSource)
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }

    STDMETHOD(Cancel)()
    {
        IFR(VerifyModelSource());
        return m_spObjectSource->Cancel();
    }

    STDMETHOD(Wait)(UINT32 timeout,foundation::ResultStatus *status)
    {
        IFR(VerifyModelSource());
        return m_spObjectSource->Wait(timeout,status);
    }

    // Override from BaseSourceModel
    HRESULT on_invoke_internal(IUnknown* pSender, foundation::ICompletedEventArgs* pArgs) override;

    // Pure Virtual Methods
    HRESULT GetSourceEventSource(foundation::ICompletedEventSource **ppEventSource) override
    {
        IFR(VerifyModelSource());
        return m_spObjectSource->GetCompletedEventSource(ppEventSource);
    }

   // Override from ComBase
   HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override;
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
   void FinalRelease() override
   {
       if (foundation::debug_util::IsOptionsFlag(_OptionTrackRefCountAllProxies | _OptionTrackRefCountAsyncOperationProxies))
       {
           pal::library::TraceMessage(U("CAsyncOperationProxy::FinalRelease unique_id:%d\n"), this->m_UniqueId);
       }
       _AsyncOperationModelProxyBaseType::FinalRelease();
   }
#endif


   // Override from IBaseModelProxyInternal
   STDMETHOD(DisconnectFromSource)(foundation::IInspectable **ppSourceInfo) override;
   STDMETHOD(ConnectToSource)(foundation::IInspectable *pSourceInfo, _SyncSourceInfo_Vector_Type& syncSourceInfo) override;
   STDMETHOD(ResyncSource)(foundation::IInspectable *pSource, _SyncSourceInfo_Vector_Type& syncSourceInfo) override;
   STDMETHOD(FireResyncEvent)(foundation::IUnknown *pEventArg) override;

   foundation::ILogCategory *GetLogCategory() override;
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
   UINT32 GetTrackRefCountOption() override { return _OptionTrackRefCountAsyncOperationProxies; }
#endif

private:
    HRESULT PerformFireCompletedEventArgs(
        _In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
        _In_ foundation::ICompletedEventArgs *pEventArgs);
    HRESULT FireCompletedEvent(foundation::ICompletedEventArgs *pEventArgs);

};

