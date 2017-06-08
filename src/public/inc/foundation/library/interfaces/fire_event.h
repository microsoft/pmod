/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:fire_event.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include "dispatcher.h"

namespace foundation
{
    namespace library
    {

        struct IFireEventHandlerContainer;

        // Event Handler Support
        BEGIN_DECLARE_INTERFACE(IFireEventSupport, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(GetType)(_Out_ IID *pType) = 0;
            STDMETHOD(FireEventHandlers)(_In_ IFireEventHandlerContainer *pContainer) = 0;
            STDMETHOD(GetEventHandlers)(_Out_ UINT32 *pSize, foundation::IUnknown ***ppHandlers) = 0;
            STDMETHOD(FireEventHandler)(_In_ IFireEventHandlerContainer *pContainer, _In_ foundation::IUnknown *pEventHandler) = 0;
        END_DECLARE_INTERFACE()

        BEGIN_DECLARE_INTERFACE(IFireEventHandlerContainer, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(IsEventSupportEnabled)(bool *value) = 0;
            STDMETHOD(IsEmpty)(bool *value) = 0;
            STDMETHOD(AddFireEventSupport)(_In_ IFireEventSupport *pFireEventSupport) = 0;
            STDMETHOD(FireAllEventHandlers)() = 0;
            STDMETHOD(Detach)(UINT32 *size, IFireEventSupport ***ppFireEventSupport) = 0;
            // Begin/End supprot
            STDMETHOD(BeginSupport)() = 0;
            STDMETHOD(EndSupport)(bool clearEventSupport) = 0;
            // when a handler notification failed
            STDMETHOD(OnFireEventError)(
                HRESULT hr,
                IUnknown *pEventHandler,
                IUnknown *pEventArgs) = 0;
			// the event dispatcher 
			STDMETHOD(GetFireEventDispatcher)(_COM_Outptr_result_maybenull_ IDispatcher **ppEventDispatcher) = 0;

        END_DECLARE_INTERFACE()

        BEGIN_DECLARE_INTERFACE(IFireEventSupportDispatchable, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(GetFireEvents)(_Out_ UINT32 *pSize, IFireEventSupport ***ppFireEvents) = 0;
            STDMETHOD(GetContainer)(IFireEventHandlerContainer **ppContainer) = 0;
        END_DECLARE_INTERFACE()

    }
}

