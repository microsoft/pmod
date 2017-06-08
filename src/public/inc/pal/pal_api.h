/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_api.h
****/
#pragma once

#if __APPLE__

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
// define static lib usage for iOS
#define PAL_API_STATIC_LIB 1
#define FOUNDATION_LIBRARY_STATIC_LIB 1
#endif

#endif


// Generic helper definitions for shared library support
#if defined(_WINDOWS) || defined __CYGWIN__
#if defined(PAL_API_EXPORTS)
#define PAL_API __declspec(dllexport)
#elif !defined(PAL_API_STATIC_LIB)
#define PAL_API __declspec(dllimport)
#else
#define PAL_API
#endif
#else
#define PAL_API __attribute__ ((visibility ("default")))
#endif
