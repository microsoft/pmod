/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoBindingBaseAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoBindingBaseAdapter
//
// Purpose: Implementation of IBindingBase Interface
//
//------------------------------------------------------------------------------
class CMoBindingBaseAdapter :
    public CMoBaseAdapter
        <
        pmod::IBindingBase,
        IBindingBase,
        IBindingValueChangedEventHandler,
        IBindingValueChangedEventArgs
        >
{
public:
    // Interface IBindingBase
    HRESULT STDMETHODCALLTYPE get_Value( 
        IInspectable **value) override
    {
        return m_pInner->GetValue(value);
    }
                                                
    HRESULT STDMETHODCALLTYPE get_Converter( 
        IBindingConverter **value) override
    {
        return E_NOTIMPL;
    }
                        
    HRESULT STDMETHODCALLTYPE put_Converter( 
        IBindingConverter *value) override
    {
        foundation::ComPtr<pmod::IBindingConverter> spConverter;
        if(value)
        {
            IFR(foundation::QueryInterface(value,spConverter.ReleaseAndGetAddressOf()));
        }
        IFR(m_pInner->SetConverter(spConverter));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE add_BindingValueChanged( 
        IBindingValueChangedEventHandler *value,
        EventRegistrationToken *token) override
    {
        return this->add_Handler(value,token);
    }
                        
    HRESULT STDMETHODCALLTYPE remove_BindingValueChanged( 
        EventRegistrationToken token) override
    {
        return this->remove_Handler(token);
    }
};
//------------------------------------------------------------------------------
// Class:   CMoBindingValueChangedEventArgsAdapter
//
// Purpose: Implementation of IBindingValueChangedEventArgs Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoBindingValueChangedEventArgsAdapter :
    public foundation::AdapterBase
    <
        pmod::IBindingValueChangedEventArgs,
        IBindingValueChangedEventArgs,
        &__uuidof(IBindingValueChangedEventArgs)
    >
{
public:
};

