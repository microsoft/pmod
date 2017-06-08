/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandProxy.h
****/

#pragma once

#include "TBaseModelProxy.h"

//------------------------------------------------------------------------------
// Class:   CCommandProxy
//
// Purpose: The Command Model Proxy implementation
//
//------------------------------------------------------------------------------

typedef 
TBaseModelProxy
<
    pmod::ICommand,
    pmod::ICanExecuteChangedEventSource,
    pmod::ICanExecuteChangedEventHandler,
    pmod::ICanExecuteChangedEventArgs,
    pmod::proxy::IProxyObjectDelegate
>
_CommandModelProxyBaseType;

class CCommandProxy:
    public _CommandModelProxyBaseType
{
public:

    // ICommandModel
    STDMETHOD (GetState)(UINT32* pVal);
    STDMETHOD (CanExecute)(IInspectable *pParameter,bool *pCanExecute);
    STDMETHOD (Execute)(IInspectable *pParameter,IInspectable **ppResult);
    STDMETHOD (GetCanExecuteChangedEventSource)(_COM_Outptr_ pmod::ICanExecuteChangedEventSource** ppEventSource)
    {
        * ppEventSource =  m_spEventHandler;
        (*ppEventSource)->AddRef();
        return S_OK;
    }
    // Override from IBaseModelProxyInternal
    STDMETHOD(DisconnectFromSource)(foundation::IInspectable **ppSourceInfo) override;
    STDMETHOD(ConnectToSource)(foundation::IInspectable *pSourceInfo, _SyncSourceInfo_Vector_Type& syncSourceInfo) override;
    STDMETHOD(ResyncSource)(foundation::IInspectable *pSource, _SyncSourceInfo_Vector_Type& syncSourceInfo) override;
    STDMETHOD(FireResyncEvent)(foundation::IUnknown *pEventArg) override;

    // Override from BaseSourceModel
    void SetIsAttached(bool isAttached) override;
    HRESULT on_invoke_internal(IUnknown* pSender,pmod::ICanExecuteChangedEventArgs* pArgs) override;

    // Overrides from BaseModelProxy
    foundation::ILogCategory *GetLogCategory() override;
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    UINT32 GetTrackRefCountOption() override { return _OptionTrackRefCountCommandModelProxies; }
#endif

    // Pure Virtual Methods
   HRESULT GetSourceEventSource(pmod::ICanExecuteChangedEventSource **ppEventSource) override
   {
        IFR(VerifyModelSource());
        return m_spObjectSource->GetCanExecuteChangedEventSource(ppEventSource);
   }
   // Override from ComBase
   HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override;

#if defined(DEBUG) && defined(TRACK_REF_COUNT)
   void FinalRelease() override
   {
       if (foundation::debug_util::IsOptionsFlag(_OptionTrackRefCountAllProxies | _OptionTrackRefCountCommandModelProxies))
       {
           pal::library::TraceMessage(U("CCommandProxy::FinalRelease unique_id:%d\n"), this->m_UniqueId);
       }
       _CommandModelProxyBaseType::FinalRelease();
   }
#endif

protected:
    CCommandProxy():
         m_State((UINT32)-1)
    {
    }
private:
    void InvalidateStateCache();
    HRESULT PerformFireCanExecuteChangedEventArgs(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		pmod::ICanExecuteChangedEventArgs *pEventArgs);
    HRESULT FireCanExecuteChangedEvent(pmod::ICanExecuteChangedEventArgs *pEventArgs);
    HRESULT GetStateInternal(UINT32 *pState);

    bool IsCommandProxyModelCacheEnabled();
private:
    UINT32 m_State;

};

