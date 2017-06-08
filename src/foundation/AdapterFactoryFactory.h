/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AdapterFactoryFactory.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/foundation_library.h>

class CAdapterFactoryFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IAdapterFactoryFactory
{
 public:
     static foundation::library::IAdapterFactoryFactory *GetInstance();
     static HRESULT GetOrCreateInstance(foundation::library::IAdapterFactoryFactory **ppAdapterFactoryFactory);
    //Interface IAdapterFactoryFactory
    STDMETHOD(CreateTypeAdapterFactory)(
        _In_opt_ const foundation::library::_TypeAdapterEntry iidTypes[],
        _COM_Outptr_ foundation::library::IInterfaceAdapterFactory **ppModelAdapterFactory) override;

    STDMETHOD(CreateGenericAdapterFactory)(
        _COM_Outptr_ foundation::library::IGenericAdapterFactory **ppGenericAdapterFactory) override;
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IAdapterFactoryFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IAdapterFactoryFactory *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};
