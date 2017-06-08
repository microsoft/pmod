/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:perf_counter_util.h
****/
#pragma once

#if defined(DEBUG)
#define PERF_CONTEXT(perf_id) \
    foundation::library::perf_counter_context perf_context(perf_id);
#else
#define PERF_CONTEXT(perf_id)
#endif

#if defined(DEBUG)

#include "debug_library.h"

#if defined(_WINDOWS)
#include "Windows.h"
#endif


namespace foundation
{
    namespace library
    {
        class perf_counter_context
        {
        public:
            perf_counter_context(UINT32 perfId):
                _perfId(perfId)
            {
#if defined(_WINDOWS)
                ::QueryPerformanceCounter(&_startTime);
#endif
            }
            ~perf_counter_context()
            {
#if defined(_WINDOWS)
                LARGE_INTEGER frequency,endTime;
                ::QueryPerformanceFrequency(&frequency);
                ::QueryPerformanceCounter(&endTime);

                LARGE_INTEGER elapsed;
                elapsed.QuadPart = endTime.QuadPart - _startTime.QuadPart;
                elapsed.QuadPart *= 1000000;
                elapsed.QuadPart /= frequency.QuadPart;
                debug_util::AddPerfCounter(_perfId, elapsed.QuadPart);
#endif
            }
        private:
#if defined(_WINDOWS)
            LARGE_INTEGER _startTime;
#endif
            UINT32 _perfId;
        };

    }

}

#endif
