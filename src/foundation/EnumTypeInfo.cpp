/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumTypeInfo.cpp
****/

#include "pch.h"

#include "EnumTypeInfo.h"
#include "EnumTypeClassInfoCache.h"
#include "EnumValue.h"

#include <foundation/errors.h>

using namespace foundation;
using namespace foundation_lib;

LPCSTR_t _EnumTypeInfo_Names_Key = U("names");
LPCSTR_t _EnumTypeInfo_Values_Key = U("values");
LPCSTR_t _EnumTypeInfo_IsFlags_Key = U("isFlags");

HRESULT CEnumTypeInfo::CreateInstance(
        _In_ const EnumTypeInfo *pEnumTypeInfo,
        _Outptr_ CEnumTypeInfo **ppEnumTypeInfo)
{
    foundation_assert(pEnumTypeInfo);

    HRESULT hr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CEnumTypeInfo>::CreateInstance(ppEnumTypeInfo));

    // Now Cache this new created Enum Type Info
    CEnumTypeClassInfoCache::AddToCache(pEnumTypeInfo->get_iid_type(), *ppEnumTypeInfo);

    // now we can initialize
    IFR_ASSERT(hr = (*ppEnumTypeInfo)->_Initialize(pEnumTypeInfo));

    return S_OK;
}

HRESULT CEnumTypeInfo::create_runtime_instance(
    _In_ const EnumTypeInfo *pEnumTypeInfo,
    _In_ UINT32     size,
    _In_ HSTRING    *pNames,
    _In_ UINT32     *pValues,
    _Outptr_ CEnumTypeInfo **ppEnumTypeInfo)
{
    foundation_assert(pEnumTypeInfo);

    IFR_ASSERT(foundation::ctl::ComInspectableObject<CEnumTypeInfo>::CreateInstance(ppEnumTypeInfo));
    IFR_ASSERT((*ppEnumTypeInfo)->_InitializeTypeInfo(pEnumTypeInfo));

    (*ppEnumTypeInfo)->SetDeleteTypeInfo(true);

    // initialize runtime attributes
    (*ppEnumTypeInfo)->_isFlags = pEnumTypeInfo->m_is_flags;

    HSTRING *pArrayNames = nullptr;
    UINT32 *pArrayValues = nullptr;
    IFR_ASSERT((*ppEnumTypeInfo)->InitializeArrays(size, &pArrayNames, &pArrayValues));

    for (UINT32 index = 0; index < size; ++index)
    {
        _pal_DuplicateString(*pNames, &pArrayNames[index]);
        pArrayValues[index] = *pValues;
        // next 
        ++pNames;
        ++pValues;
    }
    return S_OK;
}

HRESULT CEnumTypeInfo::_DeserializeTypeInfo(const DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CEnumTypeInfo_BaseType::_DeserializeTypeInfo(type_info_ptr));

    // names
    type_info_ptr.GetValueAs(_EnumTypeInfo_Names_Key,_namesWrapper);
    // values
    type_info_ptr.GetValueAs(_EnumTypeInfo_Values_Key,_valuesWrapper);
    // isFlags
    type_info_ptr.GetValue(_EnumTypeInfo_IsFlags_Key,_isFlags);

    return S_OK;
}

HRESULT CEnumTypeInfo::_SerializeTypeInfo(DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CEnumTypeInfo_BaseType::_SerializeTypeInfo(type_info_ptr));
    // names
    type_info_ptr.SetValue(_EnumTypeInfo_Names_Key,
                            pv_util::CreateValue(
                                _namesWrapper.GetSize(),
                                _namesWrapper.GetRawBuffer()));
    // values
    type_info_ptr.SetValue(_EnumTypeInfo_Values_Key,
                            pv_util::CreateValue(
                                _valuesWrapper.GetSize(),
                                _valuesWrapper.GetRawBuffer()));
    // isFlags
    type_info_ptr.SetValue(_EnumTypeInfo_IsFlags_Key,_isFlags);
    return S_OK;
}

