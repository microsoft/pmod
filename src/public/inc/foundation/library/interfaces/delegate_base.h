/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:delegate_base.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation
{
    namespace library
    {
        // The Delegate Base
        template <class TEventHandler>
        struct IDelegateBase : public foundation::IUnknown
        {
            // Initialize at runtime
            STDMETHOD(OnRuntimeInitialize)() = 0;

            // Fired when Event Handlers has been added/removed
            STDMETHOD(OnAddEventHandler)(_In_ TEventHandler *pEventHandler) = 0;
            STDMETHOD(OnRemoveEventHandler)(_In_ TEventHandler *pEventHandler) = 0;

            // Start/Stop Observe
            STDMETHOD(OnStartObserve)() = 0;
            STDMETHOD(OnStopObserve)() = 0;

        };
    }
}

