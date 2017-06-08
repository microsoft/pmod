/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_dispatch_type_info.h
****/
#pragma once

#include <foundation/library/type_info.h>
#include <foundation/library/property_type_info.h>

namespace foundation {

	const UINT32 TypeInfo_PropertyData = 10;

    // PropertyInfo Structure
    // Summary: Describe a record Structure for a Property Info
    struct PropertyInfo : public foundation::PropertyTypeInfo
    {
    public:
        PropertyInfo(
            UINT32 id,
            LPCSTR_t name,
            foundation::PropertyType propertyType,
            int flags,
            const foundation::TypeInfo *modelTypeInfo) :
            PropertyTypeInfo(propertyType, modelTypeInfo),
            m_id(id),
            m_name(name),
            m_flags(flags)
        {
        }
        PropertyInfo() :
            m_id(0),
            m_name(nullptr),
            m_flags(0)
        {
        }

    public:
        UINT32      m_id;             // Property Id
        LPCSTR_t    m_name;           // Property Name
        int         m_flags;          // Flag defined for Properties
    };

    // MethodParamInfo Structure
    // Summary: Describe a record Structure for a Property Info
    struct MethodParamInfo : public foundation::PropertyTypeInfo
    {
    public:
        MethodParamInfo(
            LPCSTR_t name,
            foundation::PropertyType propertyType,
            const foundation::TypeInfo *modelTypeInfo) :
            foundation::PropertyTypeInfo(propertyType, modelTypeInfo),
            m_parameterName(name)
        {
        }

        MethodParamInfo() :
            m_parameterName(nullptr)
        {
        }
    public:
        LPCSTR_t     m_parameterName;           // Param Name
    };

    struct MethodInfo
    {
    public:
        MethodInfo(UINT32 methodId,
            LPCSTR_t       name,
            const MethodParamInfo  *pParameters,
            const foundation::PropertyTypeInfo *pResultTypeInfo,
            bool isAsync) :
            m_methodId(methodId),
            m_methodName(name),
            m_parameters(pParameters),
            m_resultTypeInfo(pResultTypeInfo),
            m_isAsync(isAsync)
        {
        }

        MethodInfo() :
            m_methodId(0),
            m_methodName(nullptr),
            m_parameters(nullptr),
            m_resultTypeInfo(nullptr),
            m_isAsync(false)
        {
        }
#ifdef _MSC_VER
        MethodInfo & operator=(const MethodInfo &) {}
#endif
    public:
        const UINT32                 m_methodId;           // Method Id
        LPCSTR_t                     m_methodName;         // Method Name
        const MethodParamInfo*     m_parameters;         // Array of Method Parameters
        const foundation::PropertyTypeInfo     *m_resultTypeInfo;    // The Result Type Info
        bool                         m_isAsync;            // if Method is Async
    };

    // ObjectDispatchTypeInfo Structure
    // Summary: Describe a record Structure for a Property Data Info
    struct ObjectDispatchTypeInfo : public foundation::TypeInfo
    {
    public:
		ObjectDispatchTypeInfo(UINT32 typeId,
            const GUID  &iidType,
            LPCSTR_t       name,
            const PropertyInfo *properties,
            const MethodInfo *methods,
            const ObjectDispatchTypeInfo *baseType,
            UINT32 modelTypeInfoType = TypeInfo_PropertyData) :
            TypeInfo(typeId, modelTypeInfoType, iidType, name, baseType),
            m_properties(properties),
            m_methods(methods)
        {
        }
		ObjectDispatchTypeInfo():
            m_properties(nullptr),
            m_methods(nullptr)
        {}
    public:
        const PropertyInfo*    m_properties;     // Array of Property Info to describe each exposed Property
        const MethodInfo*      m_methods;        // Array of Method Info to describe each exposed Method
    };
}

