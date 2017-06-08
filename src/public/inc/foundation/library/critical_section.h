/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:critical_section.h
****/

#pragma once

#include "interfaces/critical_section.h"
#include <foundation/com_ptr.h>

namespace foundation
{
    namespace library
    {
        template <class _ComPtrImpl >
        class _FoundationCriticalSectionLock
        {
        public:
            _FoundationCriticalSectionLock():
                _p_critical_section(nullptr)
            {}

            _FoundationCriticalSectionLock(ICriticalSection *p_critical_section) :
                _p_critical_section(p_critical_section)
            {
                if (p_critical_section)
                {
                    _ComPtrImpl::_AddRef(_p_critical_section);
                }
            }
            ~_FoundationCriticalSectionLock()
            {
                if (_p_critical_section)
                {
                    _ComPtrImpl::_Release(_p_critical_section);
                }
            }
            void SetCriticalSection(ICriticalSection *p_critical_section)
            {
                if (_p_critical_section)
                {
                    _ComPtrImpl::_Release(_p_critical_section);
                    _p_critical_section = nullptr;
                }
                _p_critical_section = p_critical_section;

                if (p_critical_section)
                {
                    _ComPtrImpl::_AddRef(_p_critical_section);
                }
            }
            void lock() 
            {
                if (_p_critical_section != nullptr)
                {
                    _p_critical_section->Lock();
                }
            }
            void unlock()
            {
                if (_p_critical_section != nullptr)
                {
                    _p_critical_section->Unlock();
                }
            }
        private:
            ICriticalSection *_p_critical_section;
        };
        typedef _FoundationCriticalSectionLock<_ComPtrImpl> FoundationCriticalSectionLock;
        typedef _FoundationCriticalSectionLock<_NoRefCountPtrImpl> NoRefCountFoundationCriticalSectionLock;

        // Class: _FoundationCriticalSection
        // Purpose: lock/unlock in its context lifetime
        template <class _ComPtrImpl >
        class _FoundationCriticalSection
        {
        public:
            _FoundationCriticalSection() :
                _p_critical_section(nullptr)
            {
            }
            _FoundationCriticalSection(ICriticalSection *p_critical_section)
            {
                set_if_critical_section_interface_and_lock(p_critical_section);
            }

            _FoundationCriticalSection(const _FoundationCriticalSection& other)
            {
                this->swap(other);
            }
            _FoundationCriticalSection & operator= (ICriticalSection *p_critical_section)
            {
                ReleaseAndUnlockIf();
                set_if_critical_section_interface_and_lock(p_critical_section);
                return *this;
            }
            _FoundationCriticalSection & operator= (IUnknown *pUnk)
            {
                ReleaseAndUnlockIf();
                if (SUCCEEDED(foundation::QueryInterface(pUnk, &_p_critical_section)))
                {
                    _p_critical_section->Lock();
                }
                return *this;
            }

            _FoundationCriticalSection & operator= (const _FoundationCriticalSection & other)
            {
                if (this != &other) // protect against invalid self-assignment
                {
                    ReleaseAndUnlockIf();
                    this->swap(other);
                }
                return *this;
            }

            void Release()
            {
                ReleaseIf();
            }

            void unlock()
            {
                ReleaseAndUnlockIf();
            }

            _FoundationCriticalSection(IUnknown *pUnk) :
                _p_critical_section(nullptr)
            {
                this->operator=(pUnk);
            }

            ~_FoundationCriticalSection()
            {
                ReleaseAndUnlockIf();
            }
        private:
            void swap(const _FoundationCriticalSection& other)
            {
                this->_p_critical_section = other._p_critical_section;
                (const_cast<_FoundationCriticalSection&>(other))._p_critical_section = nullptr;
            }

            void ReleaseIf()
            {
                if (_p_critical_section)
                {
                    _ComPtrImpl::_Release(_p_critical_section);
                    _p_critical_section = nullptr;
                }
            }

            void ReleaseAndUnlockIf()
            {
                if (_p_critical_section)
                {
                    _p_critical_section->Unlock();
                    _ComPtrImpl::_Release(_p_critical_section);
                    _p_critical_section = nullptr;
                }
            }
            void set_if_critical_section_interface_and_lock(ICriticalSection *p_critical_section)
            {
                _p_critical_section = p_critical_section;
                if (p_critical_section)
                {
                    _ComPtrImpl::_AddRef(_p_critical_section);
                    p_critical_section->Lock();
                }
            }
        private:
            ICriticalSection *_p_critical_section;
        };

        typedef _FoundationCriticalSection<_ComPtrImpl> FoundationCriticalSection;
        typedef _FoundationCriticalSection<_NoRefCountPtrImpl> NoRefCountFoundationCriticalSection;

    }

}
