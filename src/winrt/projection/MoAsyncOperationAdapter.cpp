/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationAdapter.cpp
****/

#include "pch.h"

#include "MoAsyncOperationAdapter.h"
#include "FoundationBase.h"

class CResultCompletedEventArgs :
    public foundation::ctl::ImplementsInspectable<IResultCompletedEventArgs,&__uuidof(IResultCompletedEventArgs)>
{
};

HRESULT STDMETHODCALLTYPE CMoAsyncOperationAdapter::get_AsyncStatus(AsyncStatus *value)
{
    IFCPTR(value);

    foundation::ResultStatus status;
    IFR(this->m_pInner->GetStatus(&status));
    *value = (AsyncStatus)status;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoAsyncOperationAdapter::get_AsyncErrorCode(HRESULT *value)
{
    return m_pInner->GetError(value);
}

HRESULT STDMETHODCALLTYPE CMoAsyncOperationAdapter::GetAsyncResults(unsigned int timeout, IInspectable **ppResults)
{
    return m_pInner->GetResult(timeout,ppResults);
}

HRESULT STDMETHODCALLTYPE CMoAsyncOperationAdapter::add_ResultCompleted( 
    IResultCompletedEventHandler *value,
    EventRegistrationToken *token)
{
    IFCPTR(value);

    foundation::ResultStatus status;
    IGNOREHR(this->m_pInner->GetStatus(&status));
    if (status != foundation::ResultStatus::Started)
    {
        foundation::ComPtr<foundation::ctl::ComInspectableObject<CResultCompletedEventArgs>> spResultCompletedEventArgs;
        IFR_ASSERT(CResultCompletedEventArgs::CreateInstance(spResultCompletedEventArgs.GetAddressOf()));

        IFR(value->Invoke(static_cast<foundation::ctl::ComInspectableBase *>(this),spResultCompletedEventArgs));
        // Note:
        // We will return instead of adding the handler since it would cause in some cases
        // (when using a proxy layer) to trigger a fire notification and so causing firing
        // twice
        return S_OK;
    }
    return this->add_Handler(value,token);
}    

HRESULT STDMETHODCALLTYPE CMoAsyncOperationAdapter::remove_ResultCompleted( 
    EventRegistrationToken token)
{
    return this->remove_Handler(token);
}
