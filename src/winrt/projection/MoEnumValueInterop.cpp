/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEnumValueInterop.cpp
****/

#include "pch.h"

#include "MoEnumValueInterop.h"
#include <foundation\library\logger_macros.h>
#include <foundation\library\namespace_statics_util.h>
#include <foundation\type_info_statics_util.h>

HRESULT ObjectToEnumValue(IInspectable *pObject, foundation::IEnumValue **ppEnumValue)
{
    foundation_assert(pObject);
    foundation_assert(ppEnumValue);

    foundation::ComPtr<foundation::IEnumValue> spEnumValue;
    HRESULT hr = foundation::QueryInterface(pObject,spEnumValue.GetAddressOf());
    if (hr == E_NOINTERFACE)
    {
        // first attempt is to cast to a UINT64 value to get the enum value from
        // the TypeId + Value
        ABI::Windows::Foundation::PropertyType propertyType;
        foundation::pv_util::GetType(pObject,&propertyType);
        if (propertyType == ABI::Windows::Foundation::PropertyType_UInt64)
        {

            UINT64 value;
            IFR_ASSERT(foundation::pv_util::GetValue(pObject, &value));
            IFR_ASSERT(foundation::GetEnumValue(
                (UINT32)(value >> 32), 
                (value & 0xffffffff),ppEnumValue));
            return S_OK;
        }
        else if (propertyType == ABI::Windows::Foundation::PropertyType_UInt32)
        {
            UINT32 value;
            IFR_ASSERT(foundation::pv_util::GetValue(pObject, &value));
            IFR_ASSERT(foundation::GetEnumValue(
                (UINT32)(value >> 16),
                (value & 0xffff), ppEnumValue));
            return S_OK;
        }

        foundation::HStringPtr hRuntimeClassName;
        IFR_ASSERT(pObject->GetRuntimeClassName(hRuntimeClassName.GetAddressOf()));
        foundation::string_t runtimeClass(hRuntimeClassName.GetRawBuffer());
        
        const foundation::string_t _foundationReferencePrefix = L"Windows.Foundation.IReference`1<";
        if (runtimeClass.compare(0, _foundationReferencePrefix.size(), _foundationReferencePrefix) == 0)
        {
            foundation::string_t enumTypeInfo = runtimeClass.substr(
                _foundationReferencePrefix.size(),
                runtimeClass.size() - _foundationReferencePrefix.size() - 1);

            size_t pos = enumTypeInfo.find_last_of(U('.'));
            foundation_assert(pos != foundation::string_t::npos);

            foundation::string_t runtime_namespace_str = enumTypeInfo.substr(0, pos);
            foundation::string_t typeName_str = enumTypeInfo.substr(pos + 1);

            foundation::ComPtr<foundation::IEnumTypeInfo> spEnumTypeInfo;
            foundation::HStringArrayWrapper foundation_namespaces;
            foundation::library::get_namespace_statics()->LookupFoundationNamespace(
                foundation::HStringRef(runtime_namespace_str.c_str()),
                foundation_namespaces.GetSizeAddressOf(),
                foundation_namespaces.GetBufferAddressOf()
                );

            HRESULT hr;
            if (foundation_namespaces.GetSize() == 0)
            {
                IFHR(foundation::GetTypeInfoStatics()->GetEnumTypeInfoByName(
                    foundation::HStringRef(enumTypeInfo.c_str()),
                    spEnumTypeInfo.GetAddressOf()));
            }
            else
            {
                for (UINT32 i = 0; i < foundation_namespaces.GetSize(); ++i)
                {
                    enumTypeInfo = _pal_GetStringRawBuffer(foundation_namespaces[0], nullptr);
                    enumTypeInfo += U('.');
                    enumTypeInfo += typeName_str;
                    hr = foundation::GetTypeInfoStatics()->GetEnumTypeInfoByName(
                        foundation::HStringRef(enumTypeInfo.c_str()),
                        spEnumTypeInfo.GetAddressOf());
                    if (SUCCEEDED(hr))
                    {
                        break;
                    }
                }
            }
            IFHR(hr);

            UINT32 enumValue;
            hr = foundation::pv_util::GetValue(pObject, &enumValue);
            if (FAILED(hr))
            {
                hr = foundation::pv_util::GetValue(pObject, (INT32 *)&enumValue);
            }
            IFHR(hr);

            IFR_ASSERT(spEnumTypeInfo->GetEnumValue(enumValue, ppEnumValue));
            return S_OK;
        }
    }
    return hr;
}
