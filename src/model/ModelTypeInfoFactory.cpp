/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelTypeInfoFactory.cpp
****/
#include "pch.h"

#include "ModelTypeInfoFactory.h"
#include "ObservableCollectionInfo.h"
#include "CommandInfo.h"
#include "ObservableObjectInfo.h"
#include "EnumTypeInfo.h"

#include "EnumTypeClassInfoCache.h"
#include "CommandClassInfoCache.h"
#include "ObservableCollectionClassInfoCache.h"
#include "ObservableObjectClassInfoCache.h"

#include <foundation/library/com_ptr_singleton.h>
#include "pal_str.h"
#include <foundation/library/activation_factory.h>

using namespace pmod;

// {5BB94FDE-7E7D-4F00-92D9-54BE46EB909D}
const GUID pmod::library::IID_IModelTypeInfoFactory =
{ 0x5bb94fde, 0x7e7d, 0x4f00,{ 0x92, 0xd9, 0x54, 0xbe, 0x46, 0xeb, 0x90, 0x9d } };

EXTERN_C void _ModelTypeInfoInitializerStub();

library::IModelTypeInfoFactory *CModelTypeInfoFactory::GetInstance()
{
	_ModelTypeInfoInitializerStub();
	return foundation::library::_GetFactoryInstance<CModelTypeInfoFactory, library::IModelTypeInfoFactory>();
}


HRESULT CModelTypeInfoFactory::GetOrCreateInstance(library::IModelTypeInfoFactory **ppPropertyModelInfoFactory)
{
    IFCPTR_ASSERT(ppPropertyModelInfoFactory);
    *ppPropertyModelInfoFactory = GetInstance();
    (*ppPropertyModelInfoFactory)->AddRef();
    return S_OK;
}



STDMETHODIMP CModelTypeInfoFactory::CreateCommandInfo(
    _In_ UINT32 typeId,
    _In_ GUID iidType,
    _In_ HSTRING name,
    _In_ foundation::IPropertyTypeInfo *pParameterTypeInfo,
    _In_ foundation::IPropertyTypeInfo *pResultTypeInfo,
    _In_ bool isAsync,
    _Outptr_ ICommandInfo **ppCommandModelInfo)
{
    IFCPTR_ASSERT(pParameterTypeInfo);
    IFCPTR_ASSERT(pResultTypeInfo);
    IFCPTR_ASSERT(ppCommandModelInfo);

    pmod::CommandTypeInfo *p_type_info = new pmod::CommandTypeInfo(
        typeId,
        _DuplicateIIDType(iidType),
        _pal_strdup(_pal_GetStringRawBuffer(name, nullptr)),
        nullptr, /* Parameter Type Info */
        nullptr, /* Result Type Info */
        isAsync);

    CCommandModelTypeInfo *pCommandModelInfo = nullptr;
    IFR_ASSERT(CCommandModelTypeInfo::create_runtime_instance(p_type_info, pParameterTypeInfo, pResultTypeInfo, &pCommandModelInfo));
    *ppCommandModelInfo = static_cast<ICommandInfo *>(pCommandModelInfo);
    return S_OK;

}

STDMETHODIMP CModelTypeInfoFactory::CreateObservableObjectInfo(
        _In_ UINT32 typeId,
        _In_ GUID iidType,
        _In_ HSTRING name,
        _In_opt_ IObservableObjectInfo *pBaseType,
        _Outptr_ IObservableObjectInfo **ppPropertyModelInfo)
{
    LPCSTR_t pswzName = _pal_strdup(_pal_GetStringRawBuffer(name,nullptr));

    CPropertyModelInfoClass *pBaseTypeInfoClass = nullptr;
    if(pBaseType)
    {
        pBaseTypeInfoClass = static_cast<CPropertyModelInfoClass *>(pBaseType);
    }

    // pPropertyModelInfo could go out of scope after the createInstance, create a deep copy
    // of the instance. The copied will used to as ref for accessing ObservableObjectInfo data.
    pmod::ObservableObjectTypeInfo *pInfoModel = new pmod::ObservableObjectTypeInfo(
        typeId,
        _DuplicateIIDType(iidType),
        pswzName,
        nullptr,        // properties
        nullptr,        // methods
        nullptr,        // events
        pBaseType != nullptr ? pBaseTypeInfoClass->GetTypeInfo():nullptr);

    if (pInfoModel == nullptr)
    {
        _pal_MemFree((LPVOID)pswzName);
        return E_OUTOFMEMORY;
    }

    CPropertyModelInfoClass *pPropertyModelInfoClass = nullptr;
    IFR_ASSERT(CPropertyModelInfoClass::create_runtime_instance(
        pInfoModel,
        pBaseTypeInfoClass,
        &pPropertyModelInfoClass));
    // make this Model Type Info to be dynamic so we can add
    // properties and methods later
    pPropertyModelInfoClass->SetAllowChanges();

    // we want to delete when FinalRelease
    pPropertyModelInfoClass->SetDeleteTypeInfo(true);

    *ppPropertyModelInfo = static_cast<IObservableObjectInfo *>(pPropertyModelInfoClass);
    return S_OK;
}

