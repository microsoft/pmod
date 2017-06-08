/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumerableTypeInfo.cpp
****/

#include "pch.h"

#include "EnumerableTypeInfo.h"
#include "PropertyTypeInfo.h"
#include "EnumerableClassInfoCache.h"
#include "TypeInfoFactory.h"

using namespace foundation;

LPCSTR_t _EnumerableTypeInfo_ItemType_Key = U("itemType");

HRESULT CEnumerableTypeInfo::CreateInstance(
		_In_ const EnumerableTypeInfo *pEnumerableTypeInfo,
		_Outptr_ CEnumerableTypeInfo **ppEnumerableTypeInfo)
{
    foundation_assert(pEnumerableTypeInfo);

    HRESULT hr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CEnumerableTypeInfo>::CreateInstance(ppEnumerableTypeInfo));

    // Cache this
    CEnumerableClassInfoCache::AddToCache(pEnumerableTypeInfo->get_iid_type(), *ppEnumerableTypeInfo);

    // now we can initialize
    IFR_ASSERT(hr = (*ppEnumerableTypeInfo)->_Initialize(pEnumerableTypeInfo));
    return S_OK;
}

HRESULT CEnumerableTypeInfo::create_runtime_instance(
    _In_ const EnumerableTypeInfo *pEnumerableTypeInfo,
    _In_ foundation::IPropertyTypeInfo *pItemTypeInfo,
    _Outptr_ CEnumerableTypeInfo **ppEnumerableTypeInfo)
{
    foundation_assert(pEnumerableTypeInfo);
    foundation_assert(pItemTypeInfo);
    foundation_assert(ppEnumerableTypeInfo);

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CEnumerableTypeInfo>::CreateInstance(ppEnumerableTypeInfo));
    IFR_ASSERT((*ppEnumerableTypeInfo)->_Initialize(pEnumerableTypeInfo));
    (*ppEnumerableTypeInfo)->m_spItemTypeInfo = pItemTypeInfo;
    (*ppEnumerableTypeInfo)->SetDeleteTypeInfo(true);

    return S_OK;
}

HRESULT CEnumerableTypeInfo::Initialize(
    const EnumerableTypeInfo *pEnumerableTypeInfo,
    _In_ IEnumerableTypeInfo *pBaseType)
{
    foundation_assert(pEnumerableTypeInfo);   

    IFR_ASSERT(_InitializeTypeInfo(pEnumerableTypeInfo));
    this->SetBaseType(pBaseType);
    if (pEnumerableTypeInfo->m_pItemTypeInfo != nullptr)
    {
        IFR_ASSERT(CPropertyTypeInfoFactoryCache::CreatePropertyTypeInfoInternal(pEnumerableTypeInfo->m_pItemTypeInfo, m_spItemTypeInfo.GetAddressOf()));
    }    
    return S_OK;
}

HRESULT CEnumerableTypeInfo::_Initialize(
    const EnumerableTypeInfo *pEnumerableTypeInfo)
{
    // get Base Type from 'Core'
    IEnumerableTypeInfo *pBaseType = nullptr;
    //IFR_ASSERT(CCoreModelClassInfoCache::GetTypeModelInfoInternalAs(&_ObservableCollectionType, &pBaseType));

    IFR_ASSERT(Initialize(pEnumerableTypeInfo, pBaseType));
    
    return S_OK;
}

STDMETHODIMP CEnumerableTypeInfo::GetItemTypeInfo(foundation::IPropertyTypeInfo **ppVarTypeInfo)
{
    return m_spItemTypeInfo.CopyTo(ppVarTypeInfo);
}

HRESULT CEnumerableTypeInfo::_DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CEnumerableTypeInfo_BaseType::_DeserializeTypeInfo(type_info_ptr));
    IFR_ASSERT(CTypeInfoFactory::_DeserializePropertyTypeInfo(
        type_info_ptr,
        _EnumerableTypeInfo_ItemType_Key,
        m_spItemTypeInfo.GetAddressOf()));
    return S_OK;
}

HRESULT CEnumerableTypeInfo::_SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CEnumerableTypeInfo_BaseType::_SerializeTypeInfo(type_info_ptr));
    IFR_ASSERT(CTypeInfoFactory::_SerializePropertyTypeInfo(
        type_info_ptr,
        _EnumerableTypeInfo_ItemType_Key,
        m_spItemTypeInfo
        ));
    return S_OK;
}
