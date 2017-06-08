/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pinvoke_api.h
****/

#if defined(_WINDOWS) || defined __CYGWIN__
#define PINVOKE_API __declspec(dllexport)
#else
#define PINVOKE_API __attribute__ ((visibility ("default")))
#endif

