/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_datetime.h
****/
#pragma once

namespace foundation {
    const USHORT _YEAR_OFFSET = 1601;
}

// Define a PAL abstraction for a date time structure
#if defined(_WINDOWS)

namespace foundation {
    typedef SYSTEMTIME SystemTime;
}

#else

namespace foundation {
    typedef struct _SystemTime {
    USHORT wYear;
    USHORT wMonth;
    USHORT wDayOfWeek;
    USHORT wDay;
    USHORT wHour;
    USHORT wMinute;
    USHORT wSecond;
    USHORT wMilliseconds;
    } SystemTime;
}

#endif

EXTERN_C PAL_API HRESULT _pal_DateTimeToSystemTime(
    foundation::DateTime dateTime,
    _Out_ foundation::SystemTime *pSystemTime);

EXTERN_C PAL_API HRESULT _pal_SystemTimeToDateTime(
    foundation::SystemTime systemTime,
	_Out_ foundation::DateTime *pDateTime);

EXTERN_C PAL_API HRESULT _pal_SystemTimeToLocalTime(
    foundation::SystemTime systemTime,
	_Out_ foundation::SystemTime *pLocalSystemTime);

EXTERN_C PAL_API HRESULT _pal_DateTimeAdd(
    foundation::DateTime dateTime,
    INT32 milliSecs,
	_Out_ foundation::DateTime *value);

EXTERN_C PAL_API HRESULT _pal_SystemTimeNow(_Out_ foundation::SystemTime *pSystemTime);
