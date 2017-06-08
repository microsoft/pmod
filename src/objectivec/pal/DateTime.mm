/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DateTime.mm
****/

#import <Foundation/NSDate.h>
#import <Foundation/NSCalendar.h>


#import <pal/pal_foundation.h>
#import <foundation/macros.h>
#import <foundation/com_interfaces.h>
#import <math.h>

static HRESULT ToSystemTime(NSDate *nsDate,foundation::SystemTime *pSystemTime)
{
    unsigned units = NSCalendarUnitDay |
    NSCalendarUnitMonth |
    NSCalendarUnitYear |
    NSCalendarUnitHour |
    NSCalendarUnitMinute |
    NSCalendarUnitSecond |
#if !defined(TARGET_OS_IPHONE)
    NSCalendarUnitNanosecond |
#endif
    NSCalendarUnitWeekday;

    NSDateComponents *components = [[NSCalendar currentCalendar] components:units fromDate:nsDate];
    
    pSystemTime->wYear = components.year;
    pSystemTime->wMonth = components.month;
    pSystemTime->wDay = components.day;
    pSystemTime->wHour = components.hour;
    pSystemTime->wMinute = components.minute;
    pSystemTime->wSecond = components.second;
    pSystemTime->wDayOfWeek = components.weekday;
#if !defined(TARGET_OS_IPHONE)
    pSystemTime->wMilliseconds = components.nanosecond / 1000000;
#else
    NSTimeInterval time = nsDate.timeIntervalSinceReferenceDate;
    double secs;
    double millisecs = modf(time,&secs);
    pSystemTime->wMilliseconds = (USHORT)(millisecs * 1000);
#endif
    return S_OK;
}

EXTERN_C PAL_API HRESULT _pal_DateTimeToSystemTime(
                                                   foundation::DateTime dateTime,
                                                   foundation::SystemTime *pSystemTime)
{
    IFCPTR_ASSERT(pSystemTime);
    
    NSTimeInterval *pTimeInterval = reinterpret_cast<NSTimeInterval *>(&dateTime.UniversalTime);
    NSDate *date = [NSDate dateWithTimeIntervalSinceReferenceDate:*pTimeInterval];
    return ToSystemTime(date,pSystemTime);
}

EXTERN_C PAL_API HRESULT _pal_SystemTimeToDateTime(
                                                   foundation::SystemTime systemTime,
                                                   foundation::DateTime *pDateTime)
{
    IFCPTR_ASSERT(pDateTime);

    NSDateComponents *components  = [[NSDateComponents alloc] init];
    components.year = systemTime.wYear;
    components.month = systemTime.wMonth;
    components.day = systemTime.wDay;
    components.hour = systemTime.wHour;
    components.minute = systemTime.wMinute;
    components.second = systemTime.wSecond;
#if !defined(TARGET_OS_IPHONE)
    components.nanosecond = systemTime.wMilliseconds * 1000000LL;
#endif
    NSDate *nsDate = [[[NSCalendar currentCalendar] dateFromComponents:components] autorelease];
    
    NSTimeInterval *pTimeInterval = reinterpret_cast<NSTimeInterval *>(&pDateTime->UniversalTime);
    *pTimeInterval  = nsDate.timeIntervalSinceReferenceDate;
    return S_OK;
}

EXTERN_C PAL_API HRESULT _pal_DateTimeAdd(
                                          foundation::DateTime dateTime,
                                          INT32 milliSecs,
                                          foundation::DateTime *value)
{
    IFCPTR_ASSERT(value);

    NSTimeInterval *pTimeInterval = reinterpret_cast<NSTimeInterval *>(&dateTime.UniversalTime);
    
    NSDate *nsDate = [NSDate dateWithTimeIntervalSinceReferenceDate:*pTimeInterval];
    NSDate *result =[nsDate dateByAddingTimeInterval:((NSTimeInterval)milliSecs) / 1000];
    
    *reinterpret_cast<NSTimeInterval *>(&value->UniversalTime) = [result timeIntervalSinceReferenceDate];
    return S_OK;
}


EXTERN_C PAL_API HRESULT _pal_SystemTimeNow(foundation::SystemTime *pSystemTime)
{
    return ToSystemTime([NSDate date],pSystemTime);
}

EXTERN_C PAL_API HRESULT _pal_SystemTimeToLocalTime(
                                                    foundation::SystemTime systemTime,
                                                    foundation::SystemTime *pLocalSystemTime)
{
    *pLocalSystemTime = systemTime;
    return S_OK;
}
