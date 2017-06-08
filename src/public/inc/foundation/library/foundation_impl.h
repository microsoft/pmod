/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:foundation_impl.h
****/
#pragma once

#include "fire_event_support.h"

namespace foundation {

    namespace library {

        class _DefaultFireEventHandlerContainer:
            public _FireEventHandlerContainerImpl<_DefaultFireEventHandlerContainer>
        {
        public:
            virtual IFireEventHandlerContainer *_Get_fire_event_handler_container()
            {
                return static_cast<IFireEventHandlerContainer *>(this);
            }
            virtual IDispatcher *_GetEventDispatcher()
            {
                return nullptr;
            }
            virtual bool _IsContinueOnFireEventError()
            {
                return false;
            }
            virtual bool _IsEventSupportEnabled()
            {
                return true;
            }

        protected:
            template <class T, class TEventArgs>
            HRESULT FireWithCallback(
                _In_ T *pThis,
                _In_ HRESULT(T::*_FireEventDelegate)(IFireEventHandlerContainer *, TEventArgs *),
                _In_ TEventArgs *pEventArgs)
            {
                FoundationCriticalSectionLock lock(pThis->_GetCriticalSection());
                return _FireEventSupportUtil::_FireEventWithCallback(
                    lock,
                    pThis,
                    _FireEventDelegate,
                    pEventArgs);
            }
        };

        class _NoFireEventHandlerContainer
        {
        protected:
            template <class T, class TEventArgs>
            HRESULT FireWithCallback(
                _In_ T *pThis,
                _In_ HRESULT(T::*_FireEventDelegate)(IFireEventHandlerContainer *, TEventArgs *),
                _In_ TEventArgs *pEventArgs)
            {
                HRESULT hr = (pThis->*_FireEventDelegate)(nullptr, pEventArgs);
                _IFHR_(hr);
                pThis->_FireAdapters(nullptr, pEventArgs);
                return S_OK;
            }
        };
    }
}

