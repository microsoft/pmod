/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pmod_lib_api.h
****/
#pragma once

#include <foundation/pal.h>
#include <foundation/activation_factory.h>

#if __APPLE__

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
// define static lib usage for iOS
#define PMOD_LIB_LIBRARY_STATIC_LIB 1
#endif

#endif

#if defined(_WINDOWS) || defined __CYGWIN__
#if defined(PMOD_LIB_API_EXPORTS)
#define PMOD_LIB_API __declspec(dllexport)
#elif !defined(PMOD_LIB_LIBRARY_STATIC_LIB)
#define PMOD_LIB_API __declspec(dllimport)
#else
#define PMOD_LIB_API
#endif
#else
#define PMOD_LIB_API __attribute__ ((visibility ("default")))
#endif

namespace pmod {
    namespace library {
#if defined(PMOD_LIB_LIBRARY_STATIC_LIB)
        template <class T>
        static inline T *GetModelLibraryFactory(LPCSTR_t activationId)
        {
            return foundation::GetActivationFactoryWithCast<T>(activationId);
        }
#else
        template <class T>
        static inline T *GetModelLibraryFactory(LPCSTR_t activationId)
        {
            return foundation::GetActivationFactory<T>(activationId);
        }
#endif
    }
}
