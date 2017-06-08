/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:interface_adapter_host.h
****/
#pragma once
//+----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation
//
// File:        interface_adapter_host.h
//
// Contents:    Interface definition for IInterfaceAdapterHost
//
// Classes:     IInterfaceAdapterHost
//
//------------------------------------------------------------------------------

#include <foundation/com_interfaces.h>
#include "fire_event.h"

namespace foundation
{
    namespace library
    {

        BEGIN_DECLARE_INTERFACE(IInterfaceAdapterHost, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(QueryHostAdapterInterface)(REFIID iid, foundation::IUnknown **ppInterface) = 0;
            STDMETHOD(HasAnyAdapterEventHandlers)(bool *bValue) = 0;
            STDMETHOD(FireAdaptersEventArgs)(
				_In_ IFireEventHandlerContainer *pFireEventHandlerContainer,
				_In_ foundation::IUnknown *pEventArgs) = 0;
            STDMETHOD(IsObserved)(bool *bValue) = 0;
        END_DECLARE_INTERFACE()
    }
}
