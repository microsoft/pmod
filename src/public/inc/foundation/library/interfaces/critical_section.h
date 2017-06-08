/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:critical_section.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation
{
    namespace library
    {
        //------------------------------------------------------------------------------
        // Interface:   ICriticalSection
        //
        // Purpose: An Interface to allow override Lock/Unlock mechanism for a Critical Section
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(ICriticalSection, foundation::IUnknown, FOUNDATION_API)
            virtual HRESULT Lock() throw() = 0;
            virtual HRESULT Unlock() throw() = 0;
        END_DECLARE_INTERFACE()

    }
}

