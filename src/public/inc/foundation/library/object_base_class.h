/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_base_class.h
****/
#pragma once

#include "interfaces/object_base_factory.h"
#include <foundation/activation_factory.h>

namespace foundation
{

    namespace library
    {
        inline library::IObjectBaseFactory *get_object_base_factory()
        {
            return GetFoundationLibraryFactory<library::IObjectBaseFactory>(U("foundation.Object"));
        }

        static HRESULT create_object_base(
            const IID& iidType,
            library::ObjectBaseOptions options,
            LPCSTR_t name,
            UINT32 typeId,
            _In_opt_ foundation::IInspectable *pOuter,
            _Outptr_ foundation::IInspectable **ppNewInstance)
        {
            library::object_base_create_parameters ctor_params;
            ctor_params.m_options = options;
            ctor_params.m_iid_type = iidType;
            ctor_params._name = name;
            ctor_params._typeId = typeId;

            IFR_ASSERT(get_object_base_factory()->CreateObject(
                &ctor_params,
                pOuter,
                ppNewInstance));
            return S_OK;
        }
    }

}

