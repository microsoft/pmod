/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionInfoAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoEnumerableTypeInfoAdapter
//
// Purpose: Implementation of IObservableObjectInfo Interface
//
//------------------------------------------------------------------------------
class CMoEnumerableTypeInfoAdapter :
    public foundation::AdapterBase
        <
        foundation::IEnumerableTypeInfo,
        IEnumerableTypeInfo,
        &__uuidof(IEnumerableTypeInfo)
        >
{
public:                    
    HRESULT STDMETHODCALLTYPE get_ItemTypeInfo(
        PropertyTypeInfo *value) override
    {
        IFCPTR_ASSERT(value);

        foundation::ComPtr<foundation::IPropertyTypeInfo> spItemTypeInfo;
        IFR_ASSERT(m_pInner->GetItemTypeInfo(spItemTypeInfo.GetAddressOf()));

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
