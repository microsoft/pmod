/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoPropertyBindingFactory.cpp
****/

#include "pch.h"

#include "MoPropertyBindingFactory.h"

#include <pmod/library/library_util.h>

STDMETHODIMP MoPropertyBindingFactory::CreateInstanceWithSource( 
        IObservableObject *pSource,
        IInspectable *path,
        PropertyBindingOptions options,
        IPropertyBinding **ppInstance)
{
    IFCPTR(path);

    HRESULT hr;

    foundation::ComPtr<pmod::IObservableObject> spSource;
    if(pSource != nullptr)
    {
        IFHR(foundation::QueryInterface(pSource,spSource.GetAddressOf()));
    }

    pmod::library::PropertyBindingCreateParameters param;
    param.m_p_source = spSource;
    param.m_p_path = path;
    param.m_options = (pmod::library::PropertyBindingOptions)options;

    foundation::InspectablePtr spInner;
    hr = pmod::library::GetPropertyBindingClassFactory()->
            CreatePropertyBindingInstance(&param,nullptr,spInner.GetAddressOf());
    IFHR(hr);

    IFHR(foundation::QueryInterface(spInner,ppInstance));

    return S_OK;
}
