/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_hstring.h
****/
#pragma once

#include "pal_types.h"
#include "pal_core.h"
#include "pal_api.h"

#if defined(_WINDOWS)
#include <hstring.h>
#else

// Declaring a handle dummy struct for HSTRING the same way DECLARE_HANDLE does.
typedef struct HSTRING__{
    int unused;
} HSTRING__;

// Declare the HSTRING handle for C/C++
typedef HSTRING__* HSTRING;

// Declare the HSTRING_HEADER
typedef struct HSTRING_HEADER
{
    union{
        PVOID Reserved1;
#if defined(_WIN64)
        char Reserved2[24];
#else
        char Reserved2[20];
#endif
    } Reserved;
} HSTRING_HEADER;

#endif

#if defined(_WINDOWS_RUNTIME) && defined(__UTF16_STRINGS)

// String related API
#define _pal_CreateString(sourceString,length,string)       ::WindowsCreateString(sourceString,length,string)
#define _pal_DeleteString(string)                           ::WindowsDeleteString(string)
#define _pal_DuplicateString(string,newString)              ::WindowsDuplicateString(string,newString)
#define _pal_GetStringLen(string)                           ::WindowsGetStringLen(string)
#define _pal_GetStringRawBuffer(string,length)              ::WindowsGetStringRawBuffer(string,length)
#define _pal_IsStringEmpty(string)                          (::WindowsIsStringEmpty(string) ? true:false)
#define _pal_CompareStringOrdinal(string1,string2,result)   ::WindowsCompareStringOrdinal(string1,string2,result)
#define _pal_CreateStringReference(sourceString,length,hstringHeader,string)   ::WindowsCreateStringReference(sourceString,length,hstringHeader,string)

#else

EXTERN_C PAL_API  HRESULT _pal_CreateString(LPCSTR_t sourceString, UINT32 length, HSTRING *hstring);
EXTERN_C PAL_API  HRESULT _pal_DeleteString(HSTRING hstring);
EXTERN_C PAL_API  HRESULT _pal_DuplicateString(HSTRING hstring, HSTRING *newString);
EXTERN_C PAL_API  UINT32 _pal_GetStringLen(HSTRING hstring);
EXTERN_C PAL_API  LPCSTR_t _pal_GetStringRawBuffer(HSTRING hstring, UINT32 *length);
EXTERN_C PAL_API  bool _pal_IsStringEmpty(HSTRING hstring);
EXTERN_C PAL_API  HRESULT _pal_CompareStringOrdinal(HSTRING hstring1, HSTRING hstring2, INT32 *result);
EXTERN_C PAL_API  HRESULT _pal_CreateStringReference(LPCSTR_t sourceString, UINT32 length, HSTRING_HEADER * hstringHeader, HSTRING * hstring);

#endif

#ifndef _USE_NEW_MEM_ALLOCATOR
#define _USE_NEW_MEM_ALLOCATOR 0
#endif

#if _USE_NEW_MEM_ALLOCATOR

inline void *_pal_MemAlloc(UINT32 size) { return new byte[size]; }
inline void _pal_MemFree(void *p) { operator delete(p); }

#else
#if defined(_WINDOWS)

#include <Objbase.h>

// Alloc related API
#define _pal_MemAlloc(x)                ::CoTaskMemAlloc(x)
#define _pal_MemFree(x)                 ::CoTaskMemFree(x)

#else

// Alloc/Free
EXTERN_C PAL_API LPVOID _pal_MemAlloc(ULONG size);
EXTERN_C PAL_API void _pal_MemFree(LPVOID);

#endif
#endif

#if defined(_WINDOWS)

// Trace support
#define _pal_Trace                                           OutputDebugString

#else

EXTERN_C PAL_API void _pal_Trace(LPCSTR_t message);

#endif
