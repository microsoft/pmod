/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:critical_section_impl.h
****/

#pragma once

#include "interfaces/critical_section.h"
#include <foundation/ctl/com_library.h>
#include <pal/pal_critical_section.h>

// uncomment next line if you want to debug the critical section lock/unlock
//#define _DEBUG_FOUNDATION_CRITICAL_SECTION
#ifdef _DEBUG_FOUNDATION_CRITICAL_SECTION
#include <pal/pal_thread.h>
#include <pal/library/trace_util.h>
#endif

namespace foundation
{
    namespace library {
        template <const bool isRecursive,class TBASE>
        class _CriticalSectionBase :
            public TBASE
        {
        public:
            _CriticalSectionBase() throw()
            {
#if defined(_WINDOWS) || defined(__GNUC__)
                memset(&m_sec, 0, sizeof(_PAL_CRITICAL_SECTION));
#else
                m_sec = nullptr;
#endif
            }

            ~_CriticalSectionBase()
            {
            }

            HRESULT Lock() throw()
            {
#ifdef _DEBUG_FOUNDATION_CRITICAL_SECTION
                _pal_ThreadId thread_id = _pal_CurrentThreadId();
                pal::library::TraceMessage(U("_TCriticalSection::->Lock this:%08x thread id:%d\n"), this, thread_id);
#endif
                _pal_EnterCriticalSection(&m_sec);
#ifdef _DEBUG_FOUNDATION_CRITICAL_SECTION
                pal::library::TraceMessage(U("_TCriticalSection::<-Lock this:%08x thread id:%d\n"), this, thread_id);
#endif
                return S_OK;
            }

            HRESULT Unlock() throw()
            {
                _pal_LeaveCriticalSection(&m_sec);
#ifdef _DEBUG_FOUNDATION_CRITICAL_SECTION
                pal::library::TraceMessage(U("_TCriticalSection::Unlock this:%08x thread id:%d\n"), this, _pal_CurrentThreadId());
#endif
                return S_OK;
            }

            HRESULT _Initialize() throw()
            {
#ifdef _DEBUG_FOUNDATION_CRITICAL_SECTION
                pal::library::TraceMessage(U("_TCriticalSection::Init this:%08x thread id:%d\n"), this, _pal_CurrentThreadId());
#endif
                HRESULT hRes = S_OK;
                _pal_InitializeCriticalSection(&m_sec, isRecursive);
                return hRes;
            }

            HRESULT Term() throw()
            {
                _pal_DeleteCriticalSection(&m_sec);
                return S_OK;
            }

            _PAL_CRITICAL_SECTION m_sec;
        };

        typedef _CriticalSectionBase<true, ctl::Implements<ICriticalSection, &IID_ICriticalSection>> CriticalSectionImpl;

        inline HRESULT CreateCriticalSectionImpl(ICriticalSection **pp)
        {
            return ctl::CreateInstanceWithInitialize<CriticalSectionImpl>(pp);
        }
    }

}
