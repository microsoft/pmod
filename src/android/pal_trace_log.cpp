/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_trace_log.cpp
****/
#include "foundation/pal.h"

#include <android/log.h>

void _pal_Trace(LPCSTR_t message)
{
    __android_log_write(ANDROID_LOG_DEBUG, "pmod", message);
}
