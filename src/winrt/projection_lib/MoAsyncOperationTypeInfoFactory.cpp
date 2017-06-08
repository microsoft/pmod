/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationTypeInfoFactory.cpp
****/

#include "pch.h"

#include "MoAsyncOperationTypeInfoFactory.h"

#include <foundation/macros.h>
#include <pmod/library/library_util.h>
#include <pmod/errors.h>

#include <vector>

STDMETHODIMP MoAsyncOperationModelInfoFactory::CreateInstance(
    int typeId,
    GUID iidType,
    HSTRING name,
    PropertyTypeInfo resultTypeInfo,    
    IAsyncOperationTypeInfo **instance)
{
    foundation::ComPtr<foundation::IPropertyTypeInfo> spResultTypeInfo;

    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreatePropertyTypeInfo(
        resultTypeInfo.PropertyType,
        resultTypeInfo.ObjectType,
        spResultTypeInfo.GetAddressOf()));

    foundation::ComPtr<foundation::IAsyncOperationTypeInfo> spAsyncOperationModelInfo;
    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreateAsyncOperationTypeInfo(
        typeId,
        iidType,
        name,
        spResultTypeInfo,        
        spAsyncOperationModelInfo.GetAddressOf()));

    return foundation::QueryInterface(spAsyncOperationModelInfo, instance);
}

STDMETHODIMP MoAsyncOperationModelInfoFactory::RegisterType(IAsyncOperationTypeInfo *pAsyncOperationModelInfo)
{
    IFCPTR(pAsyncOperationModelInfo);

    foundation::IAsyncOperationTypeInfo *pModelTypeInfo = nullptr;
    IFR_ASSERT(foundation::QueryInterface(pAsyncOperationModelInfo, &pModelTypeInfo));

    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->RegisterAsyncOperationTypeInfo(pModelTypeInfo));
    return S_OK;
}
