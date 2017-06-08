/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_critical_section.h
****/
#pragma once

#include "pal_core.h"
#include "pal_api.h"

// Define a PAL abstraction for a critical section
#if defined(_WINDOWS)

#define _PAL_CRITICAL_SECTION CRITICAL_SECTION

#define _pal_InitializeCriticalSection(x,recursive)  InitializeCriticalSectionEx(x,0,0)
#define _pal_EnterCriticalSection       EnterCriticalSection
#define _pal_LeaveCriticalSection       LeaveCriticalSection
#define _pal_DeleteCriticalSection      DeleteCriticalSection


#elif defined(__GNUC__)

#include <pthread.h>

#define _PAL_CRITICAL_SECTION pthread_mutex_t

EXTERN_C PAL_API void _pal_InitializeCriticalSection(_PAL_CRITICAL_SECTION *, bool recursive);

#define _pal_EnterCriticalSection       pthread_mutex_lock
#define _pal_LeaveCriticalSection       pthread_mutex_unlock
#define _pal_DeleteCriticalSection      pthread_mutex_destroy

#else

#define _PAL_CRITICAL_SECTION LPVOID

EXTERN_C PAL_API void _pal_InitializeCriticalSection(_PAL_CRITICAL_SECTION *, bool recursive);
EXTERN_C PAL_API void _pal_EnterCriticalSection(_PAL_CRITICAL_SECTION *);
EXTERN_C PAL_API void _pal_LeaveCriticalSection(_PAL_CRITICAL_SECTION *);
EXTERN_C PAL_API void _pal_DeleteCriticalSection(_PAL_CRITICAL_SECTION *);

#endif
