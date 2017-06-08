/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_str.h
****/
#pragma once

#include <pal/pal_hstring.h>
#include <string.h>

// duplicate a string using PAL allocation API
template <class char_t,class _Func>
char_t *_pal_strdup(const char_t * s, _Func func)
{
    size_t alloc_size = (func(s) + 1) * sizeof(char_t);
    char_t *result = (char_t *)_pal_MemAlloc((UINT32)alloc_size);
    if (result == nullptr)
    {
        return nullptr;
    }

    memcpy((LPVOID)result, (LPVOID)s, alloc_size);
    return result;
}

inline static CHAR_t *_pal_strdup(const CHAR_t * s)
{
    return _pal_strdup(s,_pal_strlen);
}

// define _pal_strcpy_s
#ifdef __UTF16_STRINGS

#define _pal_strcpy_s wcscpy_s

// define wcscpy_s for non Windows platforms
#if !defined(_WINDOWS)

inline wchar_t* wcscpy_s (wchar_t* destination,int len, const wchar_t* source)
{
    return wcscpy(destination,source);
}
#endif

#else // !__UTF16_STRINGS
#define _pal_strcpy_s strcpy_s

// define strcpy_s for non Windows platforms
#if !defined(_WINDOWS)
inline char* strcpy_s (char* destination,int len, const char* source)
{
    return strcpy(destination,source);
}
#endif

#endif

