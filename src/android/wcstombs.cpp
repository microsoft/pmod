/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:wcstombs.cpp
****/

#include <stdlib.h>

size_t	wcstombs(char *s, const wchar_t *pwcs, size_t n)
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
