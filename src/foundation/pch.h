/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pch.h
****/

#if defined(_WINDOWS_RUNTIME)

#include <windows.foundation.h>

#elif defined(__GNUC__)

#include <pthread.h>
#else

#endif

#include <foundation/interfaces.h>
#include <foundation/macros.h>

#include "FoundationTypes.h"
#include "FoundationUtilHelper.h"
#include "ArrayHelper.h"
