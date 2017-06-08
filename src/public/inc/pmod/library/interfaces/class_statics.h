/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:class_statics.h
****/
#pragma once

#include <pmod/pmod_api.h>

#include <foundation/library/interfaces/dispatcher.h>
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // IClassStatics Interface
        BEGIN_DECLARE_INTERFACE(IClassStatics, foundation::IUnknown, PMOD_LIB_API)
            // Event Dispatcher property
            STDMETHOD(GetEventDispatcher)(_In_ foundation::library::IDispatcher **ppEventDispatcher) = 0;
            STDMETHOD(SetEventDispatcher)(_Outptr_ foundation::library::IDispatcher *pEventDispatcher) = 0;

            // Working Dispatcher property
            STDMETHOD(GetWorkingDispatcher)(_In_ foundation::library::IDispatcher **ppWorkingDispatcher) = 0;
            STDMETHOD(SetWorkingDispatcher)(_Outptr_ foundation::library::IDispatcher *pWorkingDispatcher) = 0;
        END_DECLARE_INTERFACE()

    }
}

