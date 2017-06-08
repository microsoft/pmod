/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationTypeInfo.cpp
****/

#include "pch.h"

#include "AsyncOperationTypeInfo.h"
#include "AsyncOperationClassInfoCache.h"
#include "PropertyTypeInfo.h"
#include "TypeInfoFactory.h"

using namespace foundation;

LPCSTR_t _AsyncOperationTypeInfo_ResultType_Key = U("resultType");

HRESULT CAsyncOperationTypeInfo::CreateInstance(
        _In_ const AsyncOperationTypeInfo *pAsyncOperationTypeInfo,
        _Outptr_ CAsyncOperationTypeInfo **ppAsyncOperationTypeInfo)
{
    foundation_assert(pAsyncOperationTypeInfo);

    HRESULT hr;

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CAsyncOperationTypeInfo>::CreateInstance(ppAsyncOperationTypeInfo));

    // Now Cache this new created Async Operation Model Info
    CAsyncOperationClassInfoCache::AddToCache(pAsyncOperationTypeInfo->get_iid_type(), *ppAsyncOperationTypeInfo);

    // now we can initialize
    IFR_ASSERT(hr = (*ppAsyncOperationTypeInfo)->_Initialize(pAsyncOperationTypeInfo));

    return S_OK;
}

HRESULT CAsyncOperationTypeInfo::create_runtime_instance(
    _In_ const AsyncOperationTypeInfo *pAsyncOperationTypeInfo,
    _In_ IPropertyTypeInfo *pResultTypeInfo,
    _Outptr_ CAsyncOperationTypeInfo **ppAsyncOperationTypeInfo)
{
    foundation_assert(pAsyncOperationTypeInfo);
    foundation_assert(pResultTypeInfo);
    foundation_assert(ppAsyncOperationTypeInfo);

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CAsyncOperationTypeInfo>::CreateInstance(ppAsyncOperationTypeInfo));
    IFR_ASSERT((*ppAsyncOperationTypeInfo)->_Initialize(pAsyncOperationTypeInfo));
    (*ppAsyncOperationTypeInfo)->m_spResultTypeInfo = pResultTypeInfo;
    (*ppAsyncOperationTypeInfo)->SetDeleteTypeInfo(true);

    return S_OK;
}

HRESULT CAsyncOperationTypeInfo::_Initialize(
        const AsyncOperationTypeInfo *pAsyncOperationTypeInfo)
{
    foundation_assert(pAsyncOperationTypeInfo);   

    IFR_ASSERT(_InitializeTypeInfo(pAsyncOperationTypeInfo));
    if (pAsyncOperationTypeInfo->m_p_result_type_info != nullptr)
    {
        IFR_ASSERT(CPropertyTypeInfoFactoryCache::CreatePropertyTypeInfoInternal(pAsyncOperationTypeInfo->m_p_result_type_info, m_spResultTypeInfo.GetAddressOf()));
    }

    // Add Result type info to the Cache Info Cache
    CAsyncOperationClassInfoCache::AddToResultTypeInfoCache(this);
    return S_OK;
}

STDMETHODIMP CAsyncOperationTypeInfo::GetResultTypeInfo(_COM_Outptr_ IPropertyTypeInfo **ppVarTypeInfo)
{
    IFCPTR_ASSERT(m_spResultTypeInfo);
    return m_spResultTypeInfo.CopyTo(ppVarTypeInfo);
}


HRESULT CAsyncOperationTypeInfo::_DeserializeTypeInfo(const DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CAsyncOperationTypeInfo_BaseType::_DeserializeTypeInfo(type_info_ptr));
    IFR_ASSERT(CTypeInfoFactory::_DeserializePropertyTypeInfo(
        type_info_ptr,
        _AsyncOperationTypeInfo_ResultType_Key,
        m_spResultTypeInfo.GetAddressOf()));
    return S_OK;
}

HRESULT CAsyncOperationTypeInfo::_SerializeTypeInfo(DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CAsyncOperationTypeInfo_BaseType::_SerializeTypeInfo(type_info_ptr));
    IFR_ASSERT(CTypeInfoFactory::_SerializePropertyTypeInfo(
        type_info_ptr,
        _AsyncOperationTypeInfo_ResultType_Key,
        m_spResultTypeInfo
        ));
    return S_OK;
}
