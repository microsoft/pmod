/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:critical_section.h
****/
#pragma once

#include <pal/pal_critical_section.h>
#include <foundation/assert.h>

namespace pal {
    namespace library {

        //------------------------------------------------------------------------------
        // Class:   CriticalSection
        //
        // Purpose: a wrapper to create a critical section using our PAL layer
        //
        //------------------------------------------------------------------------------
        class CriticalSection
        {
        public:
            CriticalSection(bool isRecursive = true)
            {
                _pal_InitializeCriticalSection(&_cs, isRecursive);
            }

            ~CriticalSection()
            {
                _pal_DeleteCriticalSection(&_cs);
            }

            void lock()
            {
                _pal_EnterCriticalSection(&_cs);
            }

            void unlock()
            {
                _pal_LeaveCriticalSection(&_cs);
            }
        private:
            _PAL_CRITICAL_SECTION _cs;
        };
    }
}
