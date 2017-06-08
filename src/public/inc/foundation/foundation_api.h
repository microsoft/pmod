/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:foundation_api.h
****/
#pragma once

#include "pal.h"

#if __APPLE__

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
// define static lib usage for iOS
#define FOUNDATION_LIBRARY_STATIC_LIB 1
#endif

#endif

// Generic helper definitions for shared library support
#if defined(_WINDOWS) || defined __CYGWIN__
#if defined(FOUNDATION_API_EXPORTS)
#define FOUNDATION_API __declspec(dllexport)
#elif !defined(FOUNDATION_LIBRARY_STATIC_LIB)
#define FOUNDATION_API __declspec(dllimport)
#else
#define FOUNDATION_API
#endif
#else
#define FOUNDATION_API __attribute__ ((visibility ("default")))
#endif

