/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:mbstowcs.cpp
****/
#include <stdlib.h>

size_t   mbstowcs(wchar_t *pwcs, const char *s, size_t n)
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