CEnumTypeInfo::CEnumTypeInfo()
{
    // Initialize our critical section
    m_cs.Init();
}

HRESULT CEnumTypeInfo::InitializeArrays(UINT32 size, HSTRING **ppNames, UINT32 **ppValues)
{
    // allocate buffers/size
    *ppNames = *_namesWrapper.GetBufferAddressOf() = (HSTRING*)_pal_MemAlloc(size * sizeof(HSTRING));
    *_namesWrapper.GetSizeAddressOf() = size;
    *ppValues = *_valuesWrapper.GetBufferAddressOf() = (UINT32*)_pal_MemAlloc(size * sizeof(UINT32));
    *_valuesWrapper.GetSizeAddressOf() = size;
    return S_OK;
}

HRESULT CEnumTypeInfo::_Initialize(
        _In_ const EnumTypeInfo *pEnumTypeInfo)
{
    foundation_assert(pEnumTypeInfo);

    IFR_ASSERT(_InitializeTypeInfo(pEnumTypeInfo));
    
    _isFlags = pEnumTypeInfo->m_is_flags;

    if (pEnumTypeInfo->m_base_type != nullptr)
    {
        CEnumTypeInfo *pBaseEnumTypeInfo = nullptr;
        CEnumTypeClassInfoCache::GetOrCreateEnumTypeInfo(
            (const foundation::EnumTypeInfo *)pEnumTypeInfo->m_base_type,
            &pBaseEnumTypeInfo);
        this->SetBaseType(pBaseEnumTypeInfo);
    }

    // count number of Enums
    UINT32 size = 0;
    while(pEnumTypeInfo->m_names[size])
    {
        ++size;
    }

    HSTRING *pNames = nullptr;
    UINT32 *pValues = nullptr;
    IFR_ASSERT(InitializeArrays(size, &pNames, &pValues));
        
    for(UINT32 i = 0; i < size; ++i)
    {
        _pal_CreateString(pEnumTypeInfo->m_names[i],(UINT32)_pal_strlen(pEnumTypeInfo->m_names[i]),pNames);
        *pValues = pEnumTypeInfo->m_values[i];
        // next 
        ++pNames;
        ++pValues;
    }

    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::Dispose()
{
    // Clear our cached enum values
    for(_EnumValueMapType::iterator iter = _enumValues.begin();
        iter != _enumValues.end();
        ++iter)
    {
        (*iter).second.Release();
    }
    _enumValues.clear();
    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::GetIsFlags(bool *value)
{
    IFCPTR_ASSERT(value);
    *value = _isFlags;
    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::GetNames(UINT32 *__valueSize,HSTRING **values)
{
    foundation_assert(_namesWrapper.GetBuffer());
    _namesWrapper.CopyTo(__valueSize,values);
    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::GetValues(UINT32 *__valueSize,UINT32 **values)
{
    foundation_assert(_valuesWrapper.GetBuffer());
    _valuesWrapper.CopyTo(__valueSize,values);
    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::GetDefaultValue(UINT32 *enumValue)
{
    *enumValue = 0;	// fallback value
    if(!_isFlags && _valuesWrapper.GetSize())
    {
        *enumValue = _valuesWrapper[0];
    }
    return S_OK;
}

HRESULT CEnumTypeInfo::GetFlagsStringValue(UINT32 enumValue, foundation::string_t& enumValues)
{
    // bitwise options
    for (UINT32 index = 0; index < _valuesWrapper.GetSize(); ++index)
    {
        UINT32 flags = _valuesWrapper[index];
        // check the possible 'None' option
        if (flags == 0 && enumValue == 0)
        {
            enumValues = _pal_GetStringRawBuffer(_namesWrapper[index], nullptr);
            return S_OK;
        }

        if ((_valuesWrapper[index] & enumValue) != 0)
        {
            if (enumValues.length())
            {
                enumValues += ',';
            }
            enumValues += _pal_GetStringRawBuffer(_namesWrapper[index], nullptr);
        }
    }
    if (this->GetBaseType() != nullptr)
    {
        GetBaseType()->GetFlagsStringValue(enumValue, enumValues);
    }
    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::GetStringValue(UINT32 enumValue,HSTRING *value)
{
    if(_isFlags)
    {
        foundation::string_t enumValues;
        GetFlagsStringValue(enumValue, enumValues);
        return _pal_CreateString(enumValues.c_str(),(UINT32)enumValues.length(),value);
    }
    else
    {
        UINT32 index = IndexOfValue(enumValue);
        if(index == (UINT32)-1)
        {
            if (this->GetBaseType() != nullptr)
            {
                return GetBaseType()->GetStringValue(enumValue, value);
            }
            // TODO: better error code
            return E_FAIL;
        }
        return _pal_DuplicateString(_namesWrapper[index],value);
    }
    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::IsEnumValid(UINT32 enumValue,bool *value)
{
    bool isEnumValid;
    if(_isFlags)
    {
        UINT32 flags = 0;
        for(UINT32 index = 0; index < _valuesWrapper.GetSize(); ++index)
        {
            flags = flags | _valuesWrapper[index];
        }
        flags = ~flags;
        isEnumValid = (flags & enumValue) == 0;
    }
    else
    {
        isEnumValid = IndexOfValue(enumValue) != (UINT32)-1;
    }
    if (!isEnumValid && this->GetBaseType() != nullptr)
    {
        return this->GetBaseType()->IsEnumValid(enumValue, value);
    }
    *value = isEnumValid;
    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::GetEnumValue(UINT32 enumValue,IEnumValue **ppEnumvalue)
{
    IFCPTR_ASSERT(ppEnumvalue);

    CFoundationLock<CCriticalSection> lock(m_cs);

    _EnumValueMapType::iterator iter = _enumValues.find(enumValue);
    if(iter != _enumValues.end())
    {
        return (*iter).second.CopyTo(ppEnumvalue);
    }
    bool isValid;
    IFR_ASSERT(this->IsEnumValid(enumValue,&isValid));
    if(!isValid)
    {
        if (this->GetBaseType() != nullptr)
        {
            return GetBaseType()->GetEnumValue(enumValue, ppEnumvalue);
        }
        return foundation::E_INVALID_ENUM_VALUE;
    }
    CEnumValue *pEnumValueClass = nullptr;
    IFR_ASSERT(CEnumValue::CreateInstance(this,enumValue,&pEnumValueClass));
    // cache this Enum Value
    _enumValues.insert(std::make_pair(enumValue,pEnumValueClass));

    *ppEnumvalue = static_cast<IEnumValue *>(pEnumValueClass);
    return S_OK;
}

STDMETHODIMP CEnumTypeInfo::GetEnumValueByName(HSTRING enumValueName,IEnumValue **ppEnumvalue)
{
    UINT32 index = IndexOfName(enumValueName);
    if(index == (UINT32)-1)
    {
        if (this->GetBaseType() != nullptr)
        {
            return GetBaseType()->GetEnumValueByName(enumValueName, ppEnumvalue);
        }
        return foundation::E_INVALID_ENUM_NAME;
    }
    return GetEnumValue(_valuesWrapper[index],ppEnumvalue);
}

UINT32 CEnumTypeInfo::IndexOfName(HSTRING name)
{
    for(UINT32 index = 0; index < _namesWrapper.GetSize(); ++index)
    {
        if((_pal_strcmp(_pal_GetStringRawBuffer(_namesWrapper[index],nullptr),
            _pal_GetStringRawBuffer(name,nullptr))==0))
        {
            return index;
        }
    }
    return (UINT32)-1;
}

UINT32 CEnumTypeInfo::IndexOfValue(UINT32 value)
{
    for(UINT32 index = 0; index < _valuesWrapper.GetSize(); ++index)
    {
        if(_valuesWrapper[index] == value)
        {
            return index;
        }
    }
    return (UINT32)-1;
}
