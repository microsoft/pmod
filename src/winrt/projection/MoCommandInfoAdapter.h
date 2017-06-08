/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandInfoAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoCommandInfoAdapter
//
// Purpose: Implementation of ICommandInfo Interface
//
//------------------------------------------------------------------------------
class CMoCommandInfoAdapter :
    public foundation::AdapterBase
        <
        pmod::ICommandInfo,
        ICommandInfo,
        &__uuidof(ICommandInfo)
        >
{
public:                    
    HRESULT STDMETHODCALLTYPE get_ParameterTypeInfo(
        PropertyTypeInfo *value) override
    {
        IFCPTR_ASSERT(value);

        foundation::ComPtr<foundation::IPropertyTypeInfo> spItemTypeInfo;
        IFR_ASSERT(m_pInner->GetParameterTypeInfo(spItemTypeInfo.GetAddressOf()));

        spItemTypeInfo->GetPropertyType(&value->PropertyType);

        foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
        spItemTypeInfo->GetTypeInfo(spModelTypeInfo.GetAddressOf());
        if (spModelTypeInfo != nullptr)
        {
            spModelTypeInfo->GetType(&value->ObjectType);
        }
        else
        {
            value->ObjectType = GUID_NULL;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_ResultTypeInfo(
        PropertyTypeInfo *value) override
    {
        IFCPTR_ASSERT(value);

        foundation::ComPtr<foundation::IPropertyTypeInfo> spItemTypeInfo;
        IFR_ASSERT(m_pInner->GetResultTypeInfo(spItemTypeInfo.GetAddressOf()));

        spItemTypeInfo->GetPropertyType(&value->PropertyType);

        foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
        spItemTypeInfo->GetTypeInfo(spModelTypeInfo.GetAddressOf());
        if (spModelTypeInfo != nullptr)
        {
            spModelTypeInfo->GetType(&value->ObjectType);
        }
        else
        {
            value->ObjectType = GUID_NULL;
        }
        return S_OK;
    }
};
