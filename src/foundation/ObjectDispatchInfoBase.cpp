/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchInfoBase.cpp
****/

#include "pch.h"

#include "ObjectDispatchInfoBase.h"
#include "PropertyInfo.h"
#include "MethodInfo.h"
#include "PropertyInfoProxy.h"


using namespace foundation;
using namespace foundation_lib;

void _CObjectDispatchInfoBase::AddPropertyInfoInternal(IPropertyInfo *pPropertyInfo)
{
    foundation_assert(pPropertyInfo);
    m_properties.push_back(pPropertyInfo);

    // Ask for property id
    UINT32 propertyId = 0;
    pPropertyInfo->GetId(&propertyId);
    // cache the propertie also by id
    m_propertiesById[propertyId] = pPropertyInfo;
}

void _CObjectDispatchInfoBase::AddMethodInfoInternal(IMethodInfo *pMethodInfo)
{
    foundation_assert(pMethodInfo);
    m_methods.push_back(pMethodInfo);
}

HRESULT _CObjectDispatchInfoBase::AddDeclaredMethods(MethodInfoVector::iterator iter, MethodInfoVector& methods)
{
    methods.insert(iter, m_methods.begin(), m_methods.end());
    return S_OK;
}

HRESULT _CObjectDispatchInfoBase::AddDeclaredProperties(
    PropertyInfoVector::iterator iter,
    PropertyInfoVector& properties,
    bool noReserved,
    bool noInspectable)
{
    if (noReserved || noInspectable)
    {
        PropertyInfoVector filtered_properties;
        for (PropertyInfoVector::const_iterator
            iter_properties = m_properties.begin();
            iter_properties != m_properties.end();
            ++iter_properties)
        {
            if (noReserved)
            {
                UINT32 propertyId;
                (*iter_properties)->GetId(&propertyId);
                if (propertyId >= Property_ResevervedMask)
                {
                    continue;
                }
            }
            if (noInspectable)
            {
                foundation::PropertyType propertyType;
                (*iter_properties)->GetPropertyType(&propertyType);
                if ((propertyType & 0xff) == foundation::PropertyType_Inspectable)
                {
                    continue;
                }
            }
            filtered_properties.push_back(*iter_properties);
        }
        properties.insert(iter, filtered_properties.begin(), filtered_properties.end());
    }
    else
    {
        properties.insert(iter, m_properties.begin(), m_properties.end());
    }
    return S_OK;
}

HRESULT _CObjectDispatchInfoBase::GetPropertiesInternal(UINT32 propertiesFlag, UINT32* size, IPropertyInfo ***ppProperties)
{
    IFCPTR_ASSERT(ppProperties);

    PropertyInfoVector properties;

    bool noReserved = IsOptionEnabled(propertiesFlag, InstanceFlags::NoReserved);
    bool noInspectable = IsOptionEnabled(propertiesFlag, InstanceFlags::NoInspectable);
    if (IsOptionEnabled(propertiesFlag, InstanceFlags::Inherited))
    {
        _CObjectDispatchInfoBase *pBaseType = GetBaseTypeAs<_CObjectDispatchInfoBase>();
        while (pBaseType)
        {
            IFR_ASSERT(pBaseType->AddDeclaredProperties(properties.begin(), properties, noReserved, noInspectable));
            pBaseType = pBaseType->GetBaseTypeAs<_CObjectDispatchInfoBase>();
        }
    }
    if (IsOptionEnabled(propertiesFlag, InstanceFlags::Declared))
    {
        IFR_ASSERT(AddDeclaredProperties(properties.end(), properties, noReserved, noInspectable));
    }
    ContainerRefCountToArray(properties, size, ppProperties);
    return S_OK;
}

