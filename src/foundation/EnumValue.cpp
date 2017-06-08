/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumValue.cpp
****/

#include "pch.h"

#include "EnumValue.h"
#include "NamespaceStatics.h"

#include <pal/pal_thread.h>

using namespace foundation;

HRESULT CEnumValue::CreateInstance(
        _In_ IEnumTypeInfo *pEnumTypeInfo,
        _In_ UINT32 enumValue,
        _Outptr_ CEnumValue **ppEnumValueClass)
{
    foundation_assert(pEnumTypeInfo);

    HRESULT hr;
    CEnumValue *pEnumValueClass = nullptr;

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CEnumValue>::CreateInstance(&pEnumValueClass));
    IFR_ASSERT(hr = pEnumValueClass->Initialize(pEnumTypeInfo, enumValue));

    *ppEnumValueClass = pEnumValueClass;
    return S_OK;
}


STDMETHODIMP CEnumValue::get_Type( 
        foundation::PropertyType *value)
{
    *value = PropertyType_UInt32;
    return S_OK;
}

STDMETHODIMP CEnumValue::GetValue(UINT32 *value)
{
    IFCPTR_ASSERT(value);
    *value = _enumValue;
    return S_OK;
}

STDMETHODIMP CEnumValue::ToString(HSTRING *value)
{
    foundation_assert(m_EnumTypeInfo);
    return m_EnumTypeInfo->GetStringValue(_enumValue,value);
}

STDMETHODIMP CEnumValue::GetTypeInfo(IObjectTypeInfo** value)
{
    return m_EnumTypeInfo.CopyTo(reinterpret_cast<IEnumTypeInfo **>(value));
}

STDMETHODIMP CEnumValue::GetUniqueId(UINT32 *pValue)
{
    IFCPTR_ASSERT(pValue);

	UINT32 uniqueId = _enumValue;
    if (_enumValue < 0xffff)
    {
        UINT32 enumTypeId;
        m_EnumTypeInfo->GetTypeId(&enumTypeId);
        uniqueId += ((ULONG)enumTypeId << 16);
    }
    *pValue = uniqueId;
    return S_OK;
}

STDMETHODIMP CEnumValue::GetProcessId(UINT32 *processId)
{
    *processId = (UINT32)_pal_GetCurrentProcessId();
    return S_OK;
}

HRESULT CEnumValue::Initialize(
        IEnumTypeInfo *pEnumTypeInfo,
        UINT32 enumValue)
{
    foundation_assert(pEnumTypeInfo);
    m_EnumTypeInfo = pEnumTypeInfo;
    _enumValue = enumValue;
    return S_OK;
}


HRESULT CEnumValue::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    if(iid == foundation_IID_IPropertyValue)
    {
        *ppInterface = static_cast<foundation::IPropertyValue *>(this);
    }
    else if(iid == IEnumValue::GetIID())
    {
        *ppInterface = static_cast<IEnumValue *>(this);
    }
    else if (iid == IObject::GetIID())
    {
        *ppInterface = static_cast<IObject *>(this);
    }
    else
    {
        return _CEnumValue_BaseType::QueryInterfaceImpl(iid, ppInterface);
    }
    return S_OK;
}
#if defined(_WINDOWS_RUNTIME)

HRESULT CEnumValue::GetRuntimeClassNameImpl(_Outptr_ HSTRING *pClassName)
{
    HStringPtr hEnumTypeName;
    m_EnumTypeInfo->GetName(hEnumTypeName.GetAddressOf());

    if (hEnumTypeName.GetLength() == 0)
    {
        // no specific runtime class provided
        return HStringPtr(U("Windows.Foundation.IReference`1<UInt32>")).CopyTo(pClassName);
    }

    string_t enumTypeName_str = hEnumTypeName.GetRawBuffer();
    size_t pos = enumTypeName_str.find(U('.'));
    foundation_assert(pos != string_t::npos);
    if (pos == string_t::npos)
    {
        return E_UNEXPECTED;
    }
    string_t namespace_str = enumTypeName_str.substr(0, pos);
    string_t typeName_str = enumTypeName_str.substr(pos + 1);

    HStringPtr runtimeNamespace;
    CNamespaceStatics::GetInstance()->LookupRuntimeNamespace(
        HStringRef(namespace_str.c_str()),
        runtimeNamespace.GetAddressOf());

    string_t runtimeClassName = U("Windows.Foundation.IReference`1<");
    runtimeClassName += runtimeNamespace.GetRawBuffer();
    runtimeClassName += U('.');
    runtimeClassName += typeName_str;
    runtimeClassName += U('>');

    return HStringPtr(runtimeClassName.c_str()).CopyTo(pClassName);
}
#endif

