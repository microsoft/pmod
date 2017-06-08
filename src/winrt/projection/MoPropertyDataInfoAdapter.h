/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoPropertyDataInfoAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"

using namespace ABI::Microsoft::PropertyModel;


//------------------------------------------------------------------------------
// Class:   CMoPropertyDataInfo
//
// Purpose: Implementation of IObservableObjectInfo Interface
//
//------------------------------------------------------------------------------
class CMoPropertyDataInfoAdapter :
    public foundation::AdapterBase
        <
        foundation::IObjectDispatchInfo,
        IObjectDispatchInfo,
        &__uuidof(IObjectDispatchInfo)
        >
{
public:

    HRESULT STDMETHODCALLTYPE GetProperties(
        UINT32 *length,
        PropertyInfo **value)
    {
        UNREFERENCED_PARAMETER(length);
        UNREFERENCED_PARAMETER(value);
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyById( 
        unsigned int propertyId,
        PropertyInfo *value)
    {
        IFCPTR_ASSERT(value);

        foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
        IFR(m_pInner->GetPropertyById(propertyId,spPropertyInfo.GetAddressOf()));
        return ReturnMoPropertyInfo(spPropertyInfo, value);
    }
    HRESULT STDMETHODCALLTYPE GetPropertyByName(
        HSTRING propertyName,
        PropertyInfo *value)
    {
        IFCPTR_ASSERT(value);

        foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
        IFR(m_pInner->GetPropertyByName(propertyName, spPropertyInfo.GetAddressOf()));
        return ReturnMoPropertyInfo(spPropertyInfo, value);
    }

    HRESULT STDMETHODCALLTYPE GetMethods(
        UINT32 *length,
        ABI::Microsoft::PropertyModel::IMethodInfo ***value) override
    {
        return E_NOTIMPL;
    }

private:
    HRESULT ReturnMoPropertyInfo(
		foundation::IPropertyInfo *pPropertyInfo,
        PropertyInfo *value)
    {
        IFR_ASSERT(pPropertyInfo->GetId(&value->Id));
        IFR_ASSERT(pPropertyInfo->GetFlags(&value->Flags));
        IFR_ASSERT(pPropertyInfo->GetPropertyType(&value->PropertyType));
        IFR_ASSERT(pPropertyInfo->GetName(&value->Name));

        foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
        IFR_ASSERT(pPropertyInfo->GetTypeInfo(spModelTypeInfo.GetAddressOf()));
        if (spModelTypeInfo != nullptr)
        {
            IFR_ASSERT(spModelTypeInfo->GetType(&value->ObjectType));
        }
        else
        {
            value->ObjectType = GUID_NULL;
        }
        return S_OK;
    }
};


