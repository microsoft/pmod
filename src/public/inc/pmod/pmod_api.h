/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pmod_api.h
****/
#pragma once

#include <foundation/pal.h>

#if __APPLE__

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
// define static lib usage for iOS
#define PMOD_LIBRARY_STATIC_LIB 1
#endif

#endif

#if defined(_WINDOWS) || defined __CYGWIN__
#if defined(PMOD_API_EXPORTS)
#define PMOD_API __declspec(dllexport)
#elif !defined(PMOD_LIBRARY_STATIC_LIB)
#define PMOD_API __declspec(dllimport)
#else
#define PMOD_API
#endif
#else
#define PMOD_API __attribute__ ((visibility ("default")))
#endif