HRESULT _CObjectDispatchInfoBase::GetPropertyByNameInternal(LPCSTR_t propertyName, IPropertyInfo **ppPropertyInfo)
{
    foundation_assert(propertyName != nullptr);
    CFoundationLock<CCriticalSection> lock(m_cs);

    _TypePropertiesNameMap::iterator iter = m_propertiesByName.find(propertyName);
    if (iter != m_propertiesByName.end())
    {
        // it was already in our Map
        *ppPropertyInfo = iter->second;
        (*ppPropertyInfo)->AddRef();
        return S_OK;
    }
    // attempt to lookup
    for (PropertyInfoVector::const_iterator iter = m_properties.begin();
        iter != m_properties.end();
        ++iter)
    {
        // QI the private IPropertyInfoClass interface to look
        // for the Internal Name
        foundation::ComPtr<IPropertyInfoClass> spPropertyInfoClass;
        IFR_ASSERT(foundation::QueryInterface(*iter, spPropertyInfoClass.GetAddressOf()));
        LPCSTR_t keyPropertyName = spPropertyInfoClass->GetNameInternal();

        if (_pal_strcmp(propertyName, keyPropertyName) == 0)
        {
            // add to std::map for later use
            m_propertiesByName[keyPropertyName] = (*iter);
            // AddRef and return
            *ppPropertyInfo = (*iter);
            (*ppPropertyInfo)->AddRef();
            return S_OK;
        }
    }
        _CObjectDispatchInfoBase *pBaseType = GetBaseTypeAs<_CObjectDispatchInfoBase>();
    if (pBaseType)
    {
        // not found here try the base
        return pBaseType->GetPropertyByNameInternal(propertyName, ppPropertyInfo);
    }
    // not found
    return S_FALSE;
}

HRESULT _CObjectDispatchInfoBase::GetPropertyByIdInternal(UINT32 propertyId, IPropertyInfo **ppPropertyInfo)
{
    _TypePropertiesByIdMap::iterator iter = m_propertiesById.find(propertyId);
    if (iter != m_propertiesById.end())
    {
        *ppPropertyInfo = iter->second;
        (*ppPropertyInfo)->AddRef();
        return S_OK;
    }
    _CObjectDispatchInfoBase *pBaseType = GetBaseTypeAs<_CObjectDispatchInfoBase>();
    if (pBaseType)
    {
        // not found here try the base
        return pBaseType->GetPropertyByIdInternal(propertyId, ppPropertyInfo);
    }
    // not found
    return S_FALSE;
}

HRESULT _CObjectDispatchInfoBase::GetPropertyFlagsByIdInternal(UINT32 propertyId, UINT32 *flags)
{
    CFoundationLock<CCriticalSection> lock(m_cs);

    std::map<UINT32, UINT32>::iterator iter = m_propertiesFlagsById.find(propertyId);

    if (iter != m_propertiesFlagsById.end())
    {
        *flags = iter->second;
        return S_OK;
    }

    *flags = 0;
    HRESULT hr;
    foundation::ComPtr<IPropertyInfo> spPropertyInfo;
    IFHR_ASSERT(GetPropertyByIdInternal(propertyId, spPropertyInfo.GetAddressOf()));
    if (hr == S_OK && spPropertyInfo)
    {
        IFHR_ASSERT(spPropertyInfo->GetFlags(flags));
    }
    // Add to my local map
    m_propertiesFlagsById[propertyId] = *flags;
    return hr;
}

HRESULT _CObjectDispatchInfoBase::GetMethodsInternal(UINT32 methodsFlags, UINT32* size, IMethodInfo ***ppMethods)
{
    IFCPTR_ASSERT(ppMethods);

    MethodInfoVector methods;

    if (IsOptionEnabled(methodsFlags, foundation::InstanceFlags::Inherited))
    {
        _CObjectDispatchInfoBase *pBaseType = GetBaseTypeAs<_CObjectDispatchInfoBase>();
        while (pBaseType)
        {
            IFR_ASSERT(pBaseType->AddDeclaredMethods(methods.begin(), methods));
            pBaseType = pBaseType->GetBaseTypeAs<_CObjectDispatchInfoBase>();
        }
    }
    if (IsOptionEnabled(methodsFlags, foundation::InstanceFlags::Declared))
    {
        IFR_ASSERT(AddDeclaredMethods(methods.end(), methods));
    }
    ContainerRefCountToArray(methods, size, ppMethods);
    return S_OK;
}

