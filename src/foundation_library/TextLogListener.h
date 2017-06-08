/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TextLogListener.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/base_adapter_host.h>
#include "CreateFoundationHelper.h"

//------------------------------------------------------------------------------
// Class:   CTextLogListener
//
// Purpose: Implementation for ILogCreatedEventHandler interface to create 
// a text representation of a ILogRecord
//
//------------------------------------------------------------------------------
class CTextLogListener :
    public foundation::library::_DefaultAdapterHost
    <
    foundation::ILogCreatedEventHandler,
    foundation::ctl::ImplementsInspectable
    <
        foundation::ILogCreatedEventHandler,
        &foundation::IID_ILogCreatedEventHandler
    >
    >
{
public:
    static HRESULT CreateInstance(_In_opt_ foundation::IInspectable *pOuter, _COM_Outptr_ foundation::IInspectable **ppInner)
    {
        return foundation::ctl::inspectable::CreateInstanceWithInitialize<CTextLogListener>(pOuter, ppInner);
    }

    HRESULT _Initialize()
    {
        return S_OK;
    }
protected:
    STDMETHOD(Invoke)(foundation::IUnknown* pSender, foundation::ILogCreatedEventArgs* pArgs);
};
