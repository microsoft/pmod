/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionInfoFactory.cpp
****/

#include "pch.h"

#include "MoObservableCollectionInfoFactory.h"

#include <pmod/library/type_info_factory_util.h>
#include <pmod/errors.h>

#include <vector>

STDMETHODIMP MoObservableCollectionInfoFactory::CreateInstance(
    int typeId,
    GUID iidType,
    HSTRING name,
    PropertyTypeInfo itemTypeInfo,
    IEnumerableTypeInfo **instance)
{
    foundation::ComPtr<foundation::IPropertyTypeInfo> spItemTypeInfo;

    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreatePropertyTypeInfo(
        itemTypeInfo.PropertyType,
        itemTypeInfo.ObjectType,
        spItemTypeInfo.GetAddressOf()));

    foundation::ComPtr<foundation::IEnumerableTypeInfo> spEnumerationTypeInfo;
    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreateEnumerableTypeInfo(
        typeId,
        iidType,
        name,
        spItemTypeInfo,
        pmod::TypeInfo_Collection,
        spEnumerationTypeInfo.GetAddressOf()));

    return foundation::QueryInterface(spEnumerationTypeInfo, instance);
}

STDMETHODIMP MoObservableCollectionInfoFactory::RegisterType(IEnumerableTypeInfo *pEnumerationTypeInfo)
{
    IFCPTR(pEnumerationTypeInfo);

    foundation::ComPtr<foundation::IEnumerableTypeInfo> spTypeInfo;
    IFR_ASSERT(foundation::QueryInterface(pEnumerationTypeInfo, spTypeInfo.GetAddressOf()));

    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->RegisterEnumerableTypeInfo(spTypeInfo));
    return S_OK;
}
