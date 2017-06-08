/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BinaryLogListener.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/base_adapter_host.h>

#include "CreateFoundationHelper.h"

typedef foundation::ctl::ImplementsInspectable
        <
            foundation::ILogCreatedEventHandler,
            &foundation::IID_ILogCreatedEventHandler
        > _TypeBinaryLogListenerBase;
//------------------------------------------------------------------------------
// Class:   CBinaryLogListener
//
// Purpose: Implementation for ILogCreatedEventHandler interface to create 
// a binary representation of a ILogRecord
//
//------------------------------------------------------------------------------
class CBinaryLogListener :
    public foundation::library::_DefaultAdapterHost
    <
        foundation::ILogCreatedEventHandler,
        _TypeBinaryLogListenerBase
    >
{
public:
    static HRESULT CreateInstance(_In_opt_ foundation::IInspectable *pOuter, _COM_Outptr_ foundation::IInspectable **ppInner)
    {
        return foundation::ctl::inspectable::CreateInstanceWithInitialize<CBinaryLogListener>(pOuter, ppInner);
    }
    HRESULT _Initialize()
    {
        return S_OK;
    }
protected:
    // Interface ILogCreatedEventHandler
    STDMETHOD(Invoke)(_In_ foundation::IUnknown* pSender, _In_ foundation::ILogCreatedEventArgs* pArgs);
};
