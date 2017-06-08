/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandProxy.cpp
****/
#include "pch.h"
#include "CommandProxy.h"
#include "ProxyObjectFactory.h"

#include <pmod/library/can_execute_changed_event_args_impl.h>

#include <foundation/library/perf_counter_util.h>
#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#endif

using namespace pmod;

bool CCommandProxy::IsCommandProxyModelCacheEnabled()
{
    if(!IsProxyModelCacheEnabled())
        return false;
    // TODO: we should have a mechanism where we can acces the Attribute Flag from a Property Model
    return true;
}

HRESULT CCommandProxy::QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
{
    HRESULT hr = S_OK;
    hr = _CommandModelProxyBaseType::QueryInterfaceImpl(iid, ppInterface);
    return hr;
}

HRESULT CCommandProxy::GetStateInternal(UINT32 *pState)
{
    foundation_assert(pState);
    if (IsCommandProxyModelCacheEnabled() && m_State != (UINT32)-1)
    {
        *pState = m_State;
        return S_OK;
    }
    IFR(m_spObjectSource->GetState(pState));
    // refresh our cached 'm_State'
    m_State = *pState;
    return S_OK;
}

STDMETHODIMP CCommandProxy::GetState(UINT32* pVal)
{
    IFCPTR(pVal);

    IFR(VerifyModelSource());
    return GetStateInternal(pVal);
}

STDMETHODIMP CCommandProxy::CanExecute(IInspectable *pParameter,bool *pCanExecute)
{
    PERF_CONTEXT(ProxyFactory_PerfId_CommandModel_CanExecute_Id)
    
    IFR(VerifyModelSource());

    if (IsCommandProxyModelCacheEnabled())
    {
        UINT32 state;
        GetStateInternal(&state);
        if (state != CommandStateUnknown)
        {
            // Command is not Unknown but a state set from the model
            *pCanExecute = (state == CommandStateEnabled);
            return S_OK;
        }
    }
    foundation::InspectablePtr parameterValue(pParameter);
    IFR(FromProxyValue(parameterValue));
    return m_spObjectSource->CanExecute(parameterValue,pCanExecute);
}

STDMETHODIMP CCommandProxy::Execute(IInspectable *pParameter,IInspectable **ppResult)
{
    PERF_CONTEXT(ProxyFactory_PerfId_CommandModel_Execute_Id)
    
    IFR(VerifyModelSource());

    foundation::InspectablePtr parameterValue(pParameter);
    IFR(FromProxyValue(parameterValue));
    foundation::InspectablePtr result;
    HRESULT hr = m_spObjectSource->Execute(parameterValue,result.GetAddressOf());
    IFHR(hr);
    IFR(ToProxyValue(result));

    IFR(result.CopyTo(ppResult));
    return hr;
}

HRESULT CCommandProxy::on_invoke_internal(IUnknown* pSender,ICanExecuteChangedEventArgs* pArgs)
{   
    UNREFERENCED_PARAMETER(pSender);

    UINT32 oldState;
    UINT32 newState;

    IFR(pArgs->GetOldState(&oldState));
    IFR(pArgs->GetNewState(&newState));

    // Refresh new Cached State
    m_State = newState;

    foundation::ComPtr<ICanExecuteChangedEventArgs> spCanExecuteChangedEventArgs;
    IFR_ASSERT(library::CanExecuteChangedEventArgsImpl::CreateInstance(
        oldState,
        newState,
        spCanExecuteChangedEventArgs.GetAddressOf()));

    return FireCanExecuteChangedEvent(spCanExecuteChangedEventArgs);
}

HRESULT CCommandProxy::PerformFireCanExecuteChangedEventArgs(
	_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
	ICanExecuteChangedEventArgs *pEventArgs)
{
    HRESULT hr = FireEventInternal(pEventHandlerContainer,pEventArgs);
    return hr;
}

HRESULT CCommandProxy::FireCanExecuteChangedEvent(ICanExecuteChangedEventArgs *pEventArgs)
{
    IFR(FireEventSupportInternal(
        &CCommandProxy::PerformFireCanExecuteChangedEventArgs,
        pEventArgs));
    return S_OK;
}

void CCommandProxy::InvalidateStateCache()
{
    // Invalidate the 'Cache' State
    m_State = (UINT32)-1;
}

void CCommandProxy::SetIsAttached(bool isAttached)
{
    InvalidateStateCache();
    _CommandModelProxyBaseType::SetIsAttached(isAttached);
}

foundation::ILogCategory *CCommandProxy::GetLogCategory()
{
    return this->m_pProxyObjectFactory->GetCommandProxyCategory();
}

HRESULT CCommandProxy::DisconnectFromSource(foundation::IInspectable **ppSourceInfo)
{
    return foundation::pv_util::CreateUInt32Value((UINT32)0, ppSourceInfo);
}

HRESULT CCommandProxy::ConnectToSource(foundation::IInspectable *pSourceInfo, _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    InvalidateStateCache();
    UINT32 state;
    IFR(GetState(&state));
    foundation::ComPtr<ICanExecuteChangedEventArgs> spCanExecuteChangedEventArgs;

    IFR_ASSERT(library::CanExecuteChangedEventArgsImpl::CreateInstance(
        state,
        state,
        spCanExecuteChangedEventArgs.GetAddressOf()));

    syncSourceInfo.push_back(std::make_pair(
        static_cast<IBaseObjectProxyInternal *>(this),
        spCanExecuteChangedEventArgs.Get()));
    return S_OK;
}

STDMETHODIMP CCommandProxy::ResyncSource(
    IInspectable *pSource,
    _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    IFR_ASSERT(_CommandModelProxyBaseType::ResyncSource(pSource, syncSourceInfo));

    UINT32 state;
    IFR(GetState(&state));
    foundation::ComPtr<ICanExecuteChangedEventArgs> spCanExecuteChangedEventArgs;

    IFR_ASSERT(library::CanExecuteChangedEventArgsImpl::CreateInstance(
        state,
        state,
        spCanExecuteChangedEventArgs.GetAddressOf()));

    syncSourceInfo.push_back(std::make_pair(
        static_cast<IBaseObjectProxyInternal *>(this),
        spCanExecuteChangedEventArgs.Get()));

    return S_OK;
}

STDMETHODIMP CCommandProxy::FireResyncEvent(foundation::IUnknown *pEventArg)
{
    foundation::ComPtr<ICanExecuteChangedEventArgs> spCanExecuteChangedEventArgs;
    IFR_ASSERT(foundation::QueryInterface(pEventArg, spCanExecuteChangedEventArgs.GetAddressOf()));
    LogSourceEvent(
        foundation::LoggingLevel::Info,
        (UINT32)proxy::BaseProxy_Category::ResyncEvent,
        spCanExecuteChangedEventArgs.Get());
    return this->FireCanExecuteChangedEvent(spCanExecuteChangedEventArgs);
}
