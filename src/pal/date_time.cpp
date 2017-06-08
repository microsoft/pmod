/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:date_time.cpp
****/
#include "pch.h"

#include <pal/pal_foundation.h>
#include <foundation/macros.h>
#include <foundation/com_interfaces.h>
#include <foundation/library/datetime_util.h>
#include <pal/pal_thread.h>

#include <time.h>

EXTERN_C PAL_API HRESULT _pal_DateTimeToSystemTime(
    foundation::DateTime dateTime, 
    foundation::SystemTime *systemTime)
{
    IFCPTR_ASSERT(systemTime);

    time_t time = (time_t)(dateTime.UniversalTime/1000);

    _IFR_(foundation::datetime_util::from_time_t(&time, systemTime));
    systemTime->wMilliseconds = dateTime.UniversalTime % 1000;
    return S_OK;
}

EXTERN_C PAL_API HRESULT _pal_SystemTimeToDateTime(
    foundation::SystemTime systemTime,
    foundation::DateTime *dateTime)
{
    IFCPTR_ASSERT(dateTime);

    time_t time;
    _IFR_(foundation::datetime_util::to_time_t(systemTime, &time))
    dateTime->UniversalTime = (((UINT64)time) * 1000) + systemTime.wMilliseconds;
    return S_OK;
}

EXTERN_C PAL_API HRESULT _pal_DateTimeAdd(
    foundation::DateTime dateTime,
    INT32 milliSecs,
    foundation::DateTime *value)
{
    IFCPTR_ASSERT(value);

    value->UniversalTime = dateTime.UniversalTime + milliSecs;
    return S_OK;
}

EXTERN_C PAL_API HRESULT _pal_SystemTimeNow(foundation::SystemTime *pSystemTime)
{
    time_t  t;
    time(&t);

    _IFR_(foundation::datetime_util::from_time_t(&t, pSystemTime));
    pSystemTime->wMilliseconds = _pal_GetTickCount64() % 1000;
    return S_OK;
}

EXTERN_C PAL_API HRESULT _pal_SystemTimeToLocalTime(
    foundation::SystemTime systemTime,
    foundation::SystemTime *pLocalSystemTime)
{
    time_t time;
    _IFR_(foundation::datetime_util::to_time_t(systemTime, &time));

    tm local_time;
    localtime_r(&time, &local_time);
    return S_OK;
}
