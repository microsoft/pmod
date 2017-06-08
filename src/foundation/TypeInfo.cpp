/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeInfo.cpp
****/
#include "pch.h"
#include "TypeInfo.h"
#include "TypeInfoCache.h"
#include <foundation/library/dictionary_class.h>

#include "pal_str.h"

#include <set>

using namespace foundation;

foundation::TypeInfo foundation::_ObjectTypeInfo(
    _ReservedTypeId_Base + 0x60,
    foundation::TypeInfo_Object,
    IID_IObject,
    U("foundation::IObject"),
    nullptr);
foundation::TypeInfo foundation::_DictionaryTypeInfo(
    _ReservedTypeId_Base + 0x61,
    foundation::TypeInfo_Dictionary,
    IID_IDictionary,
    U("foundation::IDictionary"),
    nullptr);

// Define the 'Core' Var Type Info to export
PropertyTypeInfo foundation::_StringTypeInfo(PropertyType_String, nullptr);
PropertyTypeInfo foundation::_Int32TypeInfo(PropertyType_Int32, nullptr);
PropertyTypeInfo foundation::_UInt32TypeInfo(PropertyType_UInt32, nullptr);
PropertyTypeInfo foundation::_Int64TypeInfo(PropertyType_Int64, nullptr);
PropertyTypeInfo foundation::_UInt64TypeInfo(PropertyType_UInt64, nullptr);
PropertyTypeInfo foundation::_Int16TypeInfo(PropertyType_Int16, nullptr);
PropertyTypeInfo foundation::_UInt16TypeInfo(PropertyType_UInt16, nullptr);
PropertyTypeInfo foundation::_UInt8TypeInfo(PropertyType_UInt8, nullptr);
PropertyTypeInfo foundation::_BoolTypeInfo(PropertyType_Boolean, nullptr);
PropertyTypeInfo foundation::_SingleTypeInfo(PropertyType_Single, nullptr);
PropertyTypeInfo foundation::_DoubleTypeInfo(PropertyType_Double, nullptr);
PropertyTypeInfo foundation::_DateTimeTypeInfo(PropertyType_DateTime, nullptr);
PropertyTypeInfo foundation::_InspectableTypeInfo(PropertyType_Inspectable, nullptr);
PropertyTypeInfo foundation::_VoidTypeInfo(PropertyType_Empty, nullptr);

PropertyTypeInfo foundation::_StringArrayTypeInfo(PropertyType_StringArray, nullptr);;
PropertyTypeInfo foundation::_Int32ArrayTypeInfo(PropertyType_Int32Array, nullptr);
PropertyTypeInfo foundation::_UInt32ArrayTypeInfo(PropertyType_UInt32Array, nullptr);
PropertyTypeInfo foundation::_Int64ArrayTypeInfo(PropertyType_Int64Array, nullptr);
PropertyTypeInfo foundation::_UInt64ArrayTypeInfo(PropertyType_UInt64Array, nullptr);
PropertyTypeInfo foundation::_Int16ArrayTypeInfo(PropertyType_Int16Array, nullptr);
PropertyTypeInfo foundation::_UInt16ArrayTypeInfo(PropertyType_UInt16Array, nullptr);
PropertyTypeInfo foundation::_UInt8ArrayTypeInfo(PropertyType_UInt8Array, nullptr);
PropertyTypeInfo foundation::_BoolArrayTypeInfo(PropertyType_BooleanArray, nullptr);
PropertyTypeInfo foundation::_DoubleArrayTypeInfo(PropertyType_DoubleArray, nullptr);
PropertyTypeInfo foundation::_DateTimeArrayTypeInfo(PropertyType_DateTimeArray, nullptr);
PropertyTypeInfo foundation::_ObjectArrayTypeInfo(PropertyType_InspectableArray, nullptr);

PropertyTypeInfo foundation::_ObjectPropertyTypeInfo(PropertyType_Inspectable, &_ObjectTypeInfo);
PropertyTypeInfo foundation::_DictionaryPropertyTypeInfo(PropertyType_Inspectable, &_DictionaryTypeInfo);

