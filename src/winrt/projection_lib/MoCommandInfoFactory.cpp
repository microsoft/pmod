/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandInfoFactory.cpp
****/

#include "pch.h"

#include "MoCommandInfoFactory.h"

#include <pmod/library/type_info_factory_util.h>
#include <pmod/errors.h>

#include <vector>

STDMETHODIMP MoCommandInfoFactory::CreateInstance(
    int typeId,
    GUID iidType,
    HSTRING name,
    PropertyTypeInfo parameterTypeInfo,
    PropertyTypeInfo resultTypeInfo,
    boolean isAsync,
    ICommandInfo **instance)
{
    foundation::ComPtr<foundation::IPropertyTypeInfo> spParameterTypeInfo;
    foundation::ComPtr<foundation::IPropertyTypeInfo> spResultTypeInfo;

    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreatePropertyTypeInfo(
        parameterTypeInfo.PropertyType, 
        parameterTypeInfo.ObjectType,
        spParameterTypeInfo.GetAddressOf()));

    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreatePropertyTypeInfo(
        resultTypeInfo.PropertyType,
        resultTypeInfo.ObjectType,
        spResultTypeInfo.GetAddressOf()));

    foundation::ComPtr<pmod::ICommandInfo> spCommandModelInfo;
    IFR_ASSERT(pmod::library::GetModelTypeInfoFactory()->CreateCommandInfo(
        typeId,
        iidType,
        name,
        spParameterTypeInfo,
        spResultTypeInfo,
        isAsync ? true:false,
        spCommandModelInfo.GetAddressOf()));

    return foundation::QueryInterface(spCommandModelInfo, instance);
}

STDMETHODIMP MoCommandInfoFactory::RegisterType(ICommandInfo *pCommandModelInfo)
{
    IFCPTR(pCommandModelInfo);

    pmod::ICommandInfo *pModelTypeInfo = nullptr;
    IFR_ASSERT(foundation::QueryInterface(pCommandModelInfo, &pModelTypeInfo));

    IFR_ASSERT(pmod::library::GetModelTypeInfoFactory()->RegisterCommandTypeInfo(pModelTypeInfo));
    return S_OK;
}
