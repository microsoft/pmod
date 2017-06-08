/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:output_stream.h
****/
#pragma once

#include <pal/pal_api.h>
#include <foundation/interfaces/async_operation.h>
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        //------------------------------------------------------------------------------
        // Interface:   IOutputStream
        //
        // Purpose: An Interface defining an output stream
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IOutputStream, IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(FlushAsync)(
                foundation::IAsyncOperation_Boolean **ppAsyncOperation) = 0;
            STDMETHOD(WriteAsync)(
                UINT32 size,
                UINT8 *buffer,
                foundation::IAsyncOperation_UInt32 **ppAsyncOperation) = 0;
        END_DECLARE_INTERFACE()
    }
}

