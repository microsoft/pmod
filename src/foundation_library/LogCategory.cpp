/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LogCategory.cpp
****/

#include "pch.h"
#include "LogCategory.h"
#include "Logger.h"
#include "../foundation/EnumTypeInfo.h"

#include <foundation/dictionary_ptr.h>
#include <foundation/errors.h>
#include <foundation/type_info_statics_util.h>

#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;

const foundation::string_t CLogCategory::_CategoryPostfix(U("_Category"));

library::_ComPtrSingleton<ILogCategory> CLogCategory::_rootInstance;
static foundation_lib::CAutoCriticalSection  _logCategory_cs;

static HRESULT GetLoggingLevelFromString(
    foundation::IInspectable *pValue,
    LoggingLevel& loggingLevel)
{
    foundation::HStringPtr h_level_name;
    IFR_ASSERT(pv_util::GetStringValue(pValue, h_level_name.GetAddressOf()));

    ComPtr<IEnumTypeInfo> spEnumTypeInfo;
    IFR_ASSERT(GetEnumTypeInfo(LoggingLevelEnum::TypeId, spEnumTypeInfo.GetAddressOf()));

    foundation::ComPtr<IEnumValue> spEnumValue;
    IFR_ASSERT(spEnumTypeInfo->GetEnumValueByName(h_level_name, spEnumValue.GetAddressOf()));
    UINT32 level_value;
    IFR_ASSERT(spEnumValue->GetValue(&level_value));
    loggingLevel = (LoggingLevel)level_value;
    return S_OK;
}

bool CLogCategory::IsEnabled()
{
    return CLogger::IsAvailable();
}

CLogCategory *CLogCategory::GetRootCategory()
{
    return static_cast<CLogCategory *>(_rootInstance._GetInstance([]() {
        CLogCategory *pLogCategory = nullptr;
        IF_ASSERT(CreateInstanceInternal(nullptr, nullptr, 0, &pLogCategory));
        return pLogCategory;
    }));
}

HRESULT CLogCategory::CreateInstance(
    CLogCategory *pParent,
    UINT32      categoryTypeId,
    CLogCategory **ppLogCategory)
{
    if(pParent == nullptr)
    {
        pParent = GetRootCategory();
    }
    ComPtr<IEnumTypeInfo> spEnumTypeInfo;
    _IFR_(GetEnumTypeInfo(categoryTypeId, spEnumTypeInfo.GetAddressOf()));

    IF_ASSERT(CreateInstanceInternal(pParent,static_cast<CEnumTypeInfo *>(spEnumTypeInfo.Get()),categoryTypeId,ppLogCategory));
    // Note: we always expect this call to return RefCnt == 2 so
    // we will release once the created Log Category
    foundation::ctl::inspectable::CastToUnknown(*ppLogCategory)->Release();

    return S_OK;
}

HRESULT CLogCategory::CreateInstanceInternal(
    CLogCategory *pParent,
    CEnumTypeInfo *pEnumTypeInfo,
    UINT32      categoryTypeId,
    CLogCategory **ppLogCategory)
{
    foundation_assert(ppLogCategory);

    CLogCategory *pLogCategory;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CLogCategory>::CreateInstance(&pLogCategory));

    pLogCategory->_level = LoggingLevel::Off;
    pLogCategory->_pParent = pParent;
    pLogCategory->_pEnumTypeInfo = pEnumTypeInfo;
    *ppLogCategory = pLogCategory;
    if(pParent)
    {
        // next line will make RefCnt == 2
        pParent->AddSubCategory(categoryTypeId,pLogCategory);
    }
    return S_OK;
}

HRESULT CLogCategory::GetOrCreateCategory(
    _In_opt_ CLogCategory *pRootCategory,
    UINT32 size,
    _In_ UINT32 *pCategoryTypeId,
    CLogCategory **ppLogCategory)
{
    foundation_assert(pCategoryTypeId);
    foundation_assert(ppLogCategory);

    if(pRootCategory == nullptr)
    {
        pRootCategory = GetRootCategory();
    }

    foundation_lib::CFoundationLock<foundation_lib::CAutoCriticalSection> csl(_logCategory_cs);

    CLogCategory *pNextCategory = pRootCategory;

    for(UINT32 index = 0;index < size; ++index,++pCategoryTypeId )
    {
        UINT32 categoryTypeId = *pCategoryTypeId;
        CLogCategory *pCategory = pNextCategory->LookupCategory(categoryTypeId);
        if(pCategory == nullptr)
        {
            IFR_ASSERT(CreateInstance(pNextCategory,categoryTypeId,&pCategory));
        }
        pNextCategory = pCategory;
    }
    *ppLogCategory = pNextCategory;
    return S_OK;
}

CLogCategory::CLogCategory():
    _pParent(nullptr),
    _pEnumTypeInfo(nullptr)
{}
CLogCategory::~CLogCategory()
{}

