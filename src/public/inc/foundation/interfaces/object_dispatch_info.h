/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_dispatch_info.h
****/
#pragma once

#include <foundation/foundation_macros.h>
#include "property_type_info.h"

namespace foundation {

    enum class InstanceFlags
    {
        None = 0,
        Declared = 1,
        Inherited = 2,
        NoReserved = 4,
        NoInspectable = 8,
        All =
        Declared |
        Inherited
    };

    enum PropertyFlagType
    {
        CanRead = 1,
        CanWrite = 2,
        NoCache = 0x10,
        IsReserved = 0x20,
        IsAncestor = 0x40,
        IsWeakReference = 0x80,
        IsEnum = 0x1000
    };

    BEGIN_DECLARE_INTERFACE(IPropertyInfo, IPropertyTypeInfo, FOUNDATION_API)
        STDMETHOD(GetId)(_Out_ UINT32* propertyId) = 0;
        STDMETHOD(GetName)(_Out_ HSTRING* propertyName) = 0;
        STDMETHOD(GetFlags)(_Out_ UINT32* flags) = 0;
    END_DECLARE_INTERFACE()

   BEGIN_DECLARE_INTERFACE(IParameterInfo, foundation::IPropertyTypeInfo, FOUNDATION_API)
        STDMETHOD(GetParameterName)(_Out_ HSTRING* parameterName) = 0;
    END_DECLARE_INTERFACE()

   BEGIN_DECLARE_INTERFACE(IMethodInfo, foundation::IInspectable, FOUNDATION_API)
        STDMETHOD(GetId)(_Out_ UINT32* methodId) = 0;
        STDMETHOD(GetMethodName)(_Out_ HSTRING* methodName) = 0;
        STDMETHOD(GetParameters)(_Out_ UINT32* size, _Outptr_ IParameterInfo ***ppParameters) = 0;
        STDMETHOD(GetResultTypeInfo)(_COM_Outptr_ foundation::IPropertyTypeInfo **ppVarTypeInfo) = 0;
        STDMETHOD(GetIsAsync)(_Out_ bool *value) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IObjectDispatchInfo, foundation::IObjectTypeInfo, FOUNDATION_API)
        STDMETHOD(GetProperties)(UINT32 propertiesFlag, _Out_ UINT32* size, _Outptr_ IPropertyInfo ***ppProperties) = 0;
        STDMETHOD(GetPropertyById)(UINT32 propertyId, _COM_Outptr_ IPropertyInfo **ppPropertyInfo) = 0;
        STDMETHOD(GetPropertyByName)(HSTRING propertyName, _COM_Outptr_ IPropertyInfo **ppPropertyInfo) = 0;

        STDMETHOD(GetMethods)(UINT32 methodsFlags, _Out_ UINT32* size, _Outptr_ IMethodInfo ***ppMethods) = 0;
        STDMETHOD(GetMethodyById)(UINT32 methodId, _COM_Outptr_ IMethodInfo **ppMethodInfo) = 0;
        STDMETHOD(GetMethodByName)(HSTRING methodName, _COM_Outptr_ IMethodInfo **ppMethodInfo) = 0;

    END_DECLARE_INTERFACE()

    const UINT32 Property_ResevervedMask = 0x2000;

}