STDMETHODIMP CModelTypeInfoFactory::CreateViewModelInfo(
        _In_ UINT32 typeId,
        _In_ GUID iidType,
        _In_ HSTRING name,
        _In_opt_ IObservableObjectInfo *pBaseType,
        _In_ IObservableObjectInfo *pSourceType,
        _In_ bool isInherited,
        _Outptr_ IViewModelInfo **ppPropertyViewModelInfo)
{
    IFCPTR_ASSERT(pSourceType);

    LPCSTR_t pswzName = _pal_strdup(_pal_GetStringRawBuffer(name,nullptr));

    CPropertyModelInfoClass *pBaseTypeInfoClass = nullptr;
    if(pBaseType)
    {
        pBaseTypeInfoClass = static_cast<CPropertyModelInfoClass *>(pBaseType);
    }

    // pPropertyModelInfo could go out of scope after the createInstance, create a deep copy
    // of the instance. The copied will used to as ref for accessing ObservableObjectInfo data.
    pmod::ViewModelTypeInfo *pInfoViewModel = new pmod::ViewModelTypeInfo(
        typeId,
        _DuplicateIIDType(iidType),
        pswzName,
        nullptr,        // properties
        nullptr,        // methods
        nullptr,        // events
        pBaseType != nullptr ? pBaseTypeInfoClass->GetTypeInfo():nullptr,
        (static_cast<CPropertyModelInfoClass *>(pSourceType))->GetTypeInfo(),
        isInherited
        );

    if (pInfoViewModel == nullptr)
    {
        _pal_MemFree((LPVOID)pswzName);
        return E_OUTOFMEMORY;
    }

    CPropertyViewModelInfoClass *pPropertyViewModelInfoClass = nullptr;
    IFR_ASSERT(CPropertyViewModelInfoClass::create_runtime_instance(
        pInfoViewModel,
        pBaseTypeInfoClass,
        &pPropertyViewModelInfoClass));
    // make this Model Type Info to be dynamic so we can add
    // properties and methods later
    pPropertyViewModelInfoClass->SetAllowChanges();

    // we want to delete when FinalRelease
    pPropertyViewModelInfoClass->SetDeleteTypeInfo(true);

    *ppPropertyViewModelInfo = static_cast<IViewModelInfo *>(pPropertyViewModelInfoClass);
    return S_OK;
}

STDMETHODIMP CModelTypeInfoFactory::RegisterObjectTypeInfo(_In_ IObservableObjectInfo *pPropertyModelInfo)
{
    IFCPTR_ASSERT(pPropertyModelInfo);

    // Please note after registering the 'cleanup' will call 'Release'
    pPropertyModelInfo->AddRef();
    IFR_ASSERT(CPropertyModelClassInfoCache::RegisterPropertyModelInfo(
        static_cast<CPropertyModelInfoClass *>(pPropertyModelInfo),
        foundation::IsTypeOf<IViewModelInfo>(pPropertyModelInfo)
        ));

    return S_OK;
}

template<class TClassInfoCache>
HRESULT add_to_cache_helper(foundation::IObjectTypeInfo *pModelTypeInfo)
{
    IID iidType;
    pModelTypeInfo->GetType(&iidType);

    // Please note after registering the 'cleanup' will call 'Release'
    pModelTypeInfo->AddRef();
    TClassInfoCache::AddToCache(iidType,
        static_cast<typename TClassInfoCache::TypeInfoClass_Type *>(pModelTypeInfo));
    return S_OK;
}

STDMETHODIMP CModelTypeInfoFactory::RegisterCommandTypeInfo(_In_ ICommandInfo *pCommandModelInfo)
{
    IFCPTR_ASSERT(pCommandModelInfo);
    return add_to_cache_helper<CCommandModelClassInfoCache>(pCommandModelInfo);
}

STDMETHODIMP CModelTypeInfoFactory::RegisterCollectionTypeInfo(_In_ foundation::IEnumerableTypeInfo *pCollectionModelInfo)
{
    IFCPTR_ASSERT(pCollectionModelInfo);
    return add_to_cache_helper<CCollectionModelClassInfoCache>(pCollectionModelInfo);

}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.TypeInfoFactory"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CModelTypeInfoFactory::GetInstance)
    );
