/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatch.cpp
****/
#include "pch.h"

#include "ObjectDispatch.h"
#include "CreateFoundationHelper.h"

#include "../foundation/ObjectDispatchInfoBase.h"
#include "FastPropertiesContainer.h"
#include "CreateFoundationHelper.h"

#include <foundation/type_info_statics_util.h>
#include <foundation/library/object_dispatch_class.h>
#include <foundation/library/dictionary_class.h>
#include <foundation/library/object_dispatch_impl.h>
#include <foundation/library/logger_macros.h>
#include <foundation/errors.h>

using namespace foundation;

HRESULT RegisterFactoryPropertyDataClassAdapters(
    foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    foundation_assert(pGenericAdapterFactory != nullptr);
    pGenericAdapterFactory->AddAdapterEntry(
        IID_IObjectDispatch,
        foundation::IID_IDictionary,
        IID_IObjectDispatch,
        foundation::CreateAdapter<CPropertyDataDictionaryAdapter>);
    return S_OK;
}

CObjectDispatch::CObjectDispatch()
{
}

HRESULT CObjectDispatch::CreateInstance(
    _In_ const foundation::library::ObjectDispatchCreateParameters *pParameters,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CObjectDispatch>(pParameters, pOuter, ppNewInstance);
}

HRESULT CObjectDispatch::_Initialize(const foundation::library::ObjectDispatchCreateParameters *pParameters)
{
    foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
    IFR_ASSERT(foundation::GetTypeInfo(
        pParameters->m_iid_type,
        spTypeInfo.GetAddressOf()));
    // Type safe to our expected type
    IFR_ASSERT(foundation::QueryInterface(spTypeInfo, m_spTypeInfo.GetAddressOf()));

    library::FastPropertiesContainerParameters parameters;
    parameters._pPropertyDataInfo = m_spTypeInfo->As<IObjectDispatchInfo>();
    IFR_ASSERT(CFastPropertiesContainer::CreateInstance(
        &parameters,
        reinterpret_cast<library::IFastPropertiesContainer **>(m_spProperties.GetAddressOf())));

    return S_OK;
}

STDMETHODIMP CObjectDispatch::GetTypeInfo(foundation::IObjectTypeInfo** ppValue)
{
    return foundation::QueryInterface(m_spTypeInfo, ppValue);
}

STDMETHODIMP CObjectDispatch::GetProperty(UINT32 propertyId, foundation::IInspectable** ppValue)
{    
    if (ppValue == nullptr)
    {
        // Protect property model implementors from having to check the pointer.
        return E_INVALIDARG;
    }

    return m_spProperties->GetValue(propertyId, ppValue);
}

STDMETHODIMP CObjectDispatch::SetProperty(UINT32 propertyId, _In_opt_ foundation::IInspectable *value)
{
    return m_spProperties->SetValue(propertyId, value);
}

HRESULT CObjectDispatch::_GetPropertyId(
    _In_ _CObjectDispatchInfoBase *pPropertyDataInfoBase,
    HSTRING propertyName,
    _Out_ UINT32 *propertyId)
{
    foundation::ComPtr<IPropertyInfo> spPropertyInfo;
    HRESULT hr = pPropertyDataInfoBase->GetPropertyByNameInternal(
        _pal_GetStringRawBuffer(propertyName,nullptr),
        spPropertyInfo.GetAddressOf());
    if(hr == S_OK)
    {
        IFR_ASSERT(spPropertyInfo->GetId(propertyId));
        return S_OK;
    }
    return foundation::Errors::E_PROPERTY_NOT_FOUND;
}

STDMETHODIMP CObjectDispatch::GetProperties(foundation::IInspectable *pProperties, UINT32* size, IPropertyIdPair ***ppProperties)
{
    return library::_GetProperties(
        this->m_spTypeInfo->As<IObjectDispatchInfo>(),
        this,
        nullptr,
        nullptr,
        pProperties, 
        size, 
        ppProperties);
}

