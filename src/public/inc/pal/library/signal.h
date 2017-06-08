/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:signal.h
****/
#pragma once

#include <pal/pal_signal.h>

namespace pal {
    namespace library {

        //------------------------------------------------------------------------------
        // Class:   Signal
        //
        // Purpose: a class that create a PAL Signal entity and allow to Wait and Signal
        //
        //------------------------------------------------------------------------------
        class Signal
        {
        public:
            Signal()
            {
                _pal_CreateSignal(&_signal);
            }
            ~Signal()
            {
                _pal_DeleteSignal(&_signal);
            }

            void set()
            {
                _pal_SetSignal(&_signal);
            }
            void Wait()
            {
                _pal_WaitSignal(&_signal);
            }
            bool wait_cond()
            {
                return _pal_WaitSignalCond(&_signal);
            }
            bool wait_timed(UINT32 ms)
            {
                return _pal_WaitSignalTimed(&_signal,ms);
            }
        private:
            _PAL_SIGNAL _signal;
        };

    }
}
