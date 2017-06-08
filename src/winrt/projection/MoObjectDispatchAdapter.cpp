/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObjectDispatchAdapter.cpp
****/

#include "pch.h"

#include "MoObjectDispatchAdapter.h"

#include "..\moproxy\MoCoreProxyFactory.h"

HRESULT STDMETHODCALLTYPE CMoObjectDispatchAdapter::GetProperty(
    unsigned int propertyId,
    IInspectable **value)
{
    return m_pInner->GetProperty((UINT32)propertyId, value);
}

HRESULT STDMETHODCALLTYPE CMoObjectDispatchAdapter::SetProperty(
    unsigned int propertyId,
    IInspectable *value)
{
    foundation::InspectablePtr spCoreValue;
    IFR_ASSERT(CMoCoreProxyFactory::ToProxyIf(value, spCoreValue.GetAddressOf()));

    return m_pInner->SetProperty((UINT32)propertyId, spCoreValue);
}

HRESULT STDMETHODCALLTYPE CMoObjectDispatchAdapter::GetProperties(
    IInspectable* pProperties,
    UINT32 *size,
    __FIKeyValuePair_2_UINT32_IInspectable ***kvpProperties)
{
    foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> kvpCorePropertiesWrapper;
    IFR_ASSERT(m_pInner->GetProperties(pProperties,
        kvpCorePropertiesWrapper.GetSizeAddressOf(),
        kvpCorePropertiesWrapper.GetBufferAddressOf()));

    std::vector<foundation::ComPtr<__FIKeyValuePair_2_UINT32_IInspectable>> kvpMoPropertiesArray;
    kvpMoPropertiesArray.resize(kvpCorePropertiesWrapper.GetSize());
    for (UINT32 index = 0; index < kvpCorePropertiesWrapper.GetSize(); ++index)
    {
        IFR_ASSERT(foundation::QueryInterface(
            kvpCorePropertiesWrapper[index],
            kvpMoPropertiesArray[index].GetAddressOf()));
    }
    ContainerRefCountToArray(kvpMoPropertiesArray, size, kvpProperties);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoObjectDispatchAdapter::Invoke(unsigned int methodId, UINT32 length, IInspectable **pParameters, IInspectable **result)
{
    foundation::InspectableArrayWrapper spCoreParameters;
    IFR_ASSERT(CMoCoreProxyFactory::ToProxyArrayIf(
        length, pParameters,
        spCoreParameters.GetSizeAddressOf(), spCoreParameters.GetBufferAddressOf()));

    return m_pInner->InvokeMethod((UINT32)methodId, length, (foundation::IInspectable **)spCoreParameters.GetBuffer(), result);
}
