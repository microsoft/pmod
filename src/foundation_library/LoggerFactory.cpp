/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LoggerFactory.cpp
****/
#include "pch.h"

#include "LoggerFactory.h"
#include "Logger.h"
#include "TextLogListener.h"
#include "BinaryLogListener.h"
#include <foundation/library/logger_util.h>
#include <foundation/library/key_value_pair_impl.h>
#include <pal/pal_thread.h>

using namespace foundation;

// Serialize factory types from the 'Core' interfaces
static HRESULT SerializeFactoryLogValue(_Inout_ foundation::IInspectable **ppLogValue)
{
    foundation_assert(ppLogValue);
    foundation_assert(*ppLogValue);


    logger_util::log_parameter_pair_ptr_vector_type logParametersVector;

    IObject *pObject = nullptr;
    if (SUCCEEDED(foundation::QueryWeakReference(*ppLogValue, &pObject)))
    {
        foundation::ComPtr<IObjectTypeInfo> spTypeInfo;
        IFR_ASSERT(pObject->GetTypeInfo(spTypeInfo.GetAddressOf()));

        // UniqueId
		UINT32 uniqueId;
        IFR_ASSERT(pObject->GetUniqueId(&uniqueId));
        IFR_ASSERT(logger_util::AppendLogParameter(
            Object_LogRecord_UniqueId_Id,
            foundation::pv_util::CreateValue((UINT32)uniqueId),
            logParametersVector));

        // TypeIId
        IID iidType;
        IFR_ASSERT(spTypeInfo->GetType(&iidType));
        IFR_ASSERT(logger_util::AppendLogParameter(
            Object_LogRecord_TypeId_Id,
            foundation::pv_util::CreateValue(iidType),
            logParametersVector));

    }
    if(logParametersVector.size())
    {
        (*ppLogValue)->Release();
        *ppLogValue = nullptr;
        IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
                (UINT32)logParametersVector.size(),
                (foundation::IInspectable **)logParametersVector.front().GetAddressOfPtr(),
                ppLogValue));
        return S_OK;
    }
    return S_FALSE;
}

//------------------------------------------------------------------------------
// Class:   CLogParameterPair
//
// Purpose: Implementation for ILogParameterPair
//
//------------------------------------------------------------------------------
typedef library::_InspectableKeyValuePair<ILogParameterPair, foundation::_RefCountedValueAlloc<IEnumValue *>> CLogParameterPair;

//------------------------------------------------------------------------------
// Class:   CLogRecord
//
// Purpose: Implementation for ILogRecord
//
//------------------------------------------------------------------------------
class CLogRecord:
    public foundation::library::_DefaultAdapterHost
    <
        ILogRecord,
        foundation::ctl::ImplementsInspectable
        <
            ILogRecord,
            &IID_ILogRecord
        >
    >
{
public:
    static HRESULT CreateInstance(
        LoggingLevel level,
        _In_ IEnumValue *pValue,
        _In_ ILogCategory *pLogCategory,
        UINT32 size,
        _In_opt_ ILogParameterPair **ppLogParameters,
        _COM_Outptr_ ILogRecord **ppLogRecord)
    {
        SystemTime now;
        _pal_SystemTimeNow(&now);
        UINT32 threadId = (UINT32)_pal_CurrentThreadId();
        return CreateInstance(
                &now,
                threadId,
                level,
                pValue,
                pLogCategory,
                size,
                ppLogParameters,
                ppLogRecord);
    }
    static HRESULT CreateInstance(
        _In_ SystemTime *pTimestamp,
        UINT32 threadId,
        LoggingLevel level,
        _In_ IEnumValue *pValue,
        _In_ ILogCategory *pLogCategory,
        UINT32 size,
        _In_opt_ ILogParameterPair **ppLogParameters,
        _COM_Outptr_ ILogRecord **ppLogRecord)
    {
        foundation_assert(pTimestamp);

        CLogRecord *pLogRecord = nullptr;
        IFR_ASSERT(ctl::inspectable::CreateInstance(&pLogRecord));

        pLogRecord->_timeStamp = *pTimestamp;
        pLogRecord->_threadId = threadId;
        pLogRecord->_level = level;
        pLogRecord->_spEnumValue = pValue;
        pLogRecord->_spLogCategory = pLogCategory;
        if (ppLogParameters != nullptr)
        {
            while (size--)
            {
                pLogRecord->_logParameters.push_back(*ppLogParameters);
                ++ppLogParameters;
            }
        }
        *ppLogRecord = pLogRecord;
        return S_OK;
    }
protected:
    STDMETHOD(GetTimestamp)(SystemTime *pValue)
    {
        IFCPTR_ASSERT(pValue);
        *pValue = _timeStamp;
        return S_OK;
    }

    STDMETHOD(GetThreadId)(UINT32 *pValue)
    {
        IFCPTR_ASSERT(pValue);
        *pValue = _threadId;
        return S_OK;
    }

    STDMETHOD (GetLevel)(LoggingLevel* level)
    {
        IFCPTR_ASSERT(level);
        *level = _level;
        return S_OK;
    }
    STDMETHOD (GetValue)(IEnumValue **ppValue)
    {
        IFCPTR_ASSERT(ppValue);
        return _spEnumValue.CopyTo(ppValue);
    }
    STDMETHOD (GetCategory)(ILogCategory **ppLogCategory)
    {
        IFCPTR_ASSERT(ppLogCategory);
        return _spLogCategory.CopyTo(ppLogCategory);
    }
    // methods.
    STDMETHOD (GetParameters)(UINT32 *size,ILogParameterPair ***ppParameters)
    {
        ContainerRefCountToArray(_logParameters,size,ppParameters);
        return S_OK;
    }

private:
    SystemTime                  _timeStamp;
    UINT32                      _threadId;
    LoggingLevel                _level;
    foundation::ComPtr<IEnumValue>    _spEnumValue;
    foundation::ComPtr<ILogCategory>  _spLogCategory;
    std::vector<foundation::ComPtr<ILogParameterPair> > _logParameters;
};


