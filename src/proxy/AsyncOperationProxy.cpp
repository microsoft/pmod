/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationProxy.cpp
****/
#include "pch.h"
#include "AsyncOperationProxy.h"
#include <foundation/library/event_args_base.h>

using namespace pmod;
typedef foundation::library::_EventArgsBase<foundation::ICompletedEventArgs> CCompletedEventArgs;

CAsyncOperationProxy::CAsyncOperationProxy()
{
}

HRESULT CAsyncOperationProxy::on_invoke_internal(IUnknown* pSender,foundation::ICompletedEventArgs* pArgs)
{   
    UNREFERENCED_PARAMETER(pSender);
    UNREFERENCED_PARAMETER(pArgs);

    return FireCompletedEvent(pArgs);
}

STDMETHODIMP CAsyncOperationProxy::GetResult(UINT32 timeout, IInspectable **ppResult)
{
    IFR(VerifyModelSource());

    foundation::InspectablePtr result;
    IFR(m_spObjectSource->GetResult(timeout,result.GetAddressOf()));
    if(result != nullptr)
    {
        IFR(ToProxyValue(result));
    }
    return result.CopyTo(ppResult);
}

STDMETHODIMP CAsyncOperationProxy::GetError(HRESULT* hResult)
{
    IFR(VerifyModelSource());
    return m_spObjectSource->GetError(hResult);
}

STDMETHODIMP CAsyncOperationProxy::GetStatus(foundation::ResultStatus *status)
{
    IFR(VerifyModelSource());
    return m_spObjectSource->GetStatus(status);
}

HRESULT CAsyncOperationProxy::PerformFireCompletedEventArgs(
	_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
	foundation::ICompletedEventArgs *pEventArgs)
{
    return FireEventInternal(pEventHandlerContainer,pEventArgs);
}

HRESULT CAsyncOperationProxy::FireCompletedEvent(foundation::ICompletedEventArgs *pEventArgs)
{
    IFR(FireEventSupportInternal(
        &CAsyncOperationProxy::PerformFireCompletedEventArgs,
        pEventArgs));
    return S_OK;
}

HRESULT CAsyncOperationProxy::QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
{
    HRESULT hr = S_OK;
    if (iid == foundation::IAsyncOperationBase::GetIID())
    {
        *ppInterface = static_cast<foundation::IAsyncOperationBase *>(this);
    }
    else
    {
        hr = _AsyncOperationModelProxyBaseType::QueryInterfaceImpl(iid, ppInterface);
    }
    return hr;
}

HRESULT CAsyncOperationProxy::DisconnectFromSource(foundation::IInspectable **ppSourceInfo)
{
    foundation::ResultStatus status;
    m_spObjectSource->GetStatus(&status);
    if (status == foundation::ResultStatus::Started)
    {
        return foundation::pv_util::CreateUInt32Value((UINT32)status, ppSourceInfo);
    }
    return S_OK;
}

HRESULT CAsyncOperationProxy::ConnectToSource(
    foundation::IInspectable *pSourceInfo, 
    _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    foundation::ResultStatus status;
    m_spObjectSource->GetStatus(&status);
    if (status != foundation::ResultStatus::Started)
    {
        foundation::ComPtr<foundation::ICompletedEventArgs> spCompletedEventArgs;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CCompletedEventArgs>::CreateInstance(spCompletedEventArgs.GetAddressOf()));

        syncSourceInfo.push_back(std::make_pair(
            static_cast<IBaseObjectProxyInternal *>(this),
            spCompletedEventArgs.Get()));
    }
    return S_OK;
}

STDMETHODIMP CAsyncOperationProxy::ResyncSource(
    IInspectable *pSource,
    _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    IFR_ASSERT(_AsyncOperationModelProxyBaseType::ResyncSource(pSource, syncSourceInfo));

    foundation_assert(pSource == nullptr);

    // we will set the completion
    foundation::ComPtr<foundation::ICompletedEventArgs> spCompletedEventArgs;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CCompletedEventArgs>::CreateInstance(spCompletedEventArgs.GetAddressOf()));

    syncSourceInfo.push_back(std::make_pair(
        static_cast<IBaseObjectProxyInternal *>(this),
        spCompletedEventArgs.Get()));

    return S_OK;
}

STDMETHODIMP CAsyncOperationProxy::FireResyncEvent(foundation::IUnknown *pEventArg)
{
    foundation::ComPtr<foundation::ICompletedEventArgs> spCompletedEventArgs;
    IFR_ASSERT(foundation::QueryInterface(pEventArg, spCompletedEventArgs.GetAddressOf()));
    LogSourceEvent(
        foundation::LoggingLevel::Info,
        (UINT32)proxy::BaseProxy_Category::ResyncEvent,
        spCompletedEventArgs.Get());
    return this->FireCompletedEvent(spCompletedEventArgs);
}

foundation::ILogCategory *CAsyncOperationProxy::GetLogCategory()
{
    return this->m_pProxyObjectFactory->GetAsyncOperationdProxyCategory();
}
