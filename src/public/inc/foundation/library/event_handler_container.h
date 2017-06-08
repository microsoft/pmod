/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:event_handler_container.h
****/
#pragma once

#include <foundation/errors.h>
#include <foundation/com_ptr.h>
#include <foundation/ctl/com_library.h>
#include <foundation/macros.h>
#include "critical_section_lock.h"
#include "critical_section.h"

#include <vector>
#include <algorithm>

namespace foundation
{
    namespace library
    {

        template <class TEventHandler>
        class _VectorEventHandlerPtrContainer : public std::vector< foundation::ComPtr<TEventHandler>>
        {
        };

        template <class TEventHandler>
        class _VectorEventHandlerContainer
        {
        public:
            typedef TEventHandler   _TEventHandlerType;
            typedef _VectorEventHandlerPtrContainer<TEventHandler>	_VectorContainerType;

            static const IID& _GetEventHandlerType()
            {
                return foundation::GetIIDOf<TEventHandler>();
            }

            const _VectorContainerType& GetEventHandlers() const { return _handlers; }

            bool ContainsEventHandler(_In_ TEventHandler* pHandler) const
            {
                return std::find(_handlers.begin(), _handlers.end(), pHandler) != _handlers.end();
            }


            HRESULT AddEventHandler(_In_ TEventHandler* pHandler)
            {
                typename _VectorContainerType::const_iterator iter = std::find(_handlers.begin(), _handlers.end(), pHandler);

                if (iter == _handlers.end())
                {
                    _handlers.push_back(pHandler);
                }
                else
                {
                    _IFR_(foundation::E_EVENT_HANDLER_EXIST);
                }
                return S_OK;
            }

            HRESULT RemoveEventHandler(_In_ TEventHandler* pHandler)
            {
                typename _VectorContainerType::iterator iter = std::find(_handlers.begin(), _handlers.end(), pHandler);
                if (iter != _handlers.end())
                {
                    _handlers.erase(iter);
                }
                else
                {
                    _IFR_(foundation::E_EVENT_HANDLER_NOT_FOUND);
                }
                return S_OK;
            }

            void RemoveHandlers(const _VectorContainerType& handlers)
            {
                for (typename _VectorContainerType::const_iterator iter = handlers.begin();
                iter != handlers.end();
                    ++iter)
                {
                    RemoveEventHandler((*iter));
                }
            }

        private:
            _VectorContainerType _handlers;
        };

        template <
            class TEventHandler,
            class THost,
            class TLock,
            class TBASE = _VectorEventHandlerContainer<TEventHandler>>
        class _EventHandlerContainerWithLock:
            public TBASE
        {
        public:
            typedef TLock _TEventHandlerContainerLockType;

            HRESULT AddEventHandler(_In_ TEventHandler* pHandler)
            {
                _CriticalSectionLock<TLock> lock(get_lock());
                return TBASE::AddEventHandler(pHandler);
            }

            HRESULT RemoveEventHandler(_In_ TEventHandler* pHandler)
            {
                _CriticalSectionLock<TLock> lock(get_lock());
                return TBASE::RemoveEventHandler(pHandler);
            }

            void RemoveHandlers(const typename TBASE::_VectorContainerType& handlers)
            {
                _CriticalSectionLock<TLock> lock(get_lock());
                return TBASE::RemoveHandlers(handlers);
            }

            TLock& get_lock()
            {
                return (static_cast<THost *>(this))->_GetLock();
            }

        };

        template <class TEventHandler, class _ComPtrImpl = _NoRefCountPtrImpl>
        class _EventHandlerContainerWithCriticalSection :
            public _EventHandlerContainerWithLock
            <
                TEventHandler,
                _EventHandlerContainerWithCriticalSection<TEventHandler, _ComPtrImpl>,
                _FoundationCriticalSectionLock<_ComPtrImpl>
            >
        {
        public:
            typedef _FoundationCriticalSectionLock<_ComPtrImpl> _TEventHandlerContainerLockType;

            _FoundationCriticalSectionLock<_ComPtrImpl>& _GetLock()
            {
                return _lock;
            }
        private:
            foundation::library::NoRefCountFoundationCriticalSectionLock _lock;
        };
    }

}