void CLogCategory::AddSubCategory(UINT32 categoryTypeId,CLogCategory *subCategory)
{
    _subCategories.insert(std::make_pair(categoryTypeId,subCategory));
}

CLogCategory *CLogCategory::LookupCategory(UINT32 categoryTypeId)
{
    _CategoryMapType::const_iterator iter = _subCategories.find(categoryTypeId);
    if(iter != _subCategories.end())
    {
        return ToLogCategoryClass((*iter).second);
    }
    return nullptr;
}

bool CLogCategory::IsCategoryNameEquals(LPCSTR_t categoryName)
{
    HStringPtr hFullName;
    _pEnumTypeInfo->GetName(hFullName.GetAddressOf());
    if (_pal_strcmp(hFullName.GetRawBuffer(), categoryName) == 0)
    {
        return true;
    }
    // attempt without the namespace and also with '_Category' postfix
    string_t namespace_, name;
    _pEnumTypeInfo->_GetNamespaceAndName(namespace_, name);
    if (_pal_strcmp(name.c_str(), categoryName) == 0 ||
       (_pal_strcmp(name.c_str(), (categoryName + _CategoryPostfix).c_str()) == 0))
    {
        return true;
    }
    return false;
}

CLogCategory *CLogCategory::LookupCategoryByName(LPCSTR_t categoryName)
{
    for (_CategoryMapType::const_iterator iter = _subCategories.begin();
        iter != _subCategories.end();
        ++iter
        )
    {
        CLogCategory *pSubLogCategory = ToLogCategoryClass((*iter).second);
        if (pSubLogCategory->IsCategoryNameEquals(categoryName))
        {
            return pSubLogCategory;
        }
    }
    return nullptr;
}

bool CLogCategory::IsLevelEnabled(
    foundation::LoggingLevel level,
    UINT32 messageEnumValue,
    GUID *pForType) const
{
    struct _ForType : _GetTypeFunc
    {
        _ForType(GUID *pForType) :
            _pForType(pForType)
        {
        }

        bool operator()(IID* pType) const
        {
            if (_pForType != nullptr)
            {
                *pType = *_pForType;
                return true;
            }
            return false;
        }
        GUID *_pForType;
    };
    return IsLevelEnabled(level, messageEnumValue, _ForType(pForType));
}

STDMETHODIMP CLogCategory::GetTypeInfo(IEnumTypeInfo **ppValue)
{
    IFCPTR_ASSERT(ppValue);
    *ppValue = _pEnumTypeInfo;
    if(_pEnumTypeInfo)
    {
        (*ppValue)->AddRef();
    }
    return S_OK;

}
STDMETHODIMP CLogCategory::GetParent(ILogCategory **ppLogCategory)
{
    IFCPTR_ASSERT(ppLogCategory);
    if(_pParent)
    {
        *ppLogCategory = _pParent;
        (*ppLogCategory)->AddRef();
    }
    else
    {
        *ppLogCategory = nullptr;
    }
    return S_OK;
}

STDMETHODIMP CLogCategory::GetSubCategories(UINT32 *size, _Outptr_ ILogCategory ***ppLogCategories)
{
    std::vector<ILogCategory *> logCategories;
    for (_CategoryMapType::const_iterator iter = _subCategories.begin();
        iter != _subCategories.end();
        ++iter)
    {
        logCategories.push_back((*iter).second);
    }
    ContainerRefCountToArray(logCategories, size, ppLogCategories);
    return S_OK;
}

STDMETHODIMP CLogCategory::GetLevel(LoggingLevel* level)
{
    IFCPTR_ASSERT(level);
    * level = _level;
    return S_OK;
}

STDMETHODIMP CLogCategory::SetLevel(LoggingLevel level)
{
    _level = level;
    return S_OK;
}

STDMETHODIMP CLogCategory::AddMessageFilter(UINT32 messageEnumValue)
{
    _filterMessages.insert(messageEnumValue);
    return S_OK;
}

STDMETHODIMP CLogCategory::RemoveMessageFilter(UINT32 messageEnumValue)
{
    std::set<UINT32>::iterator iter = _filterMessages.find(messageEnumValue);
    if(iter != _filterMessages.end())
    {
        _filterMessages.erase(iter);
    }
    return S_OK;
}

STDMETHODIMP CLogCategory::AddTypeFilter(GUID iidType)
{
    _filterTypes.insert(iidType);
    return S_OK;
}

STDMETHODIMP CLogCategory::RemoveTypeFilter(GUID iidType)
{
    std::set<GUID, foundation::library::GuidLessThan>::iterator iter = _filterTypes.find(iidType);
    if(iter != _filterTypes.end())
    {
        _filterTypes.erase(iter);
    }
    return S_OK;
}

