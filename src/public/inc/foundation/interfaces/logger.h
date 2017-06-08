/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>
#include <foundation/foundation_macros.h>

#include "enum_value.h"
#include "key_value_pair.h"
#include "dictionary.h"

namespace foundation {

    /////////////////////////////////// Logger Interfaces //////////////////////////////////////////////

    enum class LoggingLevel
    {
        Off,
        Fatal,
        Error,
        Warning,
        Info,
        Debug,
        Trace
    };

    BEGIN_DECLARE_INTERFACE(ILogCategory, IInspectable, FOUNDATION_API)
        // Properties
        STDMETHOD(GetTypeInfo)(_COM_Outptr_ IEnumTypeInfo **ppValue) = 0;
        STDMETHOD(GetParent)(_COM_Outptr_ ILogCategory **ppLogCategory) = 0;
        STDMETHOD(GetSubCategories)(UINT32 *size, _Outptr_ ILogCategory ***ppLogCategories) = 0;
        STDMETHOD(GetLevel)(_Out_ LoggingLevel* level) = 0;
        STDMETHOD(SetLevel)(LoggingLevel level) = 0;
        // methods.
        STDMETHOD(Configure)(_In_ IDictionary *pConfigParameters) = 0;
        STDMETHOD(AddMessageFilter)(UINT32 messageEnumValueId) = 0;
        STDMETHOD(RemoveMessageFilter)(UINT32 messageEnumValueId) = 0;
        STDMETHOD(AddTypeFilter)(GUID iidType) = 0;
        STDMETHOD(RemoveTypeFilter)(GUID iidType) = 0;
        STDMETHOD(SetMessageLevel)(UINT32 messageEnumValueId, LoggingLevel level) = 0;
        STDMETHOD(IsLevelEnabled)(LoggingLevel level, UINT32 messageEnumValueId, _In_opt_ GUID *piidType, _Always_(_Out_) bool *isEnabled) = 0;
        END_DECLARE_INTERFACE()

    BEGIN_DECLARE_IID(ILogParameterPair, FOUNDATION_API);
    typedef IKeyValuePair<IEnumValue*, IInspectable *, IID_ILogParameterPair> ILogParameterPair;

    BEGIN_DECLARE_INTERFACE(ILogRecord, IInspectable, FOUNDATION_API)
        // Properties
        STDMETHOD(GetTimestamp)(_Out_ SystemTime *pValue) = 0;
        STDMETHOD(GetThreadId)(_Out_ UINT32 *pValue) = 0;
        STDMETHOD(GetLevel)(_Out_ LoggingLevel* level) = 0;
        STDMETHOD(GetValue)(_COM_Outptr_ IEnumValue **ppValue) = 0;
        STDMETHOD(GetCategory)(_COM_Outptr_ ILogCategory **ppLogCategory) = 0;
        // methods.
        STDMETHOD(GetParameters)(UINT32 *size, _Outptr_ ILogParameterPair ***ppParameters) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(ILogCreatedEventArgs, IUnknown, FOUNDATION_API)
        STDMETHOD(GetLogRecord)(_COM_Outptr_ ILogRecord **ppLogRecord) = 0;
        STDMETHOD(GetContext)(_COM_Outptr_ IInspectable **ppContext) = 0;
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(ILogCreated, FOUNDATION_API)

    BEGIN_DECLARE_INTERFACE(ILogger, IInspectable, FOUNDATION_API)
        // methods.
        STDMETHOD(ConfigureCategories)(_In_ IDictionary *pConfigParameters) = 0;
        STDMETHOD(GetRootCategory)(_COM_Outptr_ ILogCategory **ppResult) = 0;
        STDMETHOD(GetCategory)(UINT32 size, _In_ UINT32 *pCategories, _COM_Outptr_ ILogCategory **ppResult) = 0;
        STDMETHOD(GetCategoryByName)(_In_ HSTRING categoryPath, _COM_Outptr_ ILogCategory **ppResult) = 0;
        STDMETHOD(Log)(_In_opt_ IInspectable *pContext, _In_ ILogRecord *pLogRecord) = 0;
        // Event methods.
        STDMETHOD(GetLogCreatedEventSource)(_COM_Outptr_ ILogCreatedEventSource** ppEventSource) = 0;
    END_DECLARE_INTERFACE()
}

