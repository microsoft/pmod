/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TimerService.cpp
****/
#pragma once

#include "palTimer.h"

#include <foundation/ctl/com_library.h>

#include "windows.foundation.h"
#include <windows.system.threading.h>
#include <windows.ui.core.h>
#include <roapi.h>

#include <foundation/com_ptr.h>
#include <foundation/library/logger_macros.h>
#include <foundation/hstring_wrapper.h>
#include <foundation/macros.h>

#include <map>

class CTimerElapsedTimerHandler :
    public foundation::ctl::ComBase,
    protected ABI::Windows::System::Threading::ITimerElapsedHandler,
    protected ABI::Windows::UI::Core::IDispatchedHandler
{
public:
    static HRESULT CreateInstance(
        __in ITimerCallback *pTimerCallback,
        __in ABI::Windows::Foundation::TimeSpan interval,
        __out UINT64 *pIdEvent,
        __deref_out CTimerElapsedTimerHandler **ppTimerElapsedTimerHandler)
    {
        foundation::ComPtr<foundation::ctl::ComObject<CTimerElapsedTimerHandler>> spTimerElapsedTimerHandler;
        IFR(foundation::ctl::ComObject<CTimerElapsedTimerHandler>::CreateInstance(spTimerElapsedTimerHandler.GetAddressOf()));
        IFR(spTimerElapsedTimerHandler->Initialize(
            pTimerCallback,
            interval,
            pIdEvent));

        *ppTimerElapsedTimerHandler = spTimerElapsedTimerHandler.Detach();
        return S_OK;
    }

    HRESULT Stop()
    {
        foundation_assert(!m_bStopped);

        m_bStopped = true;
        foundation_assert(m_spThreadPoolTimer);
        IFR(m_spThreadPoolTimer->Cancel());

        m_spThreadPoolTimer = nullptr;
        m_spCoreDispatcher = nullptr;
        m_spTimerCallback = nullptr;

        return S_OK;
    }

protected:
    ~CTimerElapsedTimerHandler()
    {
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if(iid == __uuidof(ABI::Windows::UI::Core::IDispatchedHandler))
        {
            *ppInterface = static_cast<ABI::Windows::UI::Core::IDispatchedHandler *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::System::Threading::IThreadPoolTimer *timer)
    {
        UNREFERENCED_PARAMETER(timer);

        if(m_bStopped)
        {
            return S_FALSE;
        }
        foundation::ComPtr<ABI::Windows::Foundation::IAsyncAction> spAsyncAction;

        foundation_assert(m_spCoreDispatcher);
        IFR(m_spCoreDispatcher->RunAsync(
            ABI::Windows::UI::Core::CoreDispatcherPriority_Normal,
            static_cast<ABI::Windows::UI::Core::IDispatchedHandler *>(this), 
            spAsyncAction.GetAddressOf()
        ));

        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Invoke( void)
    {
        if(m_bStopped)
        {
            return S_FALSE;
        }
        foundation_assert(m_spTimerCallback);
        return m_spTimerCallback->OnTimeout(m_idEvent);
    }

private:
    HRESULT Initialize(
        __in ITimerCallback *pTimerCallback,
        __in ABI::Windows::Foundation::TimeSpan interval,
        __out UINT64 *pIdEvent )
    {
        m_bStopped = false;

        // Get existing CoreDispatcher
        foundation::HStringRef coreWndId(RuntimeClass_Windows_UI_Core_CoreWindow);
        foundation::ComPtr<ABI::Windows::UI::Core::ICoreWindowStatic> spCoreWindowsStatic;

        IFR(Windows::Foundation::GetActivationFactory(coreWndId,spCoreWindowsStatic.GetAddressOf()));
        foundation::ComPtr<ABI::Windows::UI::Core::ICoreWindow> spUIStaCoreWnd;
        IFR(spCoreWindowsStatic->GetForCurrentThread(spUIStaCoreWnd.GetAddressOf()));
        IFCEXPECT(spUIStaCoreWnd != nullptr);
        IFR(spUIStaCoreWnd->get_Dispatcher(m_spCoreDispatcher.GetAddressOf()));

        // Access ThreadPoolTimerStatics
        foundation::HStringRef  activateId(RuntimeClass_Windows_System_Threading_ThreadPoolTimer);
        foundation::ComPtr<ABI::Windows::System::Threading::IThreadPoolTimerStatics> spThreadPoolTimerStatics;
        IFR(Windows::Foundation::GetActivationFactory(activateId,spThreadPoolTimerStatics.GetAddressOf()));
        
        // Create the Thread pool timer
        IFR(spThreadPoolTimerStatics->CreatePeriodicTimer(
            static_cast<ABI::Windows::System::Threading::ITimerElapsedHandler *>(this),
            interval,
            m_spThreadPoolTimer.GetAddressOf()));

        // assign local members
        m_idEvent = (ULONG_PTR)this;
        m_spTimerCallback = pTimerCallback;

        // return values
        *pIdEvent = m_idEvent;
        return S_OK;
    }
private:
    bool  m_bStopped;
    foundation::ComPtr<ABI::Windows::System::Threading::IThreadPoolTimer> m_spThreadPoolTimer;
    UINT64                                                m_idEvent;
    foundation::ComPtr<ITimerCallback>                          m_spTimerCallback;
    foundation::ComPtr<ABI::Windows::UI::Core::ICoreDispatcher> m_spCoreDispatcher;

};

typedef std::map<UINT64,CTimerElapsedTimerHandler *> TimerInfoMapType;
static TimerInfoMapType _timerInfoMap;
const INT64 TICKS_PER_MILLISECOND = 10000;       // 1 millisecond is 10,000 ticks.

HRESULT _pal_SetTimer(
        _Inout_  UINT64& timerId,           // Timer ID
        _In_ ITimerCallback * callback,     // Object implementing ITimerCallback interface
        _In_ UINT32 timeOut)
{
    foundation_assert(callback);

    ABI::Windows::Foundation::TimeSpan interval;
    interval.Duration = (INT64)(timeOut * TICKS_PER_MILLISECOND);

    if(timerId)
    {
        IGNOREHR(_pal_KillTimer(timerId));
    }
    CTimerElapsedTimerHandler *pTimerElapsedTimerHandler = nullptr;
    IFR(CTimerElapsedTimerHandler::CreateInstance(callback,interval,&timerId,&pTimerElapsedTimerHandler));
    
    // add to map
    _timerInfoMap[timerId] = pTimerElapsedTimerHandler;
    return S_OK;
}

HRESULT _pal_KillTimer(UINT64& timerId)
{
    TimerInfoMapType::iterator iter = _timerInfoMap.find(timerId);
    if(iter != _timerInfoMap.end())
    {
        CTimerElapsedTimerHandler *pTimerElapsedTimerHandler = (*iter).second;
        // Stop
        IFR(pTimerElapsedTimerHandler->Stop());
        // Release
        static_cast<foundation::ctl::ComBase *>(pTimerElapsedTimerHandler)->Release();
        // remove from Map
        _timerInfoMap.erase(iter);
        timerId = 0;
    }
    return S_OK;
}

