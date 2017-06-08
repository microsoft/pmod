/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_factory.h
****/
#pragma once

#include <foundation/interfaces/async_operation.h>
#include <foundation/library/type_info_create_parameter.h>
#include <foundation/library/interfaces/async_operation_class.h>
#include "../foundation_lib_api.h"

namespace foundation
{
    enum class AsyncOperationOptions
    {
        None = 0x00,
        IsCancelable = 0x01
    };

    namespace library
    {

        struct async_operation_create_parameters : public _TypeInfoCreateParameterBase<IAsyncOperationDelegate, AsyncOperationOptions>
        {
        };

        //------------------------------------------------------------------------------
        // Interface:   IAsyncOperationFactory
        //
        // Purpose: An Interface to create IAsyncOperation instances
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IAsyncOperationFactory, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(CreateAsyncOperation)(
            _In_ const async_operation_create_parameters *parameters,
            _In_opt_ foundation::IInspectable *pOuter,
            _Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

