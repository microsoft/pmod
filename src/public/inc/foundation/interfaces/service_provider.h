/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:service_provider.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation
{
    // IServiceProvider Interface
    BEGIN_DECLARE_INTERFACE(IServiceProvider, foundation::IUnknown, FOUNDATION_API)
        STDMETHOD (QueryService)(
            REFIID guidService,
            REFIID riid,
            void **ppv) = 0;
    END_DECLARE_INTERFACE()
}

