/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:critical_section_lock.h
****/
#pragma once

#include <pal/library/critical_section.h>
#include <foundation/assert.h>

namespace foundation {
    namespace library {
        //------------------------------------------------------------------------------
        // Class:   _CriticalSectionLock
        //
        // Purpose: a class that lock/unlock a context based on a critical section
        //
        //------------------------------------------------------------------------------
        template <class TLock>
        class _CriticalSectionLock
        {
        public:
            _CriticalSectionLock(TLock& cs, bool initial_lock = true) :
                _cs(cs),
                is_locked(false)
            {
                if (initial_lock)
                {
                    lock();
                }
            }

            ~_CriticalSectionLock()
            {
                if (is_locked)
                {
                    unlock();
                }
            }

            void lock()
            {
                foundation_assert(!is_locked);
                _cs.lock();
                is_locked = true;
            }

            void unlock()
            {
                foundation_assert(is_locked);
                _cs.unlock();
                is_locked = false;
            }

        private:
            TLock& _cs;
            bool is_locked;
        };

        typedef _CriticalSectionLock<pal::library::CriticalSection> CriticalSectionLock;
    }
}
