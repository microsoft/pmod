/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:input_stream.h
****/
#pragma once

#include <pal/pal_api.h>
#include <foundation/interfaces/async_operation.h>
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        enum class InputStreamOptions
        {
            None,
            Partial = 1,
            ReadAhead = 2,
        };

        //------------------------------------------------------------------------------
        // Interface:   IInputStream
        //
        // Purpose: An Interface defining an input stream
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IInputStream, IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(ReadAsync)(
                UINT32 nBytes, 
                InputStreamOptions options,
                foundation::IAsyncOperation_UInt8Array **ppAsyncOperation) = 0;
        END_DECLARE_INTERFACE()
    }
}

