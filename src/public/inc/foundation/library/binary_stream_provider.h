/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:binary_stream_provider.h
****/
#pragma once

#include <foundation/macros.h>
#include <foundation/interfaces/stream_provider.h>
#include <foundation/activation_factory.h>


namespace foundation {

    namespace library
    {
        inline IStreamProvider *GetBinaryStreamProvider()
        {
            return GetFoundationLibraryFactory<IStreamProvider>(U("foundation.BinaryStreamProvider"));
        }
    }

}

