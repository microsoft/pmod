/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:property_type_info.h
****/
#pragma once

#include "type_info.h"
#include <foundation/foundation_api.h>

namespace foundation {

    // PropertyTypeInfo Structure
    // Summary: Describe a record Structure for a Property Info
    struct PropertyTypeInfo
    {
    public:
        PropertyTypeInfo(
            foundation::PropertyType propertyType,
            const TypeInfo *pTypeInfo) :
            m_property_type(propertyType),
            m_p_type_info(pTypeInfo)
        {
        }
        PropertyTypeInfo() :
            m_property_type(foundation::PropertyType_Empty),
            m_p_type_info(nullptr)
        {
        }
    public:
        foundation::PropertyType    m_property_type;   // Type of the Property
        const TypeInfo             *m_p_type_info;    // Type Info of the Property
    };

    ////////////////////// Type Info Support ///////////////////////////////////
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _StringTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _Int32TypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _UInt32TypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _Int16TypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _UInt16TypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _Int64TypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _UInt64TypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _UInt8TypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _BoolTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _SingleTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _DoubleTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _DateTimeTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _InspectableTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _VoidTypeInfo;

    // arrays
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _StringArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _Int32ArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _UInt32ArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _Int16ArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _UInt16ArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _Int64ArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _UInt64ArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _UInt8ArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _BoolArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _DoubleArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _DateTimeArrayTypeInfo;
    EXTERN_C FOUNDATION_API  PropertyTypeInfo _ObjectArrayTypeInfo;

    EXTERN_C FOUNDATION_API TypeInfo _ObjectTypeInfo;
    EXTERN_C FOUNDATION_API PropertyTypeInfo _ObjectPropertyTypeInfo;

    EXTERN_C FOUNDATION_API TypeInfo _DictionaryTypeInfo;
    EXTERN_C FOUNDATION_API PropertyTypeInfo _DictionaryPropertyTypeInfo;

}

