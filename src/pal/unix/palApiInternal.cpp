/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:palApiInternal.cpp
****/
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#include <foundation/pal.h>

#include <pal/pal_init_once.h>
#include <pal/pal_signal.h>
#include <pal/pal_critical_section.h>
#include <pal/pal_thread.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

void get_current_timespec(struct timespec& ts)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;

#else
    clock_gettime(CLOCK_REALTIME, &ts);
#endif
}

EXTERN_C PAL_API void _pal_CreateSignal(_PAL_SIGNAL *pSignal)
{
    pSignal->isSet = false;
    pthread_mutex_init(&pSignal->mutex, NULL);
    pthread_cond_init(&pSignal->condition, NULL);
}

EXTERN_C PAL_API void _pal_DeleteSignal(_PAL_SIGNAL *pSignal)
{
    int success = pthread_mutex_unlock(&pSignal->mutex);
    assert(success == 0);

    pthread_cond_destroy(&pSignal->condition);
    pthread_mutex_destroy(&pSignal->mutex);
}

EXTERN_C PAL_API void _pal_SetSignal(_PAL_SIGNAL *pSignal)
{
    int success = pthread_mutex_lock(&pSignal->mutex);
    assert(success == 0);

    pSignal->isSet = true;

    success = pthread_cond_broadcast(&pSignal->condition);
    assert(success == 0);

    success = pthread_mutex_unlock(&pSignal->mutex);
    assert(success == 0);
}

EXTERN_C PAL_API void _pal_WaitSignal(_PAL_SIGNAL *pSignal)
{
    int success = pthread_mutex_lock(&pSignal->mutex);
    assert(success == 0);

    // pthread_cond_wait waits even if pthread_cond_broadcast was called,
    // so we must check the condition variable.
    if (!pSignal->isSet)
    {
        success = pthread_cond_wait(&pSignal->condition, &pSignal->mutex);
        assert(success == 0);
    }

    success = pthread_mutex_unlock(&pSignal->mutex);
    assert(success == 0);
}

EXTERN_C PAL_API bool _pal_WaitSignalTimed(_PAL_SIGNAL *pSignal,UINT32 wait_time)
{
    int success = pthread_mutex_lock(&pSignal->mutex);
    assert(success == 0);

    if (!pSignal->isSet)
    {
        struct timespec abs;
        get_current_timespec(abs);
        
        long int nsec = (wait_time%1000)*1000000;
        
        abs.tv_sec += wait_time/1000 + ((abs.tv_nsec + nsec) / 1000000000);
        abs.tv_nsec = (abs.tv_nsec + nsec) % 1000000000;
        
        success = pthread_cond_timedwait(
                &pSignal->condition,
                &pSignal->mutex,
                &abs);
    }
    
    pthread_mutex_unlock(&pSignal->mutex);
    
    return success == 0;
}

EXTERN_C PAL_API bool _pal_WaitSignalCond(_PAL_SIGNAL *pSignal)
{
    return _pal_WaitSignalTimed(pSignal,0);
}

EXTERN_C PAL_API void _pal_InitializeCriticalSection(_PAL_CRITICAL_SECTION *pCriticalSection,bool recursive)
{
    pthread_mutexattr_t   mta;

    if(recursive)
    {
        pthread_mutexattr_init(&mta);
        pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
    }

    pthread_mutex_init(pCriticalSection, recursive ? &mta:nullptr);
}

EXTERN_C PAL_API void _pal_InitializeInitOnce(_PAL_INIT_ONCE *InitOnce)
{
    InitOnce->state = 0;
    pthread_mutex_init(&InitOnce->mtx,0);
}

EXTERN_C PAL_API int _pal_InitOnce(_PAL_INIT_ONCE *InitOnce,_PAL_INIT_ONCE_FN InitFn,PVOID parameter,PVOID *context)
{
    int result = TRUE;
    pthread_mutex_lock(&InitOnce->mtx);
    if(!InitOnce->state)
    {
        result = InitFn(InitOnce,parameter,context);
        InitOnce->state = 1;
    }
    pthread_mutex_unlock(&InitOnce->mtx);
    return result;
}

EXTERN_C PAL_API void _pal_DeleteInitOnce(_PAL_INIT_ONCE *InitOnce)
{
    pthread_mutex_destroy(&InitOnce->mtx);
}

EXTERN_C PAL_API UINT64 _pal_GetTickCount64()
{
    struct timeval now;
    gettimeofday(&now, nullptr);
    
	UINT64 usecs = (((UINT64)now.tv_sec) * 1000000)  + now.tv_usec;
    return usecs / 1000;
}


EXTERN_C PAL_API void _pal_Delay(UINT32 time)
{
	struct timespec req;
	req.tv_sec = time/1000;
    req.tv_nsec = (time%1000)*1000000;	// ms -> nano
	nanosleep(&req,NULL);
}

