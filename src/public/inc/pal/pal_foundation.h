/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_foundation.h
****/
#pragma once

#include <pal/pal_types.h>
#include <pal/pal_hstring.h>

namespace foundation {
    // Foundation const definitions
    const UINT32 _InfiniteTimeout = (UINT32)-1;
    const UINT32 NINDEX = (UINT32)-1;

    // Our base Reserved TypeId
    const UINT32 _ReservedTypeId_Base = 0xffff0000;
}

#if !defined(_WINDOWS) || defined(_WINDOWS_CLASSIC)

#define _PM_FUNDATIONTYPES_ENABLED

namespace foundation {

    enum PropertyType
    {
        PropertyType_Empty = 0,
        PropertyType_UInt8 = 1,
        PropertyType_Int16 = 2,
        PropertyType_UInt16 = 3,
        PropertyType_Int32 = 4,
        PropertyType_UInt32 = 5,
        PropertyType_Int64 = 6,
        PropertyType_UInt64 = 7,
        PropertyType_Single = 8,
        PropertyType_Double = 9,
        PropertyType_Char16 = 10,
        PropertyType_Boolean = 11,
        PropertyType_String = 12,
        PropertyType_Inspectable = 13,
        PropertyType_DateTime = 14,
        PropertyType_TimeSpan = 15,
        PropertyType_Guid = 16,
        PropertyType_Point = 17,
        PropertyType_Size = 18,
        PropertyType_Rect = 19,
        PropertyType_OtherType = 20,
        PropertyType_UInt8Array = 1025,
        PropertyType_Int16Array = 1026,
        PropertyType_UInt16Array = 1027,
        PropertyType_Int32Array = 1028,
        PropertyType_UInt32Array = 1029,
        PropertyType_Int64Array = 1030,
        PropertyType_UInt64Array = 1031,
        PropertyType_SingleArray = 1032,
        PropertyType_DoubleArray = 1033,
        PropertyType_Char16Array = 1034,
        PropertyType_BooleanArray = 1035,
        PropertyType_StringArray = 1036,
        PropertyType_InspectableArray = 1037,
        PropertyType_DateTimeArray = 1038,
        PropertyType_TimeSpanArray = 1039,
        PropertyType_GuidArray = 1040,
        PropertyType_PointArray = 1041,
        PropertyType_SizeArray = 1042,
        PropertyType_RectArray = 1043,
        PropertyType_OtherTypeArray = 1044
    };

    typedef enum PropertyType PropertyType;
}

#endif

#if defined(_WINDOWS_RUNTIME)

#include <windows.foundation.h>

namespace foundation {
    // for windows runtime support re define the foundation types under our namespace
    typedef ABI::Windows::Foundation::PropertyType PropertyType;
    typedef ABI::Windows::Foundation::DateTime DateTime;
    typedef ABI::Windows::Foundation::TimeSpan TimeSpan;
    typedef ABI::Windows::Foundation::Point Point;
    typedef ABI::Windows::Foundation::Size Size;
    typedef ABI::Windows::Foundation::Rect Rect;

#define foundation_IID_IPropertyValue  ABI::Windows::Foundation::IID_IPropertyValue

    const PropertyType PropertyType_Empty = ABI::Windows::Foundation::PropertyType_Empty;
    const PropertyType PropertyType_UInt8 = ABI::Windows::Foundation::PropertyType_UInt8;
    const PropertyType PropertyType_Int16 = ABI::Windows::Foundation::PropertyType_Int16;
    const PropertyType PropertyType_UInt16 = ABI::Windows::Foundation::PropertyType_UInt16;
    const PropertyType PropertyType_Int32 = ABI::Windows::Foundation::PropertyType_Int32;
    const PropertyType PropertyType_UInt32 = ABI::Windows::Foundation::PropertyType_UInt32;
    const PropertyType PropertyType_Int64 = ABI::Windows::Foundation::PropertyType_Int64;
    const PropertyType PropertyType_UInt64 = ABI::Windows::Foundation::PropertyType_UInt64;
    const PropertyType PropertyType_Single = ABI::Windows::Foundation::PropertyType_Single;
    const PropertyType PropertyType_Double = ABI::Windows::Foundation::PropertyType_Double;
    const PropertyType PropertyType_Char16 = ABI::Windows::Foundation::PropertyType_Char16;
    const PropertyType PropertyType_Boolean = ABI::Windows::Foundation::PropertyType_Boolean;
    const PropertyType PropertyType_String = ABI::Windows::Foundation::PropertyType_String;
    const PropertyType PropertyType_Inspectable = ABI::Windows::Foundation::PropertyType_Inspectable;
    const PropertyType PropertyType_DateTime = ABI::Windows::Foundation::PropertyType_DateTime;
    const PropertyType PropertyType_TimeSpan = ABI::Windows::Foundation::PropertyType_TimeSpan;
    const PropertyType PropertyType_Guid = ABI::Windows::Foundation::PropertyType_Guid;
    const PropertyType PropertyType_Point = ABI::Windows::Foundation::PropertyType_Point;
    const PropertyType PropertyType_Size = ABI::Windows::Foundation::PropertyType_Size;
    const PropertyType PropertyType_Rect = ABI::Windows::Foundation::PropertyType_Rect;

    const PropertyType PropertyType_UInt8Array = ABI::Windows::Foundation::PropertyType_UInt8Array;
    const PropertyType PropertyType_Int16Array = ABI::Windows::Foundation::PropertyType_Int16Array;
    const PropertyType PropertyType_UInt16Array = ABI::Windows::Foundation::PropertyType_UInt16Array;
    const PropertyType PropertyType_Int32Array = ABI::Windows::Foundation::PropertyType_Int32Array;
    const PropertyType PropertyType_UInt32Array = ABI::Windows::Foundation::PropertyType_UInt32Array;
    const PropertyType PropertyType_Int64Array = ABI::Windows::Foundation::PropertyType_Int64Array;
    const PropertyType PropertyType_UInt64Array = ABI::Windows::Foundation::PropertyType_UInt64Array;
    const PropertyType PropertyType_SingleArray = ABI::Windows::Foundation::PropertyType_SingleArray;
    const PropertyType PropertyType_DoubleArray = ABI::Windows::Foundation::PropertyType_DoubleArray;
    const PropertyType PropertyType_DateTimeArray = ABI::Windows::Foundation::PropertyType_DateTimeArray;
    const PropertyType PropertyType_GuidArray = ABI::Windows::Foundation::PropertyType_GuidArray;
    const PropertyType PropertyType_Char16Array = ABI::Windows::Foundation::PropertyType_Char16Array;
    const PropertyType PropertyType_BooleanArray = ABI::Windows::Foundation::PropertyType_BooleanArray;
    const PropertyType PropertyType_StringArray = ABI::Windows::Foundation::PropertyType_StringArray;
    const PropertyType PropertyType_InspectableArray = ABI::Windows::Foundation::PropertyType_InspectableArray;

}

#else

namespace foundation {

    struct DateTime
    {
        // The number of 100-nanosecond intervals that have elapsed since 12:00:00 midnight on January 1, 1601 CE, UTC time.
        INT64 UniversalTime;
    };
    struct TimeSpan
    {
        INT64 Duration;
    };
}

#endif

#include "pal_datetime.h"
