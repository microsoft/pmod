/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeInfoFactory.cpp
****/
#include "pch.h"

#include "TypeInfoFactory.h"
#include "PropertyTypeInfo.h"
#include "PropertyInfo.h"
#include "MethodInfo.h"
#include "EnumTypeInfo.h"
#include "AsyncOperationTypeInfo.h"
#include "AsyncOperationClassInfoCache.h"
#include "EnumerableClassInfoCache.h"
#include "ActivationFactory.h"
#include "FoundationLibraryInstance.h"

#include "EnumTypeClassInfoCache.h"
#include <foundation/library/dictionary_class.h>
#include <foundation/library/property_type_info.h>

#include <foundation/library/com_ptr_singleton.h>
#include "pal_str.h"

using namespace foundation;

// {14D1D3E6-F614-4C30-9BCB-1C1F0197F342}
const GUID foundation::library::IID_ITypeInfoFactory =
{ 0x14d1d3e6, 0xf614, 0x4c30,{ 0x9b, 0xcb, 0x1c, 0x1f, 0x1, 0x97, 0xf3, 0x42 } };

LPCSTR_t _PropertyTypeInfo_PropertyType_Key = U("propertyType");
LPCSTR_t _PropertyTypeInfo_TypeInfo_Key = U("typeInfo");

static HRESULT CreateRuntimePropertyTypeInfo(
    _In_ PropertyTypeInfo *p_type_info,
    _In_ GUID *pType,
    _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo)
{
    foundation_assert(p_type_info);
    IFR_ASSERT(CPropertyTypeInfo::CreateInstance(
        p_type_info,
        pType,
        ppPropertyTypeInfo));
    (static_cast<CPropertyTypeInfo *>(*ppPropertyTypeInfo))->SetDeleteVarTypeInfo(true);
    return S_OK;

}

HRESULT CPropertyTypeInfoFactoryCache::CreateOrGetFactoryPropertyTypeInfo(
    PropertyType type,
    _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo)
{
    _FoundationPropertyTypeInfo_MapType::const_iterator iter = _foundationPropertyTypeInfos.find(type);
    if (iter != _foundationPropertyTypeInfos.end())
    {
        *ppPropertyTypeInfo = (*iter).second;
        (*ppPropertyTypeInfo)->AddRef();
        return S_OK;
    }
    // create a dynamic property type info
    IFR_ASSERT(CreateRuntimePropertyTypeInfo(
        new PropertyTypeInfo(type, nullptr), 
        nullptr,
        ppPropertyTypeInfo));

    // this will AddRef
    _foundationPropertyTypeInfos[type] = *ppPropertyTypeInfo;
    return S_OK;
}

HRESULT CPropertyTypeInfoFactoryCache::CreateOrGetFactoryPropertyTypeInfo(
    _In_ const PropertyTypeInfo *pTypeInfo,
    _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo)
{
    foundation_assert(pTypeInfo);
    IFCPTR_COM_OUT_PTR_ASSERT(ppPropertyTypeInfo);

    if (pTypeInfo->m_p_type_info != nullptr)
    {
        return CPropertyTypeInfo::CreateInstance(
            pTypeInfo,
            nullptr,
            ppPropertyTypeInfo);
    }
    else
    {
        return CreateOrGetFactoryPropertyTypeInfo(pTypeInfo->m_property_type, ppPropertyTypeInfo);
    }
}

HRESULT CPropertyTypeInfoFactoryCache::CreatePropertyTypeInfoInternal(
    _In_ const PropertyTypeInfo *pTypeInfo,
    _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo)
{
    return GetInstance().CreateOrGetFactoryPropertyTypeInfo(pTypeInfo,ppPropertyTypeInfo);
}

CPropertyTypeInfoFactoryCache& CPropertyTypeInfoFactoryCache::GetInstance()
{
    static CPropertyTypeInfoFactoryCache _PropertyTypeInfoFactoryCache;
    return _PropertyTypeInfoFactoryCache;
}

library::ITypeInfoFactory *CTypeInfoFactory::GetInstance()
{
    // Ensure we have a dependency on the CFoundationLibraryInstance class
    CFoundationLibraryInstance::_Initialize();

    return library::_GetFactoryInstance<CTypeInfoFactory, library::ITypeInfoFactory>();
}


