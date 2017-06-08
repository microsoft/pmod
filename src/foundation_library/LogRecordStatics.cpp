/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LogRecordStatics.cpp
****/
#include "pch.h"

#include "LogRecordStatics.h"
#include "BinaryStreamProvider.h"
#include "LogCategory.h"
#include "LoggerFactory.h"

#include <sstream>

using namespace foundation;

// forward
static string_t toString(foundation::IInspectable *pInspectable);
static void appendLogParameter(string_t& text, ILogParameterPair* pLogParameterPair);

// Helper to convert an Enum to a String
static string_t toString(IEnumValue *pEnumValue)
{
    foundation::HStringPtr enumStr;
    pEnumValue->ToString(enumStr.GetAddressOf());
    return _pal_GetStringRawBuffer(enumStr, nullptr);
}

static string_t toString(IDictionary* pDictionary)
{
    string_t objStr = U("{");

    foundation::ArrayRefCountWrapper<IDictionaryPair> properties;
    pDictionary->GetView(properties.GetSizeAddressOf(), properties.GetBufferAddressOf());


    for (UINT32 index = 0; index < properties.GetSize(); ++index)
    {
        if (index)
        {
            objStr += U(",");
        }
        foundation::HStringPtr key;
        properties[index]->GetKey(key.GetAddressOf());

        foundation::InspectablePtr spValue;
        properties[index]->GetValue(spValue.GetAddressOf());

        objStr += U("\"");
        objStr += _pal_GetStringRawBuffer(key, nullptr);
        objStr += U("\":");
        objStr += toString(spValue);
    }
    objStr += U("}");
    return objStr;
}

static string_t toString(foundation::IInspectable *pInspectable)
{
    if (!pInspectable)
    {
        return U("null");
    }

    // before attempt to QI IPropertyValue we may need to check an IEnumValue since it 
    // also support the interface
    foundation::ComPtr<IEnumValue> spEnumValue;
    if (SUCCEEDED(foundation::QueryInterface(pInspectable, spEnumValue.GetAddressOf())))
    {

        foundation::ComPtr<IObjectTypeInfo> spEnumTypeInfo;
        spEnumValue->GetTypeInfo(spEnumTypeInfo.GetAddressOf());
        HStringPtr name;
        spEnumTypeInfo->GetName(name.GetAddressOf());

        string_t enum_value_str = name.GetRawBuffer();
        enum_value_str += U('.');

        foundation::HStringPtr text;
        spEnumValue->ToString(text.GetAddressOf());
        enum_value_str += text.GetRawBuffer();

        return enum_value_str;
    }

    foundation::PropertyType type;
    pv_util::GetType(pInspectable, &type);

    if (type == foundation::PropertyType_Inspectable)
    {
        foundation::ComPtr<IDictionary> sDictionary;
        foundation::ComPtr<ILogParameterPair> spLogParameterPair;
        if (SUCCEEDED(foundation::QueryInterface(pInspectable, sDictionary.GetAddressOf())))
        {
            return toString(sDictionary);
        }
        else if (SUCCEEDED(foundation::QueryInterface(pInspectable, spLogParameterPair.GetAddressOf())))
        {
            string_t text;
            appendLogParameter(text, spLogParameterPair);
            return text;
        }
        return U("[object]");
    }
    else if (type == foundation::PropertyType_InspectableArray)
    {
        string_t arrayStr;
        arrayStr += U("[");

        foundation::InspectableArrayWrapper inspectableArrayWrapper;
        IF_ASSERT(pv_util::GetInspectableArray(
            pInspectable,
            inspectableArrayWrapper.GetSizeAddressOf(),
            inspectableArrayWrapper.GetBufferAddressOf()));

        for (UINT32 index = 0; index < inspectableArrayWrapper.GetSize(); ++index)
        {
            if (index)
            {
                arrayStr += U(",");
            }
            arrayStr += toString(inspectableArrayWrapper[index]);
        }
        arrayStr += U("]");
        return arrayStr;
    }
    else
    {
        HStringPtr hstr;
        pv_util::ToString(pInspectable, hstr.GetAddressOf());
        return hstr.GetRawBuffer();
    }
}

// Helper to convert a level enum to a String
static const CHAR_t* toString(LoggingLevel level)
{
    switch (level)
    {
	case LoggingLevel::Off:
        return U("Off");
    case LoggingLevel::Fatal:
        return U("Fatal");
    case LoggingLevel::Error:
        return U("Error");
    case LoggingLevel::Warning:
        return U("Warning");
    case LoggingLevel::Info:
        return U("Info");
    case LoggingLevel::Debug:
        return U("Debug");
    case LoggingLevel::Trace:
        return U("Trace");
    default:
        break;
    }
    foundation_assert(false);
    return U("");
}

static void appendLogParameter(string_t& text, ILogParameterPair* pLogParameterPair)
{
    foundation_assert(pLogParameterPair);

    text += U("{");

    // Get Enum Key
    foundation::ComPtr<IEnumValue> spKey;
    IF_ASSERT(pLogParameterPair->GetKey(spKey.GetAddressOf()));
    text += U("\"");
    text += toString(spKey);
    text += U("\":");

    // Get Logging Value
    foundation::ComPtr<foundation::IInspectable> spValue;
    IF_ASSERT(pLogParameterPair->GetValue(spValue.GetAddressOf()));
    text += toString(spValue);
    text += U("}");
}

