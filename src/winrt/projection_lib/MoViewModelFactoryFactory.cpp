/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModelFactoryFactory.cpp
****/

#include "pch.h"

#include "MoViewModelFactoryFactory.h"
#include "MoBaseDelegateAdapter.h"

#include <pmod/library/library_util.h>

//------------------------------------------------------------------------------
// Class:   CMoViewModelFactoryDelegateAdapter
//
// Purpose: Implementation of IObservableObjectDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoViewModelFactoryDelegateAdapter :
    public CMoDelegeteBaseAdapter
    <
        pmod::library::IViewModelFactoryDelegate,
        pmod::IViewModelCreatedEventHandler
    >
{
protected:
     // IPropertyViewModelFactoryDelegate Implementation

};
STDMETHODIMP MoViewModelFactoryFactory::CreateInstance( 
            IInspectable *pOuter,
            IInspectable **ppInner,
            ABI::Microsoft::PropertyModel::IViewModelFactory **ppInstance)
{

    HRESULT hr;

    foundation::ComPtr<CMoViewModelFactoryDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    pmod::library::ViewModelFactoryCreateParameters param;
    param.m_p_delegate = spDelegateAdapter;

    foundation::ComPtr<IInspectable> spInner;
    hr = pmod::library::GetViewModelFactoryClassFactory()->CreateViewModelFactoryInstance(&param,pOuter,spInner.GetAddressOf());
    IFHR(hr);
    IFHR(foundation::QueryInterface(spInner,ppInstance));
    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}
