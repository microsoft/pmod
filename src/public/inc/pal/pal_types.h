/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_types.h
****/
#pragma once

#include "pal_core.h"

#if !defined(_WINDOWS)

#include "guiddef.h"

#define VOID void
typedef VOID *PVOID;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef int INT;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef wchar_t WCHAR;
typedef const WCHAR *LPCTSTR;

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed long long    INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned long long  UINT64, *PUINT64;
typedef double              DOUBLE;
typedef float               FLOAT;
typedef bool                boolean;
typedef unsigned char       BYTE;

typedef VOID *LPVOID;

#define TRUE 1
#define FALSE 0

#elif defined(_WINDOWS)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#endif


