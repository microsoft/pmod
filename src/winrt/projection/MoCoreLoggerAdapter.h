/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreLoggerAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

#include "MoBaseAdapter.h"


using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoCoreLoggerAdapter
//
// Purpose: Implementation of ICoreLogger Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoCoreLoggerAdapter :
    public CMoBaseAdapter
    <
    foundation::ILogger,
    ILogger,
    ILogCreatedEventHandler,
    ILogCreatedEventArgs
    >
{
public:

    // Interface ILogger
    HRESULT STDMETHODCALLTYPE get_RootCategory( 
        ILogCategory **value) override
    {
        foundation::ComPtr<foundation::ILogCategory> spLogCategory;
        IFR_ASSERT(this->m_pInner->GetRootCategory(spLogCategory.GetAddressOf()));
        return foundation::QueryInterface(spLogCategory,value);
    }

    HRESULT STDMETHODCALLTYPE GetCategory( 
        UINT32 length,
        UINT32 *categoryTypeIdValues,
        ILogCategory **value) override
    {
        foundation::ComPtr<foundation::ILogCategory> spLogCategory;
        IFR_ASSERT(this->m_pInner->GetCategory(length,categoryTypeIdValues,spLogCategory.GetAddressOf()));
        return foundation::QueryInterface(spLogCategory,value);
    }

    HRESULT STDMETHODCALLTYPE GetCategoryByName( 
        HSTRING categoryPath,
        ILogCategory **value) override
    {
        foundation::ComPtr<foundation::ILogCategory> spLogCategory;
        IFR_ASSERT(this->m_pInner->GetCategoryByName(categoryPath,spLogCategory.GetAddressOf()));
        return foundation::QueryInterface(spLogCategory,value);
    }

    HRESULT STDMETHODCALLTYPE Log(
        IInspectable *pContext,
        ILogRecord *pLogRecord) override
    {
        IFCPTR_ASSERT(pLogRecord);

        foundation::ComPtr<foundation::ILogRecord> spLogRecord;
        IFR_ASSERT(foundation::QueryInterface(pLogRecord,spLogRecord.GetAddressOf()));
        return this->m_pInner->Log(pContext,spLogRecord);
    }

    HRESULT STDMETHODCALLTYPE ConfigureCategories(
        ABI::Microsoft::PropertyModel::IDictionary *pConfigureParameters)
    {
        IFCPTR_ASSERT(pConfigureParameters);

        foundation::ComPtr<foundation::IDictionary> spConfigureParameters;
        IFR_ASSERT(foundation::QueryInterface(pConfigureParameters, spConfigureParameters.GetAddressOf()));
        return this->m_pInner->ConfigureCategories(spConfigureParameters);
    }
    HRESULT STDMETHODCALLTYPE ConfigureCategoriesWithJson(HSTRING jsonConfig)
    {
        foundation::ComPtr<foundation::IDictionary> logger_config_ptr;
        IFR_ASSERT(foundation::json_util::FromJson(
            jsonConfig,
            *logger_config_ptr.GetAddressOf()));
        return this->m_pInner->ConfigureCategories(logger_config_ptr);
    }

    HRESULT STDMETHODCALLTYPE add_LogCreated( 
        ILogCreatedEventHandler *value,
        EventRegistrationToken *token) override
    {
        return this->add_Handler(value,token);
    }

    HRESULT STDMETHODCALLTYPE remove_LogCreated( 
        EventRegistrationToken token) override
    {
        return this->remove_Handler(token);
    }
};
//------------------------------------------------------------------------------
// Class:   CMoLogCategoryAdapter
//
// Purpose: Implementation of ILogCategory Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoLogCategoryAdapter :
    public foundation::AdapterBase
    <
    foundation::ILogCategory,
    ILogCategory,
    &__uuidof(ILogCategory)
    >
{
    HRESULT STDMETHODCALLTYPE get_TypeInfo( 
        IEnumTypeInfo **value) override
    {
        return GetAndQueryInterface(
            this->m_pInner,
            &foundation::ILogCategory::GetTypeInfo,
            value);
    }

    HRESULT STDMETHODCALLTYPE get_Level( 
        LoggingLevel *value) override
    {
        foundation::LoggingLevel level;
        IFR_ASSERT(this->m_pInner->GetLevel(&level));
        *value = (LoggingLevel)level;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE put_Level( 
        LoggingLevel value) override
    {
        return this->m_pInner->SetLevel((foundation::LoggingLevel)value);
    }

    HRESULT STDMETHODCALLTYPE get_Parent(
        ILogCategory **value) override
    {
        foundation::ComPtr<foundation::ILogCategory> spLogCategory;
        IFR_ASSERT(this->m_pInner->GetParent(spLogCategory.GetAddressOf()));
        return foundation::QueryInterfaceIf(spLogCategory, value);
    }

    HRESULT STDMETHODCALLTYPE get_SubCategories(
        UINT32 *length,
        ILogCategory ***values) override
    {
        IFCPTR_ASSERT(length);
        IFCPTR_ASSERT(values);

        foundation::ArrayRefCountWrapper<foundation::ILogCategory> logCategories;
        IFR_ASSERT(this->m_pInner->GetSubCategories(
            logCategories.GetSizeAddressOf(), 
            logCategories.GetBufferAddressOf()));

        std::vector<foundation::ComPtr<ILogCategory>> logCategoriesVector;
        logCategoriesVector.resize(logCategories.GetSize());

        for (UINT32 index = 0; index < logCategories.GetSize(); ++index)
        {
            IFR_ASSERT(foundation::QueryInterface(
                logCategories[index],
                logCategoriesVector[index].GetAddressOf()));
        }
        ContainerRefCountToArray(logCategoriesVector, length, values);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Configure(
        IDictionary *pConfigureParameters)
    {
        IFCPTR_ASSERT(pConfigureParameters);

        foundation::ComPtr<foundation::IDictionary> spConfigureParameters;
        IFR_ASSERT(foundation::QueryInterface(pConfigureParameters, spConfigureParameters.GetAddressOf()));
        return this->m_pInner->Configure(spConfigureParameters);
    }

    HRESULT STDMETHODCALLTYPE AddTypeFilter( 
        GUID iidType) override
    {
        return this->m_pInner->AddTypeFilter(iidType);
    }

    HRESULT STDMETHODCALLTYPE RemoveTypeFilter( 
        GUID iidType) override
    {
        return this->m_pInner->RemoveTypeFilter(iidType);
    }
    HRESULT STDMETHODCALLTYPE SetMessageLevel(
        UINT32 messageEnumValue,
        ABI::Microsoft::PropertyModel::LoggingLevel level)
    {
        return this->m_pInner->SetMessageLevel(messageEnumValue, (foundation::LoggingLevel)level);
    }

    HRESULT STDMETHODCALLTYPE IsLevelEnabled( 
        LoggingLevel level,
        UINT32 messageEnumValue,
        GUID iidType,
        boolean *result) override
    {
       bool _result;
       IFR_ASSERT(this->m_pInner->IsLevelEnabled(
           (foundation::LoggingLevel)level,
           messageEnumValue,
           IsEqualIID(iidType,GUID_NULL) ? nullptr:&iidType,
           &_result));
       *result = _result;
       return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   CMoLogParameterAdapter
//
// Purpose: Implementation of ILogParameter Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoLogParameterAdapter :
    public foundation::AdapterBase
    <
    foundation::ILogParameterPair,
    ILogParameter,
    &__uuidof(ILogParameter)
    >
{
protected:
    HRESULT STDMETHODCALLTYPE get_Property( 
        IEnumValue **value) 
    {
        foundation::ComPtr<foundation::IEnumValue> spEnumValue;
        IFR_ASSERT(this->m_pInner->GetKey(spEnumValue.GetAddressOf()));
        return foundation::QueryInterface(spEnumValue,value);
    }
    HRESULT STDMETHODCALLTYPE get_Value( 
        IInspectable **value)
    {
        return this->m_pInner->GetValue(value);
    }
};
//------------------------------------------------------------------------------
// Class:   CMoLogRecordAdapter
//
// Purpose: Implementation of ILogRecord Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoLogRecordAdapter :
    public foundation::AdapterBase
        <
        foundation::ILogRecord,
        ILogRecord,
        &__uuidof(ILogRecord)
        >
{
protected:
    
    HRESULT STDMETHODCALLTYPE get_Timestamp( 
        ABI::Windows::Foundation::DateTime *value) override
    {
        foundation::SystemTime timestamp;
        _IFR_(this->m_pInner->GetTimestamp(&timestamp));
        _IFR_(_pal_SystemTimeToDateTime(timestamp, value));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_ThreadId( 
        UINT32 *value) override
    {
        return this->m_pInner->GetThreadId(value);
    }

    HRESULT STDMETHODCALLTYPE get_Level( 
        LoggingLevel *value) override
    {
        foundation::LoggingLevel level;
        this->m_pInner->GetLevel(&level);
        *value = (LoggingLevel)level;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_Category( 
        ILogCategory **value) override
    {
        foundation::ComPtr<foundation::ILogCategory> spLogCategory;
        IFR_ASSERT(this->m_pInner->GetCategory(spLogCategory.GetAddressOf()));
        return foundation::QueryInterface(spLogCategory,value);
    }

    HRESULT STDMETHODCALLTYPE get_Message( 
        IEnumValue **value) override
    {
        foundation::ComPtr<foundation::IEnumValue> spEnumValue;
        IFR_ASSERT(this->m_pInner->GetValue(spEnumValue.GetAddressOf()));
        return foundation::QueryInterface(spEnumValue,value);
    }

    HRESULT STDMETHODCALLTYPE get_LogParameters( 
        UINT32 *length,
        ILogParameter ***values) override
    {
        IFCPTR_ASSERT(length);
        IFCPTR_ASSERT(values);

        foundation::ArrayRefCountWrapper<foundation::ILogParameterPair> logParameters;
        IFR_ASSERT(this->m_pInner->GetParameters(logParameters.GetSizeAddressOf(),logParameters.GetBufferAddressOf()));

        std::vector<foundation::ComPtr<ILogParameter>> logParametersVector;
        logParametersVector.resize(logParameters.GetSize());

        for(UINT32 index = 0;index < logParameters.GetSize();++index)
        {
            IFR_ASSERT(foundation::QueryInterface(
                logParameters[index],
                logParametersVector[index].GetAddressOf()));
        }
        ContainerRefCountToArray(logParametersVector,length,values);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ToText(HSTRING *text)
    {
        IFCPTR_ASSERT(text);
        return foundation::logger_util::to_text(this->m_pInner, text);
    }

};

//------------------------------------------------------------------------------
// Class:   CMoLogCreatedEventArgsAdapter
//
// Purpose: Implementation of ILogCreatedEventArgs Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoLogCreatedEventArgsAdapter :
    public foundation::AdapterBase
    <
    foundation::ILogCreatedEventArgs,
    ILogCreatedEventArgs,
    &__uuidof(ILogCreatedEventArgs)
    >
{
public:

    HRESULT STDMETHODCALLTYPE get_LogRecord( 
        ILogRecord **logRecord) override
    {
        return GetAndQueryInterface(
            this->m_pInner,
            &foundation::ILogCreatedEventArgs::GetLogRecord,
            logRecord);
    }

    HRESULT STDMETHODCALLTYPE get_Context(
        IInspectable **ppContext) override
    {
        return this->m_pInner->GetContext(ppContext);
    }

};
