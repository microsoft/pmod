/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandAdapter.cpp
****/

#include "pch.h"

#include "MoCommandAdapter.h"

#include "../moproxy/MoCoreProxyFactory.h"

HRESULT STDMETHODCALLTYPE CMoCommandAdapter::get_State(unsigned int *value)
{
    return m_pInner->GetState(value);
}

// ICommand/IAsyncCommand Impl
HRESULT STDMETHODCALLTYPE CMoCommandAdapter::CanExecute( 
    IInspectable *parameter,
    boolean *value)
{
    foundation::InspectablePtr spCoreParameter;
    IFR_ASSERT(CMoCoreProxyFactory::ToProxyIf(parameter, spCoreParameter.GetAddressOf()));

    bool canExecute;
    HRESULT hr = S_OK;
    IFHR(m_pInner->CanExecute(spCoreParameter,&canExecute));
    *value = canExecute ? true:false;
    return hr;
}
                    
// ICommand Impl
HRESULT STDMETHODCALLTYPE CMoCommandAdapter::Execute(
    IInspectable *parameter,
    IInspectable **result)
{
    foundation::InspectablePtr spCoreParameter;
    IFR_ASSERT(CMoCoreProxyFactory::ToProxyIf(parameter, spCoreParameter.GetAddressOf()));

    return m_pInner->Execute(spCoreParameter,result);
}

// IAsyncCommand Impl
HRESULT STDMETHODCALLTYPE CMoCommandAdapter::Execute(
    IInspectable *parameter,
    ABI::Windows::Foundation::IAsyncOperation<IInspectable *> **ppAsyncAction)
{
    foundation::InspectablePtr spResult;
    IFR_ASSERT(this->Execute(parameter, spResult.GetAddressOf()));
    return foundation::QueryInterface(spResult, ppAsyncAction);
}

// ISimpleCommand/ISimpleAsyncCommand Impl
HRESULT STDMETHODCALLTYPE CMoCommandAdapter::CanExecute(boolean *value)
{
    return CanExecute(nullptr, value);
}
                        
// ISimpleCommand Impl
HRESULT STDMETHODCALLTYPE CMoCommandAdapter::Execute(void) 
{
    foundation::InspectablePtr spResult;
    return Execute(nullptr, spResult.GetAddressOf());
}

// ISimpleAsyncCommand Impl
HRESULT STDMETHODCALLTYPE CMoCommandAdapter::Execute(
    ABI::Windows::Foundation::IAsyncOperation<IInspectable *> **ppAsyncAction)
{
    foundation::InspectablePtr spResult;
    IFR(this->Execute(nullptr, spResult.GetAddressOf()));
    return foundation::QueryInterface(spResult, ppAsyncAction);
}