CLoggerFactory *CLoggerFactory::GetLoggerFactory()
{
	return library::_GetFactoryInstance<CLoggerFactory, CLoggerFactory>();
}

HRESULT CLoggerFactory::GetOrCreateInstance(library::ILoggerFactory **ppCoreLoggerFactory)
{
    IFCPTR_ASSERT(ppCoreLoggerFactory);
    *ppCoreLoggerFactory = GetLoggerFactory();
    (*ppCoreLoggerFactory)->AddRef();
    return S_OK;
}

STDMETHODIMP CLoggerFactory::CreateLogger(
    _In_ const library::LoggerCreateParameters *pCoreLoggerParam,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CLogger::CreateInstance(pCoreLoggerParam,pOuter,ppNewInstance);
}

STDMETHODIMP CLoggerFactory::CreateLogParameter(
    _In_ IEnumValue *pEnumValue,
    _In_opt_ foundation::IInspectable *pValue,
    _COM_Outptr_ ILogParameterPair **ppLogParameterPair)
{
    IFCPTR_ASSERT(pEnumValue);
    IFCPTR_ASSERT(ppLogParameterPair);

    foundation::InspectablePtr spLogValue(pValue);
    IFR_ASSERT(SerializeLogValue(spLogValue.GetAddressOfPtr()));

    return foundation::library::CreateKeyValuePair<CLogParameterPair>(
        pEnumValue,
        spLogValue,
        ppLogParameterPair);
}

HRESULT CLoggerFactory::CreateLogRecordInternal(
    _In_ SystemTime *pTimestamp,
    UINT32 threadId,
    LoggingLevel level,
    _In_ IEnumValue *pValue,
    _In_ ILogCategory *pLogCategory,
    UINT32 size,
    _In_opt_ ILogParameterPair **ppLogParameters,
    _COM_Outptr_ ILogRecord **ppLogRecord)
{
    return CLogRecord::CreateInstance(
        pTimestamp,
        threadId,
        level,
        pValue,
        pLogCategory,
        size,
        ppLogParameters,
        ppLogRecord);
}

STDMETHODIMP CLoggerFactory::CreateLogRecord(
    LoggingLevel level,
    _In_ IEnumValue *pValue,
    _In_ ILogCategory *pLogCategory,
    UINT32 size,
    _In_opt_ ILogParameterPair **ppLogParameters,
    _COM_Outptr_ ILogRecord **ppLogRecord)
{
    return CLogRecord::CreateInstance(
        level,
        pValue,
        pLogCategory,
        size,
        ppLogParameters,
        ppLogRecord);
}

STDMETHODIMP CLoggerFactory::CreateTextLogListener(
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    IFCPTR_COM_OUT_PTR_ASSERT(ppNewInstance);

    return CTextLogListener::CreateInstance(pOuter,ppNewInstance);
}

STDMETHODIMP CLoggerFactory::CreateBinaryLogListener(
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    IFCPTR_ASSERT(ppNewInstance);

    return CBinaryLogListener::CreateInstance(pOuter,ppNewInstance);
}

HRESULT CLoggerFactory::SerializeLogValue(_Inout_ foundation::IInspectable **ppLogValue)
{
    foundation_assert(ppLogValue);
    if(*ppLogValue == nullptr || IsPropertyValue(*ppLogValue))
    {
        // quickly return if it is Property Value or Null
        return S_FALSE;
    }

    library::ILoggerSerialize *pLoggerSerialize = nullptr;
    if (*ppLogValue && SUCCEEDED(foundation::QueryWeakReference(*ppLogValue, &pLoggerSerialize)))
    {
        foundation::ArrayRefCountWrapper<ILogParameterPair> logParameters;
        IFR_ASSERT(pLoggerSerialize->CreateLogParameters(
            logParameters.GetSizeAddressOf(),
            logParameters.GetBufferAddressOf()));
        for(UINT32 index = 0 ;index < logParameters.GetSize();++index)
        {
            foundation::InspectablePtr spLogValue;
            IFR_ASSERT(logParameters[index]->GetValue(spLogValue.GetAddressOf()));

            HRESULT hr = SerializeLogValue(spLogValue.GetAddressOfPtr());
            IFHR_ASSERT(hr);
            if(hr == S_OK)
            {
                CLogParameterPair *pLogParameterPair = static_cast<CLogParameterPair *>(logParameters[index]);
                pLogParameterPair->_SetValue(spLogValue);
            }
        }
        // Release and prepare the return as an Array
        (*ppLogValue)->Release();
        *ppLogValue = nullptr;
        IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
                logParameters.GetSize(),
                (foundation::IInspectable **)logParameters.GetBuffer(),
                ppLogValue));
        return S_OK;
    }
    return SerializeFactoryLogValue(ppLogValue);
}
