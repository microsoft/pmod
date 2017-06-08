/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_load_library.h
****/
#pragma once

#include "pal_api.h"
#include "pal_core.h"

#if defined(_WINDOWS)
#include <Windows.h>
typedef HMODULE _pal_module_t;
#else
typedef void * _pal_module_t;
#endif

EXTERN_C PAL_API _pal_module_t _pal_LoadLibrary(_In_ LPCSTR_t moduleName);
EXTERN_C PAL_API HRESULT _pal_FreeLibrary(_In_ _pal_module_t hModule);