LPCSTR_t _TypeInfo_Type_Key = U("type");
LPCSTR_t _TypeInfo_BaseType_Key = U("baseType");
LPCSTR_t _TypeInfo_TypeId_Key = U("typeId");
LPCSTR_t _TypeInfo_IIdType_Key = U("iidType");
LPCSTR_t _TypeInfo_Name_Key = U("name");

HRESULT _DeserializeTypeInfo(
	const foundation::DictionaryPtr& type_info_ptr,
	_In_ foundation::TypeInfo *pTypeInfo)
{
	IFR_ASSERT(type_info_ptr.GetValue(
		_TypeInfo_Type_Key,
		pTypeInfo->m_type));

	IFR_ASSERT(type_info_ptr.GetValue(
		_TypeInfo_TypeId_Key,
		const_cast<UINT32 *>(&pTypeInfo->m_type_id)));

	GUID iidType;
	IFR_ASSERT(type_info_ptr.GetValue(
		_TypeInfo_IIdType_Key,
		&iidType));

	foundation::HStringPtr name;
	IFR_ASSERT(type_info_ptr.GetValue(_TypeInfo_Name_Key, name));

	// duplicate both Name & Type fields
	*(const_cast<CHAR_t **>(&pTypeInfo->m_name)) = _pal_strdup(name.GetRawBuffer());
	pTypeInfo->m_p_iid_type = &_DuplicateIIDType(iidType);
	return S_OK;
}

HRESULT _SerializeTypeInfo(
	_In_ const foundation::TypeInfo *pTypeInfo,
	foundation::DictionaryPtr& type_info_ptr)
{
	type_info_ptr.SetValue(_TypeInfo_Type_Key, pTypeInfo->m_type);
	type_info_ptr.SetValue(_TypeInfo_TypeId_Key, pTypeInfo->m_type_id);
	type_info_ptr.SetValue(_TypeInfo_IIdType_Key, pTypeInfo->get_iid_type());
	type_info_ptr.SetValue(_TypeInfo_Name_Key, pTypeInfo->m_name);
	return S_OK;
}

typedef CTypeInfo<foundation::IObjectTypeInfo, foundation::TypeInfo> CObjectBaseTypeInfoClass_Base;

class CObjectBaseTypeInfoClass :
    public CObjectBaseTypeInfoClass_Base
{
public:
    static HRESULT CreateInstance(const foundation::TypeInfo* pTypeInfo, CObjectBaseTypeInfoClass **pp)
    {
        return ctl::inspectable::CreateInstanceWithInitialize(pp, pTypeInfo);
    }
    HRESULT _Initialize(const foundation::TypeInfo* pTypeInfo)
    {
        return _InitializeTypeInfo(pTypeInfo);
    }
};
class CObjectTypeClassInfoCache :
    public _TStaticTypeInfoCache<CObjectTypeClassInfoCache, CObjectBaseTypeInfoClass>
{
public:
    static CObjectTypeClassInfoCache::_TypeModelInfoCacheType& GetInstance()
    {
        static _TypeModelInfoCacheType _instance;
        return _instance;
    }
};

class _ITypeInfoCacheProvider_lock_set :
    public std::set < ITypeInfoCacheProvider * >
{
public:
    ~_ITypeInfoCacheProvider_lock_set()
    {
        for (_ITypeInfoCacheProvider_lock_set::const_iterator iter = this->begin();
            iter != this->end();
            ++iter)
        {
            (*iter)->Unlock();
        }
    }
};

CTypeInfoCache& CTypeInfoCache::GetInstance()
{
    static CTypeInfoCache _TypeInfoCache;
    return _TypeInfoCache;
}

