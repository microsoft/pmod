/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:disposable.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation
{
    namespace library
    {
        //------------------------------------------------------------------------------
        // Interface:   IDisposable
        //
        // Purpose: An Interface defining a 'Dispose' pattern
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IDisposable, IInspectable, FOUNDATION_API)
            STDMETHOD(Dispose)() = 0;
        END_DECLARE_INTERFACE()
    }
}

