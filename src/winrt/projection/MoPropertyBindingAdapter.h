/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoPropertyBindingAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

// Public Headers
#include <foundation/library/adapter_base.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoPropertyBindingAdapter
//
// Purpose: Implementation of IPropertyBinding Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoPropertyBindingAdapter :
    public foundation::AdapterBase
    <
        pmod::IPropertyBinding,
        IPropertyBinding,
        &__uuidof(IPropertyBinding)
    >
{
public:
    // Interface IPropertyBinding                        
    HRESULT STDMETHODCALLTYPE get_Path( 
        IInspectable **value) override
    {
        return m_pInner->GetPath(value);
    }
                        
    HRESULT STDMETHODCALLTYPE put_Path( 
        IInspectable *value) override
    {
        return m_pInner->SetPath(value);
    }
};

