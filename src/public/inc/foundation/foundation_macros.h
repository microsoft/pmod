/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:foundation_macros.h
****/
#pragma once

#include <foundation/com_interfaces.h>

// declare a Event by specifying both an Event Handler & Event Source interfaces
#define DECLARE_EVENT(ievent,decl_api)           \
    BEGIN_DECLARE_INTERFACE(ievent##EventHandler, foundation::IUnknown, decl_api) \
        typedef ievent##EventArgs _EventArgs_Type; \
        STDMETHOD(Invoke)(foundation::IUnknown* pSender, ievent##EventArgs* pArgs) = 0; \
    END_DECLARE_INTERFACE()\
    \
    BEGIN_DECLARE_INTERFACE(ievent##EventSource, foundation::IUnknown, decl_api) \
        STDMETHOD(AddHandler)(ievent##EventHandler* pHandler) = 0; \
        STDMETHOD(RemoveHandler)(ievent##EventHandler* pHandler) = 0;  \
    END_DECLARE_INTERFACE()

