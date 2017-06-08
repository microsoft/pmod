/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:palTimer.h
****/
#pragma once

#include <foundation/com_interfaces.h>


// A Timer Callback Interface
BEGIN_DECLARE_INTERNAL_INTERFACE(ITimerCallback, foundation::IUnknown)
    STDMETHOD (OnTimeout)(UINT64 timerId) = 0;
END_DECLARE_INTERFACE()

EXTERN_C HRESULT _pal_SetTimer(
        _Inout_  UINT64& timerId,           // Timer ID
        _In_ ITimerCallback * callback,     // Object implementing ITimerCallback interface
        _In_ UINT32 timeOut);                // Milliseconds

EXTERN_C HRESULT _pal_KillTimer(UINT64& timerId);