HRESULT CObjectDispatch::_GetView(
    _In_ _CObjectDispatchInfoBase *pPropertyDataInfoBase,
    IObjectDispatch *pPropertyData,
    UINT32* size, foundation::IDictionaryPair ***ppProperties)
{
    foundation::ArrayRefCountWrapper<IPropertyIdPair> propertiesById;
    IFR(pPropertyData->GetProperties(
            foundation::pv_util::CreateValue(static_cast<UINT32>(InstanceFlags::All)),
            propertiesById.GetSizeAddressOf(),
            propertiesById.GetBufferAddressOf()));

    // returned properties
    std::vector<foundation::ComPtr<foundation::IDictionaryPair> > properties;

    for(UINT32 index = 0;index < propertiesById.GetSize(); ++index)
    {
        foundation::InspectablePtr spValue;
        UINT32 propertyId;

        IFR_ASSERT(propertiesById[index]->GetKey(&propertyId));
        IFR_ASSERT(propertiesById[index]->GetValue(spValue.GetAddressOf()));

        foundation::ComPtr<IPropertyInfo> spPropertyInfo;
        IFR(pPropertyDataInfoBase->GetPropertyByIdInternal(propertyId,spPropertyInfo.GetAddressOf()));

        foundation::HStringPtr propertyName;
        IFR_ASSERT(spPropertyInfo->GetName(propertyName.GetAddressOf()));

        foundation::ComPtr<foundation::IDictionaryPair> spPropertyNamePair;
        IFR_ASSERT(foundation::library::CreateDictionaryPair(propertyName, spValue, spPropertyNamePair.GetAddressOf()));

        properties.push_back(spPropertyNamePair);
    }
    // copy now to public API
    ContainerRefCountToArray(properties,size,ppProperties);
    return S_OK;
}

HRESULT CObjectDispatch::_GetView(
    UINT32* size, foundation::IDictionaryPair ***ppProperties)
{
    return _GetView(
        this->m_spTypeInfo,
        this,
        size,
        ppProperties);
}

HRESULT CObjectDispatch::_GetSize(_Out_ UINT32* size)
{
    m_spProperties->GetSize(size);
    return S_OK;
}

HRESULT CObjectDispatch::_Lookup(HSTRING propertyName, foundation::IInspectable** value)
{
    IFCPTR(value);

    UINT32 propertyId;
    IFR_PARAM(_GetPropertyId(m_spTypeInfo,propertyName, &propertyId), propertyName);

    return this->GetProperty(propertyId,value);
}

HRESULT CObjectDispatch::_HasKey(HSTRING propertyName, bool* found)
{
    IFCPTR(found);
    *found = false;

    UINT32 propertyId;
    HRESULT hr = _GetPropertyId(m_spTypeInfo,propertyName, &propertyId);
    *found = hr != Errors::E_PROPERTY_NOT_FOUND;
     return S_OK;
}

HRESULT CObjectDispatch::_Insert(HSTRING propertyName, foundation::IInspectable *value,bool* replaced)
{
    IFCPTR(replaced);
    *replaced = false;

    UINT32 propertyId;
    _IFR_(_GetPropertyId(m_spTypeInfo, propertyName, &propertyId));
    return this->SetProperty(propertyId,value);
}

HRESULT CObjectDispatch::_Remove(HSTRING propertyName)
{
    return E_NOTIMPL;
}

HRESULT CObjectDispatch::_Clear()
{
    return E_NOTIMPL;
}

HRESULT CObjectDispatch::_Keys(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first)
{
    foundation_assert(false);
    return E_NOTIMPL;
}

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

void CObjectDispatch::DumpObject()
{
    foundation::HStringPtr typeName;
    m_spTypeInfo->As<IObjectDispatchInfo>()->GetName(typeName.GetAddressOf());

    UINT32 uniqueId;
    GetUniqueId(&uniqueId);

    _traceMessage(U("Type:%s UniqueId:%d this:0x%04x RefCount:%d"), 
        typeName.GetRawBuffer(), 
        uniqueId,
        this, 
        this->GetRefCount());
}

#endif

