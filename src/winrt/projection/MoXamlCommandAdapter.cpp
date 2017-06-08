/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoXamlCommandAdapter.cpp
****/

#include "pch.h"

#include "MoXamlCommandAdapter.h"


//------------------------------------------------------------------------------
// Class:   CAdapterCanExecuteChangedEventArgs
//
// Purpose: An implementation for the IInspectable Event Args
//
//------------------------------------------------------------------------------
class CAdapterCanExecuteChangedEventArgs:
    public foundation::ctl::ComInspectableBase
{
public:
    static HRESULT CreateInstance(
        IInspectable **ppCanExecuteChangedEventArgs)
    {
        CAdapterCanExecuteChangedEventArgs *pCanExecuteChangedEventArgs;
        IFR(foundation::ctl::ComInspectableObject<CAdapterCanExecuteChangedEventArgs>::CreateInstance(&pCanExecuteChangedEventArgs));
        *ppCanExecuteChangedEventArgs = pCanExecuteChangedEventArgs;
        return S_OK;
    }
};

IFACEMETHODIMP CMoXamlCommandAdapter::CanExecute(_In_opt_ IInspectable *parameter,_Out_ boolean *returnValue)
{
    bool bCanExecute;
    HRESULT hr = S_OK;
    IFHR(m_pInner->CanExecute(parameter,&bCanExecute));
    *returnValue = bCanExecute ? true: false;
    return hr;
}
                        
IFACEMETHODIMP CMoXamlCommandAdapter::Execute(_In_opt_ IInspectable *parameter)
{
    foundation::InspectablePtr result;
    HRESULT hr = S_OK;
    IFHR(m_pInner->Execute(parameter,result.GetAddressOf()));
    return hr;
}

IFACEMETHODIMP CMoXamlCommandAdapter::FireAdapterEvent(
	_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
	_In_ IUnknown* pEventArgs)
{
    foundation_assert(pEventArgs);

    IInspectable *pSender = static_cast<foundation::ctl::ComInspectableBase *>(this);

    foundation::ComPtr<pmod::ICanExecuteChangedEventArgs>spCanExecuteChangedEventArgs;
    if(SUCCEEDED(foundation::QueryInterface(pEventArgs,spCanExecuteChangedEventArgs.GetAddressOf())))
    {
        foundation::InspectablePtr spInspectableEvent;
        IFR(CAdapterCanExecuteChangedEventArgs::CreateInstance(spInspectableEvent.GetAddressOf()));
        this->FireEvent(pFireEventHandlerContainer, pSender,spInspectableEvent);
    }
    return S_OK;
}

