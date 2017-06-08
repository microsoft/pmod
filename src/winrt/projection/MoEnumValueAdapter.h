/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEnumValueAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

// Public Headers
#include <foundation/library/adapter_base.h>
#include <foundation/com_ptr.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoEnumValueAdapter
//
// Purpose: Implementation of IEnumValue Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoEnumValueAdapter :
    public foundation::AdapterBase
        <
        foundation::IEnumValue,
        IEnumValue,
        &__uuidof(IEnumValue)
        >
{
protected:
    HRESULT STDMETHODCALLTYPE get_Value( 
        UINT32 *value) override
    {
        return this->m_pInner->GetValue(value);
    }

    HRESULT STDMETHODCALLTYPE get_TypeInfo( 
        IEnumTypeInfo **value) override
    {
        foundation::ComPtr<foundation::IObjectTypeInfo> spEnumTypeInfo;
        IFR_ASSERT(this->m_pInner->GetTypeInfo(spEnumTypeInfo.GetAddressOf()));
        return foundation::QueryInterface(spEnumTypeInfo,value);
    }

    HRESULT STDMETHODCALLTYPE ToString( 
        HSTRING *value)  override
    {
        return this->m_pInner->ToString(value);
    }
};
