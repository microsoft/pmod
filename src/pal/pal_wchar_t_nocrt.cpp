/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_wchar_t_nocrt.cpp
****/
#include "foundation/pal.h"

#include <wchar.h>
#include <stdlib.h>
#include <string>

size_t _pal_wcslen(const wchar_t * _Str)
{
    const wchar_t *eos = _Str;

    while( *eos++ ) ;

    return( (size_t)(eos - _Str - 1) );
}

int _pal_wcscmp(const wchar_t * _Str1,const wchar_t * _Str2)
{
    int ret = 0 ;

    while( ! (ret = (int)(*_Str1 - *_Str2)) && *_Str2)
            ++_Str1, ++_Str2;

    if ( ret < 0 )
            ret = -1 ;
    else if ( ret > 0 )
            ret = 1 ;

    return( ret );
}

int _pal_wcscpy_s(wchar_t * _Dst,size_t _SizeInWords,const wchar_t * _Src)
{
    wchar_t *start = _Dst;

    while (_SizeInWords && (*_Dst++ = *_Src++))    /* copy string */
            _SizeInWords--;

    if (_SizeInWords)                              /* pad out with zeroes */
            while (--_SizeInWords)
                    *_Dst++ = '\0';

    return (int)(_Dst - start);
}

size_t _pal_wcstombs(char * s, const wchar_t * pwcs, size_t n)
{
    size_t count = 0;

    while(count < n)
    {
        if (*pwcs > 255)  /* validate high byte */
        {
            //errno = EILSEQ;
            return (size_t)-1;  /* error */
        }
        s[count] = (char) *pwcs;
        if (*pwcs++ == L'\0')
        {
            break;
        }
        count++;
    }
    /* ensure the string is null terminated */
    s[count] = 0;
    return count;
}

size_t _pal_mbstowcs(wchar_t * pwcs, const char * s, size_t n)
{
    size_t count = 0;

    /* C locale: easy and fast */
    while (count < n)
    {
        *pwcs = (wchar_t) ((unsigned char)s[count]);
        if (!s[count])
        {
            break;
        }
        count++;
        pwcs++;
    }
    /* ensure the string is null terminated */
    *pwcs = '\0';

    return count;
}
