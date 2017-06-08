/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:interface_adapter_util.h
****/
#pragma once

#include <foundation/macros.h>
#include <foundation/foundation_factory.h>

#include "interfaces/adapter_factory.h"
#include "interfaces/interface_adapter.h"


namespace foundation {

    namespace library
    {

        inline library::IInterfaceAdapterFactoryHost *GetInterfaceAdapterFactoryHost()
        {
            return GetFoundationFactory<library::IInterfaceAdapterFactoryHost>(U("foundation.InterfaceAdapterFactoryHost"));
        }

        inline library::IAdapterFactoryFactory *GetAdapterFactoryFactory()
        {
            return GetFoundationFactory<library::IAdapterFactoryFactory>(U("foundation.AdapterFactory"));
        }

        inline HRESULT RegisterInterfaceAdapterFactory(_In_ library::IInterfaceAdapterFactory *pModelAdapterFactory)
        {
            IFCPTR_ASSERT(pModelAdapterFactory);

            IFR_ASSERT(GetInterfaceAdapterFactoryHost()->RegisterInterfaceAdapterFactory(pModelAdapterFactory));
            return S_OK;
        }

        inline HRESULT UnregisterInterfaceAdapterFactory(_In_ library::IInterfaceAdapterFactory *pModelAdapterFactory)
        {
            IFCPTR_ASSERT(pModelAdapterFactory);

            IFR_ASSERT(GetInterfaceAdapterFactoryHost()->UnregisterInterfaceAdapterFactory(pModelAdapterFactory));
            return S_OK;
        }

        inline HRESULT CreateTypeAdapterFactory(
            _In_opt_ const library::_TypeAdapterEntry iidTypes[],
            _COM_Outptr_ library::IInterfaceAdapterFactory **ppModelAdapterFactory)
        {
            IFCPTR_ASSERT(ppModelAdapterFactory);
            *ppModelAdapterFactory = nullptr;

            IFR_ASSERT(GetAdapterFactoryFactory()->CreateTypeAdapterFactory(iidTypes, ppModelAdapterFactory));
            return S_OK;
        }

        inline HRESULT CreateGenericAdapterFactory(
            _COM_Outptr_ foundation::library::IGenericAdapterFactory **ppGenericAdapterFactory)
        {
            IFCPTR_ASSERT(ppGenericAdapterFactory);
            *ppGenericAdapterFactory = nullptr;

            IFR_ASSERT(GetAdapterFactoryFactory()->CreateGenericAdapterFactory(ppGenericAdapterFactory));
            return S_OK;
        }
    }

}

