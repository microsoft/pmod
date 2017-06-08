/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_semaphore.h
****/
#pragma once

#include "pal_core.h"
#include "pal_types.h"
#include "pal_api.h"

// Define a PAL abstraction for a signal
#if defined(_WINDOWS)

#define _PAL_SEMAPHORE HANDLE

#define _pal_CreateSemaphore(x,InitialValue,MaxValue)  *x = ::CreateSemaphoreEx(NULL,InitialValue,MaxValue,NULL,0,SEMAPHORE_ALL_ACCESS)
#define _pal_DeleteSemaphore(x)  CloseHandle(*x)
#define _pal_ReleaseSemaphore(x,ReleaseCount,PrevCount) ReleaseSemaphore(*x,ReleaseCount,(LPLONG)PrevCount)

#define _pal_WaitSemaphore(sema) WaitForSingleObjectEx(*sema, INFINITE, false)
#define _pal_WaitCondSemaphore(sema) (::WaitForSingleObjectEx(*sema, 0, false) != WAIT_TIMEOUT)
#define _pal_WaitTimedSemaphore(sema,waitTime) (::WaitForSingleObjectEx(*sema, waitTime, false) == WAIT_OBJECT_0)

#elif defined(__GNUC__)

#ifdef __MACH__
#include <mach/semaphore.h>
#include <mach/mach.h>
#include <sys/time.h>

#define _PAL_SEMAPHORE semaphore_t

#define _pal_CreateSemaphore(sema,InitialValue,MaxValue) semaphore_create(mach_task_self(),sema,SYNC_POLICY_FIFO,InitialValue)
#define _pal_DeleteSemaphore(sema)  semaphore_destroy(mach_task_self(),*sema)

inline void _pal_WaitSemaphore(_PAL_SEMAPHORE *sema)
{
    while(1)
    {
        kern_return_t err = semaphore_wait(*sema);
        if(err == KERN_SUCCESS)
        {
            break;
        }
    }
}

inline bool _pal_WaitTimedSemaphore(_PAL_SEMAPHORE *sema,UINT32 wait_time)
{
    mach_timespec_t wait_time_t;

    wait_time_t.tv_sec = wait_time / 1000;
    wait_time_t.tv_nsec = (wait_time % 1000) * 1000000;

    return semaphore_timedwait(*sema,wait_time_t) == KERN_SUCCESS;
}

inline bool _pal_WaitCondSemaphore(_PAL_SEMAPHORE *sema)
{
    return _pal_WaitTimedSemaphore(sema,0);
}

inline void _pal_ReleaseSemaphore(_PAL_SEMAPHORE *sema,ULONG ReleaseCount,ULONG *PrevCount)
{
    while(ReleaseCount--)
    {
        semaphore_signal(*sema);
    }
}

#else
#include <semaphore.h>

#define _PAL_SEMAPHORE sem_t
#define _pal_CreateSemaphore(sema,InitialValue,MaxValue) sem_init(sema,0,InitialValue)
#define _pal_DeleteSemaphore(sema)  sem_destroy(sema)


inline bool _pal_WaitTimedSemaphore(_PAL_SEMAPHORE *sema,UINT32 wait_time)
{
    struct timespec abs;
    clock_gettime(CLOCK_REALTIME, &abs);
    
    long int nsec = (wait_time%1000)*1000000;
    
    abs.tv_sec += wait_time/1000 + ((abs.tv_nsec + nsec) / 1000000000);
    abs.tv_nsec = (abs.tv_nsec + nsec) % 1000000000;
    
    return sem_timedwait (sema,&abs)==0;
}

inline void _pal_WaitSemaphore(_PAL_SEMAPHORE *sema)
{
    sem_wait(sema);
}

inline bool _pal_WaitCondSemaphore(_PAL_SEMAPHORE *sema)
{
    return sem_trywait(sema) == 0;
}

inline void _pal_ReleaseSemaphore(_PAL_SEMAPHORE *sema,ULONG ReleaseCount,ULONG *PrevCount)
{
    while(ReleaseCount--)
    {
        sem_post(sema);
    }
}

#endif

#else

#define _PAL_SEMAPHORE LPVOID

EXTERN_C PAL_API void _pal_CreateSemaphore(_PAL_SEMAPHORE *, ULONG InitialValue, ULONG MaxValue);
EXTERN_C PAL_API void _pal_DeleteSemaphore(_PAL_SEMAPHORE *);
EXTERN_C PAL_API void _pal_ReleaseSemaphore(_PAL_SEMAPHORE *, ULONG ReleaseCount, ULONG *PrevCount);
EXTERN_C PAL_API void _pal_WaitSemaphore(_PAL_SEMAPHORE *);
EXTERN_C PAL_API bool _pal_WaitCondSemaphore(_PAL_SEMAPHORE *);
EXTERN_C PAL_API bool _pal_WaitTimedSemaphore(_PAL_SEMAPHORE *, UINT32 wait_time);

#endif
