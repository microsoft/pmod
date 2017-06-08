/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyInfoProxy.cpp
****/

#include "pch.h"

#include "PropertyInfoProxy.h"

HRESULT CPropertyInfoProxy::CreateInstance(
    _In_ foundation::IPropertyInfo *pPropertyInfo,
    _In_ foundation::IObjectTypeInfo *pModelTypeInfo,
    _Outptr_ foundation::IPropertyInfo **ppPropertyInfo)
{
    foundation_assert(pPropertyInfo);

    HRESULT hr;
    foundation::ctl::ComInspectableObject<CPropertyInfoProxy> *pPropertyInfoInstance;

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CPropertyInfoProxy>::CreateInstance(&pPropertyInfoInstance));
    IFR_ASSERT(hr = pPropertyInfoInstance->Initialize(pPropertyInfo, pModelTypeInfo));

    *ppPropertyInfo = pPropertyInfoInstance;
    return S_OK;
}

HRESULT CPropertyInfoProxy::Initialize(
	foundation::IPropertyInfo* pPropertyInfo,
    _In_ foundation::IObjectTypeInfo *pTypeInfo)
{
    IFR_ASSERT(pPropertyInfo->GetId(&m_Id));
    IFR_ASSERT(pPropertyInfo->GetName(m_Name.GetAddressOf()));
    IFR_ASSERT(pPropertyInfo->GetPropertyType(&m_PropertyType));
    IFR_ASSERT(pPropertyInfo->GetFlags(&m_Flags));


    this->SetTypeInfo(pTypeInfo);
    return S_OK;
}
