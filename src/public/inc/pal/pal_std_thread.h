/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_std_thread.h
****/
#pragma once

#include <foundation/com_interfaces.h>

#include <thread>
#include "pal_thread.h"

typedef size_t _pal_ThreadId;

class _pal_ThreadHandle
{
public:
    _pal_ThreadHandle():
        _Pthread(nullptr)
    {
    }

    _pal_ThreadHandle(const _pal_ThreadHandle& other)
    {
        _Pthread = other._Pthread;
        _Pthread->AddRef();
    }

    _pal_ThreadHandle & operator= (const _pal_ThreadHandle & other)
    {
        if (this != &other) // protect against invalid self-assignment
        {
            if (_Pthread != nullptr)
            {
                _Pthread->Release();
            }
            _Pthread = other._Pthread;
            _Pthread->AddRef();
        }
        return *this;
    }
    ~_pal_ThreadHandle()
    {
        if (_Pthread != nullptr)
        {
            _Pthread->Release();
        }
    }
    foundation::IUnknown *_Pthread;
};


EXTERN_C PAL_API _pal_ThreadId _pal_CurrentThreadId();

EXTERN_C PAL_API void _pal_CreateThread(
    _pal_ThreadHandle *thread_handle,
    _pal_ThreadProc _threadFunc,
    LPVOID parameter);

EXTERN_C PAL_API void _pal_JoinThread(
    _pal_ThreadHandle *thread_handle);

EXTERN_C PAL_API void _pal_TerminateThread(
    _pal_ThreadHandle *thread_handle);


