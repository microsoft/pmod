/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectClassInfoCache.cpp
****/

#include "pch.h"

#include "ObservableObjectClassInfoCache.h"
#include <pmod/errors.h>

#include "PtrSingleton.h"

#include <vector>

using namespace pmod;
using namespace foundation;

const UINT32 _CORE_BASE_PROPERTY_TYPE_ID = foundation::_ReservedTypeId_Base + 0x200;

const ObservableObjectTypeInfo   pmod::_ObservableObjectType(
    _CORE_BASE_PROPERTY_TYPE_ID + 3, 
    IID_IObservableObject, 
    U("ObservableObject"), 
    nullptr, 
    nullptr, 
    nullptr, 
    nullptr);

//////////////////////////////// Private Factory Property Infos ////////////////////////////////////////
static const PropertyInfo _ParentPropertyInfo
(
    Property_Parent,
    U("Parent"),
    PropertyType_Inspectable,
    PropertyFlagType::CanRead,
    &_ObservableObjectType
);

static const PropertyInfo _SourcePropertyInfo
(
    Property_Source,
    U("Source"),
    PropertyType_Inspectable,
    PropertyFlagType::CanRead,
    &_ObservableObjectType
);

static const PropertyInfo _RootPropertyInfo
(
    Property_Root,
    U("Root"),
    PropertyType_Inspectable,
    PropertyFlagType::CanRead,
    &_ObservableObjectType
);

static const PropertyInfo _IsInModelChangesPropertyInfo
(
    Property_IsInModelChanges,
    U("IsInModelChanges"),
    PropertyType_Boolean,
    PropertyFlagType::CanRead,
    nullptr
);
//////////////////////////////// CollectionModel.ObservableObjectInfo ////////////////////////////////////////
const ObservableObjectTypeInfo ObservableCollection::ObservableObjectInfo(
    _CORE_BASE_PROPERTY_TYPE_ID + 4,
    IIDType,
    U("ObservableCollection"),
    PropertyInfos,
    nullptr,
    nullptr,
    nullptr
    );

