/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:hstring.h
****/
#ifndef _HSTRING_DEFINED
#define _HSTRING_DEFINED

#include "foundation/pal.h"


HRESULT CreateString(LPCSTR_t sourceString,UINT32 length,HSTRING *string);
HRESULT DeleteString(HSTRING string);
HRESULT DuplicateString(HSTRING string,HSTRING *newString);
UINT32 GetStringLen(HSTRING string);
LPCSTR_t GetStringRawBuffer(HSTRING string,UINT32 *length);
bool IsStringEmpty(HSTRING string);
HRESULT CompareStringOrdinal(HSTRING string1, HSTRING string2, INT32 *result);
HRESULT CreateStringReference(LPCSTR_t sourceString,UINT32 length,HSTRING_HEADER * hstringHeader,HSTRING * string);

#endif
