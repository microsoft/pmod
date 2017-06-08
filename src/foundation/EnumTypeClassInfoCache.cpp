/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumTypeClassInfoCache.cpp
****/

#include "pch.h"

#include "EnumTypeClassInfoCache.h"
#include "PtrSingleton.h"
using namespace foundation;


typedef  void (STDAPICALLTYPE *_EnumTypeInfoDisposedCallback)();
static _EnumTypeInfoDisposedCallback _disposedCallback;

EXTERN_C FOUNDATION_API void STDAPICALLTYPE SetEnumTypeInfoDisposedCallback(_EnumTypeInfoDisposedCallback pCallback)
{
    _disposedCallback = pCallback;
}

class CEnumTypeClassInfoCache_Type : public CEnumTypeClassInfoCache::_TypeModelInfoCacheType
{
public:
    ~CEnumTypeClassInfoCache_Type();
    void _DiposeInternal()
    {
        this->DisposeTypeInfos();
        this->CleanupInternal();
    }
private:
    void DisposeTypeInfos();
};

static PtrSingleton<CEnumTypeClassInfoCache_Type> _enumTypeClassInfoCache_Instance;

void _DiposeEnumTypeClassInfoCacheInternal()
{
    CEnumTypeClassInfoCache_Type *pEnumTypeClassInfoCache_Type = _enumTypeClassInfoCache_Instance._Get();
    if (pEnumTypeClassInfoCache_Type != nullptr)
    {
        pEnumTypeClassInfoCache_Type->_DiposeInternal();
    }
}

_Dispatchable_ptr_vector_type& CEnumTypeClassInfoCache::GetCallbacks()
{
    static _Dispatchable_ptr_vector_type _callbacks;
    return _callbacks;
}

HRESULT CEnumTypeClassInfoCache::register_enum_factory_type_info(foundation::library::IDelegateDispatchable *pCallback)
{
    GetCallbacks().push_back(pCallback);
    return S_OK;
}

void CEnumTypeClassInfoCache_Type::DisposeTypeInfos()
{
    if (_disposedCallback != nullptr)
    {
        (*_disposedCallback)();
    }

    const CEnumTypeClassInfoCache::_TypeModelInfoCacheType::_TypeInfoClassMapType& map = this->GetTypeModelInfoClassMap();
    for (CEnumTypeClassInfoCache::_TypeModelInfoCacheType::_TypeInfoClassMapType::const_iterator
        iter = map.begin();
        iter != map.end();
    iter++)
    {
        // Dispose the Enum Values
        (*iter).second->Dispose();
    }
}

CEnumTypeClassInfoCache_Type::~CEnumTypeClassInfoCache_Type()
{
    DisposeTypeInfos();
}

CEnumTypeClassInfoCache::_TypeModelInfoCacheType& CEnumTypeClassInfoCache::GetInstance()
{
    static _pal_AtomicIntegerType _registerFactoryEnumTypes = 0;

    // next call will only be successfully once when the refCount==0 and we
    // increment 1
    if (_pal_InterlockedCompareExchange(&_registerFactoryEnumTypes, 1, 0) == 0)
    {
        // register initial enum factory types
        for (_Dispatchable_ptr_vector_type::const_iterator iter =
            GetCallbacks().begin();
            iter != GetCallbacks().end();
        ++iter)
        {
            (*iter)->Invoke();
        }
    }
    return *_enumTypeClassInfoCache_Instance.GetInstance();
}

HRESULT CEnumTypeClassInfoCache::RegisterEnumTypeInfo(_In_ const EnumTypeInfo *pEnumTypeInfo)
{
    foundation_assert(pEnumTypeInfo);
    CEnumTypeInfo *pEnumTypeInfoClass = nullptr;
	// the internal Create will register the Type in my cache
    IFR_ASSERT(CEnumTypeInfo::CreateInstance(pEnumTypeInfo,&pEnumTypeInfoClass));
    return S_OK;
}

HRESULT CEnumTypeClassInfoCache::GetOrCreateEnumTypeInfo(
        _In_ const EnumTypeInfo *pEnumTypeInfo,
        _Outptr_ CEnumTypeInfo **ppEnumTypeInfoClass)
{
    foundation_assert(ppEnumTypeInfoClass);

    *ppEnumTypeInfoClass = LookupType(pEnumTypeInfo->get_iid_type());
    if(*ppEnumTypeInfoClass == nullptr)
    {
        IFR_ASSERT(CEnumTypeInfo::CreateInstance(pEnumTypeInfo,ppEnumTypeInfoClass));
    }
    return S_OK;
}

HRESULT CEnumTypeClassInfoCache::LookupEnumValue(UINT32 enumValue, foundation::IEnumValue **ppEnumValue)
{
    const _TypeModelInfoCacheType::_TypeInfoClassMapType& typeInfoClassmap = 
        InternalGetInstance().GetTypeModelInfoClassMap();

    for (_TypeModelInfoCacheType::_TypeInfoClassMapType::const_iterator
        iter = typeInfoClassmap.begin();
        iter != typeInfoClassmap.end();
    iter++)
    {
        UINT32 index = ((*iter).second)->IndexOfValue(enumValue);
        if (index != ((UINT32)-1))
        {
            return ((*iter).second)->GetEnumValue(enumValue, ppEnumValue);
        }
    }
    // not found code
    return S_FALSE;
}
