/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEnumTypeInfoAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

// Public Headers
#include <foundation/library/adapter_base.h>
#include <foundation/com_ptr.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoEnumTypeInfoAdapter
//
// Purpose: Implementation of IEnumTypeInfo Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoEnumTypeInfoAdapter :
    public foundation::AdapterBase
        <
        foundation::IEnumTypeInfo,
        IEnumTypeInfo,
        &__uuidof(IEnumTypeInfo)
        >
{
protected:
    HRESULT STDMETHODCALLTYPE get_IsFlags(boolean *value) override
    {
        bool _isFlags;
        IFR_ASSERT(this->m_pInner->GetIsFlags(&_isFlags));
        *value = _isFlags;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_Names( 
        UINT32 *length,
        HSTRING **values) override
    {
        return this->m_pInner->GetNames(length,values);
    }

    HRESULT STDMETHODCALLTYPE get_Values( 
        UINT32 *length,
        UINT32 **values) override
    {
        return this->m_pInner->GetValues(length,values);
    }

    HRESULT STDMETHODCALLTYPE get_DefaultValue( 
        UINT32 *value)
    {
        return this->m_pInner->GetDefaultValue(value);
    }

    HRESULT STDMETHODCALLTYPE GetStringValue( 
       UINT32 enumValue,
       HSTRING *value)
    {
        return this->m_pInner->GetStringValue(enumValue,value);
    }

    HRESULT STDMETHODCALLTYPE GetEnumValue( 
        UINT32 enumValue,
        IEnumValue **value)
    {
        foundation::ComPtr<foundation::IEnumValue> spEnumValue;
        IFR_ASSERT(this->m_pInner->GetEnumValue(enumValue,spEnumValue.GetAddressOf()));
        return foundation::QueryInterface(spEnumValue,value);
    }
    HRESULT STDMETHODCALLTYPE GetEnumValueByName( 
        HSTRING enumValueName,
        IEnumValue **value)
    {
        foundation::ComPtr<foundation::IEnumValue> spEnumValue;
        IFR_ASSERT(this->m_pInner->GetEnumValueByName(enumValueName,spEnumValue.GetAddressOf()));
        return foundation::QueryInterface(spEnumValue,value);
    }

};
