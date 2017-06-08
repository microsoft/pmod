/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CriticalSection.h
****/
#pragma once

#include <foundation/library/critical_section_impl.h>
#include <foundation/macros.h>
#include <foundation/assert.h>


namespace foundation_lib {

    enum class FoundationLockState
    {
        None,
        InitialLock,
        AlredyLocked
    };

    template< class TLock >
    class CFoundationLock
    {
    public:
        CFoundationLock(
            _Inout_ TLock& cs,
            _In_ FoundationLockState lockstate = FoundationLockState::InitialLock);

        ~CFoundationLock() throw();

        inline void lock() { return Lock(); }
        inline void unlock() { return Unlock(); }

        HRESULT Lock() throw();
        void Unlock() throw();

        // Implementation
    private:
        TLock& m_cs;
        bool m_bLocked;

        // Private to avoid accidental use
        CFoundationLock(_In_ const CFoundationLock&) throw();
        CFoundationLock& operator=(_In_ const CFoundationLock&) throw();
    };

    template< class TLock >
    inline CFoundationLock< TLock >::CFoundationLock(
        _Inout_ TLock& cs,
        _In_ FoundationLockState lockstate) :
        m_cs(cs),
        m_bLocked(false)
    {
        if (lockstate == FoundationLockState::InitialLock)
        {
            HRESULT hr;

            hr = Lock();
            if (FAILED(hr))
            {
                //AtlThrow( hr );
            }
        }
        else if (lockstate == FoundationLockState::AlredyLocked)
        {
            m_bLocked = true;
        }
    }

    template< class TLock >
    inline CFoundationLock< TLock >::~CFoundationLock() throw()
    {
        if (m_bLocked)
        {
            Unlock();
        }
    }

    template< class TLock >
    inline HRESULT CFoundationLock< TLock >::Lock() throw()
    {
        HRESULT hr;

        foundation_assert(!m_bLocked);
        hr = m_cs.Lock();
        if (FAILED(hr))
        {
            return(hr);
        }
        m_bLocked = true;
        return(S_OK);
    }

    template< class TLock >
    inline void CFoundationLock< TLock >::Unlock() throw()
    {
        foundation_assert(m_bLocked);
        m_cs.Unlock();
        m_bLocked = false;
    }

    struct _CCriticalSectionBase
    {
    };


    typedef foundation::library::CriticalSectionImpl CCriticalSectionImpl;

    typedef foundation::library::_CriticalSectionBase<true, _CCriticalSectionBase> CCriticalSection;
    typedef CFoundationLock<foundation::library::ICriticalSection> _FoundationLock;
    typedef CFoundationLock<CCriticalSection> _CriticalSectionLock;

    class CAutoCriticalSection :
        public CCriticalSection
    {
    public:
        CAutoCriticalSection()
        {
            HRESULT hr = CCriticalSection::_Initialize();
            if (FAILED(hr))
            {
                //AtlThrow(hr);
            }
        }
        ~CAutoCriticalSection() throw()
        {
            CCriticalSection::Term();
        }
    private:
        HRESULT Init(); // Not implemented. CAutoCriticalSection::Init should never be called
        HRESULT Term(); // Not implemented. CAutoCriticalSection::Term should never be called
    };

    class CSafeDeleteCriticalSection :
        public CCriticalSection
    {
    public:
        CSafeDeleteCriticalSection() : m_bInitialized(false)
        {
        }

        ~CSafeDeleteCriticalSection() throw()
        {
            if (!m_bInitialized)
            {
                return;
            }
            m_bInitialized = false;
            CCriticalSection::Term();
        }

        HRESULT Init() throw()
        {
            foundation_assert(!m_bInitialized);
            HRESULT hr = CCriticalSection::_Initialize();
            if (SUCCEEDED(hr))
            {
                m_bInitialized = true;
            }
            return hr;
        }

        HRESULT Term() throw()
        {
            if (!m_bInitialized)
            {
                return S_OK;
            }
            m_bInitialized = false;
            return CCriticalSection::Term();
        }

        HRESULT Lock()
        {
            foundation_assert(m_bInitialized);
            return CCriticalSection::Lock();
        }

    private:
        bool m_bInitialized;
    };

    class CAutoDeleteCriticalSection :
        public CSafeDeleteCriticalSection
    {
    private:
        // CComAutoDeleteCriticalSection::Term should never be called
        HRESULT Term() throw();
    };

    class CFakeCriticalSection
    {
    public:
        HRESULT Lock() throw()
        {
            return S_OK;
        }
        HRESULT Unlock() throw()
        {
            return S_OK;
        }
        HRESULT Init() throw()
        {
            return S_OK;
        }
        HRESULT Term() throw()
        {
            return S_OK;
        }
    };

}
