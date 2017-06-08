/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelTypeInfoStatics.cpp
****/

#include "pch.h"

#include "MoModelTypeInfoStatics.h"
#include "MoEnumValueInterop.h"

#include <pmod/library/model_library.h>
#include <foundation/library/logger_macros.h>
#include <pmod/library/library_util.h>

#include <foundation/com_ptr.h>

STDMETHODIMP MoObjectTypeInfoStatics::get_CommandModelType( 
    IObjectTypeInfo **value)
{
    IFCPTR(value);
    return GetObjectTypeInfo(pmod::ICommand::GetIID(), value);
}
                    
STDMETHODIMP MoObjectTypeInfoStatics::get_CollectionModelType( 
    IObjectTypeInfo **value)
{
    IFCPTR(value);
    return GetObjectTypeInfo(pmod::IObservableCollection::GetIID(), value);
}
                    
STDMETHODIMP MoObjectTypeInfoStatics::get_PropertyModelType( 
    IObjectTypeInfo **value)
{
    IFCPTR(value);
    return GetObjectTypeInfo(pmod::IObservableObject::GetIID(), value);
}
                    
STDMETHODIMP MoObjectTypeInfoStatics::get_AsyncOperationType( 
    IObjectTypeInfo **value)
{
    IFCPTR(value);
    return GetObjectTypeInfo(foundation::IAsyncOperationTypeInfo::GetIID(), value);
}

STDMETHODIMP MoObjectTypeInfoStatics::GetObjectTypeInfo(
            GUID iidType,
            IObjectTypeInfo **value)
{
    IFCPTR(value);
    foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
    IFR(foundation::GetTypeInfo(iidType, spModelTypeInfo.GetAddressOf()));
    foundation_assert(spModelTypeInfo != nullptr);
    IFCEXPECT(spModelTypeInfo != nullptr);
    IFR(foundation::QueryInterface(spModelTypeInfo,value));
    return S_OK;
}

STDMETHODIMP MoObjectTypeInfoStatics::GetEnumTypeInfo( 
    UINT32 typeId,
    IEnumTypeInfo **value)
{
    foundation::ComPtr<foundation::IEnumTypeInfo> spEnumTypeInfo;
    IFR(foundation::GetTypeInfoStatics()->GetEnumTypeInfo(typeId,spEnumTypeInfo.GetAddressOf()));
    return foundation::QueryInterface(spEnumTypeInfo,value);
}

STDMETHODIMP MoObjectTypeInfoStatics::GetEnumTypeInfoByName( 
    HSTRING typeName,
    IEnumTypeInfo **value)
{
    foundation::ComPtr<foundation::IEnumTypeInfo> spEnumTypeInfo;
    IFR(foundation::GetTypeInfoStatics()->GetEnumTypeInfoByName(typeName, spEnumTypeInfo.GetAddressOf()));
    return foundation::QueryInterface(spEnumTypeInfo,value);
}

STDMETHODIMP MoObjectTypeInfoStatics::GetEnumValue(
    UINT32 uniqueEnumValue,
    IEnumValue **value)
{
    IFCPTR(value);

    foundation::ComPtr<foundation::IEnumValue> spEnumValue;
    IFR(foundation::GetEnumValue(uniqueEnumValue, spEnumValue.GetAddressOf()));
    // previous call could return S_FALSE which mean the IEnumValue was not found
    if (spEnumValue != nullptr)
    {
        return foundation::QueryInterface(spEnumValue, value);
    }
    *value = nullptr;
    return S_OK;
}


STDMETHODIMP MoObjectTypeInfoStatics::GetEnumTypeInfoFromReference(
    IInspectable *pObject,
    IEnumTypeInfo **value)
{
    foundation::ComPtr<foundation::IEnumValue> spEnumValue;
    IFR(ObjectToEnumValue(pObject, spEnumValue.GetAddressOf()));
    IFR_ASSERT(foundation::GetObjectTypeInfo(spEnumValue, value));
    return S_OK;
}
