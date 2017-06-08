/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:foundation_lib_api.h
****/
#pragma once

#include <foundation/pal.h>
#include <foundation/activation_factory.h>

#if __APPLE__

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
// define static lib usage for iOS
#define FOUNDATION_LIB_LIBRARY_STATIC_LIB 1
#endif

#endif

// Generic helper definitions for shared library support
#if defined(_WINDOWS) || defined __CYGWIN__
#if defined(FOUNDATION_LIB_API_EXPORTS)
#define FOUNDATION_LIB_API __declspec(dllexport)
#elif !defined(FOUNDATION_LIB_LIBRARY_STATIC_LIB)
#define FOUNDATION_LIB_API __declspec(dllimport)
#else
#define FOUNDATION_LIB_API
#endif
#else
#define FOUNDATION_LIB_API __attribute__ ((visibility ("default")))
#endif

namespace foundation { namespace library {
#if defined(FOUNDATION_LIB_LIBRARY_STATIC_LIB)
    template <class T>
    static inline T *GetFoundationLibraryFactory(LPCSTR_t activationId)
    {
        return GetActivationFactoryWithCast<T>(activationId);
    }
#else
    template <class T>
    static inline T *GetFoundationLibraryFactory(LPCSTR_t activationId)
    {
        return GetActivationFactory<T>(activationId);
    }
#endif
}}
