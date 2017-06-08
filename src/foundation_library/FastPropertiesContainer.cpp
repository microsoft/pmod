/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FastPropertiesContainer.cpp
****/

#include "pch.h"

#include "FastPropertiesContainer.h"
#include "FastContainerUtil.h"

#include <foundation/type_info_statics_util.h>
#include <foundation/errors.h>
#include <foundation/library/logger_macros.h>
#include <foundation/library/critical_section_lock.h>

using namespace foundation;

const UINT32 Properties_Offset_Start = 0x1000;

// {46391020-A93C-4272-9F7A-E6BEEE1136CB}
const GUID foundation::library::IID_IPropertiesContainer =
{ 0x46391020, 0xa93c, 0x4272,{ 0x9f, 0x7a, 0xe6, 0xbe, 0xee, 0x11, 0x36, 0xcb } };

// {6D84142A-978E-4159-A3D7-EC0979203455}
const GUID foundation::library::IID_IFastPropertiesContainer =
{ 0x6d84142a, 0x978e, 0x4159,{ 0xa3, 0xd7, 0xec, 0x9, 0x79, 0x20, 0x34, 0x55 } };

// {96429B74-10CE-4EB4-BEDE-E8B1DA86080A}
const GUID foundation::library::IID_IFastPropertiesContainerDelegate =
{ 0x96429b74, 0x10ce, 0x4eb4,{ 0xbe, 0xde, 0xe8, 0xb1, 0xda, 0x86, 0x8, 0xa } };


// Private export
EXTERN_C FOUNDATION_LIB_API HRESULT  STDAPICALLTYPE CreateFastPropertiesContainer(
    const library::FastPropertiesContainerParameters *pParameters,
    library::IFastPropertiesContainer **ppPropertiesContainer)
{
    return CFastPropertiesContainer::CreateInstance(
        pParameters,
        ppPropertiesContainer);
}


CFastPropertiesContainer::CFastPropertiesDataInfoCache CFastPropertiesContainer::_propertySlotCacheMap;

HRESULT CFastPropertiesContainer::CreateInstance(
    const library::FastPropertiesContainerParameters *pParameters,
    library::IFastPropertiesContainer **ppPropertiesContainer)
{
    CFastPropertiesContainer *pFastPropertiesContainer = nullptr;
    IFR_ASSERT(foundation::ctl::inspectable::CreateInstanceWithInitialize(&pFastPropertiesContainer, pParameters));

    *ppPropertiesContainer = static_cast<library::IFastPropertiesContainer *>(pFastPropertiesContainer);
    return S_OK;
}

HRESULT CFastPropertiesContainer::ClearPropertySlot(_PropertySlotCacheInfo & propertySlotItem)
{
    return CFastContainerUtil::ClearPropertySlot(propertySlotItem._propertyType, GetSlotPtr(propertySlotItem));
}

HRESULT CFastPropertiesContainer::CFastPropertiesDataInfoCache::GetProperties(
    const foundation::ArrayRefCountWrapper<foundation::IPropertyInfo>& allPropertiesInfo,
    std::vector<foundation::ComPtr<foundation::IPropertyInfo>>& propertInfos)
{
    bool isViewModel = false;
    for (UINT32 index = 0; index < allPropertiesInfo.GetSize(); ++index)
    {
        UINT32 propertyId;
        allPropertiesInfo[index]->GetId(&propertyId);
        // skip reserved properties
        if (propertyId >= Property_ResevervedMask)
        {
            continue;
        }
        if (propertyId >= Properties_Offset_Start)
        {
            isViewModel = true;
        }
        else if (isViewModel && propertyId < Properties_Offset_Start)
        {
            // stop here since Model based properties need to be skiped
            break;
        }
        propertInfos.push_back(allPropertiesInfo[index]);
    }
    return S_OK;
}
HRESULT CFastPropertiesContainer::CFastPropertiesDataInfoCache::AddPropertiesToCache(
    const std::vector<foundation::ComPtr<foundation::IPropertyInfo>>& propertInfos,
    std::shared_ptr<_PropertiesContainerType>& cache)
{
    size_t allocSize = 0;
    _IFR_(_CFastPropertiesDataInfoCache_BaseType::AddPropertiesToCache(propertInfos, cache));

    // fill Allocate info
    for (auto iter = cache->begin();
        iter != cache->end();
        ++iter)
    {
        iter->_offset = allocSize;
        allocSize += CFastContainerUtil::GetSlotSize(iter->_propertyType);
    }
    return S_OK;
}

