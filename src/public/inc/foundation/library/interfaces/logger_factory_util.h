/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger_factory_util.h
****/
#pragma once

#include <foundation/com_ptr.h>
#include <foundation/logger_type_info.h>
#include <foundation/library/interfaces/logger_factory.h>

namespace foundation
{
    namespace library
    {
        struct _LogParameter {
            UINT32          _logRecordPropertyId;
            IInspectable    *_pLogRecordValue;

        };
        // ILoggerFactoryUtil Interface
        BEGIN_DECLARE_INTERFACE(ILoggerFactoryUtil, foundation::IUnknown, FOUNDATION_LIB_API)
            virtual ILogger *GetFoundationLogger() = 0;
            virtual library::ILoggerFactory *GetLoggerFactory() = 0;
            virtual library::ILogRecordStatics *GetLogRecordStatics() = 0;
            virtual HRESULT CreateLogger(
                _In_ UINT32               rootCategoryTypeId,
                _In_ LoggerOptions        options,
                _COM_Outptr_ ILogger     **ppLogger) = 0;

            virtual HRESULT Log(
                _In_ ILogger *pLogger,
                _In_opt_ IInspectable *pContext,
                _In_ LoggingLevel level,
                _In_ UINT32 logMessageEnumValueId,
                _In_ ILogCategory *pLogCategory,
                _In_ UINT32 logRecordTypeId,
                _In_opt_ _LogParameter *pLogParameters,
                _In_ UINT32 logParametersSize) = 0;

            virtual HRESULT CreateLogParameter(
                _In_ IEnumValue *pLogEnumValue,
                _In_opt_ foundation::IInspectable *pLogValue,
                _COM_Outptr_ ILogParameterPair** ppLogParameterPair) = 0;

            virtual HRESULT CreateLogParameter(
                UINT32 logEnumValueId,
                _In_opt_ foundation::IInspectable *pLogValue,
                _COM_Outptr_ ILogParameterPair** ppLogParameterPair) = 0;

            virtual HRESULT LogTrace(
                LoggingLevel logLevel,
                Error_Category errorEnumId,
                HRESULT hr,
                LPCSTR_t     pFileName,
                INT32        iLine,
                LPCSTR_t     pSource,
                _In_opt_ foundation::IInspectable *pParameter) = 0;
        END_DECLARE_INTERFACE()
    }
}

