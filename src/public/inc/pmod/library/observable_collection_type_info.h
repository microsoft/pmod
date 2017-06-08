/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection_type_info.h
****/
#pragma once

#include <foundation/library/enumerable_type_info.h>

namespace pmod {

    const UINT32 TypeInfo_Collection = 0x03;
    const UINT32 TypeInfo_List = 0x04;


    // type safe collection type info
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_Inspectable;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_String;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_Int32;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_UInt32;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_Int64;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_UInt64;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_Int16;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_UInt16;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_UInt8;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_DateTime;
    EXTERN_C PMOD_API const foundation::EnumerableTypeInfo _CollectionTypeInfo_Boolean;


}

