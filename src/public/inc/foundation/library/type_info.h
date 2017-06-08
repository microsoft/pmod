/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_info.h
****/

#pragma once

#include <foundation/com_interfaces.h>

namespace foundation {

    const UINT32 TypeInfo_None = 0;

    const UINT32 TypeInfo_Object = 8;
    const UINT32 TypeInfo_Dictionary = 9;

    // TypeInfo Structure
    // Summary: Describe a base type information
    struct TypeInfo
    {
    public:
        TypeInfo(
            UINT32 typeId,
            UINT32 type,
            const GUID     &iidType,
            LPCSTR_t name,
            const TypeInfo *baseType) :
            m_type(type),
            m_type_id(typeId),
            m_p_iid_type(&iidType),
            m_name(name),
            m_base_type(baseType)
        {
        }
        TypeInfo() :
            m_type(0),
            m_type_id(0),
            m_p_iid_type(&foundation_GUID_NULL),
            m_name(nullptr),
            m_base_type(nullptr)
        {
        }

        inline const GUID& get_iid_type() const
        {
            return *m_p_iid_type;
        }

#ifdef _MSC_VER
        TypeInfo & operator=(const TypeInfo &) {}
#endif
        UINT32                  m_type;           // The Type of Type Info from this structure
        const UINT32            m_type_id;         // a Unique Id describing this Type
        const GUID              *m_p_iid_type;        // a GUID Type to identify this Type
        LPCSTR_t                m_name;           // Name of this Type
        const TypeInfo         *m_base_type;  // The Base Type  associated with this Model Info
    };
}