static void appendLogParameters(string_t& text,
    UINT32 size,
    ILogParameterPair** ppLogParameterPair)
{
    text += U("{");

    for (UINT32 index = 0; index < size; ++index)
    {
        if (index > 0)
        {
            text += U(",");
        }
        appendLogParameter(text, ppLogParameterPair[index]);
    }
    text += U("}");
}

library::ILogRecordStatics *CLogRecordStatics::GetInstance()
{
    return library::_GetFactoryInstance<CLogRecordStatics, library::ILogRecordStatics>();
}

HRESULT CLogRecordStatics::SerializeLogRecord(
    ILogRecord *pLogRecord,
    std::ostream& os)
{
    foundation_assert(pLogRecord);

    binary_serializer bos(os);

    LoggingLevel level;
    IFR_ASSERT(pLogRecord->GetLevel(&level));
    bos.serializeByte((UINT8)level);

    SystemTime timestamp;
    IFR_ASSERT(pLogRecord->GetTimestamp(&timestamp));
    bos.serializeValue(timestamp);

    UINT32 threadId;
    IFR_ASSERT(pLogRecord->GetThreadId(&threadId));
    bos.serializeUInt(threadId);

    foundation::ComPtr<ILogCategory> spLogCategory;
    IFR_ASSERT(pLogRecord->GetCategory(spLogCategory.GetAddressOf()));

    // serialize the hierarchy
    std::vector<UINT32> categoryTypeIds;
    while(spLogCategory != nullptr)
    {
        foundation::ComPtr<IEnumTypeInfo> spEnumTypeInfo;
        IFR_ASSERT(spLogCategory->GetTypeInfo(spEnumTypeInfo.GetAddressOf()));
        if(spEnumTypeInfo == nullptr)
        {
            break;
        }
        UINT32 typeId;
        IFR_ASSERT(spEnumTypeInfo->GetTypeId(&typeId));

        categoryTypeIds.insert(categoryTypeIds.begin(),typeId);

        foundation::ComPtr<ILogCategory> spNextCategory;
        spLogCategory->GetParent(spNextCategory.GetAddressOf());
        spLogCategory = spNextCategory;
    }

    bos.serializeUInt((UINT32)categoryTypeIds.size());
    for(std::vector<UINT32>::const_iterator iter = categoryTypeIds.begin();
        iter != categoryTypeIds.end();
        ++iter)
    {
        bos.serializeUInt16((UINT16)*iter);
    }
    // Serialize Message
    foundation::ComPtr<IEnumValue> spMessageEnumValue;
    IFR_ASSERT(pLogRecord->GetValue(spMessageEnumValue.GetAddressOf()));
    IFR_ASSERT(CBinaryStreamProvider::SerializeEnum(bos, spMessageEnumValue));

    // Serialize Log Parameters
    foundation::ArrayRefCountWrapper<ILogParameterPair> logParameters;
    IFR_ASSERT(pLogRecord->GetParameters(logParameters.GetSizeAddressOf(),logParameters.GetBufferAddressOf()));

    // array size
    bos.serializeUInt16((UINT16)logParameters.GetSize());

    for(UINT32 index = 0;index < logParameters.GetSize();++index )
    {
        IFR_ASSERT(CBinaryStreamProvider::SerializeLogParameter(bos, logParameters[index]));
    }
    return S_OK;
}

HRESULT CLogRecordStatics::DeserializeLogRecord(
    std::istream& is,
    _COM_Outptr_ ILogRecord **ppLogRecord)
{
    foundation_assert(ppLogRecord);

    {
    binary_deserializer bis(is);
    LoggingLevel level = (LoggingLevel)bis.deserializeByte();
    SystemTime timestamp = bis.deserializeValue<SystemTime>();
    UINT32 threadId = bis.deserializeUInt(); 

    // deserialize category
    std::vector<UINT32> categoryTypeIds;
    categoryTypeIds.resize(bis.deserializeUInt());

    for(size_t i = 0;i < categoryTypeIds.size();++i)
    {
        categoryTypeIds[i] = bis.deserializeUInt16();
    }
    CLogCategory *pLogCategory = nullptr;
    IFR_ASSERT(CLogCategory::GetOrCreateCategory(
        nullptr,
        (UINT32)categoryTypeIds.size(),
        &categoryTypeIds.front(),
        &pLogCategory));
    // deserialize Message
    foundation::ComPtr<IEnumValue> spMessageEnumValue;
    IFR_ASSERT(CBinaryStreamProvider::DeserializeEnum(bis, spMessageEnumValue.GetAddressOf()));

    // deserialize Log Parameters
    UINT32 size = bis.deserializeUInt16();
    _Inspectable_ptr_vector_type logParameters;
    logParameters.resize(size);
    for(size_t index = 0;index < logParameters.size();++index)
    {
        IFR_ASSERT(CBinaryStreamProvider::DeserializeLogParameter(bis, (ILogParameterPair**)logParameters[index].GetAddressOf()));
    }

    IFR_ASSERT(CLoggerFactory::GetLoggerFactory()->
        CreateLogRecordInternal(
        &timestamp,
        threadId,
        level,
        spMessageEnumValue,
        pLogCategory,
        (UINT32)logParameters.size(),
        (ILogParameterPair **)logParameters.front().GetAddressOfPtr(),
        ppLogRecord
        ));
    }
    return S_OK;
}

