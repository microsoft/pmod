/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_trace_printf.cpp
****/
#include "foundation/pal.h"

#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>

void _pal_Trace(LPCSTR_t message)
{
#ifdef __UTF16_STRINGS
    std::string buffer;
    size_t size = _pal_wcslen(message);

    buffer.reserve(size + 1);
    _pal_wcstombs(&buffer[0], message, size);
    printf("%s", buffer.c_str());
#else
    printf("%s", message);
#endif
    fflush(stdout);
}
