/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_object_type_info.h
****/
#pragma once

#include <foundation/library/object_dispatch_type_info.h>
#include <pmod/pmod_api.h>

namespace pmod {

    const UINT32 TypeInfo_PropertyModel = 0x01;
    const UINT32 TypeInfo_PropertyViewModel = 0x20;

    struct EventInfo
    {
    public:
        EventInfo(UINT32 eventId,
            LPCSTR_t       name,
            const foundation::PropertyTypeInfo *pEventArgsTypeInfo) :
            m_eventId(eventId),
            m_eventName(name),
            m_pEventArgsTypeInfo(pEventArgsTypeInfo)
        {
        }

        EventInfo() :
            m_eventId(0),
            m_eventName(nullptr),
            m_pEventArgsTypeInfo(nullptr)
        {
        }
#ifdef _MSC_VER
        EventInfo & operator=(const EventInfo &) {}
#endif
    public:
        const UINT32                 m_eventId;                 // Event Id
        LPCSTR_t                     m_eventName;              // Event Name
        const foundation::PropertyTypeInfo     *m_pEventArgsTypeInfo; // The EventArgs Type Info
    };

    // ObservableObjectTypeInfo Structure
    // Summary: Describe a record Structure for a Property Model Info
    struct ObservableObjectTypeInfo : public foundation::ObjectDispatchTypeInfo
    {
    public:
        ObservableObjectTypeInfo(UINT32 typeId,
            const GUID  &iidType,
            LPCSTR_t       name,
            const foundation::PropertyInfo *properties,
            const foundation::MethodInfo *methods,
            const EventInfo *events,
            const ObservableObjectTypeInfo *baseType,
            UINT32 modelTypeInfoType = TypeInfo_PropertyModel) :
			ObjectDispatchTypeInfo(typeId, iidType, name, properties, methods, baseType, modelTypeInfoType),
            m_events(events)
        {
        }
        ObservableObjectTypeInfo():
            m_events(nullptr)
        {}
    public:
        const EventInfo*       m_events;         // Array of Event Info to describe each exposed Event
    };

    // ViewModelTypeInfo Structure
    // Summary: Describe a record Structure for a Property Model View Info
    struct ViewModelTypeInfo : public ObservableObjectTypeInfo
    {
    public:
        ViewModelTypeInfo(int type,
            const GUID  &  iIdType,
            LPCSTR_t       name,
            const foundation::PropertyInfo *properties,
            const foundation::MethodInfo *methods,
            const EventInfo *events,
            const ObservableObjectTypeInfo *baseType,
            const ObservableObjectTypeInfo *sourceModelType,
            bool inheritProperties) :
            ObservableObjectTypeInfo(type,
            iIdType,
            name,
            properties,
            methods,
            events,
            baseType,
            TypeInfo_PropertyViewModel),
            m_p_source_model_type_info(sourceModelType),
            m_inherit_properties(inheritProperties)
        {
        }
        ViewModelTypeInfo():
            m_inherit_properties(false),
            m_p_source_model_type_info(nullptr)
        {}
    public:
        const ObservableObjectTypeInfo *m_p_source_model_type_info; // The Model Type associated with a View
        bool m_inherit_properties;
    };

    EXTERN_C PMOD_API const ObservableObjectTypeInfo   _ObservableObjectType;

    // IAsyncOperation
    struct AsyncOperation
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const ObservableObjectTypeInfo ObservableObjectInfo;
        static const foundation::PropertyInfo PropertyInfos[];

        static const UINT32 Property_Result = 1;
        static const UINT32 Property_Error = 2;
        static const UINT32 Property_ResultStatus = 3;
        static const UINT32 Property_Progress = 4;
    };

    // IObservableCollection
    struct ObservableCollection
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const ObservableObjectTypeInfo ObservableObjectInfo;
        static const foundation::PropertyInfo PropertyInfos[];

        static const UINT32 Property_Count = 1;
        static const UINT32 Property_IsEmpty = 2;
    };

    // ICommand
    struct CommandModel
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const ObservableObjectTypeInfo ObservableObjectInfo;
        static const foundation::PropertyInfo PropertyInfos[];

        static const UINT32 Property_State = 1;
        static const UINT32 Property_CommandId = 2;
        static const UINT32 Property_IsEnabled = 3;
    };

    // IPropertyBinding
    struct  PropertyBinding
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const ObservableObjectTypeInfo ObservableObjectInfo;
        static const foundation::PropertyInfo PropertyInfos[];

        static const UINT32 Property_Value = 1;
    };

    // ICommand View
    struct CommandViewModel
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const ObservableObjectTypeInfo ObservableObjectInfo;
        static const foundation::PropertyInfo PropertyInfos[];

        static const UINT32 Property_CommandLabel = 3;
    };
    // IEnumValue
    struct EnumValue
    {
        static PMOD_API const IID IIDType;
        static PMOD_API const ObservableObjectTypeInfo ObservableObjectInfo;
        static const foundation::PropertyInfo PropertyInfos[];

        static const UINT32 Property_Value = 1;
        static const UINT32 Property_Name = 2;
    };
}

