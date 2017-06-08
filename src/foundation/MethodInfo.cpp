/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MethodInfo.cpp
****/

#include "pch.h"

#include "MethodInfo.h"
#include "TypeInfoFactory.h"

#include "pal_str.h"

using namespace foundation;

HRESULT CParameterInfo::CreateInstance(
         _In_ const MethodParamInfo *pMethodParamInfo,
         _In_ const GUID *pModelType,
         _In_ bool bDeleteMethodParamInfo,
         _Outptr_ IParameterInfo **ppParameterInfo)
{
    foundation_assert(pMethodParamInfo);

    HRESULT hr;
    foundation::ctl::ComInspectableObject<CParameterInfo> *pParameterInfo;

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CParameterInfo>::CreateInstance(&pParameterInfo));
    IFR_ASSERT(hr = pParameterInfo->Initialize(pMethodParamInfo, pModelType, bDeleteMethodParamInfo));

    *ppParameterInfo = pParameterInfo;
    return S_OK;
}

HRESULT CParameterInfo::Initialize(
        const MethodParamInfo* pMethodParamInfo,
        const GUID *pModelType,
        bool bDeleteMethodParamInfo)
{
    IFR_ASSERT(CParameterInfoBase::Initialize(pMethodParamInfo, pModelType));
    this->SetDeleteVarTypeInfo(bDeleteMethodParamInfo);
    return S_OK;
}

void CMethodInfo::AddParameterInfoInternal(IParameterInfo *pParameterInfo)
{
    _parameters.push_back(pParameterInfo);
}

HRESULT CMethodInfo::AddParameterInfo(
        _In_ const MethodParamInfo *pParameterInfo,
        _In_ const GUID *pModelType,
         _In_ bool bDeleteMethodParamInfo,
        _Out_opt_ CParameterInfo **ppParameterInfo)
{
    foundation_assert(pParameterInfo);
    foundation_assert(pParameterInfo->m_parameterName);

    foundation::ComPtr<IParameterInfo> spParameterInfo;
    IFR_ASSERT(CParameterInfo::CreateInstance(pParameterInfo,pModelType,bDeleteMethodParamInfo,spParameterInfo.GetAddressOf()));
    AddParameterInfoInternal(spParameterInfo);

    if(ppParameterInfo)
    {
        *ppParameterInfo = static_cast<CParameterInfo *>(spParameterInfo.Detach());
    }
    return S_OK;
}

HRESULT CMethodInfo::AddParameterInfo(
        _In_ HSTRING parameterName,
        _In_ foundation::PropertyType parameterType,
        _In_ GUID *pParameterModelType)
{
    MethodParamInfo *pMethodParamInfo = new MethodParamInfo(
        _pal_strdup(_pal_GetStringRawBuffer(parameterName,nullptr)),
        parameterType,
        nullptr);
    return AddParameterInfo(pMethodParamInfo,pParameterModelType,true,nullptr);
}

HRESULT CMethodInfo::AddParameterInfoArray(
    _In_ const MethodParamInfo *pParameterInfo)
{
    foundation_assert(pParameterInfo);
    // Build the Parameters Array for each defined Parameter
    while(pParameterInfo->m_parameterName)
    {
        IFR_ASSERT(AddParameterInfo(pParameterInfo,nullptr,false,nullptr));
        // Next Property Info description
        ++pParameterInfo;
    }
    return S_OK;
}

HRESULT CMethodInfo::Initialize(
    _In_ const MethodInfo *pMethodInfo,
    _In_ const GUID             *pResultTypeType)
{
    foundation_assert(pMethodInfo);

    this->_methodId = pMethodInfo->m_methodId;
    CreateStringHelper(pMethodInfo->m_methodName, _methodName.GetAddressOf());

    // TODO: we need to start using CTypeInfoFactory all together
    if (pResultTypeType != nullptr)
    {
        IFR_ASSERT(CPropertyTypeInfo::CreateInstance(pMethodInfo->m_resultTypeInfo, pResultTypeType, m_spResultTypeInfo.GetAddressOf()));
    }
    else
    {
        IFR_ASSERT(CPropertyTypeInfoFactoryCache::CreatePropertyTypeInfoInternal(pMethodInfo->m_resultTypeInfo, m_spResultTypeInfo.GetAddressOf()));
    }

    if(pMethodInfo->m_parameters)
    {
        IFR_ASSERT(AddParameterInfoArray(pMethodInfo->m_parameters));
    }
    this->m_isAsync = pMethodInfo->m_isAsync;
    return S_OK;
}

HRESULT CMethodInfo::CreateInstance(
    _In_ const MethodInfo *pMethodInfo,
    _In_ const GUID             *pResultTypeType,
    _Outptr_ IMethodInfo **ppMethodInfo)
{
    foundation_assert(pMethodInfo);

    HRESULT hr;

    CMethodInfo *pMethodInfoClass;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CMethodInfo>::CreateInstance(&pMethodInfoClass));
    IFR_ASSERT(hr = (pMethodInfoClass)->Initialize(pMethodInfo,pResultTypeType));
    *ppMethodInfo = pMethodInfoClass;

    return S_OK;
}
