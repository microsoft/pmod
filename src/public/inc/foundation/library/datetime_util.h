/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:datetime_util.h
****/
#pragma once

#include <foundation/pv_util.h>
#include <time.h>

namespace foundation {

    class datetime_util
    {
    public:
        static HRESULT Initialize(SystemTime *p_system_time)
        {
            p_system_time->wYear = _YEAR_OFFSET;
            p_system_time->wMonth = 1;
            p_system_time->wDay = 1;
            p_system_time->wHour = 0;
            p_system_time->wMinute = 0;
            p_system_time->wSecond = 0;
            p_system_time->wMilliseconds = 0;

            return S_OK;
        }

        static HRESULT from_tm_t(
            const struct tm& time,
            SystemTime *p_system_time)
        {
            p_system_time->wYear = time.tm_year + 1900;
            p_system_time->wMonth = time.tm_mon + 1;
            p_system_time->wDay = time.tm_mday;
            p_system_time->wHour = time.tm_hour;
            p_system_time->wMinute = time.tm_min;
            p_system_time->wSecond = time.tm_sec;
            p_system_time->wMilliseconds = 0;
            return S_OK;
        }

        static HRESULT from_time_t(
            const time_t *timep,
            SystemTime *p_system_time)
        {
            IFCPTR_ASSERT(p_system_time);

            struct tm time;
#if defined(_WINDOWS)
            gmtime_s(&time, timep);
#else
            gmtime_r(timep, &time);
#endif
            return from_tm_t(time, p_system_time);
        }

        static HRESULT to_time_t(
            const SystemTime& system_time,
            time_t *timep)
        {
            IFCPTR_ASSERT(timep);

            struct tm time;

            time.tm_year = system_time.wYear - 1900;
            time.tm_mon = system_time.wMonth -1;
            time.tm_mday = system_time.wDay;
            time.tm_hour = system_time.wHour;
            time.tm_min = system_time.wMinute;
            time.tm_sec = system_time.wSecond;
            time.tm_isdst = 0;

#if defined(_WINDOWS)
            *timep = _mkgmtime(&time);
#else
            *timep = timegm(&time);
#endif
            return S_OK;
        }

        static InspectablePtr CreateValue(const time_t *timep)
        {
            foundation_assert(timep != nullptr);
            SystemTime system_time;
            from_time_t(timep, &system_time);
            return pv_util::CreateValue(system_time);
        }
    private:
#ifdef ANDROID
        static time_t
            timegm(struct tm *tm)
        {
            time_t ret = mktime(tm);
            struct tm time;
            time_t t = 0;
            localtime_r(&t,&time);
            if(time.tm_year == 69)
            {
                ret -= (24 - time.tm_hour) * 3600;
            }
            else
            {
                ret += (time.tm_hour) * 3600;
            }
            return ret;
        }
#endif
    };

}

