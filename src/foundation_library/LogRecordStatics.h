/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LogRecordStatics.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/interfaces/logger_class.h>

//------------------------------------------------------------------------------
// Class:   CLogRecordStatics
//
//------------------------------------------------------------------------------
class CLogRecordStatics :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::ILogRecordStatics
{
public:
    static foundation::library::ILogRecordStatics *GetInstance();

    // Interface ILogRecordStatics
    STDMETHOD(ToText)(_In_ foundation::ILogParameterPair *pLogParameterPair, HSTRING *text) override;
    STDMETHOD(ToText)(UINT32 size, _In_ foundation::ILogParameterPair **ppLogParameterPair, HSTRING *text) override;
    STDMETHOD(ToText)(_In_ foundation::ILogRecord *pLogRecord, HSTRING *htext) override;
    STDMETHOD(SerializeLogRecord)(_In_ foundation::ILogRecord *pLogRecord, _Out_ UINT32 *size, _Outptr_ UINT8 **data) override;
    STDMETHOD(DeserializeLogRecord)(UINT32 size, UINT8 *data, _COM_Outptr_ foundation::ILogRecord **ppLogRecord) override;
    STDMETHOD(SerializeLogRecord)(_In_ foundation::ILogRecord *pLogRecord, _In_ foundation::IDictionary *pDictionary) override;
    STDMETHOD(DeserializeLogRecord)(_In_ foundation::IDictionary *pDictionary, /*_COM_Outptr_*/ foundation::ILogRecord **ppLogRecord) override;

    static HRESULT SerializeLogRecord(foundation::ILogRecord *pLogRecord, std::ostream& os);
    static HRESULT DeserializeLogRecord(std::istream& is, _COM_Outptr_ foundation::ILogRecord **ppLogRecord);
    static HRESULT ToText(foundation::ILogRecord *pLogRecord, foundation::string_t& text);
};
