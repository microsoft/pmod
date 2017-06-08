/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEnumTypeInfoFactory.cpp
****/

#include "pch.h"

#include "MoEnumTypeInfoFactory.h"

#include <pmod/library/library_util.h>
#include <pmod/errors.h>

#include <vector>

STDMETHODIMP MoEnumTypeInfoFactory::CreateInstance(
    int typeId,
    GUID iidType,
    HSTRING name,
    boolean isflags,
    UINT32 length_names,
    HSTRING *names,
    UINT32 length_values,
    UINT32 *values,
    IEnumTypeInfo **instance)
{
    foundation::ComPtr<foundation::IEnumTypeInfo> spEnumTypeInfo;
    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreateEnumTypeInfo(
        typeId,
        iidType,
        name,
        isflags ? true : false,
        length_names,
        names,
        values,
        spEnumTypeInfo.GetAddressOf()));

    return foundation::QueryInterface(spEnumTypeInfo, instance);
}

STDMETHODIMP MoEnumTypeInfoFactory::RegisterType(IEnumTypeInfo *pEnumTypeInfo)
{
    IFCPTR(pEnumTypeInfo);

    foundation::IEnumTypeInfo *pModelTypeInfo = nullptr;
    IFR_ASSERT(foundation::QueryInterface(pEnumTypeInfo, &pModelTypeInfo));

    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->RegisterEnumTypeInfo(pModelTypeInfo));
    return S_OK;
}
