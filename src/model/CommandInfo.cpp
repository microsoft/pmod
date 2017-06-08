/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandInfo.cpp
****/

#include "pch.h"

#include "CommandInfo.h"
#include "PropertyTypeInfo.h"
#include "CommandClassInfoCache.h"
#include "TypeInfoFactory.h"

using namespace pmod;

LPCSTR_t _CommandModelTypeInfo_ParameterType_Key = U("parameterType");
LPCSTR_t _CommandModelTypeInfo_ResultType_Key = U("resultType");

HRESULT CCommandModelTypeInfo::CreateInstance(
    _In_ const CommandTypeInfo *pCommandModelInfo,
    _Outptr_ CCommandModelTypeInfo **ppCommandModelInfo)
{
    foundation_assert(pCommandModelInfo);

    HRESULT hr;

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CCommandModelTypeInfo>::CreateInstance(ppCommandModelInfo));

    // Now Cache this new created Property Model Info
    CCommandModelClassInfoCache::AddToCache(pCommandModelInfo->get_iid_type(), *ppCommandModelInfo);

    // now we can initialize
    IFR_ASSERT(hr = (*ppCommandModelInfo)->_Initialize(pCommandModelInfo));

    return S_OK;
}

HRESULT CCommandModelTypeInfo::create_runtime_instance(
    _In_ const CommandTypeInfo *pCommandModelInfo,
    _In_ foundation::IPropertyTypeInfo *pParameterTypeInfo,
    _In_ foundation::IPropertyTypeInfo *pResultTypeInfo,
    _Outptr_ CCommandModelTypeInfo **ppCCommandModelInfo)
{
    foundation_assert(pCommandModelInfo);
    foundation_assert(pParameterTypeInfo);
    foundation_assert(pResultTypeInfo);
    foundation_assert(ppCCommandModelInfo);

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CCommandModelTypeInfo>::CreateInstance(ppCCommandModelInfo));
    IFR_ASSERT((*ppCCommandModelInfo)->_Initialize(pCommandModelInfo));
    (*ppCCommandModelInfo)->m_spParameterTypeInfo = pParameterTypeInfo;
    (*ppCCommandModelInfo)->m_spResultTypeInfo = pResultTypeInfo;
    (*ppCCommandModelInfo)->SetDeleteTypeInfo(true);

    return S_OK;
}


HRESULT CCommandModelTypeInfo::_Initialize(
    const CommandTypeInfo *pCommandModelInfo)
{
    foundation_assert(pCommandModelInfo);   

    IFR_ASSERT(_InitializeTypeInfo(pCommandModelInfo));
    if (pCommandModelInfo->m_p_parameter_type_info != nullptr)
    {
        IFR_ASSERT(CPropertyTypeInfoFactoryCache::CreatePropertyTypeInfoInternal(pCommandModelInfo->m_p_parameter_type_info, m_spParameterTypeInfo.GetAddressOf()));
    }
    if (pCommandModelInfo->m_p_result_type_info != nullptr)
    {
        IFR_ASSERT(CPropertyTypeInfoFactoryCache::CreatePropertyTypeInfoInternal(pCommandModelInfo->m_p_result_type_info, m_spResultTypeInfo.GetAddressOf()));
    }
    m_isAsync = pCommandModelInfo->m_isAsync;
    return S_OK;
}

STDMETHODIMP CCommandModelTypeInfo::GetParameterTypeInfo(foundation::IPropertyTypeInfo **ppVarTypeInfo)
{
    return m_spParameterTypeInfo.CopyTo(ppVarTypeInfo);
}

STDMETHODIMP CCommandModelTypeInfo::GetResultTypeInfo(foundation::IPropertyTypeInfo **ppVarTypeInfo)
{
    return m_spResultTypeInfo.CopyTo(ppVarTypeInfo);
}


HRESULT CCommandModelTypeInfo::_DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CCommandModelTypeInfo_BaseType::_DeserializeTypeInfo(type_info_ptr));
    IFR_ASSERT(CTypeInfoFactory::_DeserializePropertyTypeInfo(
        type_info_ptr,
        _CommandModelTypeInfo_ParameterType_Key,
        m_spParameterTypeInfo.GetAddressOf()));
    IFR_ASSERT(CTypeInfoFactory::_DeserializePropertyTypeInfo(
        type_info_ptr,
        _CommandModelTypeInfo_ResultType_Key,
        m_spResultTypeInfo.GetAddressOf()));
    return S_OK;
}

HRESULT CCommandModelTypeInfo::_SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CCommandModelTypeInfo_BaseType::_SerializeTypeInfo(type_info_ptr));
    IFR_ASSERT(CTypeInfoFactory::_SerializePropertyTypeInfo(
        type_info_ptr,
        _CommandModelTypeInfo_ParameterType_Key,
        m_spParameterTypeInfo
        ));
    IFR_ASSERT(CTypeInfoFactory::_SerializePropertyTypeInfo(
        type_info_ptr,
        _CommandModelTypeInfo_ResultType_Key,
        m_spResultTypeInfo
        ));
    return S_OK;
}
