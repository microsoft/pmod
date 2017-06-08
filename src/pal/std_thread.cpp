/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:std_thread.cpp
****/
#include "pch.h"

#include <pal/pal_thread.h>
#include <foundation/ctl/com_library.h>

class _pal_thread_handle_class_t:
    public foundation::ctl::ComBase
{
public:
    _pal_thread_handle_class_t()
    {
    }

    ~_pal_thread_handle_class_t()
    {
        if (_thread.joinable())
        {
            _thread.detach();
        }
    }

    inline std::thread& get_thread()
    {
        return _thread;
    }

    inline void _Swap(std::thread& other)
    {
        _thread.swap(other);
    }

    inline void _Join()
    {
        _thread.join();
    }

    inline void _Terminate()
    {
        // out of scope will terminate the thread
        std::thread thread;
        _thread.swap(thread);
    }
private:
    std::thread _thread;
};

EXTERN_C PAL_API _pal_ThreadId _pal_CurrentThreadId()
{
    return std::hash<std::thread::id>()(std::this_thread::get_id());
}

EXTERN_C PAL_API void _pal_CreateThread(
    _pal_ThreadHandle *thread_handle,
    _pal_ThreadProc _threadFunc,
    LPVOID parameter)
{
    assert(thread_handle != nullptr);
    assert(thread_handle->_Pthread == nullptr);
    std::thread thread(_threadFunc, parameter);

    _pal_thread_handle_class_t *pthread;
    foundation::ctl::ComObject<_pal_thread_handle_class_t>::CreateInstance(&pthread);

    thread.swap(pthread->get_thread());
    thread_handle->_Pthread = pthread;
}

EXTERN_C PAL_API void _pal_JoinThread(
    _pal_ThreadHandle *thread_handle)
{
    assert(thread_handle != nullptr);
    assert(thread_handle->_Pthread != nullptr);

    (static_cast<_pal_thread_handle_class_t*>(thread_handle->_Pthread))->_Join();
}

EXTERN_C PAL_API void _pal_TerminateThread(
    _pal_ThreadHandle *thread_handle)
{
    assert(thread_handle != nullptr);
    assert(thread_handle->_Pthread != nullptr);

    (static_cast<_pal_thread_handle_class_t*>(thread_handle->_Pthread))->_Terminate();
}

EXTERN_C PAL_API void _pal_Delay(UINT32 ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
