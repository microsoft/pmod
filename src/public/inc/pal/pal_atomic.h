/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_atomic.h
****/
#pragma once

#include "pal_api.h"
#include "pal_core.h"
#include "pal_types.h"

#if defined(_WINDOWS)
typedef ULONG _pal_AtomicIntegerType;
#else
typedef int _pal_AtomicIntegerType;
#endif

#if __LP64__ || _WIN64
typedef UINT64 _pal_AtomicValueType;
#else
typedef UINT32 _pal_AtomicValueType;
#endif


#if defined(_WINDOWS)

#define _pal_InterlockedIncrement(x)       ::InterlockedIncrement(x)
#define _pal_InterlockedDecrement(x)       ::InterlockedDecrement(x)
#define _pal_InterlockedCompareExchange(destination, exchange, comparand) \
    ::InterlockedCompareExchange(destination, exchange, comparand)
#define _pal_atomic_exchange(destination, exchange) \
    ::InterlockedExchange(destination, exchange)

#elif defined(ANDROID)
#include <sys/atomics.h>
#define _pal_InterlockedIncrement(x)  (__atomic_inc(x) + 1)
#define _pal_InterlockedDecrement(x)  (__atomic_dec(x) - 1)
#define _pal_atomic_exchange(destination, exchange)  __atomic_swap(exchange, destination)

__ATOMIC_INLINE__ int
_pal_InterlockedCompareExchange(volatile int* destination, int exchange, int comparand)
{
    return __sync_val_compare_and_swap(destination, comparand, exchange);
}

#elif defined(__GNUC__)

#if defined(_USE_PAL_ATOMICITY)
#include <ext/atomicity.h>
#define _pal_InterlockedIncrement(x)  (__gnu_cxx::__exchange_and_add(x, 1) + 1)
#define _pal_InterlockedDecrement(x)  (__gnu_cxx::__exchange_and_add(x, -1) - 1)
#else
#define _pal_InterlockedIncrement(x)  (__sync_fetch_and_add(x, 1) + 1)
#define _pal_InterlockedDecrement(x)  (__sync_fetch_and_sub(x, 1) - 1)
#endif

#define _pal_InterlockedCompareExchange(destination, exchange, comparand) \
    __sync_val_compare_and_swap(destination, comparand, exchange)
#define _pal_atomic_exchange(destination, exchange) \
    __sync_lock_test_and_set(destination, exchange)

#else

EXTERN_C PAL_API _pal_AtomicIntegerType _pal_InterlockedIncrement(volatile _pal_AtomicIntegerType*);
EXTERN_C PAL_API _pal_AtomicIntegerType _pal_InterlockedDecrement(volatile _pal_AtomicIntegerType*);
EXTERN_C PAL_API _pal_AtomicIntegerType _pal_InterlockedCompareExchange(
    volatile _pal_AtomicIntegerType *destination,
    _pal_AtomicIntegerType exchange,
    _pal_AtomicIntegerType comparand);
EXTERN_C PAL_API _pal_AtomicIntegerType _pal_atomic_exchange(
    volatile _pal_AtomicIntegerType *destination,
    _pal_AtomicIntegerType exchange);

#endif
