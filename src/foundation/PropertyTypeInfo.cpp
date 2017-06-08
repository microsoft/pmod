/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyTypeInfo.cpp
****/

#include "pch.h"

#include "PropertyTypeInfo.h"
#include "FoundationUtilHelper.h"

HRESULT CPropertyTypeInfo::CreateInstance(
    _In_ const foundation::PropertyTypeInfo *pPropertyTypeInfo,
    _In_opt_ const GUID *pModelType,
    _Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfoInstance)
{
    foundation_assert(pPropertyTypeInfo);
    foundation_assert(ppPropertyTypeInfoInstance);

    HRESULT hr;
    CPropertyTypeInfo *pPropertyTypeInfoInstance = nullptr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CPropertyTypeInfo>::CreateInstance(&pPropertyTypeInfoInstance));
    IFR_ASSERT(hr = pPropertyTypeInfoInstance->Initialize(pPropertyTypeInfo, pModelType));

    *ppPropertyTypeInfoInstance = pPropertyTypeInfoInstance;
    return S_OK;
}

IID GetTypeInfoTypeInternal(foundation::IPropertyTypeInfo *pPropertyTypeInfo)
{
    foundation::ComPtr<foundation::IObjectTypeInfo> spObjectTypeInfo;
    pPropertyTypeInfo->GetTypeInfo(spObjectTypeInfo.GetAddressOf());
    if (spObjectTypeInfo == nullptr)
    {
        return foundation_GUID_NULL;
    }
    return ::GetType(spObjectTypeInfo);
}
