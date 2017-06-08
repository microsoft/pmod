/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:event_handler_source.h
****/
#pragma once

#include "event_handler_container.h"
#include "fire_event_support.h"


namespace foundation
{
    namespace library
    {
        template <
            class TEventSource,
            class _EventHandlerContainer
        >
        class _EventHandlerSource :
                public foundation::ctl::ComBase,
                public _EventHandlerContainer,
                public TEventSource
            {
            public:
                typedef typename _EventHandlerContainer::_TEventHandlerType TEventHandler;

                // TEventSource
                STDMETHOD(AddHandler)(_In_ TEventHandler* pHandler)
                {
                    return this->AddEventHandler(pHandler);
                }

                STDMETHOD(RemoveHandler)(_In_ TEventHandler* pHandler)
                {
                    return this->RemoveEventHandler(pHandler);
                }

                template <class TEventArgs>
                HRESULT FireEvent(
                    foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
                    _In_ foundation::IInspectable *pSender,
                    _In_ TEventArgs *pEventArgs)
                {
                    return foundation::library::_FireEventSupportUtil::_FireWithEventSupport
                            < 
                                foundation::library::_SimpleInvokeHandler, 
                                foundation::ctl::_ComBasePtrImpl
                            >(
                            pEventHandlerContainer,
                            pSender,
                            pEventArgs,
                            *this);
                }

            protected:
                HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
                {
                    if (iid == TEventSource::GetIID())
                    {
                        *ppInterface = static_cast<TEventSource *>(this);
                    }
                    else
                    {
                        return foundation::ctl::ComBase::QueryInterfaceImpl(iid, ppInterface);
                    }
                    return S_OK;
                }
            };
    }
}