HRESULT CTypeInfoFactory::GetOrCreateInstance(library::ITypeInfoFactory **ppPropertyModelInfoFactory)
{
    IFCPTR_ASSERT(ppPropertyModelInfoFactory);
    *ppPropertyModelInfoFactory = GetInstance();
    (*ppPropertyModelInfoFactory)->AddRef();
    return S_OK;
}

HRESULT CTypeInfoFactory::_DeserializePropertyTypeInfo(
    const DictionaryPtr& type_info_ptr,
    LPCSTR_t propertyTypeInfoKey,
    _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo)
{
    DictionaryPtr property_type_info_ptr;
    IFR_ASSERT(type_info_ptr.GetValuePtr(propertyTypeInfoKey, property_type_info_ptr));

    PropertyType propertyType;
    IID typeInfoType;
    IFR_ASSERT(_DeserializePropertyTypeInfo(property_type_info_ptr, propertyType, typeInfoType));
    return CreatePropertyTypeInfoInternal((PropertyType)propertyType, typeInfoType, ppPropertyTypeInfo);
}

HRESULT CTypeInfoFactory::_SerializePropertyTypeInfo(
    foundation::DictionaryPtr& property_type_info_ptr,
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo)
{
    PropertyType propertyType;
    pPropertyTypeInfo->GetPropertyType(&propertyType);
    property_type_info_ptr.SetValue(_PropertyTypeInfo_PropertyType_Key, (UINT32)propertyType);

    foundation::ComPtr<IObjectTypeInfo> object_type_info_ptr;
    pPropertyTypeInfo->GetTypeInfo(object_type_info_ptr.GetAddressOf());

    if (object_type_info_ptr != nullptr)
    {
        IID iidType;
        object_type_info_ptr->GetType(&iidType);
        property_type_info_ptr.SetValue(_PropertyTypeInfo_TypeInfo_Key, iidType);
    }
    return S_OK;
}

HRESULT CTypeInfoFactory::_DeserializePropertyTypeInfo(
    const foundation::DictionaryPtr& property_type_info_ptr,
    PropertyType& propertyType,
    IID& typeInfoType)
{
    UINT32 value;
    IFR_ASSERT(property_type_info_ptr.GetValue(_PropertyTypeInfo_PropertyType_Key, value));
    propertyType = (PropertyType)value;

    if (property_type_info_ptr.HasKey(_PropertyTypeInfo_TypeInfo_Key))
    {
        IFR_ASSERT(property_type_info_ptr.GetValue(_PropertyTypeInfo_TypeInfo_Key, typeInfoType));
    }
    else
    {
        typeInfoType = foundation_GUID_NULL;
    }
    return S_OK;
}

HRESULT CTypeInfoFactory::_SerializePropertyTypeInfo(
    DictionaryPtr& type_info_ptr,
    LPCSTR_t propertyTypeInfoKey,
    _In_ IPropertyTypeInfo *pPropertyTypeInfo)
{
    DictionaryPtr property_type_info_ptr;
    IFR_ASSERT(library::CreateDictionaryClass(property_type_info_ptr.ReleaseAndGetAddressOf()));

    IFR_ASSERT(_SerializePropertyTypeInfo(property_type_info_ptr, pPropertyTypeInfo));
    // add the key object
    type_info_ptr.SetValue(propertyTypeInfoKey, property_type_info_ptr);

    return S_OK;
}

HRESULT CTypeInfoFactory::CreatePropertyTypeInfoInternal(
    foundation::PropertyType type,
    GUID typeInfoType,
    _COM_Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfo)
{
    foundation_assert(ppPropertyTypeInfo != nullptr);

    if (IsEqualIID(typeInfoType, foundation_GUID_NULL))
    {
        return CPropertyTypeInfoFactoryCache::GetInstance().CreateOrGetFactoryPropertyTypeInfo(type, ppPropertyTypeInfo);
    }
    else
    {
        IFR_ASSERT(CreateRuntimePropertyTypeInfo(
            new PropertyTypeInfo(type, nullptr),
            &typeInfoType,
            ppPropertyTypeInfo));
        return S_OK;
    }
}


