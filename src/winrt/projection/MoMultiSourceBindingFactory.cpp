/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceBindingFactory.cpp
****/

#include "pch.h"

#include "MoMultiSourceBindingFactory.h"

#include <pmod/library/factory_util.h>

STDMETHODIMP MoMultiSourceBindingFactory::CreateInstanceWithSource( 
        MultiSourceBindingOptions options,
        IMultiSourceBinding **ppInstance)
{
    HRESULT hr;

    pmod::library::MultiSourcePropertyBindingCreateParameters param;
    param.m_options = (pmod::library::MultiSourceBindingOptions)options;
    param.m_p_delegate = nullptr;

    foundation::InspectablePtr spInner;
    hr = pmod::library::GetMultiSourceBindingClassFactory()->
        CreateMultiSourceBindingInstance(&param,nullptr,spInner.GetAddressOf());
    IFHR(hr);
    IFHR(foundation::QueryInterface(spInner,ppInstance));

    return S_OK;
}