STDMETHODIMP CLogCategory::SetMessageLevel(UINT32 messageEnumValue, LoggingLevel level)
{
    if ((INT32)level == -1)
    {
        _MessageLevelMapType::iterator iter = _messageLevels.find(messageEnumValue);
        if (iter != _messageLevels.end())
        {
            _messageLevels.erase(iter);
        }
    }
    else
    {
        _messageLevels[messageEnumValue] = level;
    }
    return S_OK;
}

STDMETHODIMP CLogCategory::IsLevelEnabled(
    LoggingLevel level,
    UINT32 messageEnumValue,
    GUID *piidType,
    bool *isEnabled)
{
    IFCPTR_ASSERT(isEnabled);
    *isEnabled = this->IsLevelEnabled(level,messageEnumValue,piidType);
    return S_OK;
}

HRESULT CLogCategory::GetMessageEnumValue(HSTRING messageId, IEnumValue **ppEnumValue)
{
    // next block will allow passing the message directly from Category
    string_t messageId_str(_pal_GetStringRawBuffer(messageId,nullptr));
    if (messageId_str.find_first_of(U('.')) == string_t::npos)
    {
        HStringPtr hName;
        _pEnumTypeInfo->GetName(hName.GetAddressOf());
        messageId_str = hName.GetRawBuffer();
        messageId_str += U('.');
        messageId_str += _pal_GetStringRawBuffer(messageId, nullptr);
    }
    return GetEnumValueByName(HStringRef(messageId_str.c_str()), ppEnumValue);
}

STDMETHODIMP CLogCategory::Configure(foundation::IDictionary *pConfigParameters)
{
    DictionaryPtr config_parameters_ptr(pConfigParameters);
    for (DictionaryPtr::keys_iterator iter = config_parameters_ptr.keys_begin();
        iter != config_parameters_ptr.keys_end();
        ++iter)
    {
        LPCSTR_t key = _pal_GetStringRawBuffer(*iter, nullptr);
        foundation::InspectablePtr valuePtr;

        IFR_ASSERT(config_parameters_ptr.GetValue(key, valuePtr.GetAddressOf()));
        if (_pal_strcmp(key, U("level")) == 0)
        {
            LoggingLevel logging_level;
            _IFR_(GetLoggingLevelFromString(valuePtr, logging_level));
            this->SetLevel(logging_level);
        }
        else if (_pal_strcmp(key, U("typeFilters")) == 0)
        {
            InspectableArrayWrapper types;
            _IFR_(pv_util::GetValue(
                valuePtr,
                types.GetSizeAddressOf(),
                types.GetBufferAddressOf()));
            for (UINT32 index = 0; index < types.GetSize(); ++index)
            {
                valuePtr = types[index];
                _IFR_(pv_util::ChangeType(PropertyType_Guid, valuePtr.GetAddressOfPtr()));
                IID iidType;
                pv_util::GetValue(valuePtr,&iidType);
                this->AddTypeFilter(iidType);
            }
        }
        else if (_pal_strcmp(key, U("messageFilters")) == 0)
        {
            InspectableArrayWrapper messages;
            _IFR_(pv_util::GetValue(
                valuePtr,
                messages.GetSizeAddressOf(),
                messages.GetBufferAddressOf()));
            for (UINT32 index = 0; index < messages.GetSize(); ++index)
            {
                HStringPtr messageId;
                _IFR_(pv_util::GetValue(messages[index], messageId.GetAddressOfPtr()));

                foundation::ComPtr<IEnumValue> spEnumValue;
                _IFR_(GetMessageEnumValue(messageId, spEnumValue.GetAddressOf()));
                UINT32 enumMessageValueId;
                spEnumValue->GetUniqueId(&enumMessageValueId);
                this->AddMessageFilter((UINT32)enumMessageValueId);
            }
        }
        else if (_pal_strcmp(key, U("messageLevels")) == 0)
        {
            InspectableArrayWrapper messageLevels;
            _IFR_(pv_util::GetValue(
                valuePtr,
                messageLevels.GetSizeAddressOf(),
                messageLevels.GetBufferAddressOf()));
            for (UINT32 index = 0; index < messageLevels.GetSize(); ++index)
            {
                DictionaryPtr message_level_ptr;
                _IFR_(foundation::QueryInterface(messageLevels[index], message_level_ptr.GetAddressOf()));
                HStringPtr messageId;
                _IFR_(message_level_ptr.GetValue(U("messageId"), messageId));
                _IFR_(message_level_ptr.GetValue(U("level"), valuePtr.ReleaseAndGetAddressOf()));

                foundation::ComPtr<IEnumValue> spEnumValue;
                _IFR_(GetMessageEnumValue(messageId, spEnumValue.GetAddressOf()));

                LoggingLevel logging_level_value;
                _IFR_(GetLoggingLevelFromString(valuePtr, logging_level_value));

                UINT32 enumMessageValueId;
                spEnumValue->GetUniqueId(&enumMessageValueId);
                this->SetMessageLevel((UINT32)enumMessageValueId, logging_level_value);
            }
        }
    }
    return S_OK;
}
