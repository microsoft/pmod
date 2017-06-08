/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TimerService.cpp
****/
#pragma once

#include "palTimer.h"

#include "windows.h"

#include <foundation/library/critical_section_lock.h>
#include <foundation/com_ptr.h>
#include <map>

class CTimerService
{
public:
    static CTimerService& GetInstance()
    {
        static CTimerService instance;
        return instance;
    }

    HRESULT SetTimer(
        _Inout_  UINT64& timerId,           // Timer ID
        _In_ ITimerCallback * callback,     // Object implementing ITimerCallback interface
        _In_ UINT32 timeOut)
    {
        foundation_assert(callback);
		foundation::library::CriticalSectionLock lock(_cs);

        if (timerId)
        {
            TimerInfoMapType::iterator iter = _timerInfoMap.find(timerId);
            if (iter != _timerInfoMap.end())
            {
                ::SetTimer(nullptr, (UINT_PTR)timerId, timeOut, _TimerServiceCallback);
            }
        }
        else
        {
            timerId = ::SetTimer(nullptr, 0, timeOut, _TimerServiceCallback);
            _timerInfoMap[timerId] = callback;
        }
        return S_OK;
    }

    HRESULT KillTimer(UINT64& timerId)
    {
		foundation::library::CriticalSectionLock lock(_cs);

        TimerInfoMapType::iterator iter = _timerInfoMap.find(timerId);
        if (iter != _timerInfoMap.end())
        {
            ::KillTimer(nullptr, (UINT_PTR)timerId);
            _timerInfoMap.erase(iter);
            timerId = 0;
        }
        return S_OK;
    }
private:
    void _TimerServiceCallbackInternal(
        _In_  HWND hwnd,
        _In_  UINT uMsg,
        _In_  UINT_PTR idEvent,
        _In_  DWORD dwTime)
    {
        UNREFERENCED_PARAMETER(hwnd);
        UNREFERENCED_PARAMETER(uMsg);
        UNREFERENCED_PARAMETER(dwTime);

		foundation::library::CriticalSectionLock lock(_cs);

        TimerInfoMapType::iterator iter = _timerInfoMap.find(idEvent);
        if (iter != _timerInfoMap.end())
        {
            iter->second->OnTimeout(idEvent);
        }
    }

    static void CALLBACK _TimerServiceCallback(
        _In_  HWND hwnd,
        _In_  UINT uMsg,
        _In_  UINT_PTR idEvent,
        _In_  DWORD dwTime)
    {
        GetInstance()._TimerServiceCallbackInternal(hwnd, uMsg, idEvent, dwTime);
    }

private:
    typedef std::map<UINT64, foundation::ComPtr<ITimerCallback>> TimerInfoMapType;
    TimerInfoMapType _timerInfoMap;
    pal::library::CriticalSection _cs;
};

HRESULT _pal_SetTimer(
        _Inout_  UINT64& timerId,           // Timer ID
        _In_ ITimerCallback * callback,     // Object implementing ITimerCallback interface
        _In_ UINT32 timeOut)
{
    return CTimerService::GetInstance().SetTimer(timerId, callback, timeOut);
}

HRESULT _pal_KillTimer(UINT64& timerId)
{
    return CTimerService::GetInstance().KillTimer(timerId);
}