CTypeInfoCache::CTypeInfoCache()
{
    static CTypeInfoCacheProvider<CObjectTypeClassInfoCache, TypeInfo_Object> _ObjectClassInfoCacheProvider(false);
    _typeInfoCaches[TypeInfo_Object] = &_ObjectClassInfoCacheProvider;

    const foundation::TypeInfo *factoryTypeInfos[] = {
        &_ObjectTypeInfo,
        &_DictionaryTypeInfo
    };
    for (int index = 0;index < sizeof(factoryTypeInfos) / sizeof(const foundation::TypeInfo *);index++)
    {
        CObjectBaseTypeInfoClass *pObjectBaseTypeInfoClass = nullptr;
        CObjectBaseTypeInfoClass::CreateInstance(factoryTypeInfos[index],&pObjectBaseTypeInfoClass);
        CObjectTypeClassInfoCache::AddToCache(factoryTypeInfos[index]->get_iid_type(), pObjectBaseTypeInfoClass);
    }
}

void CTypeInfoCache::RegisterTypeInfoCacheProvider(UINT32 type_info_type, ITypeInfoCacheProvider *pTypeInfoProvider)
{
    GetInstance()._typeInfoCaches[type_info_type] = pTypeInfoProvider;
}

HRESULT CTypeInfoCache::GetTypeInfos(
    foundation::_TypeInfoFilter *pTypeInfoFilter,
    UINT32 *size,
    foundation::IObjectTypeInfo ***ppTypeInfos)
{
    return GetInstance().GetTypeInfosInternal(pTypeInfoFilter, size, ppTypeInfos);
}

HRESULT CTypeInfoCache::GetTypeInfo(
    const IID& iidType,
    /*_COM_Outptr_result_maybenull_*/ foundation::IObjectTypeInfo **ppTypeInfo)
{
    HRESULT hr = LookupTypeInfo(iidType, ppTypeInfo);
    if (SUCCEEDED(hr))
    {
        (*ppTypeInfo)->AddRef();
    }
    return hr;
}

HRESULT CTypeInfoCache::LookupTypeInfo(
    const IID& iidType,
    /*_COM_Outptr_result_maybenull_*/ foundation::IObjectTypeInfo **ppTypeInfo)
{
    return GetInstance().LookupTypeInfoInternal(iidType, ppTypeInfo);
}

HRESULT CTypeInfoCache::CreateOrGetTypeInfo(
    _In_ const foundation::TypeInfo *pTypeInfo,
    /*_COM_Outptr_*/ foundation::IObjectTypeInfo **ppTypeInfo)
{
    return GetInstance().CreateOrGetTypeInfoInternal(pTypeInfo, ppTypeInfo);
}

HRESULT CTypeInfoCache::RegisterMultipleTypeInfos(
    _In_ const foundation::TypeInfo* typeInfoEntries[])
{
    return GetInstance().RegisterMultipleTypeInfosInternal(typeInfoEntries);
}

HRESULT CTypeInfoCache::RegisterMultipleTypeInfos(
    UINT32 size, 
    foundation::IDictionary **ppDictionaryTypeInfo)
{
    return GetInstance().RegisterMultipleTypeInfosInternal(size, ppDictionaryTypeInfo);
}

HRESULT CTypeInfoCache::SerializeTypeInfo(
    _In_ foundation::IObjectTypeInfo *pTypeInfo,
    foundation::IDictionary **ppDictionaryTypeInfo)
{
    return GetInstance().SerializeTypeInfoInternal(pTypeInfo, ppDictionaryTypeInfo);
}

HRESULT CTypeInfoCache::GetTypeInfosInternal(
    foundation::_TypeInfoFilter *pTypeInfoFilter,
    UINT32 *size,
    foundation::IObjectTypeInfo ***ppTypeInfos)
{
    foundation_assert(ppTypeInfos);
    *ppTypeInfos = nullptr;

    std::vector<foundation::IObjectTypeInfo *> typeInfos;
    for (
        _TypeInfoCacheProvider_MapType::const_iterator iter = _typeInfoCaches.begin();
        iter != _typeInfoCaches.end();
    ++iter)
    {
        (*iter).second->GetTypeInfos(pTypeInfoFilter, typeInfos);
    }
    ContainerRefCountToArray(typeInfos, size, ppTypeInfos);
    return S_OK;
}

