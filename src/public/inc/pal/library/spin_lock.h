/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:spin_lock.h
****/
#pragma once

#include <pal/pal_atomic.h>
#include "critical_section.h"

namespace pal {
    namespace library {

        //------------------------------------------------------------------------------
        // Class:   SpinLock
        //
        // Purpose: a wrapper to create a spin lock using our PAL layer
        //
        //------------------------------------------------------------------------------
        class SpinLock
        {
        public:
            SpinLock():
                _lock(0)
            {
            }

            void lock()
            {
                for (;;)
                {
                    // attempt to increment if values hasn't changed
                    if (_pal_InterlockedCompareExchange(&_lock, 1, 0) == 0)
                    {
                        // successfully incremented
                        break;
                    }
                }
            }

            void unlock()
            {
                _lock = 0;
            }
        private:
            volatile _pal_AtomicIntegerType _lock;
        };

    }
}
