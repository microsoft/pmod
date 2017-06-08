/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:foundation_factory.h
****/
#pragma once

#include "foundation_api.h"
#include "activation_factory.h"

namespace foundation {
#if defined(FOUNDATION_LIBRARY_STATIC_LIB)
        template <class T>
        static inline T *GetFoundationFactory(LPCSTR_t activationId)
        {
            return GetActivationFactoryWithCast<T>(activationId);
        }
#else
        template <class T>
        static inline T *GetFoundationFactory(LPCSTR_t activationId)
        {
            return GetActivationFactory<T>(activationId);
        }
#endif
}
