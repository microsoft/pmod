/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_str.h
****/
#pragma once

#include "pal_core.h"
#include "pal_api.h"

// define pal string for utf 16/8
#ifdef __UTF16_STRINGS

#if defined(_WINDOWS)
#define _pal_strlen wcslen
#define _pal_strcmp wcscmp
#define _pal_strncmp wcsncmp
#define _pal_strtod wcstod 
#define _pal_strtol wcstol 
#define _pal_strtoul wcstoul 
#define _pal_strtoll _wcstoi64
#define _pal_strtoull _wcstoui64

#else
EXTERN_C PAL_API size_t _pal_strlen(LPCSTR_t _Str);
EXTERN_C PAL_API int _pal_strcmp(LPCSTR_t _Str1, LPCSTR_t _Str2);
#endif

#else

#include <stdlib.h>

#define _pal_strlen strlen
#define _pal_strcmp strcmp
#define _pal_strncmp strncmp
#define _pal_strtod strtod
#define _pal_strtol strtol 
#define _pal_strtoul strtoul
#define _pal_strtoll strtoll 
#define _pal_strtoull strtoull

#endif

// define pal string for wide characters

EXTERN_C PAL_API size_t _pal_wcslen(const wchar_t * _Str);
EXTERN_C PAL_API size_t _pal_wcstombs(char * s, size_t size,const wchar_t * pwcs, size_t n);
EXTERN_C PAL_API size_t _pal_mbstowcs(wchar_t * pwcs, size_t size,const char * s, size_t n);


