/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_wchar_t_crt.cpp
****/
#include "pch.h"

#include "pal/pal_str.h"

#include <wchar.h>
#include <stdlib.h>

EXTERN_C PAL_API size_t _pal_wcslen(const wchar_t * _Str)
{
    return wcslen(_Str);
}

EXTERN_C PAL_API size_t _pal_wcstombs(char * s, size_t size, const wchar_t * pwcs, size_t n)
{
#if defined(_WINDOWS)
    return (size_t)WideCharToMultiByte(CP_UTF8, 0, pwcs, (int)n, s, (int)size, nullptr, nullptr);
#else
    return wcstombs(s, pwcs, n);
#endif
}

EXTERN_C PAL_API size_t _pal_mbstowcs(wchar_t * pwcs, size_t size, const char * s, size_t n)
{
#if defined(_WINDOWS)
    return (size_t)MultiByteToWideChar(CP_UTF8, 0, s, (int)n, pwcs, (int)size);
#else
    return mbstowcs(pwcs, s, n);
#endif
}
