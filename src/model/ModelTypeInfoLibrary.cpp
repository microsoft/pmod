/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelTypeInfoLibrary.cpp
****/
#include "pch.h"

#include "ObservableObjectClassInfoCache.h"
#include "CommandClassInfoCache.h"
#include "ObservableCollectionClassInfoCache.h"
#include "EnumTypeClassInfoCache.h"

// from foundation_library
#include "FoundationLibraryInstance.h"

#include <foundation/library/dispatcher_callback_adapter.h>

using namespace pmod;
using namespace foundation;

// from __codegen
extern HRESULT  STDAPICALLTYPE register_core_errors_type_info(void);
#if defined(DEBUG)
extern HRESULT  STDAPICALLTYPE register_perf_logger_type_info(void);
#endif

typedef CStaticTypeInfoCache<CObservableObjectInfo, TStaticCast> CPropertyViewModelClassInfoCache;
typedef CStaticTypeInfoCache<CCollectionModelTypeInfo> CListModelClassInfoCache;

template <class T, const UINT32 _type_info_type, class TypeInfoClass_Create, class type_info_type >
class CObservableObjectTypeInfoCacheProvider_Base : public
    CTypeInfoCacheProvider_Base<T, _type_info_type, TypeInfoClass_Create, TQICast>
{
    // interface ITypeInfoCacheProvider
    HRESULT CreateTypeInfo(
        _In_ const foundation::TypeInfo *pTypeInfo,
        _Outptr_ foundation::IObjectTypeInfo **ppTypeInfo)
    {
        const ObservableObjectTypeInfo *p_type_info = reinterpret_cast<const ObservableObjectTypeInfo *>(pTypeInfo);

        CObservableObjectInfo *pObservableObjectInfo = nullptr;
        IFR_ASSERT(CPropertyModelClassInfoCache::GetOrCreatePropertyModelInfo(p_type_info, &pObservableObjectInfo));
        IFR_ASSERT(foundation::QueryWeakReference(pObservableObjectInfo, ppTypeInfo));
        return S_OK;
    }

    HRESULT InitializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pCreatedTypeInfo,
        _In_ const foundation::TypeInfo *pTypeInfo
        )
    {
        foundation_assert(pCreatedTypeInfo);

        HRESULT hr;

        const ObservableObjectTypeInfo *p_type_info = reinterpret_cast<const ObservableObjectTypeInfo *>(pTypeInfo);
        CObservableObjectInfo *pBaseType = nullptr;
        if (p_type_info->m_base_type != nullptr)
        {
            foundation::IObjectTypeInfo *pBaseTypeInfo = nullptr;
            CTypeInfoCache::LookupTypeInfo(
                p_type_info->m_base_type->get_iid_type(),
                &pBaseTypeInfo);
            foundation_assert(pBaseTypeInfo != nullptr);
            foundation::QueryWeakReference(pBaseTypeInfo, &pBaseType);
            foundation_assert(pBaseType != nullptr);
        }
        bool isViewModel = (pTypeInfo->m_type == TypeInfo_PropertyViewModel);
        TypeInfoClass_Create *pPropertyModelTypeInfoClass = static_cast<TypeInfoClass_Create *>(pCreatedTypeInfo);

        IFHR_ASSERT(pPropertyModelTypeInfoClass->Initialize(
            reinterpret_cast<const type_info_type *>(pTypeInfo),
            pBaseType
            ));
        IFHR_ASSERT(CPropertyModelClassInfoCache::AddInstrinsicProperties(pPropertyModelTypeInfoClass, isViewModel));

        return S_OK;
    }

    HRESULT InitializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pCreatedTypeInfo,
        const foundation::DictionaryPtr& type_info_ptr
        ) override
    {
        TypeInfoClass_Create *pPropertyModelTypeInfoClass = static_cast<TypeInfoClass_Create *>(pCreatedTypeInfo);
        IFR_ASSERT(pPropertyModelTypeInfoClass->_DeserializeTypeInfo(type_info_ptr));
        return S_OK;
    }

    HRESULT SerializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pTypeInfo,
        foundation::DictionaryPtr& type_info_ptr
        ) override
    {
        TypeInfoClass_Create *pPropertyModelTypeInfoClass = static_cast<TypeInfoClass_Create *>(pTypeInfo);
        IFR_ASSERT(pPropertyModelTypeInfoClass->_SerializeTypeInfo(type_info_ptr));
        return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   _ModelTypeInfoInitializer
//
// Purpose: Static Initializer for the Core Type Info Library
//
//------------------------------------------------------------------------------
struct _ModelTypeInfoInitializer
{
    _ModelTypeInfoInitializer();

    CTypeInfoCacheProvider<CCommandModelClassInfoCache, TypeInfo_CommandModel> _CommandModelClassInfoCacheProvider;
    CTypeInfoCacheProvider<CCollectionModelClassInfoCache, TypeInfo_Collection> _CollectionModelClassInfoCacheProvider;
    CTypeInfoCacheProvider<CListModelClassInfoCache, TypeInfo_List> _ListModelClassInfoCacheProvider;

    CObservableObjectTypeInfoCacheProvider_Base<CPropertyModelClassInfoCache, TypeInfo_PropertyModel, CPropertyModelInfoClass, ObservableObjectTypeInfo> _PropertyModelInfoClassInfoCacheProvider;
    CObservableObjectTypeInfoCacheProvider_Base<CPropertyViewModelClassInfoCache, TypeInfo_PropertyViewModel, CPropertyViewModelInfoClass, ViewModelTypeInfo> _PropertyViewModelInfoClassInfoCacheProvider;
};

static _ModelTypeInfoInitializer _coreTypeInfoInitializer;

_ModelTypeInfoInitializer::_ModelTypeInfoInitializer()
{
    // Initialize Foundation Library Instance if needed
    CFoundationLibraryInstance::_Initialize();

    ComPtr<foundation::library::IDelegateDispatchable> spCallback;

    // register_core_errors_type_info initialize
    DispatcherCallbackAdapter::CreateInstance(&register_core_errors_type_info, spCallback.ReleaseAndGetAddressOf());
    CEnumTypeClassInfoCache::register_enum_factory_type_info(spCallback);

#if defined(DEBUG)
    // register_core_errors_type_info initialize
    DispatcherCallbackAdapter::CreateInstance(&register_perf_logger_type_info, spCallback.ReleaseAndGetAddressOf());
    CEnumTypeClassInfoCache::register_enum_factory_type_info(spCallback);
#endif

}

EXTERN_C void _ModelTypeInfoInitializerStub()
{
    // empty stub to force this module to be linked when a static lib is being used
}