HRESULT _CObjectDispatchInfoBase::GetMethodByNameInternal(LPCSTR_t methodName, IMethodInfo **ppMethodInfo)
{
    for (MethodInfoVector::const_iterator iter = m_methods.begin();
        iter != m_methods.end();
        ++iter
        )
    {
        foundation::HStringPtr hMethodName;
        (*iter)->GetMethodName(hMethodName.GetAddressOf());
        if (_pal_strcmp(methodName, hMethodName.GetRawBuffer()) == 0)
        {
            *ppMethodInfo = (*iter);
            (*ppMethodInfo)->AddRef();
            return S_OK;
        }
    }
    return S_FALSE;
}

HRESULT _CObjectDispatchInfoBase::GetMethodByIdInternal(UINT32 methodId, IMethodInfo **ppMethodInfo)
{
    foundation_assert(ppMethodInfo);

    // attempt to lookup
    for (MethodInfoVector::const_iterator iter = m_methods.begin();
        iter != m_methods.end();
        ++iter)
    {
        UINT32 _methodId;
        (*iter)->GetId(&_methodId);
        if (methodId == _methodId)
        {
            // Note: no add ref count
            *ppMethodInfo = *iter;
            return S_OK;
        }
    }
    return S_FALSE;
}

HRESULT _CObjectDispatchInfoBase::AddPropertyInfoArray(const foundation::PropertyInfo *pPropertyInfoArray)
{
    foundation_assert(pPropertyInfoArray);
    // Build the Array for each defined Property
    while (pPropertyInfoArray->m_name)
    {
        IFR_ASSERT(AddPropertyInfo(pPropertyInfoArray));
        // Next Property Info description
        ++pPropertyInfoArray;
    }
    return S_OK;
}

HRESULT _CObjectDispatchInfoBase::AddMethodInfoArray(const foundation::MethodInfo *pMethodInfoArray)
{
    foundation_assert(pMethodInfoArray);
    // Build the Array for each defined Property
    while (pMethodInfoArray->m_methodName)
    {
        IFR_ASSERT(AddMethodInfo(pMethodInfoArray));
        // Next method Info description
        ++pMethodInfoArray;
    }
    return S_OK;
}

HRESULT _CObjectDispatchInfoBase::AddPropertyInfo(
    foundation::IPropertyInfo *pPropertyInfo,
    _In_ foundation::IObjectTypeInfo *pModelTypeInfo,
    _Out_opt_ CPropertyInfo **ppPropertyInfo)
{
    foundation_assert(pPropertyInfo);

    foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
    IFR_ASSERT(CPropertyInfoProxy::CreateInstance(pPropertyInfo, pModelTypeInfo, spPropertyInfo.GetAddressOf()));
    AddPropertyInfoInternal(spPropertyInfo);

    if (ppPropertyInfo)
    {
        *ppPropertyInfo = static_cast<CPropertyInfo *>(spPropertyInfo.Detach());
    }
    return S_OK;
}

HRESULT _CObjectDispatchInfoBase::AddPropertyInfo(
    const foundation::PropertyInfo *pPropertyInfo,
    _In_ const GUID *pModelType,
    _Out_opt_ CPropertyInfo **ppPropertyInfo)
{
    foundation_assert(pPropertyInfo);
    foundation_assert(pPropertyInfo->m_name);

    foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
    IFR_ASSERT(CPropertyInfo::CreateInstance(pPropertyInfo, pModelType, spPropertyInfo.GetAddressOf()));
    AddPropertyInfoInternal(spPropertyInfo);

    if (ppPropertyInfo)
    {
        *ppPropertyInfo = static_cast<CPropertyInfo *>(spPropertyInfo.Detach());
    }
    return S_OK;
}

HRESULT _CObjectDispatchInfoBase::AddMethodInfo(
    const foundation::MethodInfo *pMethodInfo)
{
    foundation_assert(pMethodInfo);
    foundation_assert(pMethodInfo->m_methodName);

    foundation::ComPtr<foundation::IMethodInfo> spMethodInfo;
    IFR_ASSERT(CMethodInfo::CreateInstance(pMethodInfo, nullptr, spMethodInfo.GetAddressOf()));
    AddMethodInfoInternal(spMethodInfo);

    return S_OK;
}
