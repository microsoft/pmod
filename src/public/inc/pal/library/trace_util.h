/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:trace_util.h
****/
#pragma once

#include <pal/pal_hstring.h>
#include <foundation/string_t.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(_WINDOWS)

#ifdef __UTF16_STRINGS
#define _x_vsnprintf_s _vsnwprintf_s
#else
#define _x_vsnprintf_s _vsnprintf_s
#endif

#endif
namespace pal {
    namespace library {

        inline static int _Sprintf_helper(
            CHAR_t *buffer,
            int buffersize,
            const CHAR_t * _Format,
            va_list args)
        {
#if defined(_WINDOWS)
#if defined(__UTF16_STRINGS)
            return _x_vsnprintf_s(
#else
            return _vsnprintf_s(
#endif
                buffer,
                buffersize,
                _TRUNCATE,
                _Format,
                args);
#else
            return vsprintf(buffer,_Format,args);
#endif
        }

#if defined(_WINDOWS) && defined(__UTF16_STRINGS)
        inline static int _Sprintf_helper(
            char *buffer,
            int buffersize,
            const char * _Format,
            va_list args)
        {
            return _vsnprintf_s(
                buffer,
                buffersize,
                _TRUNCATE,
                _Format,
                args);
        }
#endif
        inline static foundation::string_t _Format_helper(const CHAR_t * _Format, ...)
        {
            CHAR_t buffer[256];

            va_list argList;
            va_start(argList, _Format);     /* Initialize variable arguments. */
            _Sprintf_helper(buffer, 256, _Format, argList);
            return buffer;
        }

        __unused_attribute__
        static void TraceMessage(const CHAR_t * _Format, ...)
        {
            CHAR_t buffer[2048];

            va_list argList;
            va_start(argList, _Format);     /* Initialize variable arguments. */
            _Sprintf_helper(buffer, 2048, _Format, argList);
            _pal_Trace(buffer);
        }
    }
}
