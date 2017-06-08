/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:command_type_info.h
****/
#pragma once

#include <foundation/library/type_info.h>
#include <foundation/library/property_type_info.h>

namespace pmod {

    const UINT32 TypeInfo_CommandModel = 0x02;

    // CommandTypeInfo Structure
    // Summary: Describe a record Structure for a Command Model Info
    struct CommandTypeInfo : public foundation::TypeInfo
    {
    public:
        CommandTypeInfo(
            UINT32 typeId,
            const GUID       &iidType,
            const LPCSTR_t    name,
            const foundation::PropertyTypeInfo *pParameterTypeInfo,
            const foundation::PropertyTypeInfo *pResultTypeInfo,
            bool isAsync
            ) :
            TypeInfo(typeId, TypeInfo_CommandModel, iidType, name, nullptr),
            m_p_parameter_type_info(pParameterTypeInfo),
            m_p_result_type_info(pResultTypeInfo),
            m_isAsync(isAsync)
        {
        }
        CommandTypeInfo():
            m_p_parameter_type_info(nullptr),
            m_p_result_type_info(nullptr),
            m_isAsync(false)
        {}
    public:
        const foundation::PropertyTypeInfo *m_p_parameter_type_info;   // The Parameter Type Info
        const foundation::PropertyTypeInfo *m_p_result_type_info;      // The Result Type Info
        bool                     m_isAsync;              // if Command is Async
    };

    // ObjectObjectCommandModelInfo
    struct ObjectObjectCommandTypeInfo
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const CommandTypeInfo _CommandTypeInfo;
    };

    // AsyncCommandModelInfo
    struct AsyncCommandTypeInfo
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const CommandTypeInfo _CommandTypeInfo;
    };

    // SimpleCommandModelInfo
    struct SimpleCommandTypeInfo
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const CommandTypeInfo _CommandTypeInfo;
    };

    // SimpleAsyncCommandModelInfo
    struct SimpleAsyncCommandTypeInfo
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const CommandTypeInfo _CommandTypeInfo;
    };

}
