/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger_util.h
****/
#pragma once

#include <stdarg.h>
#include <vector>

#include <foundation/assert.h>
#include <foundation/macros.h>
#include <foundation/com_ptr.h>

#include "json_statics_util.h"
#include "interfaces/logger_factory_util.h"

#include <foundation/hstring_wrapper.h>
#include <foundation/activation_factory.h>
#include <foundation/pv_util.h>

namespace foundation
{

struct create_trace_parameter
{
    virtual foundation::IInspectable *operator()() = 0;
};

class logger_util
{
private:
    static inline LPCSTR_t GetActivationId() { return U("foundation.LoggerFactoryUtil"); }
    static library::ILoggerFactoryUtil *GetLoggerFactoryUtil()
    {
        return GetActivationFactory<library::ILoggerFactoryUtil>(GetActivationId());
    }

public:
    typedef std::vector< ComPtr<ILogParameterPair> > log_parameter_pair_ptr_vector_type;

    static void _EnsureAvailable()
    {
        GetActivationFactoryWithCast<library::ILoggerFactoryUtil>(GetActivationId());
    }

    static bool IsAvailable()
    {
        return IsActivationFactoryAvailable(GetActivationId());
    }
    static ILogger *GetFoundationLogger()
    {
        return GetLoggerFactoryUtil()->GetFoundationLogger();
    }
    static library::ILoggerFactory *GetLoggerFactory()
    {
        return GetLoggerFactoryUtil()->GetLoggerFactory();
    }
    static library::ILogRecordStatics *GetLogRecordStatics()
    {
        return GetLoggerFactoryUtil()->GetLogRecordStatics();
    }

    static HRESULT to_text(_In_ ILogParameterPair *pLogParameterPair, HSTRING *hstr)
    {
        return GetLogRecordStatics()->ToText(pLogParameterPair, hstr);
    }

    static HRESULT to_text(UINT32 size,_In_ ILogParameterPair **pLogParameterPair, HSTRING *hstr)
    {
        return GetLogRecordStatics()->ToText(size, pLogParameterPair, hstr);
    }

    static HRESULT to_text(_In_ ILogRecord *pLogRecord, HSTRING *hstr)
    {
        return GetLogRecordStatics()->ToText(pLogRecord, hstr);
    }

    static HRESULT create_foundation_logger(
        _In_ UINT32               rootCategoryTypeId,
        _In_ LoggerOptions        options,
        _COM_Outptr_ ILogger     **ppLogger)
    {
        return GetLoggerFactoryUtil()->CreateLogger(rootCategoryTypeId, options, ppLogger);
    }


	template <class Enum>
    static inline UINT32 ToLogMessageUniqueId(
        UINT32 logMessageEnumTypeId,
        Enum logMessageEnumValue)
    {
        return (logMessageEnumTypeId << 16) + (UINT32)logMessageEnumValue;
    }

    static HRESULT Log(
        _In_ ILogger *pLogger,
        _In_opt_ IInspectable *pContext,
        _In_ LoggingLevel level,
        _In_ UINT32 logMessageEnumValueId,
        _In_ ILogCategory *pLogCategory,
        _In_ UINT32 logRecordTypeId,
        ...)
    {
        va_list args;
        va_start(args, logRecordTypeId);

        std::vector<foundation::library::_LogParameter> logParameters;
        for (;;)
        {
            foundation::library::_LogParameter logParameter;
            logParameter._logRecordPropertyId = va_arg(args, UINT32);
            if (logParameter._logRecordPropertyId == (UINT32)-1)
            {
                break;
            }
            logParameter._pLogRecordValue = va_arg(args, foundation::IInspectable *);
            logParameters.push_back(logParameter);
        }
        return GetLoggerFactoryUtil()->Log(
            pLogger,
            pContext,
            level,
            logMessageEnumValueId,
            pLogCategory,
            logRecordTypeId,
            logParameters.size() ? &logParameters.front() : nullptr,
            (UINT32)logParameters.size());
    }

    static HRESULT CreateLogParameter(
        IEnumValue *pLogEnumValue,
        foundation::IInspectable *pLogValue,
        ILogParameterPair** ppLogParameterPair)
    {
        return GetLoggerFactoryUtil()->CreateLogParameter(
            pLogEnumValue,
            pLogValue,
            ppLogParameterPair);
    }

