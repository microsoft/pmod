/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dispatcher_callback_adapter.h
****/
#pragma once

#include "callback_delegate_adapter_base.h"
#include "interfaces/dispatcher.h"

namespace foundation {
    // Dispatcher Callback
    typedef HRESULT(STDMETHODCALLTYPE *DispatcherCallback)();

    // DispatcherCallbackAdapter Class
    class DispatcherCallbackAdapter :
        public _CallbackDelegateAdapterBase
        <
        DispatcherCallbackAdapter,
        DispatcherCallback,
        foundation::library::IDelegateDispatchable,
        &foundation::library::IID_IDelegateDispatchable
        >
    {
    protected:
        STDMETHOD(Invoke)()
        {
            return (*this->GetCallback())();
        }
    };

}