STDMETHODIMP CTypeInfoFactory::CreatePropertyTypeInfo(
    foundation::PropertyType type,
    GUID typeInfoType,
    _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo)
{
    IFCPTR_ASSERT(ppPropertyTypeInfo);

    return CreatePropertyTypeInfoInternal(type, typeInfoType, ppPropertyTypeInfo);
}

STDMETHODIMP CTypeInfoFactory::CreatePropertyInfo(
    UINT32 Id,
    _In_ HSTRING name,
    foundation::PropertyType type,
    UINT32 flags,
    GUID objectType,
    _COM_Outptr_ IPropertyInfo **ppPropertyInfo)
{
    bool hasObjectType = !IsEqualIID(objectType, foundation_GUID_NULL);

    PropertyInfo propertyInfo(
        Id,
        _pal_GetStringRawBuffer(name, nullptr),
        type,
        flags,
        nullptr
        );
    IFR_ASSERT(CPropertyInfo::CreateInstance(
        &propertyInfo,
        hasObjectType ? &objectType : nullptr,
        true,
        ppPropertyInfo));
    return S_OK;
}

STDMETHODIMP CTypeInfoFactory::CreateMethodInfo(
    _In_ UINT32 methodId,
    _In_ HSTRING name,
    _In_ foundation::PropertyType returnPropertyType,
    _In_ GUID *pReturnModelType,
    _In_ bool isAsync,
    _Outptr_ library::IMethodInfoClass **ppMethodInfoClass)
{
    foundation::PropertyTypeInfo resultTypeInfo(returnPropertyType, nullptr);
    foundation::MethodInfo methodInfo(
        methodId,
        _pal_GetStringRawBuffer(name, nullptr),
        nullptr,
        &resultTypeInfo,
        isAsync
    );

    IMethodInfo *pMethodInfo = nullptr;
    IFR_ASSERT(CMethodInfo::CreateInstance(
        &methodInfo,
        pReturnModelType,
        &pMethodInfo));
    // cast to expected result
    *ppMethodInfoClass = static_cast<library::IMethodInfoClass *>(
        static_cast<CMethodInfo *>(pMethodInfo));
    return S_OK;
}
STDMETHODIMP CTypeInfoFactory::CreateEnumTypeInfo(
    UINT32 typeId,
    GUID iidType,
    _In_ HSTRING name,
    bool isFlags,
    UINT32 size,
    _In_ HSTRING *pName,
    _In_ UINT32 *pValues,
    _COM_Outptr_ IEnumTypeInfo **ppEnumTypeInfo)
{
    IFCPTR_ASSERT(ppEnumTypeInfo);
    CEnumTypeInfo *pEnumTypeInfo = nullptr;
    IFR_ASSERT(CEnumTypeInfo::create_runtime_instance(
        new EnumTypeInfo(
            typeId, 
            _DuplicateIIDType(iidType),
            _pal_strdup(_pal_GetStringRawBuffer(name, nullptr)),
            isFlags, 
            nullptr, 
            nullptr),
        size,
        pName,
        pValues,
        &pEnumTypeInfo
        ));
    *ppEnumTypeInfo = static_cast<IEnumTypeInfo *>(pEnumTypeInfo);
    return S_OK;
}

STDMETHODIMP CTypeInfoFactory::CreateAsyncOperationTypeInfo(
    UINT32 typeId,
    GUID iidType,
    _In_ HSTRING name,
    _In_ IPropertyTypeInfo *pResultTypeInfo,
    _COM_Outptr_ IAsyncOperationTypeInfo **ppAsyncOperationTypeInfo)
{
    IFCPTR_ASSERT(pResultTypeInfo);
    IFCPTR_ASSERT(ppAsyncOperationTypeInfo);

    AsyncOperationTypeInfo *p_type_info = new AsyncOperationTypeInfo(
        typeId,
        _DuplicateIIDType(iidType),
        _pal_strdup(_pal_GetStringRawBuffer(name, nullptr)),
        nullptr /* Result Type Info */
        );

    CAsyncOperationTypeInfo *pAsyncOperationTypeInfo = nullptr;
    IFR_ASSERT(CAsyncOperationTypeInfo::create_runtime_instance(p_type_info, pResultTypeInfo, &pAsyncOperationTypeInfo));
    *ppAsyncOperationTypeInfo = static_cast<IAsyncOperationTypeInfo *>(pAsyncOperationTypeInfo);
    return S_OK;
}

