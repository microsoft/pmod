/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger_factory.h
****/
#pragma once

#include <foundation/library/type_info_create_parameter.h>
#include <foundation/interfaces/logger.h>
#include "logger_class.h"
#include "../foundation_lib_api.h"

namespace foundation
{
    enum class LoggerOptions
    {
        None = 0x00
    };

    namespace library
    {
        struct LoggerCreateParameters : public _CreateParameterBase<library::ILoggerDelegate, LoggerOptions>
        {
            LoggerCreateParameters(UINT32 rootCategoryId) :
            m_root_category_id(rootCategoryId)
            {
            }

            UINT32 m_root_category_id;
        };
        // ILoggerFactory Interface
        BEGIN_DECLARE_INTERFACE(ILoggerFactory, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(CreateLogger)(
                _In_ const LoggerCreateParameters *paramerters,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
            STDMETHOD(CreateLogParameter)(
                _In_ IEnumValue *pEnumValue,
                _In_opt_ foundation::IInspectable *pValue,
                _COM_Outptr_ ILogParameterPair **ppLogParameterPair) = 0;
            STDMETHOD(CreateLogRecord)(
                LoggingLevel level,
                _In_ IEnumValue *pValue,
                _In_ ILogCategory *pLogCategory,
                UINT32 size,
                _In_opt_ ILogParameterPair **ppLogParameters,
                _COM_Outptr_ ILogRecord **ppLogRecord) = 0;
            STDMETHOD(CreateTextLogListener)(
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
            STDMETHOD(CreateBinaryLogListener)(
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

