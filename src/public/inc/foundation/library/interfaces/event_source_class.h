/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:event_source_class.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation
{
    namespace library
    {

        // IEventSourceClass Interface
        BEGIN_DECLARE_INTERFACE(IEventSourceClass, foundation::IUnknown, FOUNDATION_API)
            // AddEventHandler method
            STDMETHOD(AddEventHandler)(
                _In_ GUID eventSourceType,
                _In_opt_ foundation::IUnknown *pEventHandler,
                _In_opt_ foundation::IInspectable *pOptions,
                _COM_Outptr_ foundation::IInspectable **ppResult) = 0;
            // RemoveEventHandler method
            STDMETHOD(RemoveEventHandler)(
                _In_ GUID eventSourceType,
                _In_ foundation::IUnknown *pEventHandler) = 0;
        END_DECLARE_INTERFACE()

    }
}

