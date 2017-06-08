/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_info_factory.h
****/
#pragma once

#include <foundation/interfaces/property_type_info.h>
#include <foundation/interfaces/enum_value.h>
#include <foundation/interfaces/async_operation.h>
#include <foundation/interfaces/object_dispatch_info.h>
#include <foundation/interfaces/enumerable.h>
#include <foundation/library/interfaces/method_info_class.h>

namespace foundation
{
    struct TypeInfo;

    namespace library
    {
        //------------------------------------------------------------------------------
        // Interface:   ITypeInfoFactory
        //
        // Purpose: An Interface for TypeInfo factory support
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(ITypeInfoFactory, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(CreatePropertyTypeInfo)(
            _In_ PropertyType type,
            _In_ GUID modelType,
            _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo) = 0;

        STDMETHOD(CreatePropertyInfo)(
            _In_ UINT32 Id,
            _In_ HSTRING name,
            _In_ PropertyType type,
            _In_ UINT32 flags,
            _In_ GUID objectType,
            _Outptr_ IPropertyInfo **ppPropertyInfo) = 0;

        STDMETHOD(CreateMethodInfo)(
            _In_ UINT32 methodId,
            _In_ HSTRING name,
            _In_ foundation::PropertyType returnPropertyType,
            _In_ GUID *pReturnModelType,
            _In_ bool isAsync,
            _Outptr_ foundation::library::IMethodInfoClass **ppMethodInfoClass) = 0;

        STDMETHOD(CreateEnumTypeInfo)(
            _In_ UINT32 typeId,
            _In_ GUID iidType,
            _In_ HSTRING name,
            _In_ bool isFlags,
            _In_ UINT32 size,
            _In_ HSTRING *pName,
            _In_ UINT32 *pValues,
            _COM_Outptr_ IEnumTypeInfo **ppEnumTypeInfo) = 0;

        STDMETHOD(CreateAsyncOperationTypeInfo)(
            _In_ UINT32 typeId,
            _In_ GUID iidType,
            _In_ HSTRING name,
            _In_ IPropertyTypeInfo *pResultTypeInfo,
            _COM_Outptr_ IAsyncOperationTypeInfo **ppAsyncOperationTypeInfo) = 0;

        STDMETHOD(CreateEnumerableTypeInfo)(
            _In_ UINT32 typeId,
            _In_ GUID iidType,
            _In_ HSTRING name,
            _In_ foundation::IPropertyTypeInfo *pItemTypeInfo,
            _In_ UINT32  type,
            _Outptr_ IEnumerableTypeInfo **ppEnumerableTypeInfo) = 0;

        STDMETHOD(RegisterEnumTypeInfo)(_In_ IEnumTypeInfo *pEnumTypeInfo) = 0;
        STDMETHOD(RegisterAsyncOperationTypeInfo)(_In_ IAsyncOperationTypeInfo *pAsyncOperationTypeInfo) = 0;
        STDMETHOD(RegisterEnumerableTypeInfo)(_In_ IEnumerableTypeInfo *pEnumerableTypeInfo) = 0;

        STDMETHOD(RegisterTypeInfo)(
            _In_ const TypeInfo* pTypeInfoEntry) = 0;

        STDMETHOD(RegisterMultipleTypeInfos)(
            _In_ const TypeInfo* pTypeInfoEntries[]) = 0;

        STDMETHOD(RegisterMultipleTypeInfos)(
            UINT32 size, 
            IDictionary **ppDictionaryTypeInfo) = 0;

        STDMETHOD(SerializeTypeInfo)(
            IObjectTypeInfo *pTypeInfo,
            IDictionary **ppDictionaryTypeInfo) = 0;

        END_DECLARE_INTERFACE()
    }
}

