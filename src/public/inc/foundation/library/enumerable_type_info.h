/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:enumerable_type_info.h
****/
#pragma once

#include "type_info.h"
#include "property_type_info.h"

namespace foundation {

    const UINT32 TypeInfo_Enumerable = 7;

    // EnumerableTypeInfo Structure
    // Summary: Describe a record Structure for a Enumerable Type Info
    struct EnumerableTypeInfo : public TypeInfo
    {
    public:
        EnumerableTypeInfo(
            UINT32 typeId,
            const GUID  &iidType,
            LPCSTR_t    name,
            const foundation::PropertyTypeInfo *pItemTypeInfo,
            UINT32 type = TypeInfo_Enumerable
            ) :
            TypeInfo(typeId, type, iidType, name, nullptr),
            m_pItemTypeInfo(pItemTypeInfo)
        {
        }
        EnumerableTypeInfo():
            m_pItemTypeInfo(nullptr)
        {}
    public:
        const PropertyTypeInfo *m_pItemTypeInfo;   // The Item Type Info
    };

    // type safe collection type info
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_Inspectable;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_String;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_Int32;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_UInt32;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_Int64;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_UInt64;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_Int16;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_UInt16;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_UInt8;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_DateTime;
    EXTERN_C FOUNDATION_API const EnumerableTypeInfo _EnumerableTypeInfo_Boolean;


}

