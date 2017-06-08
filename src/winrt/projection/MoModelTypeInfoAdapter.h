/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelTypeInfoAdapter.h
****/

#pragma once

using namespace ABI::Microsoft::PropertyModel;
//------------------------------------------------------------------------------
// Class:   CMoModelTypeInfoAdapter
//
// Purpose: Implementation of IObjectTypeInfo Interface
//
//------------------------------------------------------------------------------
class CMoModelTypeInfoAdapter :
    public foundation::AdapterBase<
        foundation::IObjectTypeInfo,
        IObjectTypeInfo,
        &__uuidof(IObjectTypeInfo)
        >
{
public:
    HRESULT STDMETHODCALLTYPE  get_TypeId( UINT32 *value)
    {
        return m_pInner->GetTypeId(value);
    }

    HRESULT STDMETHODCALLTYPE get_IIdType(GUID *value)
    {
        return m_pInner->GetType(value);
    }
                   
    HRESULT STDMETHODCALLTYPE get_Name(HSTRING *value)
    {
        return m_pInner->GetName(value);
    }
                    
    HRESULT STDMETHODCALLTYPE get_BaseType(IObjectTypeInfo **value)
    {
        foundation::ComPtr<foundation::IObjectTypeInfo> spBaseType;
        IFR(m_pInner->GetBaseType(spBaseType.GetAddressOf()));
        if(spBaseType != nullptr)
        {
            return foundation::QueryInterface(spBaseType,value);
        }
        else
        {
            *value = nullptr;
            return S_OK;
        }
    }
};
