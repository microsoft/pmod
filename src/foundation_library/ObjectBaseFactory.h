/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectBaseFactory.h
****/


#pragma once

#include <foundation/library/interfaces/object_base_factory.h>
#include "ObjectBase.h"


class CObjectBaseFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IObjectBaseFactory
{
public:
    static foundation::library::IObjectBaseFactory *GetInstance();
    //Interface IObjectBaseFactory
    STDMETHOD(CreateObject)(
        _In_ const foundation::library::object_base_create_parameters *parameters,
        _In_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override;
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IObjectBaseFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IObjectBaseFactory *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};
