/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:namespace_statics.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation
{
    namespace library
    {
        // INamespaceStatics Interface
        BEGIN_DECLARE_INTERFACE(INamespaceStatics, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(RegisterNamespace)(
            _In_ HSTRING foundationNamespace,
            _In_ HSTRING runtimeNamespace) = 0;
        STDMETHOD(LookupRuntimeNamespace)(
            _In_ HSTRING foundationNamespace,
            _In_ HSTRING *runtimeNamespace) = 0;
        STDMETHOD(LookupFoundationNamespace)(
            _In_ HSTRING runtimeNamespace,
            _Out_ UINT32 *size,
            _Out_ HSTRING **foundationNamespaces) = 0;
        END_DECLARE_INTERFACE()

    }
}

