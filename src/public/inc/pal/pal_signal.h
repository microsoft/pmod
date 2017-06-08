/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_signal.h
****/
#pragma once

#include "pal_core.h"
#include "pal_api.h"


// Define a PAL abstraction for a signal
#if defined(_WINDOWS)

#define _PAL_SIGNAL HANDLE
#define _pal_CreateSignal(x)  *x = CreateEventExW(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE);
#define _pal_DeleteSignal(x)  CloseHandle(*x)
#define _pal_SetSignal(x)  SetEvent(*x)
#define _pal_WaitSignal(x) (WaitForSingleObjectEx(*x, INFINITE, false)==WAIT_OBJECT_0)
#define _pal_WaitSignalCond(x) (::WaitForSingleObjectEx(*x, 0,false) != WAIT_TIMEOUT)
#define _pal_WaitSignalTimed(x,waitTime) (::WaitForSingleObjectEx(*x, waitTime, false) == WAIT_OBJECT_0)

#else
#if defined(__GNUC__)

#include <pthread.h>

struct pthread_signal {

    // The POSIX resources used:
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    bool isSet;
};
typedef struct pthread_signal _PAL_SIGNAL;

#else
#define _PAL_SIGNAL LPVOID
#endif

EXTERN_C PAL_API void _pal_CreateSignal(_PAL_SIGNAL *);
EXTERN_C PAL_API void _pal_DeleteSignal(_PAL_SIGNAL *);
EXTERN_C PAL_API void _pal_SetSignal(_PAL_SIGNAL *);
EXTERN_C PAL_API void _pal_WaitSignal(_PAL_SIGNAL *);

EXTERN_C PAL_API bool _pal_WaitSignalCond(_PAL_SIGNAL *);
EXTERN_C PAL_API bool _pal_WaitSignalTimed(_PAL_SIGNAL *,UINT32 waitTime);


#endif

