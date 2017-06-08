/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_info_statics.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include "object.h"
#include "enum_value.h"
#include "property_type_info.h"
#include "async_operation.h"

namespace foundation {

    enum class InstanceTypeOptions
    {
        None = 0,        // Exact match
        AllowBaseType = 1,        // A Base Type can compared
        AllowInheritance = 2,        // Inheritance is allowed
        AllowQueryInterface = 4,        // QI is allowed
        All =
        AllowBaseType |
        AllowInheritance
    };

    struct _TypeInfoFilter 
    {
        _TypeInfoFilter():
            _NamePrefix(nullptr),
            _Type(foundation_GUID_NULL),
            _IncludeReserved(false)
        {}

        LPCSTR_t    _NamePrefix;
        GUID        _Type;
        bool        _IncludeReserved;
    };
    /////////////////////////////////// Model Type Statics //////////////////////////////////////////////
    BEGIN_DECLARE_INTERFACE(ITypeInfoStatics, IUnknown, FOUNDATION_API)
    STDMETHOD(GetTypeInfos)(
        _In_opt_ _TypeInfoFilter *pTypeInfoFilter,
        _Out_ UINT32 *size,
        _Outptr_ IObjectTypeInfo ***ppTypeInfos) = 0;
    STDMETHOD(GetTypeInfo)(
        GUID iidType,
        _COM_Outptr_result_maybenull_ IObjectTypeInfo **ppModelTypeInfo) = 0;
    // EnumType Info support
    STDMETHOD(GetEnumTypeInfo)(
        UINT32 enumTypeId,
        _COM_Outptr_result_maybenull_ IEnumTypeInfo **ppEnumTypeInfo) = 0;
    STDMETHOD(GetEnumTypeInfoByName)(
        HSTRING enumTypeName,
        _COM_Outptr_result_maybenull_ IEnumTypeInfo **ppEnumTypeInfo) = 0;
    STDMETHOD(GetEnumValueByName)(
        HSTRING enumValueName,
        _COM_Outptr_result_maybenull_ IEnumValue **ppEnumValue) = 0;
    STDMETHOD(GetEnumTypeInfoByNamespaceAndName)(
        HSTRING Namespace,
        HSTRING Name,
        _COM_Outptr_result_maybenull_ IEnumTypeInfo **ppEnumTypeInfo) = 0;
    STDMETHOD(GetEnumValueByValue)(
        UINT32 enumValue,
        _COM_Outptr_result_maybenull_ IEnumValue **ppEnumValue) = 0;
    // Async Operation support
    STDMETHOD(GetAsyncOperationTypeInfo)(
        _In_ IPropertyTypeInfo *pResultTypeInfo,
        _COM_Outptr_result_maybenull_ IAsyncOperationTypeInfo **ppAsyncOperationTypeInfo) = 0;

    STDMETHOD(IsInstanceOfType)(_In_ foundation::IObject *pObject, REFIID iidInstanceType, _Out_ bool *pResult) = 0;
    STDMETHOD(VerifyType)(_In_ IObject *pObject, REFIID iidExpectedType) = 0;
    STDMETHOD(IsInstanceOfTypeWithOptions)(
        _In_ IObject *pObject,
        REFIID iidInstanceType,
        InstanceTypeOptions options,
        _Out_ bool *pResult) = 0;

    STDMETHOD(VerifyPropertyTypeInfo)(
        _In_ IPropertyTypeInfo *pPropertyTypeInfo,
        _Inout_ foundation::IInspectable **ppValue) = 0;

    END_DECLARE_INTERFACE()
}

