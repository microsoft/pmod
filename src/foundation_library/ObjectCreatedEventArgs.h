/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectCreatedEventArgs.h
****/
#pragma once

#include <foundation/library/event_args_base.h>
//------------------------------------------------------------------------------
// Class:   CObjectCreatedEventArgs
//
// Purpose: Implementation of the IObjectCreatedEventArgs interface
//
//------------------------------------------------------------------------------
class CObjectCreatedEventArgs :
    public foundation::library::_EventArgsBase<foundation::IObjectCreatedEventArgs>
{
public:
    void Initialize(foundation::IObject *pObject)
    {
        m_Object = pObject;
    }
protected:
    STDMETHOD(GetObject)(foundation::IObject ** ppVal)
    {
        return m_Object.CopyTo(ppVal);
    }
private:
    foundation::ComPtr<foundation::IObject> m_Object;
};

