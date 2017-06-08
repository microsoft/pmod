/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_info_factory.h
****/
#pragma once

#include <pmod/pmod_api.h>

#include <pmod/interfaces/command.h>
#include <pmod/interfaces/observable_collection.h>
#include <pmod/interfaces/observable_object.h>
#include <pmod/library/interfaces/observable_object_info_class.h>

namespace pmod
{
    namespace library
    {
        // ITypeInfoFactory Interface
        BEGIN_DECLARE_INTERFACE(IModelTypeInfoFactory, foundation::IUnknown, PMOD_API)

        STDMETHOD(CreateCommandInfo)(
            _In_ UINT32 typeId,
            _In_ GUID iidType,
            _In_ HSTRING name,
            _In_ foundation::IPropertyTypeInfo *pParameterTypeInfo,
            _In_ foundation::IPropertyTypeInfo *pResultTypeInfo,
            _In_ bool isAsync,
            _Outptr_ ICommandInfo **ppCommandModelInfo) = 0;

        STDMETHOD(CreateObservableObjectInfo)(
            _In_ UINT32 typeId,
            _In_ GUID iidType,
            _In_ HSTRING name,
            _In_opt_ IObservableObjectInfo *pBaseType,
            _Outptr_ IObservableObjectInfo **ppPropertyModelInfo) = 0;

        STDMETHOD(CreateViewModelInfo)(
            _In_ UINT32 typeId,
            _In_ GUID iidType,
            _In_ HSTRING name,
            _In_opt_ IObservableObjectInfo *pBaseType,
            _In_ IObservableObjectInfo *pSourceType,
            _In_ bool isInherited,
            _Outptr_ IViewModelInfo **ppPropertyViewModelInfo) = 0;

        STDMETHOD(RegisterObjectTypeInfo)(_In_ IObservableObjectInfo *pPropertyModelInfo) = 0;
        STDMETHOD(RegisterCommandTypeInfo)(_In_ ICommandInfo *pCommandModelInfo) = 0;
        STDMETHOD(RegisterCollectionTypeInfo)(_In_ foundation::IEnumerableTypeInfo *pCollectionModelInfo) = 0;

        END_DECLARE_INTERFACE()

    }
}

