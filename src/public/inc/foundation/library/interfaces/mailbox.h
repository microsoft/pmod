/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:mailbox.h
****/
#pragma once
#include <pal/pal_api.h>

#include "../foundation_lib_api.h"

namespace foundation {

    namespace library {
        BEGIN_DECLARE_INTERFACE(IMailbox, foundation::IUnknown, FOUNDATION_LIB_API)
            // Properties
            STDMETHOD(GetCount)(UINT32 *pValue) = 0;
        // methods.
        STDMETHOD(Put)(void *Data, UINT32 waitTime) = 0;
        STDMETHOD(Get)(void *Data, UINT32 waitTime) = 0;
        END_DECLARE_INTERFACE()
    }
}