HRESULT CTypeInfoCache::LookupTypeInfoInternal(
    const IID& iidType,
    /*_COM_Outptr_result_maybenull_*/ foundation::IObjectTypeInfo **ppTypeInfo)
{
    foundation_assert(ppTypeInfo);
    *ppTypeInfo = nullptr;
    for (
        _TypeInfoCacheProvider_MapType::const_iterator iter = _typeInfoCaches.begin();
        iter != _typeInfoCaches.end();
        ++iter)
    {
        HRESULT hr = (*iter).second->LookupTypeInfo(iidType, ppTypeInfo);
        if (hr == S_OK)
        {
            return S_OK;
        }
        else if (hr != foundation::E_TYPE_INFO_NOT_REGISTERED)
        {
            return hr;
        }
    }
    return foundation::E_TYPE_INFO_NOT_REGISTERED;
}

HRESULT CTypeInfoCache::CreateOrGetTypeInfoInternal(
    _In_ const foundation::TypeInfo *pTypeInfo,
    /*_COM_Outptr_*/ foundation::IObjectTypeInfo **ppTypeInfo)
{
    foundation_assert(pTypeInfo);
    IFCPTR_COM_OUT_PTR_ASSERT(ppTypeInfo);

    if (SUCCEEDED(LookupTypeInfoInternal(pTypeInfo->get_iid_type(), ppTypeInfo)))
    {
        return S_OK;
    }
    _TypeInfoCacheProvider_MapType::const_iterator iter = _typeInfoCaches.find(pTypeInfo->m_type);
    if (iter != _typeInfoCaches.end())
    {
        return (*iter).second->CreateTypeInfo(pTypeInfo, ppTypeInfo);
    }
    foundation_assert(false);
    return E_NOTIMPL;
}

HRESULT CTypeInfoCache::RegisterMultipleTypeInfosInternal(_In_ const foundation::TypeInfo* pTypeInfoEntries[])
{
    typedef std::vector< std::pair<foundation::IObjectTypeInfo*, const foundation::TypeInfo*> > CreateAndRegisterTypeInfoVector;
    CreateAndRegisterTypeInfoVector createdTypeInfos;
    _ITypeInfoCacheProvider_lock_set lockedCacheProviders;

    while (*pTypeInfoEntries)
    {
        const foundation::TypeInfo *pTypeInfo = *pTypeInfoEntries;
        foundation::IObjectTypeInfo *pTypeInfoClass = nullptr;
        LookupTypeInfo(pTypeInfo->get_iid_type(), &pTypeInfoClass);
        if (pTypeInfoClass == nullptr)
        {
            _TypeInfoCacheProvider_MapType::const_iterator iter = _typeInfoCaches.find(pTypeInfo->m_type);
            if (iter == _typeInfoCaches.end())
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }
            ITypeInfoCacheProvider *pTypeInfoCacheProvider = (*iter).second;
            if (lockedCacheProviders.insert(pTypeInfoCacheProvider).second)
            {
                pTypeInfoCacheProvider->Lock();
            }
            IFR_ASSERT(pTypeInfoCacheProvider->CreateTypeInfoClass(pTypeInfo->get_iid_type(), false, &pTypeInfoClass));
            createdTypeInfos.push_back(std::pair<foundation::IObjectTypeInfo*, const foundation::TypeInfo*>(
                pTypeInfoClass, pTypeInfo));
        }
        ++pTypeInfoEntries;
    }
    // After all type infos are registered we will finalize the initialization
    for (CreateAndRegisterTypeInfoVector::iterator iter = createdTypeInfos.begin();
        iter != createdTypeInfos.end();
        ++iter)
    {
        foundation::IObjectTypeInfo *pTypeInfoClass = (*iter).first;
        const foundation::TypeInfo *pTypeInfo = (*iter).second;
        _TypeInfoCacheProvider_MapType::const_iterator iter_provider = _typeInfoCaches.find(pTypeInfo->m_type);
        foundation_assert(iter_provider != _typeInfoCaches.end());

        IFR_ASSERT((*iter_provider).second->InitializeTypeInfoClass(pTypeInfoClass, pTypeInfo));
    }
    return S_OK;
}