    static HRESULT CreateLogParameter(
        UINT32 logEnumValueId,
        foundation::IInspectable *pLogValue,
        ILogParameterPair** ppLogParameterPair)
    {
        return GetLoggerFactoryUtil()->CreateLogParameter(
            logEnumValueId,
            pLogValue,
            ppLogParameterPair);
    }

    static HRESULT AppendLogParameter(
        IEnumValue *pLogEnumValue,
        foundation::IInspectable *pLogValue,
        log_parameter_pair_ptr_vector_type& logParameters)
    {
        ComPtr<ILogParameterPair> spLogParameterPair;
        IFR_ASSERT(CreateLogParameter(
            pLogEnumValue,
            pLogValue,
            spLogParameterPair.GetAddressOf()));
        logParameters.push_back(spLogParameterPair);
        return S_OK;
    }

    static HRESULT AppendLogParameter(
        UINT32 logEnumValueId,
        _In_opt_ foundation::IInspectable *pLogValue,
        log_parameter_pair_ptr_vector_type& logParameters)
    {
        ComPtr<ILogParameterPair> spLogParameterPair;
        IFR_ASSERT(CreateLogParameter(
            logEnumValueId,
            pLogValue,
            spLogParameterPair.GetAddressOf()));
        logParameters.push_back(spLogParameterPair);
        return S_OK;
    }

    static HRESULT LogTrace(
        LoggingLevel logLevel,
        Error_Category errorEnumId,
        HRESULT hr,
        LPCSTR_t     pFileName,
        INT32        iLine,
        LPCSTR_t     pSource,
        _In_opt_ foundation::IInspectable *pParameter = nullptr)
    {
        return GetLoggerFactoryUtil()->LogTrace(
            logLevel,
            errorEnumId,
            hr,
            pFileName,
            iLine,
            pSource,
            pParameter);
    }

    static HRESULT LogMessage(
        LoggingLevel logLevel,
        UINT32      messageId,
        LPCSTR_t    text,
        _In_opt_ foundation::IInspectable *pParameter = nullptr)
    {
        ComPtr<ILogCategory> log_category_ptr;
        IFR_ASSERT(
            GetCategory(
            GetFoundationLogger(),
            log_category_ptr.GetAddressOf(),
            Foundation_CategoryEnum::TypeId,
            (UINT32)-1));

        if (IsLevelEnabled(log_category_ptr, logLevel, Foundation_Category_Message_Id))
        {
            return Log(
                GetFoundationLogger(),
                nullptr,
                logLevel,
                Foundation_Category_Message_Id,
                log_category_ptr,
                Message_LogRecordEnum::TypeId,
                // Id
                Message_LogRecord::Id,
                pv_util::CreateValue(messageId).Get(),
                // Text
                Message_LogRecord::Text,
                pv_util::CreateValue(text).Get(),
                // Parameter
                Message_LogRecord::Parameter,
                pParameter,
                // end
                (UINT32)-1
                );
        }
        return S_FALSE;
    }

    template <class T>
    static HRESULT LogMessageWithParameter(
        LoggingLevel logLevel,
        UINT32      messageId,
        LPCSTR_t    text,
        T value)
    {

        return LogMessage(logLevel, messageId, text, pv_util::CreateValue(value));
    }

#if defined(_VARIADIC_SUPPORT)
    template<typename... Args>
    static HRESULT LogMessageWithParameters(
        LoggingLevel logLevel,
        UINT32      messageId,
        LPCSTR_t    text,
        Args&&... args)
    {
        std::vector<InspectablePtr> values;
		_PassValues(_ExpandValues(values, args)...);

        return LogMessage(logLevel, messageId, text, foundation::CreateValueFromVectorPtr(values));
    }
#endif

    static HRESULT GetCategory(
        _In_ ILogger *pLogger,
        _COM_Outptr_ ILogCategory **ppLogCategory,
        _In_ UINT32 categoryTypeId,
        _In_ ...)
    {
        va_list args;
        va_start( args, categoryTypeId );

        std::vector<UINT32> categoryTypeIdVector;
        while (categoryTypeId != (UINT32)-1)
        {
            categoryTypeIdVector.push_back(categoryTypeId);
            // next category id
            categoryTypeId = va_arg(args, UINT32);
        }
        return pLogger->GetCategory(
            (UINT32)categoryTypeIdVector.size(),
            &(categoryTypeIdVector.front()),
            ppLogCategory
            );
    }
    
