/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InterfaceAdapterFactoryHost.h
****/
#pragma once

#include "InterfaceAdapterFactoryBase.h"

class CInterfaceAdapterFactoryHost:
    public CInterfaceAdapterFactoryBase
    <
        foundation::ctl::ComInspectableBase,
        foundation::library::IInterfaceAdapterFactoryHost
    >
{
public:
    static foundation::library::IInterfaceAdapterFactoryHost*GetInstance();
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IInterfaceAdapterFactoryHost::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IInterfaceAdapterFactoryHost *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};


