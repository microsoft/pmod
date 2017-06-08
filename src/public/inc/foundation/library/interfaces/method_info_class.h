/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:method_info_class.h
****/
#pragma once

#include <foundation/foundation_api.h>

namespace foundation
{
    namespace library
    {
        // IMethodInfoClass Interface
        BEGIN_DECLARE_INTERFACE(IMethodInfoClass, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(AddParameterInfo)(
            _In_ HSTRING parameterName,
            _In_ PropertyType parameterType,
            _In_ GUID *pParameterModelType) = 0;
        END_DECLARE_INTERFACE()
        
    }
}

