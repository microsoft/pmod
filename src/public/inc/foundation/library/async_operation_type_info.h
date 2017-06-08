/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_type_info.h
****/
#pragma once

#include "type_info.h"
#include "property_type_info.h"

namespace foundation {

    const UINT32 TypeInfo_AsyncOperation = 0x10;

    // AsyncOperationTypeInfo Structure
    // Summary: Describe a record Structure for a Async Operation Result Type Info
    struct AsyncOperationTypeInfo : public TypeInfo
    {
    public:
        AsyncOperationTypeInfo(
            UINT32        typeId,
            const GUID    &iidType,
            LPCSTR_t    name,
            const PropertyTypeInfo *pResultTypeInfo
            ) :
            TypeInfo(typeId, TypeInfo_AsyncOperation, iidType, name, nullptr),
            m_p_result_type_info(pResultTypeInfo)
        {
        }

        AsyncOperationTypeInfo():
            m_p_result_type_info(nullptr)
        {}
    public:
        const PropertyTypeInfo *m_p_result_type_info;   // The Result Type Info
    };

}

