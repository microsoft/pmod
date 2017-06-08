/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationExports.cpp
****/
#include <foundation/interfaces/property_value.h>
#include <foundation/macros.h>

#include "hstring.h"

#ifdef _WINDOWS

// declare Windows runtime String API
STDAPI WindowsCreateString(LPCTSTR sourceString,UINT32 length,HSTRING *string);
STDAPI WindowsDeleteString(HSTRING string);
STDAPI WindowsDuplicateString(HSTRING string,HSTRING *newString);
STDAPI_(UINT32) WindowsGetStringLen(HSTRING string);
STDAPI_(LPCTSTR)WindowsGetStringRawBuffer(HSTRING string,UINT32 *length);
STDAPI_(int) WindowsIsStringEmpty(HSTRING string);
STDAPI WindowsCompareStringOrdinal(HSTRING string1,HSTRING string2,INT32 *result);
STDAPI WindowsCreateStringReference(LPCTSTR sourceString,UINT32 length,HSTRING_HEADER *hstringHeader,HSTRING  *string);

#endif

#if defined(_WINDOWS) && defined(__UTF16_STRINGS)
// comment this next line if you want to force no winRT HSTRING
// if available
#define _USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE

// extern method to find out if the winrt runtime is available on this machine
extern bool IsWindowsRuntimeAvailable();

#if defined(DEBUG)
static bool _windowsRuntimeHStringEnabled = true;

EXTERN_C FOUNDATION_API HRESULT STDAPICALLTYPE SetWindowsRuntimeHStringEnabled(bool value)
{
    _windowsRuntimeHStringEnabled = value;
    return S_OK;
}
#endif

#if defined(DEBUG)
#define _IsWindowsRuntimeHStringEnabled \
    (_windowsRuntimeHStringEnabled && IsWindowsRuntimeAvailable())
#else
#define _IsWindowsRuntimeHStringEnabled \
    IsWindowsRuntimeAvailable()
#endif

#endif

EXTERN_C HRESULT CreatePropertyValueStaticsInternal(_COM_Outptr_ foundation::IPropertyValueStatics **ppPropertyValueStatics);


EXTERN_C FOUNDATION_API HRESULT  STDAPICALLTYPE GetPropertyValueStatics(
    _COM_Outptr_ foundation::IPropertyValueStatics **ppPropertyValueStatics)
{
	IFCPTR_ASSERT(ppPropertyValueStatics);
    return CreatePropertyValueStaticsInternal(ppPropertyValueStatics);
}

EXTERN_C PAL_API HRESULT _pal_CreateString(LPCSTR_t sourceString, UINT32 length, HSTRING *string)
{
#if defined(_USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE)
    if (_IsWindowsRuntimeHStringEnabled)
    {
        return WindowsCreateString(sourceString,length,string);
    }
#endif
    return CreateString(sourceString,length,string);
}

EXTERN_C PAL_API HRESULT _pal_DeleteString(HSTRING string)
{
#if defined(_USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE)
    if (_IsWindowsRuntimeHStringEnabled)
    {
        return WindowsDeleteString(string);
    }
#endif
    return DeleteString(string);
}

EXTERN_C PAL_API  HRESULT _pal_DuplicateString(HSTRING string, HSTRING *newString)
{
#if defined(_USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE)
    if (_IsWindowsRuntimeHStringEnabled)
    {
        return WindowsDuplicateString(string,newString);
    }
#endif
    return DuplicateString(string,newString);
}

EXTERN_C PAL_API  UINT32 _pal_GetStringLen(HSTRING string)
{
#if defined(_USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE)
    if (_IsWindowsRuntimeHStringEnabled)
    {
        return WindowsGetStringLen(string);
    }
#endif
    return GetStringLen(string);
}

EXTERN_C PAL_API  LPCSTR_t _pal_GetStringRawBuffer(HSTRING string, UINT32 *length)
{
#if defined(_USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE)
    if (_IsWindowsRuntimeHStringEnabled)
    {
        return WindowsGetStringRawBuffer(string,length);
    }
#endif
    return GetStringRawBuffer(string,length);
}

EXTERN_C PAL_API  bool _pal_IsStringEmpty(HSTRING string)
{
#if defined(_USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE)
    if (_IsWindowsRuntimeHStringEnabled)
    {
        return WindowsIsStringEmpty(string) ? true:false;
    }
#endif
    return IsStringEmpty(string);
}

EXTERN_C PAL_API  HRESULT _pal_CompareStringOrdinal(HSTRING string1, HSTRING string2, INT32 *result)
{
#if defined(_USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE)
    if (_IsWindowsRuntimeHStringEnabled)
    {
        return WindowsCompareStringOrdinal(string1,string2,result);
    }
#endif
    return CompareStringOrdinal(string1,string2,result);
}

EXTERN_C PAL_API  HRESULT _pal_CreateStringReference(LPCSTR_t sourceString, UINT32 length, HSTRING_HEADER * hstringHeader, HSTRING * string)
{
#if defined(_USE_WINDOWS_RUNTIME_HSTRING_IF_AVAILABLE)
    if (_IsWindowsRuntimeHStringEnabled)
    {
        return WindowsCreateStringReference(sourceString,length,hstringHeader,string);
    }
#endif
    return CreateStringReference(sourceString,length,hstringHeader,string);
}