LPVOID CFastPropertiesContainer::GetSlotPtr(const _PropertySlotCacheInfo & propertyCacheInfo)
{
    size_t offset = propertyCacheInfo._offset + (propertyCacheInfo._index + 1) * this->_propertySlotPrefixSize;
    return static_cast<UINT8 *>(_propertiesSlotsAlloc) + offset;
}

LPVOID CFastPropertiesContainer::GetCustomSlotPtr(const _PropertySlotCacheInfo & propertyCacheInfo)
{
    size_t offset = propertyCacheInfo._offset + propertyCacheInfo._index * this->_propertySlotPrefixSize;
    return static_cast<UINT8 *>(_propertiesSlotsAlloc) + offset;
}

HRESULT CFastPropertiesContainer::_Initialize(const library::FastPropertiesContainerParameters* pParameters)
{
   _isDelegateWeakRef = IsOptionEnabled(pParameters->m_options, FastPropertiesContainerOptions::IsDelegateWeakRef);

    _pDelegate = pParameters->m_p_delegate;
    if (!_isDelegateWeakRef && _pDelegate != nullptr)
    {
        _pDelegate->AddRef();
    }
    // Note:
    // we only support IObservableObjectInfo 'normalized' type infos
    this->_propertySlotPrefixSize = pParameters->_slotPrefixSize;

    IFR_ASSERT(_propertySlotCacheMap.GetCache(pParameters->_pPropertyDataInfo, _propertiesSlotCache));

    size_t allocSize = 0;
    if (_propertiesSlotCache->size())
    {
        const _PropertySlotCacheInfo& last = _propertiesSlotCache->back();
        allocSize =
            last._offset +
            CFastContainerUtil::GetSlotSize(last._propertyType) +
            _propertiesSlotCache->size() * this->_propertySlotPrefixSize;
    }
    // allocate our slots
    _propertiesSlotsAlloc = _pal_MemAlloc(allocSize);
    IFCPTR_ASSERT(_propertiesSlotsAlloc);

    // zero allocation
    memset(_propertiesSlotsAlloc, 0, allocSize);

    // initialize our enum type slots
    for (CFastPropertiesDataInfoCache::_PropertiesContainerType::const_iterator iter = _propertiesSlotCache->begin();
        iter != _propertiesSlotCache->end();
        ++iter)
    {
        if (iter->_flags & PropertyFlagType::IsEnum)
        {
            foundation::ComPtr<foundation::IEnumTypeInfo> spEnumTypeInfo;
            IFR_ASSERT(foundation::GetTypeInfoAs(
                iter->_iidType,
                spEnumTypeInfo.GetAddressOf()));
            UINT32 enumValue;
            spEnumTypeInfo->GetDefaultValue(&enumValue);
            if (enumValue != 0)
            {
                *reinterpret_cast<UINT32 *>(GetSlotPtr(*iter)) = enumValue;
            }
        }
    }
    return S_OK;
}

void CFastPropertiesContainer::FinalRelease()
{
    DisposeInternal();
    if (_pDelegate != nullptr && !_isDelegateWeakRef)
    {
        RELEASE_INTERFACE(_pDelegate);
    }
}

HRESULT CFastPropertiesContainer::DisposeInternal()
{
    for (CFastPropertiesDataInfoCache::_PropertiesContainerType::iterator iter = _propertiesSlotCache->begin();
        iter != _propertiesSlotCache->end();
        ++iter)
    {
        ClearPropertySlot(*iter);
    }
    if (_propertiesSlotsAlloc)
    {
        _pal_MemFree(_propertiesSlotsAlloc);
        _propertiesSlotsAlloc = nullptr;
    }
    return S_OK;
}

STDMETHODIMP CFastPropertiesContainer::HasProperty(UINT32 propertyId, _Out_ bool *found)
{
    IFCPTR_ASSERT(found);

    *found = CheckBounds(ToPropertyIdIndex(propertyId));
    return S_OK;
}

