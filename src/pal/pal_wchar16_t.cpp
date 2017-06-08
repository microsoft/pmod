/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_wchar16_t.cpp
****/
#include "foundation/pal.h"

#include "wc16.h"

size_t _pal_wcslen(const wchar_t * _Str)
{
    return wc16::wcslen(_Str);
}

int _pal_wcscmp(const wchar_t * _Str1,const wchar_t * _Str2)
{
    return wc16::wcscmp(_Str1,_Str2);
}
int _pal_wcscpy_s(wchar_t * _Dst,size_t _SizeInWords,const wchar_t * _Src)
{
    wc16::wcscpy(_Dst,_Src);
    return 0;
}

size_t _pal_wcstombs(char * s, const wchar_t * pwcs, size_t n)
{
    return wc16::wcstombs(s,pwcs,n);
}

size_t _pal_mbstowcs(wchar_t * pwcs, const char * s, size_t n)
{
    return wc16::mbstowcs(pwcs,s,n);
}
