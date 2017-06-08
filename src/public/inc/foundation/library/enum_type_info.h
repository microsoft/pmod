/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:enum_type_info.h
****/
#pragma once

#include "type_info.h"

namespace foundation {

    const UINT32 TypeInfo_EnumType = 0xff;

    // EnumTypeInfo Structure
    // Summary: Describe a Enum type structure
    struct EnumTypeInfo : public TypeInfo
    {
    public:
        EnumTypeInfo(
            UINT32 typeId,
            const GUID       &iidType,
            const LPCSTR_t  name,
            bool            isFlags,
            const LPCSTR_t  *pNames,
            const UINT32	*pValues,
            const EnumTypeInfo *baseType = nullptr
            ) :
            TypeInfo(typeId, TypeInfo_EnumType, iidType, name, baseType),
            m_is_flags(isFlags),
            m_names(pNames),
            m_values(pValues)
        {
        }
        EnumTypeInfo():
            m_is_flags(false),
            m_names(nullptr),
            m_values(nullptr)
        {}
#ifdef _MSC_VER
        EnumTypeInfo & operator=(const EnumTypeInfo &) {}
#endif
    public:
        const bool      m_is_flags;
        const LPCSTR_t  *m_names;
        const UINT32    *m_values;
    };


}