const PropertyInfo ObservableCollection::PropertyInfos[] = {
    PropertyInfo(Property_Count, U("Count"), PropertyType_UInt32, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(Property_IsEmpty, U("IsEmpty"), PropertyType_Boolean, PropertyFlagType::CanRead, nullptr),
    PropertyInfo()
};

//////////////////////////////// CommandModel.ObservableObjectInfo ////////////////////////////////////////
// {2E44ACCC-AF33-46E4-8EF0-884F6838F89A}
const GUID ObservableCollection::IIDType =
{ 0x2e44accc, 0xaf33, 0x46e4, { 0x8e, 0xf0, 0x88, 0x4f, 0x68, 0x38, 0xf8, 0x9a } };

const ObservableObjectTypeInfo CommandModel::ObservableObjectInfo(
    _CORE_BASE_PROPERTY_TYPE_ID + 5,
    IIDType,
    U("CommandModel"),
    PropertyInfos,
    nullptr,
    nullptr,
    nullptr
    );

const PropertyInfo CommandModel::PropertyInfos[] = {
    PropertyInfo(Property_State, U("State"), PropertyType_UInt32, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(Property_CommandId, U("CommandId"), PropertyType_UInt32, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(Property_IsEnabled, U("IsEnabled"), PropertyType_Boolean, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(),
};

// {7DBEB866-655C-41EA-A2AB-22567379863E}
const GUID CommandModel::IIDType =
{ 0x7dbeb866, 0x655c, 0x41ea, { 0xa2, 0xab, 0x22, 0x56, 0x73, 0x79, 0x86, 0x3e } };

//////////////////////////////// PropertyBinding.ObservableObjectInfo ////////////////////////////////////////
// {3E68F7E6-3F2D-4A82-8729-5A7B1E0A5654}
const GUID PropertyBinding::IIDType =
{ 0x3e68f7e6, 0x3f2d, 0x4a82, { 0x87, 0x29, 0x5a, 0x7b, 0x1e, 0xa, 0x56, 0x54 } };

const ObservableObjectTypeInfo PropertyBinding::ObservableObjectInfo(
    _CORE_BASE_PROPERTY_TYPE_ID + 6,
    IIDType,
    U("PropertyBinding"),
    PropertyInfos,
    nullptr,
    nullptr,
    nullptr
    );

const PropertyInfo PropertyBinding::PropertyInfos[] = {
    PropertyInfo(Property_Value, U("Value"), PropertyType_Inspectable, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(),
};
//////////////////////////////// EnumValue.ObservableObjectInfo ////////////////////////////////////////
// {9A7D5EB1-CF02-46F7-BBC9-14CC66ACEC36}
const GUID EnumValue::IIDType =
{ 0x9a7d5eb1, 0xcf02, 0x46f7, { 0xbb, 0xc9, 0x14, 0xcc, 0x66, 0xac, 0xec, 0x36 } };

const ObservableObjectTypeInfo EnumValue::ObservableObjectInfo(
    _CORE_BASE_PROPERTY_TYPE_ID + 7,
    IIDType,
    U("EnumValue"),
    PropertyInfos,
    nullptr,
    nullptr,
    nullptr
    );

const PropertyInfo EnumValue::PropertyInfos[] = {
    PropertyInfo(Property_Value, U("Value"), PropertyType_UInt32, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(Property_Name, U("Name"), PropertyType_String, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(),
};

//////////////////////////////// CommandViewModel.ObservableObjectInfo ////////////////////////////////////////
const ObservableObjectTypeInfo CommandViewModel::ObservableObjectInfo(
    _CORE_BASE_PROPERTY_TYPE_ID + 8,
    IIDType,
    U("CommandViewModel"),
    PropertyInfos,
    nullptr,
    nullptr,
    &CommandModel::ObservableObjectInfo
    );

const PropertyInfo CommandViewModel::PropertyInfos[] = {
    PropertyInfo(Property_CommandLabel, U("CommandLabel"), PropertyType_String, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(),
};

// {0D7919E7-9BF6-4B29-A70A-2EC9001BD46C}
const GUID CommandViewModel::IIDType =
{ 0xd7919e7, 0x9bf6, 0x4b29, { 0xa7, 0xa, 0x2e, 0xc9, 0x0, 0x1b, 0xd4, 0x6c } };//CPropertyModelClassInfoCache::_TypeModelInfoCacheType& CPropertyModelClassInfoCache::GetInstance()

//////////////////////////////// AsyncOperation.ObservableObjectInfo ////////////////////////////////////////

const ObservableObjectTypeInfo AsyncOperation::ObservableObjectInfo(
    _CORE_BASE_PROPERTY_TYPE_ID + 9,
    IIDType,
    U("AsyncOperation"),
    PropertyInfos,
    nullptr,
    nullptr,
    nullptr
    );

const PropertyInfo AsyncOperation::PropertyInfos[] = {
    PropertyInfo(Property_Result, U("Result"), PropertyType_Inspectable, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(Property_Error, U("Error"), PropertyType_UInt32, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(Property_ResultStatus, U("ResultStatus"), PropertyType_UInt32, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(Property_Progress, U("Progress"), PropertyType_Inspectable, PropertyFlagType::CanRead, nullptr),
    PropertyInfo(),
};

// {9AEED04B-76BC-48B8-8F6B-F57709924EDD}
const GUID AsyncOperation::IIDType =
{ 0x9aeed04b, 0x76bc, 0x48b8, { 0x8f, 0x6b, 0xf5, 0x77, 0x9, 0x92, 0x4e, 0xdd } };

// Internal entry point to early dispose the enum type info
extern void _DiposeEnumTypeClassInfoCacheInternal();

class CPropertyModelClassInfoCache_Type : public CPropertyModelClassInfoCache::_TypeModelInfoCacheType
{
public:
    CPropertyModelClassInfoCache_Type()
    {
        IGNOREHR(RegisterPropertyModelTypeInfoInternal(&_ObservableObjectType));

        IGNOREHR(RegisterPropertyModelTypeInfoInternal(&ObservableCollection::ObservableObjectInfo));
        IGNOREHR(RegisterPropertyModelTypeInfoInternal(&CommandModel::ObservableObjectInfo));
        IGNOREHR(RegisterPropertyModelTypeInfoInternal(&PropertyBinding::ObservableObjectInfo));
        IGNOREHR(RegisterPropertyModelTypeInfoInternal(&EnumValue::ObservableObjectInfo));
        IGNOREHR(RegisterPropertyModelTypeInfoInternal(&AsyncOperation::ObservableObjectInfo));

        IGNOREHR(RegisterPropertyModelTypeInfoInternal(&CommandViewModel::ObservableObjectInfo));
    }

    ~CPropertyModelClassInfoCache_Type()
    {
        // Note: we need to early dispose the Enum Type Info since it could hold pmod model object that will keep reference
        // into our cached type infos and so cause assert when release type infos later
        _DiposeEnumTypeClassInfoCacheInternal();
    }

    HRESULT RegisterPropertyModelTypeInfoInternal(_In_ const ObservableObjectTypeInfo *pPropertyModelTypeInfo)
    {
        CObservableObjectInfo         *pBaseType = nullptr;
        if (pPropertyModelTypeInfo->m_base_type)
        {
            pBaseType = LookupType(pPropertyModelTypeInfo->m_base_type->get_iid_type());
        }
        CPropertyModelInfoClass *pPropertyModelInfoClass = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CPropertyModelInfoClass>::CreateInstance(&pPropertyModelInfoClass));
        IFR_ASSERT(pPropertyModelInfoClass->Initialize(pPropertyModelTypeInfo, pBaseType));

        this->AddToCache(pPropertyModelTypeInfo->get_iid_type(), pPropertyModelInfoClass);
        return S_OK;
    }
};

CPropertyModelClassInfoCache::_TypeModelInfoCacheType& CPropertyModelClassInfoCache::GetInstance()
{
	return *_GetPtrInstance<CPropertyModelClassInfoCache_Type>();
}

HRESULT CPropertyModelClassInfoCache::GetOrCreatePropertyModelInfo(
        _In_ const ObservableObjectTypeInfo *pmi,
        _Outptr_ CObservableObjectInfo **ppPropertyModelInfo)
{
    foundation_assert(pmi);

    CObservableObjectInfo *pPropertyModelInfo = LookupType(pmi->get_iid_type());
    if(pPropertyModelInfo == nullptr)
    {
        IFR_ASSERT(CreatePropertyModelInfoInternal(pmi,&pPropertyModelInfo));
    }
    foundation_assert(pPropertyModelInfo);
    *ppPropertyModelInfo = pPropertyModelInfo;
    return S_OK;
}

HRESULT CPropertyModelClassInfoCache::RegisterPropertyModelInfo(_In_ const ObservableObjectTypeInfo *pmi)
{
    foundation_assert(pmi);
    CObservableObjectInfo *pPropertyModelInfo = nullptr;
    // the next call may create multiple Type Info
    IFR_ASSERT(GetOrCreatePropertyModelInfo(pmi, &pPropertyModelInfo));
    return S_OK;
}


HRESULT CPropertyModelClassInfoCache::CreatePropertyModelInfoInternal(
    _In_ const ObservableObjectTypeInfo *pmi,
    _Outptr_ CObservableObjectInfo **pValue)
{
    foundation_assert(pmi);

    std::vector<const ObservableObjectTypeInfo *> modelTypeHierarchy;

    // Next block will create the Property Model Info Hierarchy
    while(pmi)
    {
        modelTypeHierarchy. insert(modelTypeHierarchy.begin(),pmi);
        pmi = static_cast<const ObservableObjectTypeInfo *>(pmi->m_base_type);
    }
    // Now create if necesary but using a Base Model Type
    CObservableObjectInfo *pBaseModelType = nullptr;
    for(std::vector<const ObservableObjectTypeInfo *>::const_iterator iter = modelTypeHierarchy.begin();
        iter != modelTypeHierarchy.end();
        ++iter)
    {
        pmi = (*iter);
        CObservableObjectInfo *pModelType = LookupType(pmi->get_iid_type());
        if(pModelType == nullptr)
        {
            IFR_ASSERT(CreatePropertyModelInfoInternal(pmi, pBaseModelType, &pModelType));
        }
        pBaseModelType = pModelType;
    }
    *pValue = pBaseModelType;
    return S_OK;
}

HRESULT CPropertyModelClassInfoCache::CreatePropertyModelInfoInternal(
    _In_ const ObservableObjectTypeInfo *pPropertyModelInfo,
    _In_ CObservableObjectInfo *pBaseType,
    _Outptr_ CObservableObjectInfo **pValue)
{
    if (pPropertyModelInfo->m_type == TypeInfo_PropertyModel)
    {
        CPropertyModelInfoClass *pPropertyModelInfoClass = nullptr;
        IFR_ASSERT(CPropertyModelInfoClass::CreateInstance(pPropertyModelInfo, pBaseType, &pPropertyModelInfoClass));
        foundation_assert(pPropertyModelInfoClass);
        *pValue  = static_cast<CObservableObjectInfo *>(pPropertyModelInfoClass);
    }
    else if (pPropertyModelInfo->m_type == TypeInfo_PropertyViewModel)
    {
        CPropertyViewModelInfoClass *pPropertyViewModelInfoClass = nullptr;
        IFR_ASSERT(CPropertyViewModelInfoClass::CreateInstance(
            static_cast<const ViewModelTypeInfo *>(pPropertyModelInfo),
            pBaseType,
            &pPropertyViewModelInfoClass));
        foundation_assert(pPropertyViewModelInfoClass);
        *pValue  = static_cast<CObservableObjectInfo *>(pPropertyViewModelInfoClass);
    }
    else
    {
        // Unexpected Type passed
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    // Insert here intrinsic properties
    AddInstrinsicProperties(*pValue, pPropertyModelInfo->m_type == TypeInfo_PropertyViewModel);
    return S_OK;
}

HRESULT CPropertyModelClassInfoCache::AddInstrinsicProperties(
    _In_ CObservableObjectInfo *pPropertyModelInfo,
    _In_ bool isPropertyViewModel
    )
{
    // Insert here intrinsic properties
    IFR_ASSERT(pPropertyModelInfo->AddPropertyInfo(&_ParentPropertyInfo));
    IFR_ASSERT(pPropertyModelInfo->AddPropertyInfo(&_RootPropertyInfo));
    IFR_ASSERT(pPropertyModelInfo->AddPropertyInfo(&_IsInModelChangesPropertyInfo));

    if(isPropertyViewModel)
    {
        // Insert the supported 'Source' property
        IFR_ASSERT(pPropertyModelInfo->AddPropertyInfo(&_SourcePropertyInfo));
    }
    return S_OK;
}

HRESULT CPropertyModelClassInfoCache::RegisterPropertyModelInfo(
    _In_ CPropertyModelInfoClass *pPropertyModelInfo,
    _In_ bool isViewModel)
{
    foundation_assert(pPropertyModelInfo);

    IFR_ASSERT(AddInstrinsicProperties(pPropertyModelInfo, isViewModel));

    IID iidType;
    IFR_ASSERT(pPropertyModelInfo->GetType(&iidType));
    AddToCache(iidType,pPropertyModelInfo);
    return S_OK;
}


class CPropertyModelClassInfoCacheProvider :
    public ITypeInfoCacheProvider
{

};
