/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LoggerFactory.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/foundation_library.h>
#include <foundation/library/base_adapter_host.h>

//------------------------------------------------------------------------------
// Class:   CLoggerFactory
//
// Purpose: Implementation for ICoreLoggerFactory
//
//------------------------------------------------------------------------------
class CLoggerFactory :
    public foundation::library::_DefaultAdapterHost
    <
    foundation::library::ILoggerFactory,
    foundation::ctl::ImplementsInspectable
    <
    foundation::library::ILoggerFactory,
    &foundation::library::IID_ILoggerFactory
    >
    >
{
 public:
     static HRESULT GetOrCreateInstance(foundation::library::ILoggerFactory **ppLoggerFactory);
    static CLoggerFactory *GetLoggerFactory();

    // Create a Log Record
    HRESULT CreateLogRecordInternal(
        _In_ foundation::SystemTime *pTimestamp,
        UINT32 threadId,
        foundation::LoggingLevel level,
        _In_ foundation::IEnumValue *pValue,
        _In_ foundation::ILogCategory *pLogCategory,
        UINT32 size,
        _In_opt_ foundation::ILogParameterPair **ppLogParameters,
        _COM_Outptr_ foundation::ILogRecord **ppLogRecord);

protected:
    STDMETHOD(CreateLogger)(
        _In_ const foundation::library::LoggerCreateParameters *pLoggerParameters,
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);
    STDMETHOD(CreateLogParameter)(
        _In_ foundation::IEnumValue *pEnumValue,
        _In_opt_ foundation::IInspectable *pValue,
        _COM_Outptr_ foundation::ILogParameterPair **ppLogParameterPair);
    STDMETHOD(CreateLogRecord)(
        foundation::LoggingLevel level,
        _In_ foundation::IEnumValue *pValue,
        _In_ foundation::ILogCategory *pLogCategory,
        UINT32 size,
        _In_opt_ foundation::ILogParameterPair **ppLogParameters,
        _COM_Outptr_ foundation::ILogRecord **ppLogRecord);
    STDMETHOD(CreateTextLogListener)(
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);
    STDMETHOD(CreateBinaryLogListener)(
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

private:
    static HRESULT SerializeLogValue(_Inout_ foundation::IInspectable **ppLogValue);
};