STDMETHODIMP CTypeInfoFactory::CreateEnumerableTypeInfo(
    _In_ UINT32 typeId,
    _In_ GUID iidType,
    _In_ HSTRING name,
    _In_ foundation::IPropertyTypeInfo *pItemTypeInfo,
    _In_ UINT32 type,
    _Outptr_ IEnumerableTypeInfo **ppCollectionModelInfo)
{
    IFCPTR_ASSERT(pItemTypeInfo);
    IFCPTR_ASSERT(ppCollectionModelInfo);

    EnumerableTypeInfo *p_type_info = new EnumerableTypeInfo(
        typeId,
        _DuplicateIIDType(iidType),
        _pal_strdup(_pal_GetStringRawBuffer(name, nullptr)),
        nullptr, /* Item Type Info */
        type);

    CEnumerableTypeInfo *pEnumerableTypeInfo = nullptr;
    IFR_ASSERT(CEnumerableTypeInfo::create_runtime_instance(p_type_info, pItemTypeInfo, &pEnumerableTypeInfo));
    *ppCollectionModelInfo = static_cast<IEnumerableTypeInfo *>(pEnumerableTypeInfo);
    return S_OK;
}

template<class TClassInfoCache>
HRESULT add_to_cache_helper(IObjectTypeInfo *pTypeInfo)
{
    IID iidType;
    pTypeInfo->GetType(&iidType);

    // Please note after registering the 'cleanup' will call 'Release'
    pTypeInfo->AddRef();
    TClassInfoCache::AddToCache(iidType,
        static_cast<typename TClassInfoCache::TypeInfoClass_Type *>(pTypeInfo));
    return S_OK;
}

STDMETHODIMP CTypeInfoFactory::RegisterEnumTypeInfo(_In_ IEnumTypeInfo *pEnumTypeInfo)
{
    IFCPTR_ASSERT(pEnumTypeInfo);
    return add_to_cache_helper<CEnumTypeClassInfoCache>(pEnumTypeInfo);
}

STDMETHODIMP CTypeInfoFactory::RegisterAsyncOperationTypeInfo(_In_ IAsyncOperationTypeInfo *pAsyncOperationTypeInfo)
{
    IFCPTR_ASSERT(pAsyncOperationTypeInfo);
    return add_to_cache_helper<CAsyncOperationClassInfoCache>(pAsyncOperationTypeInfo);
}

STDMETHODIMP CTypeInfoFactory::RegisterEnumerableTypeInfo(_In_ IEnumerableTypeInfo *pEnumerableTypeInfo)
{
    IFCPTR_ASSERT(pEnumerableTypeInfo);
    return add_to_cache_helper<CEnumerableClassInfoCache>(pEnumerableTypeInfo);
}

STDMETHODIMP CTypeInfoFactory::RegisterTypeInfo(
    _In_ const foundation::TypeInfo* pTypeInfoEntry)
{
    foundation::IObjectTypeInfo *pTypeInfo;
    return CTypeInfoCache::CreateOrGetTypeInfo(pTypeInfoEntry, &pTypeInfo);
}

STDMETHODIMP CTypeInfoFactory::RegisterMultipleTypeInfos(
    _In_ const foundation::TypeInfo* pTypeInfoEntries[])
{
    return CTypeInfoCache::RegisterMultipleTypeInfos(pTypeInfoEntries);
}

STDMETHODIMP CTypeInfoFactory::RegisterMultipleTypeInfos(
    UINT32 size,
    foundation::IDictionary **ppDictionaryTypeInfo)
{
    return CTypeInfoCache::RegisterMultipleTypeInfos(size, ppDictionaryTypeInfo);
}

STDMETHODIMP CTypeInfoFactory::SerializeTypeInfo(
    foundation::IObjectTypeInfo *pTypeInfo,
    foundation::IDictionary **ppDictionaryTypeInfo)
{
    return CTypeInfoCache::SerializeTypeInfo(pTypeInfo, ppDictionaryTypeInfo);
}

static _RegisterActivationFactory _typeInfoRegister(
    U("foundation.TypeInfo"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CTypeInfoFactory::GetInstance)
    );
