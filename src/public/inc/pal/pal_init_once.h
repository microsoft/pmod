/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_init_once.h
****/
#pragma once

#include "pal_core.h"
#include "pal_api.h"

// Define a PAL abstraction for Init Once
#if defined(_WINDOWS)

#define _PAL_INIT_ONCE INIT_ONCE
#define _pal_InitializeInitOnce(x)  (x)->Ptr = nullptr
#define _pal_InitOnce(init_once_ptr,callback,parameter,context)  InitOnceExecuteOnce(init_once_ptr,callback,parameter,context)
#define _pal_DeleteInitOnce(x)  (x)->Ptr = nullptr

#else

#if defined(__GNUC__)

#include <pthread.h>

struct pthread_once_ex_s {
    int state;
    pthread_mutex_t mtx;
};
typedef struct pthread_once_ex_s _PAL_INIT_ONCE;


#else
#define _PAL_INIT_ONCE LPVOID

#endif

typedef
int
(STDMETHODCALLTYPE *_PAL_INIT_ONCE_FN) (
_PAL_INIT_ONCE *InitOnce,
PVOID Parameter,
PVOID *Context
);

EXTERN_C PAL_API void _pal_InitializeInitOnce(_PAL_INIT_ONCE *);
EXTERN_C PAL_API int _pal_InitOnce(_PAL_INIT_ONCE *, _PAL_INIT_ONCE_FN InitFn, PVOID parameter, PVOID *context);
EXTERN_C PAL_API void _pal_DeleteInitOnce(_PAL_INIT_ONCE *);


#endif

