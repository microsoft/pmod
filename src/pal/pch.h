/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pch.h
****/

#if defined(_WINDOWS) && !defined(_WINDOWS_CLASSIC)

#include <windows.foundation.h>

#elif defined(__GNUC__)

#include <pthread.h>
#else

#endif

#include <foundation/pal.h>

