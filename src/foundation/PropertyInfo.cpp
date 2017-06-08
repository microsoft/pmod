/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyInfo.cpp
****/

#include "pch.h"

#include "PropertyInfo.h"

#include "pal_str.h"

using namespace foundation;

HRESULT CPropertyInfo::CreateInstance(
    _In_ const PropertyInfo *pPropertyInfo,
    _In_opt_ const GUID *pModelType,
    _In_ bool isDeleteVarTypeInfo,
    _COM_Outptr_ IPropertyInfo **ppPropertyInfo)
{
    foundation_assert(pPropertyInfo);

    HRESULT hr;
    foundation::ctl::ComInspectableObject<CPropertyInfo> *pPropertyInfoInstance;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CPropertyInfo>::CreateInstance(&pPropertyInfoInstance));

    const PropertyInfo *propertyInfo;
    if(isDeleteVarTypeInfo)
    {
        propertyInfo = new PropertyInfo(
            pPropertyInfo->m_id,
            _pal_strdup(pPropertyInfo->m_name),
            pPropertyInfo->m_property_type,
            pPropertyInfo->m_flags,
            nullptr
            );
    }
    else
    {
        propertyInfo = pPropertyInfo;
    }

    IFR_ASSERT(hr = pPropertyInfoInstance->Initialize(propertyInfo, pModelType));

    if(isDeleteVarTypeInfo)
    {
        pPropertyInfoInstance->SetDeleteVarTypeInfo(true);
    }
    *ppPropertyInfo = pPropertyInfoInstance;
    return S_OK;
}

HRESULT CPropertyInfo::Initialize(const PropertyInfo* pPropertyInfo,const GUID *pModelType)
{
    IFR_ASSERT(CPropertyInfoBase::Initialize(pPropertyInfo, pModelType));
    return S_OK;
}