HRESULT CTypeInfoCache::RegisterMultipleTypeInfosInternal(
    UINT32 size, 
    foundation::IDictionary **ppDictionaryTypeInfo)
{
    typedef std::vector< std::pair<foundation::IObjectTypeInfo*, foundation::IDictionary*> > CreateAndRegisterTypeInfoVector;
    CreateAndRegisterTypeInfoVector createdTypeInfos;
    _ITypeInfoCacheProvider_lock_set lockedCacheProviders;

    for (UINT32 index = 0; index < size; ++index)
    {
        DictionaryPtr type_info_ptr = ppDictionaryTypeInfo[index];
        UINT32 type;
        IFR_ASSERT(type_info_ptr.GetValue(
            _TypeInfo_Type_Key,
            type));
        IID iidType;
        IFR_ASSERT(type_info_ptr.GetValue(
            _TypeInfo_IIdType_Key,
            iidType));

        _TypeInfoCacheProvider_MapType::const_iterator iter = _typeInfoCaches.find(type);
        if (iter == _typeInfoCaches.end())
        {
            foundation_assert(false);
            return E_NOTIMPL;
        }
        ITypeInfoCacheProvider *pTypeInfoCacheProvider = (*iter).second;
        if (lockedCacheProviders.insert(pTypeInfoCacheProvider).second)
        {
            pTypeInfoCacheProvider->Lock();
        }
        foundation::IObjectTypeInfo *pTypeInfoClass = nullptr;
        IFR_ASSERT(pTypeInfoCacheProvider->CreateTypeInfoClass(iidType,true, &pTypeInfoClass));
        createdTypeInfos.push_back(std::pair<foundation::IObjectTypeInfo*, foundation::IDictionary*>(
            pTypeInfoClass, type_info_ptr));
    }
    // After all type infos are registered we will finalize the initialization
    for (CreateAndRegisterTypeInfoVector::iterator iter = createdTypeInfos.begin();
        iter != createdTypeInfos.end();
        ++iter)
    {
        foundation::IObjectTypeInfo *pTypeInfoClass = (*iter).first;
        DictionaryPtr type_info_ptr = (*iter).second;

        UINT32 type;
        IFR_ASSERT(type_info_ptr.GetValue(
            _TypeInfo_Type_Key,
            type));
        _TypeInfoCacheProvider_MapType::const_iterator iter_provider = _typeInfoCaches.find(type);

        foundation_assert(iter_provider != _typeInfoCaches.end());
        IFR_ASSERT((*iter_provider).second->InitializeTypeInfoClass(pTypeInfoClass, type_info_ptr));
    }
    return S_OK;
}

HRESULT CTypeInfoCache::SerializeTypeInfoInternal(
    _In_ foundation::IObjectTypeInfo *pTypeInfo,
    foundation::IDictionary **ppDictionaryTypeInfo)
{
    IObjectTypeInfoInternal *pObjectTypeInfoInternal;
    IFR_ASSERT(foundation::QueryWeakReference(pTypeInfo, &pObjectTypeInfoInternal));

    foundation::TypeInfo *p_type_info;
    pObjectTypeInfoInternal->GetTypeInfo(&p_type_info);
    
    _TypeInfoCacheProvider_MapType::const_iterator iter = _typeInfoCaches.find(p_type_info->m_type);
    if (iter == _typeInfoCaches.end())
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }
    DictionaryPtr type_info_ptr;
    IFR_ASSERT(foundation::library::CreateDictionaryClass(type_info_ptr.ReleaseAndGetAddressOf()));
    IFR_ASSERT((*iter).second->SerializeTypeInfoClass(pTypeInfo,type_info_ptr));
    *ppDictionaryTypeInfo = type_info_ptr.Detach();
    return S_OK;
}
