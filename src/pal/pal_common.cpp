/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_common.cpp
****/

#include <pal/pal_thread.h>

EXTERN_C PAL_API void _pal_DelayUntil(UINT64 *t)
{
    UINT64 ticks = _pal_GetTickCount64();
    if (*t >= ticks)
    {
        UINT64 delay = *t - ticks;
        _pal_Delay((UINT32)delay);
    }
    else
    {
        *t = ticks;
    }
}
