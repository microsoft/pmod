/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationProxy.h
****/

#pragma once

#include <foundation/assert.h>

#include "Microsoft.PropertyModel.h"
#include "MoAdapterProxyInterface.h"
#include "MoModelProxy.h"
#include <pmod/library/model_library.h>
#include "MoEventSourceProxy.h"
#include "MoCoreProxyFactory.h"
#include "MoObservableObjectProxy.h"

//------------------------------------------------------------------------------
// Class:   CMoCompletedEventArgsProxy
//
// Purpose: Implementation of pmod::ICompletedEventArgs
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoCompletedEventArgsProxy :
    public TAdapterProxyInterface
    <
        foundation::ICompletedEventArgs,
        ABI::Microsoft::PropertyModel::IResultCompletedEventArgs
    >
{
protected:
};

//------------------------------------------------------------------------------
// Class:   CCompletedEventHandlerProxy
//
// Purpose: Implementation of Modern IResultCompletedEventHandler
//          Interface using the Core Interface callback
//------------------------------------------------------------------------------
class CCompletedEventHandlerProxy :
    public TFreeThreadedAdapterProxyInterface
    <
        ABI::Microsoft::PropertyModel::IResultCompletedEventHandler,
        foundation::ICompletedEventHandler
    >
{
protected:
    HRESULT STDMETHODCALLTYPE Invoke(
        IInspectable *sender,
        ABI::Microsoft::PropertyModel::IResultCompletedEventArgs *e) override
    {
        foundation::ComPtr<foundation::ICompletedEventArgs> spCompletedEventArgs;
        HRESULT hr = CMoCompletedEventArgsProxy::CreateInstance<CMoCompletedEventArgsProxy>
           (e,spCompletedEventArgs.GetAddressOf());
        IFR_ASSERT(hr);
        return this->m_pSourceInterface->Invoke(sender,spCompletedEventArgs);
    }
};

typedef 
    TMoEventSourceProxy<
        foundation::ICompletedEventSource,
        foundation::ICompletedEventHandler,
        ABI::Microsoft::PropertyModel::IResultCompletedEventHandler,
        ABI::Microsoft::PropertyModel::IAsyncOperation,
        &ABI::Microsoft::PropertyModel::IAsyncOperation::add_ResultCompleted,
        &ABI::Microsoft::PropertyModel::IAsyncOperation::remove_ResultCompleted,
        CCompletedEventHandlerProxy>
    CMoCompletedEventSourceProxy;

//------------------------------------------------------------------------------
// Class:   CMoAsyncOperationProxy
//
// Purpose: Implementation of foundation::IAsyncOperation
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoAsyncOperationProxy :
    public TMoObjectProxy
    <
        foundation::IAsyncOperation,
        ABI::Microsoft::PropertyModel::IAsyncOperation
    >
{
protected:
    STDMETHOD (GetResult)(UINT32 timeout,IInspectable **ppResult)
    {
        IFCPTR_ASSERT(ppResult);

        foundation::InspectablePtr spResults;
        IFR(this->m_pSourceInterface->GetAsyncResults(timeout,spResults.GetAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spResults, ppResult));
        return S_OK;
    }

    STDMETHOD (GetError)(HRESULT* hResult)
    {
        return this->m_pSourceInterface->get_AsyncErrorCode(hResult);
    }

    STDMETHOD(GetStatus)(foundation::ResultStatus* status)
    {
        IFCPTR_ASSERT(status);

        AsyncStatus asyncStatus;
        this->m_pSourceInterface->get_AsyncStatus(&asyncStatus);
        *status = (foundation::ResultStatus)asyncStatus;
        return S_OK;
    }

    STDMETHOD (GetIsCancelable)(bool* pVal)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(Cancel)() 
    {
        foundation::ComPtr<IAsyncInfo> spMoAsyncInfo;
        IFR_ASSERT(foundation::QueryInterface(
            this->m_pSourceInterface, 
            spMoAsyncInfo.GetAddressOf()));
        return spMoAsyncInfo->Cancel();
    }

    STDMETHOD(Wait)(UINT32 timeout, foundation::ResultStatus* status)
    {
        IFCPTR_ASSERT(status);
        // implement by using GetResult which will block and Wait
        foundation::ComPtr<IInspectable> pResult;
        HRESULT hr = GetResult(timeout,pResult.GetAddressOf());
        IGNOREHR(GetStatus(status));
        IFHR(hr);
        return S_OK;
    }

    STDMETHOD(GetCompletedEventSource)(foundation::ICompletedEventSource** ppEventSource)
    {
        if(_spCompletedEventSource == nullptr)
        {
            HRESULT hr =  CMoCompletedEventSourceProxy::CreateInstance<CMoCompletedEventSourceProxy>
                (this->m_pSourceInterface,_spCompletedEventSource.GetAddressOf());
        }
        return _spCompletedEventSource.CopyTo(ppEventSource);
    }

    STDMETHOD(GetProgressEventSource)(foundation::IProgressEventSource** ppEventSource)
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }
private:
    foundation::ComPtr<foundation::ICompletedEventSource>      _spCompletedEventSource;
};