HRESULT CLogRecordStatics::ToText(_In_ foundation::ILogParameterPair *pLogParameterPair, HSTRING *text)
{
    string_t text_t;
    appendLogParameter(text_t, pLogParameterPair);

    return foundation::HStringPtr(text_t.c_str()).CopyTo(text);
}

HRESULT CLogRecordStatics::ToText(UINT32 size, _In_ foundation::ILogParameterPair **ppLogParameterPair, HSTRING *text)
{
    string_t text_str;
    appendLogParameters(text_str, size, ppLogParameterPair);
    return HStringPtr(text_str.c_str()).CopyTo(text);
}

HRESULT CLogRecordStatics::ToText(foundation::ILogRecord *pLogRecord, string_t& text)
{
    foundation_assert(pLogRecord);
    LoggingLevel level;
    IFR_ASSERT(pLogRecord->GetLevel(&level));
    text += U("Level=");
    text += toString(level);
    text += U(",");

    SystemTime timestamp;
    IFR_ASSERT(pLogRecord->GetTimestamp(&timestamp));

    SystemTime localTimestamp;
    _pal_SystemTimeToLocalTime(timestamp, &localTimestamp);

    text += U("Time=");
    HStringPtr dtStr;
    IFR_ASSERT(pv_util::ToString(localTimestamp, dtStr.GetAddressOf()));
    text += dtStr.GetRawBuffer();
    text += U(",");

    UINT32 threadId;
    IFR_ASSERT(pLogRecord->GetThreadId(&threadId));
    text += U("ThreadId=");
    text += pal::library::_Format_helper(U("%x"), threadId);
    text += U(",");

    foundation::ComPtr<ILogCategory> spLogCategory;
    IFR_ASSERT(pLogRecord->GetCategory(spLogCategory.GetAddressOf()));

    foundation::ComPtr<IEnumTypeInfo> spEnumTypeInfo;
    IFR_ASSERT(spLogCategory->GetTypeInfo(spEnumTypeInfo.GetAddressOf()));
    foundation::HStringPtr categoryName;
    IFR_ASSERT(spEnumTypeInfo->GetName(categoryName.GetAddressOf()));

    text += U("Category=");
    text += _pal_GetStringRawBuffer(categoryName, nullptr);
    text += U(",");

    foundation::ComPtr<IEnumValue> spValue;
    IFR_ASSERT(pLogRecord->GetValue(spValue.GetAddressOf()));
    text += U("Message=");
    text += toString(spValue);

    foundation::ArrayRefCountWrapper<ILogParameterPair> logParameters;
    IFR_ASSERT(pLogRecord->GetParameters(logParameters.GetSizeAddressOf(), logParameters.GetBufferAddressOf()));

    appendLogParameters(text, logParameters.GetSize(), logParameters.GetRawBuffer());
    text += U("\n");

    return S_OK;
}

STDMETHODIMP CLogRecordStatics::SerializeLogRecord(_In_ ILogRecord *pLogRecord, _Out_ UINT32 *size, _Outptr_ UINT8 **data)
{
    std::stringstream os;
    IFR_ASSERT(SerializeLogRecord(pLogRecord, os));
    IFR_ASSERT(StringStreamToArray(os, size, data));
    return S_OK;
}

STDMETHODIMP CLogRecordStatics::DeserializeLogRecord(UINT32 size, UINT8 *data, _COM_Outptr_ ILogRecord **ppLogRecord)
{
    IFCPTR_COM_OUT_PTR_ASSERT(ppLogRecord);

    std::string buffer((const char *)data, size);
    std::stringstream is(buffer);

    _IFR_(DeserializeLogRecord(is, ppLogRecord));
    return S_OK;
}

STDMETHODIMP CLogRecordStatics::SerializeLogRecord(_In_ foundation::ILogRecord *pLogRecord, _In_ foundation::IDictionary *ppDictionary)
{
    return S_OK;
}

STDMETHODIMP CLogRecordStatics::DeserializeLogRecord(_In_ foundation::IDictionary *pDictionary, /*_COM_Outptr_*/ foundation::ILogRecord **ppLogRecord)
{
    IFCPTR_COM_OUT_PTR_ASSERT(ppLogRecord);
    return S_OK;
}

STDMETHODIMP CLogRecordStatics::ToText(_In_ foundation::ILogRecord *pLogRecord, HSTRING *htext)
{
    string_t text;
    IFR_ASSERT(ToText(pLogRecord, text));
    return foundation::HStringPtr(text.c_str()).CopyTo(htext);
}