    static bool IsLevelEnabled(
        _In_ ILogCategory *pLogCategory,
        _In_    foundation::LoggingLevel level,
        _In_ UINT32 messageEnumValue,
        _In_opt_ GUID *pType = nullptr)
    {
        bool isEnabled(false);
        pLogCategory->IsLevelEnabled(level,messageEnumValue,pType,&isEnabled);
        return isEnabled;
    }

    inline static bool IsDebugEnabled(
        _In_ ILogCategory *pLogCategory,
        _In_ UINT32 messageEnumValue,
        _In_opt_ GUID *pType = nullptr
        )
    {
        return IsLevelEnabled(pLogCategory, LoggingLevel::Debug, messageEnumValue, pType);
    }

    inline static bool IsInfoEnabled(
        _In_ ILogCategory *pLogCategory,
        _In_ UINT32 messageEnumValue,
        _In_opt_ GUID *pType = nullptr
        )
    {
        return IsLevelEnabled(pLogCategory, LoggingLevel::Info, messageEnumValue,pType);
    }

    inline static bool IsWarningEnabled(
        _In_ ILogCategory *pLogCategory,
        _In_ UINT32 messageEnumValue,
        _In_opt_ GUID *pType = nullptr
        )
    {
        return IsLevelEnabled(pLogCategory, LoggingLevel::Warning, messageEnumValue, pType);
    }

    inline static HRESULT ConfigureLogger(
        _In_ ILogger *pLogger,
        _In_ LPCSTR_t json_configuration)
    {
        IFCPTR_ASSERT(pLogger);

        InspectablePtr spJson;
        IFR_ASSERT(json_util::GetJsonStatics()->FromJson(foundation::HStringRef(json_configuration), spJson.GetAddressOf()));
        IFR_ASSERT(pLogger->ConfigureCategories(ComQIPtr<IDictionary>(spJson)));
        return S_OK;
    }

    inline static HRESULT ConfigureLoggerFromFile(
        _In_ ILogger *pLogger,
        _In_ const char *file_path)
    {
        ComPtr<IDictionary> logger_config_ptr;
        IFR_ASSERT(foundation::json_util::ParseFromFile(
            file_path,
            *logger_config_ptr.GetAddressOf()));
        IFR_ASSERT(pLogger->ConfigureCategories(logger_config_ptr));
        return S_OK;
    }

    inline static HRESULT ConfigureLoggerFromFile(
        _In_ const char *file_path)
    {
        return ConfigureLoggerFromFile(GetFoundationLogger(), file_path);
    }

    inline static HRESULT GetCategory(
        _In_ ILogger *pLogger,
        _In_opt_ LPCSTR_t p_category_path,
        _COM_Outptr_ ILogCategory **ppLogCategory
        )
    {
        IFCPTR_ASSERT(pLogger);

        if (p_category_path == nullptr)
        {
            _IFR_(pLogger->GetRootCategory(ppLogCategory));
        }
        else
        {
            _IFR_(pLogger->GetCategoryByName(HStringRef(p_category_path), ppLogCategory));
        }
        return S_OK;
    }

    inline static HRESULT GetCategoryLevel(
        _In_ ILogger *pLogger,
        _Out_ LoggingLevel& level,
        _In_opt_ LPCSTR_t p_category_path = nullptr)
    {
        ComPtr<ILogCategory> log_category_ptr;
        IFR_ASSERT(GetCategory(pLogger, p_category_path, log_category_ptr.GetAddressOf()));
        IFR_ASSERT(log_category_ptr->GetLevel(&level));
        return S_OK;
    }

    inline static HRESULT GetCategoryLevel(
        _Out_ LoggingLevel& level,
        _In_opt_ LPCSTR_t p_category_path = nullptr)
    {
        return GetCategoryLevel(GetFoundationLogger(), level, p_category_path);
    }

    inline static HRESULT SetCategoryLevel(
        _In_ ILogger *pLogger,
        _In_ LoggingLevel level,
        _In_opt_ LPCSTR_t p_category_path = nullptr)
    {
        ComPtr<ILogCategory> log_category_ptr;
        IFR_ASSERT(GetCategory(pLogger, p_category_path, log_category_ptr.GetAddressOf()));
        IFR_ASSERT(log_category_ptr->SetLevel(level));
        return S_OK;
    }

    inline static HRESULT SetCategoryLevel(
        _In_ LoggingLevel level,
        _In_ LPCSTR_t p_category_path = nullptr)
    {
        return SetCategoryLevel(GetFoundationLogger(), level, p_category_path);
    }

};

}