STDMETHODIMP CFastPropertiesContainer::GetValue(UINT32 propertyId, foundation::IInspectable **ppValue)
{
    IFCPTR_ASSERT(ppValue);

    *ppValue = nullptr;
    size_t propertyIdIndex = ToPropertyIdIndex(propertyId);
    if (!CheckBounds(propertyIdIndex))
    {
        return E_BOUNDS_;
    }
    _PropertySlotCacheInfo& propertySlotItem = _propertiesSlotCache->at(propertyIdIndex);
    if (propertySlotItem._propertyId != propertyId)
    {
        return (HRESULT)E_PROPERTY_NOT_FOUND;
    }

    if (_pDelegate != nullptr)
    {
        HRESULT hr = _pDelegate->OnGetSlotProperty(propertyId, ppValue);
        IFHR(hr);
        if (hr == S_OK)
        {
            return S_OK;
        }
    }

    LPVOID pPropertySlot = GetSlotPtr(propertySlotItem);

    if (propertySlotItem._flags & PropertyFlagType::IsEnum)
    {
        UINT32 enumValue = CFastContainerUtil::ReadSlotValue<UINT32>(pPropertySlot);
        foundation::ComPtr<foundation::IEnumTypeInfo> spEnumTypeInfo;
        IFR_ASSERT(foundation::GetTypeInfoAs(
            propertySlotItem._iidType,
            spEnumTypeInfo.GetAddressOf()));

        return spEnumTypeInfo->GetEnumValue(enumValue, reinterpret_cast<IEnumValue **>(ppValue));
    }
    return CFastContainerUtil::GetPropertyValue(propertySlotItem._propertyType, pPropertySlot, ppValue);
}

STDMETHODIMP CFastPropertiesContainer::SetValue(UINT32 propertyId, _In_opt_ foundation::IInspectable *value)
{
    size_t propertyIdIndex = ToPropertyIdIndex(propertyId);
    if (!CheckBounds(propertyIdIndex))
    {
        return E_BOUNDS_;
    }
    _PropertySlotCacheInfo& propertySlotItem = _propertiesSlotCache->at(propertyIdIndex);
    if (propertySlotItem._propertyId != propertyId)
    {
        return (HRESULT)E_PROPERTY_NOT_FOUND;
    }
    ClearPropertySlot(propertySlotItem);
    if (_pDelegate != nullptr)
    {
        HRESULT hr = _pDelegate->OnSetSlotProperty(propertyId, value);
        IFHR(hr);
        if (hr == S_OK)
        {
            return S_OK;
        }
    }
    LPVOID pPropertySlot = GetSlotPtr(propertySlotItem);
    return CFastContainerUtil::SetPropertyValue(propertySlotItem._propertyType, pPropertySlot, value);
}

STDMETHODIMP CFastPropertiesContainer::RemoveProperty(UINT32 propertyId)
{
    UNREFERENCED_PARAMETER(propertyId);
    foundation_assert(false);
    return E_NOTIMPL;
}

STDMETHODIMP CFastPropertiesContainer::GetSize(_Out_ UINT32* size)
{
    *size = (UINT32)_propertiesSlotCache->size();
    return S_OK;
}

STDMETHODIMP CFastPropertiesContainer::ReadCustomData(UINT32 propertyId, _Inout_ LPVOID pCustomData)
{
    LPVOID pCustomSlotPtr = nullptr;
    _IFR_(GetCustomSlotPtr(propertyId, &pCustomSlotPtr));

    memcpy(pCustomData, pCustomSlotPtr, _propertySlotPrefixSize);
    return S_OK;
}

STDMETHODIMP CFastPropertiesContainer::WriteCustomData(UINT32 propertyId, _Inout_ LPVOID pCustomData)
{
    LPVOID pCustomSlotPtr = nullptr;
    _IFR_(GetCustomSlotPtr(propertyId, &pCustomSlotPtr));

    memcpy(pCustomSlotPtr,pCustomData, _propertySlotPrefixSize);
    return S_OK;
}

HRESULT CFastPropertiesContainer::GetCustomSlotPtr(UINT32 propertyId, LPVOID* ppCustomSlotPtr)
{
    size_t propertyIdIndex = ToPropertyIdIndex(propertyId);
    if (!CheckBounds(propertyIdIndex))
    {
        return E_BOUNDS_;
    }
    _PropertySlotCacheInfo&  slotCacheInfo = _propertiesSlotCache->at(propertyIdIndex);
    if (slotCacheInfo._propertyId != propertyId)
    {
        return (HRESULT)E_PROPERTY_NOT_FOUND;
    }
    *ppCustomSlotPtr = GetCustomSlotPtr(slotCacheInfo);
    return S_OK;
}

STDMETHODIMP CFastPropertiesContainer::GetPropertType(UINT32 propertyId, _Out_ foundation::PropertyType* pPropertyType)
{
    IFCPTR_ASSERT(pPropertyType);

    size_t propertyIdIndex = ToPropertyIdIndex(propertyId);
    if (!CheckBounds(propertyIdIndex))
    {
        return E_BOUNDS_;
    }
    _PropertySlotCacheInfo&  slotCacheInfo = _propertiesSlotCache->at(propertyIdIndex);
    if (slotCacheInfo._propertyId != propertyId)
    {
        return (HRESULT)E_PROPERTY_NOT_FOUND;
    }
    *pPropertyType = slotCacheInfo._propertyType;
    return S_OK;

}
