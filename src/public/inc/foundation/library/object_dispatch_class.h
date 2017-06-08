/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_dispatch_class.h
****/
#pragma once

#include "interfaces/object_dispatch_factory.h"
#include <foundation/activation_factory.h>
#include <foundation/com_ptr.h>

namespace foundation
{
    namespace library
    {
        inline library::IObjectDispatchFactory *GetObjectDispatchFactory()
        {
            return GetFoundationLibraryFactory<library::IObjectDispatchFactory>(U("foundation.ObjectDispatch"));
        }

        template <class T>
        static HRESULT CreateObjectDispatch(
            _In_ const IID&                   iidType,
            _In_  ObjectDispatchOptions        options,
            _COM_Outptr_ T   **ppNewInstance)
        {
            library::ObjectDispatchCreateParameters parameters;
            parameters.m_iid_type = iidType;
            parameters.m_options = options;

            foundation::InspectablePtr instance_ptr;
            IFR_ASSERT(GetObjectDispatchFactory()->CreateObjectDispatch(
                &parameters, 
                nullptr, 
                instance_ptr.GetAddressOf()));
            return QueryInterface(instance_ptr, ppNewInstance);
        }

        inline HRESULT CreatePropertyIdPair(
            UINT32 propertyId,
            _In_opt_ foundation::IInspectable *pValue,
            _COM_Outptr_ IPropertyIdPair **ppPropertyIdPair)
        {
            return GetObjectDispatchFactory()->CreatePropertyIdPair(propertyId, pValue, ppPropertyIdPair);
        }
    }

}

