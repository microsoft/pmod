/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LoggerUtil.cpp
****/

#include "pch.h"

#include "LogCategory.h"
#include "Logger.h"
#include "LoggerFactory.h"
#include "LogRecordStatics.h"

#include <foundation/library/logger_util.h>
#include <foundation/pv_util.h>
#include <foundation/type_info_statics_util.h>

using namespace foundation;

// {C0E8F2F4-4B34-4875-8C76-C1CC919BB64D}
const GUID library::IID_ILoggerFactoryUtil =
{ 0xc0e8f2f4, 0x4b34, 0x4875,{ 0x8c, 0x76, 0xc1, 0xcc, 0x91, 0x9b, 0xb6, 0x4d } };

class CLoggerFactoryUtil : 
    public foundation::ctl::ComBase,
    public library::ILoggerFactoryUtil

{
public:
    static library::ILoggerFactoryUtil *GetInstance()
    {
        return library::_GetFactoryInstance<CLoggerFactoryUtil, library::ILoggerFactoryUtil>();
    }
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == library::ILoggerFactoryUtil::GetIID())
        {
            *ppInterface = static_cast<library::ILoggerFactoryUtil *>(this);
        }
        else
        {
            return ctl::ComBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

ILogger *GetFoundationLogger() override
{
    return CLogger::GetLogger();
}

library::ILoggerFactory *GetLoggerFactory() override
{
    return CLoggerFactory::GetLoggerFactory();
}

library::ILogRecordStatics *GetLogRecordStatics() override
{
    return CLogRecordStatics::GetInstance();
}

HRESULT CreateLogger(
    _In_ UINT32                   rootCategoryTypeId,
    _In_ LoggerOptions        options,
    _COM_Outptr_ ILogger     **ppLogger) override
{
    foundation_assert(ppLogger);

    library::LoggerCreateParameters coreLoggerParam(rootCategoryTypeId);
    coreLoggerParam.m_options = options;

    foundation::InspectablePtr spNewInstance;
    IFR_ASSERT(GetLoggerFactory()->CreateLogger(
        &coreLoggerParam,
        nullptr,
        spNewInstance.GetAddressOf()));
    return foundation::QueryInterface(spNewInstance,ppLogger);
}

HRESULT Log(
    _In_ ILogger *pLogger,
    _In_ foundation::IInspectable *pContext,
    _In_ LoggingLevel level,
    _In_ UINT32 logMessageEnumValueId,
    _In_ ILogCategory *pLogCategory,
    _In_ UINT32 logRecordTypeId,
    _In_ library::_LogParameter *pLogParameters,
    _In_ UINT32 logParametersSize)
{
    IFCPTR_ASSERT(pLogger);
    IFCPTR_ASSERT(pLogCategory);

    foundation::ComPtr<IEnumValue> spLogMessageEnumValue;
    IFR_ASSERT(foundation::GetEnumValue(
        logMessageEnumValueId >> 16,
        logMessageEnumValueId & 0xffff,
        spLogMessageEnumValue.GetAddressOf()));

    library::ILoggerFactory *pLoggerFactory = GetLoggerFactory();

    // prepare the vector to fill parameters
    std::vector<foundation::ComPtr<ILogParameterPair> > logParameters;
    logParameters.resize(logParametersSize);

    // the log record Enum Type Info
    foundation::ComPtr<IEnumTypeInfo> spLogRecordEnumTypeInfo;
    IFR_ASSERT(foundation::GetEnumTypeInfo(logRecordTypeId, spLogRecordEnumTypeInfo.GetAddressOf()));

    // fill the parameters
    for (size_t index = 0; index < logParametersSize; ++index, ++pLogParameters)
    {

        foundation::ComPtr<IEnumValue> spLogRecordPropertyEnumValue;
        IFR_ASSERT(spLogRecordEnumTypeInfo->GetEnumValue(
            pLogParameters->_logRecordPropertyId,
            spLogRecordPropertyEnumValue.ReleaseAndGetAddressOf()));

        IFR_ASSERT(pLoggerFactory->CreateLogParameter(
            spLogRecordPropertyEnumValue,
            pLogParameters->_pLogRecordValue,
            logParameters[index].GetAddressOf()));
    }

    foundation::ComPtr<ILogRecord> spLogRecord;
    IFR_ASSERT(pLoggerFactory->CreateLogRecord(
        level,
        spLogMessageEnumValue,
        pLogCategory,
        (UINT32)logParameters.size(),
        logParameters.size() ? logParameters.front().GetAddressOfPtr() : nullptr,
        spLogRecord.GetAddressOf()
        ));

    return pLogger->Log(pContext,spLogRecord);
}
HRESULT CreateLogParameter(
    IEnumValue *pLogEnumValue,
    foundation::IInspectable *pLogValue,
    ILogParameterPair** ppLogParameterPair)
{
    IFR_ASSERT(GetLoggerFactory()->CreateLogParameter
        (pLogEnumValue, pLogValue, ppLogParameterPair));
    return S_OK;
}

HRESULT CreateLogParameter(
    UINT32 logEnumValueId,
    foundation::IInspectable *pLogValue,
    ILogParameterPair** ppLogParameterPair)
{
    foundation::ComPtr<IEnumValue> spEnumValue;
    IFR_ASSERT(foundation::GetEnumValue(
        logEnumValueId >> 16, 
        logEnumValueId & 0xffff, 
        spEnumValue.GetAddressOf()));

    IFR_ASSERT(GetLoggerFactory()->CreateLogParameter
        (spEnumValue, pLogValue, ppLogParameterPair));
    return S_OK;
}

HRESULT LogTrace(
    LoggingLevel logLevel,
    Error_Category errorEnumId,
    HRESULT hr,
    LPCSTR_t     pFileName,
    INT32       iLine,
    LPCSTR_t     pSource,
    foundation::IInspectable *pParameter) override
{
    foundation_assert(pFileName);
    // Validat the logger is availble before trying to use it
    if(!CLogger::IsAvailable())
    {
        return S_OK;
    }

    foundation::string_t filename = pFileName;
    size_t pos;
    if( (pos = filename.rfind('\\')) != std::string::npos
        ||
        (pos = filename.rfind('/')) != std::string::npos)
    {
        filename = filename.substr(pos + 1);
    }

    UINT32 categoryTypeId = Error_CategoryEnum::TypeId;
    foundation::ComPtr<ILogCategory> spLogErrorCategory;
    IFR_ASSERT(GetFoundationLogger()->GetCategory(1,&categoryTypeId,spLogErrorCategory.GetAddressOf()));

    foundation::InspectablePtr spErrorCode;
    if (hr == S_OK || hr == S_FALSE)
    {
        foundation::GetEnumValue(ErrorsEnum::TypeId, hr, reinterpret_cast<IEnumValue **>(spErrorCode.GetAddressOf()));
    }
    else
    {
        // next line will attempt to resolve the Enum Value from our cache
        if (GetEnumValue(
            (UINT32)hr, reinterpret_cast<IEnumValue **>(spErrorCode.GetAddressOf())) != S_OK)
        {
            // fallback when no HRESULT code was found in our cache
            spErrorCode = foundation::pv_util::CreateValue((INT32)hr);
        }
    }

    return logger_util::Log(
        GetFoundationLogger(),
        nullptr,
        logLevel,
        // Message Enum Value
        logger_util::ToLogMessageUniqueId(Error_CategoryEnum::TypeId, errorEnumId),
        // Log Category
        spLogErrorCategory,
        Error_LogRecordEnum::TypeId,
        // HResult
        Error_LogRecord::HResult,
        spErrorCode.Get(),
        // File
        Error_LogRecord::File,
        foundation::pv_util::CreateValue(filename.c_str()).Get(),
        // Line
        Error_LogRecord::Line,
        foundation::pv_util::CreateValue(iLine).Get(),
        // Source
        Error_LogRecord::Source,
        foundation::pv_util::CreateValue(pSource).Get(),
        // Parameter
        Error_LogRecord::Parameter,
        pParameter,
        -1);
}

};

library::ILoggerFactoryUtil *GetLoggerFactoryUtilInstance()
{
    return CLoggerFactoryUtil::GetInstance();
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.LoggerFactoryUtil"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&GetLoggerFactoryUtilInstance)
    );



