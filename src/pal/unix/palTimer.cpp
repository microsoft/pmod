/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:palTimer.cpp
****/
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#include "palApiInternal.h"


HRESULT _pal_SetTimer(
        _Inout_  UINT64& timerId,           // Timer ID
        _In_ ITimerCallback * callback,     // Object implementing ITimerCallback interface
        _In_ UINT32 timeOut)
{
    return S_OK;
}

HRESULT _pal_KillTimer(UINT64& timerId)
{
    return S_OK;
}

