/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_thread.h
****/
#pragma once

#include "pal_core.h"
#include "pal_types.h"
#include "pal_api.h"

#include <assert.h>

typedef LPVOID(STDAPICALLTYPE *_pal_ThreadProc)(LPVOID parameter);

#if defined(_WINDOWS)


#define _pal_GetCurrentProcessId()                            ::GetCurrentProcessId()

#if defined(_WINDOWS_CLASSIC)

typedef UINT32 _pal_ThreadId;
typedef HANDLE _pal_ThreadHandle;

#define _pal_CurrentThread()                                  ::GetCurrentThread()
#define _pal_CurrentThreadId()                                ::GetCurrentThreadId()

#define _pal_CreateThread(x,pThreadProc,lpParameter) *x = ::CreateThread( \
    NULL,\
    0,\
    (LPTHREAD_START_ROUTINE)pThreadProc,\
    lpParameter,\
    0,\
    NULL)
#define _pal_TerminateThread(x) ::TerminateThread(*x, 0);
#define _pal_JoinThread(x) ::WaitForSingleObjectEx(*x,INFINITE,false)
#define _pal_TerminateThread(x) ::TerminateThread(*x, 0);

#else

#include "pal_std_thread.h"

#endif

#elif defined(__GNUC__)

#include <pthread.h>

typedef pthread_t _pal_ThreadHandle;
typedef UINT64 _pal_ThreadId;

#include <unistd.h>
#include <pthread.h>

#define _pal_GetCurrentProcessId()                            (LONG)getpid()
#define _pal_CurrentThread()                                  pthread_self()
#define _pal_CurrentThreadId()                                (UINT64)pthread_self()

#define _pal_CreateThread(x,pThreadProc,lpParameter) pthread_create( \
    x,\
    NULL,\
    pThreadProc,\
    lpParameter)

#define _pal_TerminateThread(x) pthread_cancel(*x);
#define _pal_JoinThread(x) pthread_join(*x,NULL)


#else
typedef UINT32 _pal_ThreadId;

EXTERN_C PAL_API ULONG _pal_GetCurrentProcessId();
EXTERN_C PAL_API _pal_ThreadId _pal_CurrentThreadId();

#endif


// Define a PAL abstraction for GetTickCount64
#if defined(_WINDOWS)

#define _pal_GetTickCount64()                            ::GetTickCount64()

#else
EXTERN_C PAL_API UINT64 _pal_GetTickCount64();
#endif

// Define a PAL abstraction for Delay
#if defined(_WINDOWS_CLASSIC)
#define _pal_Delay(ms)                            ::Sleep(ms)
#else
EXTERN_C PAL_API void _pal_Delay(UINT32 ms);
#endif

// Define a PAL abstraction for DelayUntil
EXTERN_C PAL_API void _pal_DelayUntil(UINT64 *t);

