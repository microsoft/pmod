/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:component.h
****/
#pragma once

#include <pal/pal_api.h>
#include <foundation/com_interfaces.h>
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        //------------------------------------------------------------------------------
        // Interface:   IComponent
        //
        // Purpose: An Interface defining a component module
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IComponent, IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(Start)() = 0;
            STDMETHOD(Stop)() = 0;
        END_DECLARE_INTERFACE()
    }
}

