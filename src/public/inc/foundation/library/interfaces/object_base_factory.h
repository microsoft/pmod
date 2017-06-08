/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_base_factory.h
****/
#pragma once

#include <foundation/interfaces/object.h>
#include <foundation/library/interfaces/delegate_base.h>
#include <foundation/library/type_info_create_parameter.h>
#include <foundation/foundation_macros.h>
#include "../foundation_lib_api.h"

namespace foundation
{
    BEGIN_DECLARE_INTERFACE(IObjectBaseEventArgs, IUnknown, FOUNDATION_LIB_API)
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(IObjectBase, FOUNDATION_LIB_API)

    namespace library
    {
        enum ObjectBaseOptions
        {
            None = 0x00,
        };

        BEGIN_DECLARE_INTERFACE(IObjectBaseDelegate, IDelegateBase<IObjectBaseEventHandler>, FOUNDATION_LIB_API)
        END_DECLARE_INTERFACE()

        struct object_base_create_parameters : public _TypeInfoCreateParameterBase<IObjectBaseDelegate, ObjectBaseOptions>
        {
            object_base_create_parameters():
                _typeId(0),
                _name(nullptr)
            {}

            UINT32   _typeId;
            LPCSTR_t _name;
        };

        //------------------------------------------------------------------------------
        // Interface:   IObjectBaseFactory
        //
        // Purpose: An Interface to create IObject instances
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IObjectBaseFactory, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(CreateObject)(
            _In_ const object_base_create_parameters *parameters,
            _In_ foundation::IInspectable *pOuter,
            _Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

