/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionClassInfoCache.cpp
****/

#include "pch.h"

#include "ObservableCollectionClassInfoCache.h"

// from public
#include <pmod/library/type_adapter_base.h>

#include "PtrSingleton.h"

using namespace pmod;
using namespace pmod::library;

// define Factory type adapters
typedef _ObservableCollectionAdapterBase<HSTRING,IObservableCollection_String,&IID_IObservableCollection_String,foundation::_PropertyValueConverter > CObservableCollection_String_Adapter;
typedef _ObservableCollectionAdapterBase<INT32, IObservableCollection_Int32, &IID_IObservableCollection_Int32, foundation::_PropertyValueConverter > CObservableCollection_Int32_Adapter;
typedef _ObservableCollectionAdapterBase<UINT32, IObservableCollection_UInt32, &IID_IObservableCollection_UInt32, foundation::_PropertyValueConverter > CObservableCollection_UInt32_Adapter;
typedef _ObservableCollectionAdapterBase<INT16, IObservableCollection_Int16, &IID_IObservableCollection_Int16, foundation::_PropertyValueConverter > CObservableCollection_Int16_Adapter;
typedef _ObservableCollectionAdapterBase<UINT16, IObservableCollection_UInt16, &IID_IObservableCollection_UInt16, foundation::_PropertyValueConverter > CObservableCollection_UInt16_Adapter;
typedef _ObservableCollectionAdapterBase<BYTE, IObservableCollection_UInt8, &IID_IObservableCollection_UInt8, foundation::_PropertyValueConverter > CObservableCollection_Byte_Adapter;
typedef _ObservableCollectionAdapterBase<bool, IObservableCollection_Boolean, &IID_IObservableCollection_Boolean, foundation::_PropertyValueConverter > CObservableCollection_Boolean_Adapter;
typedef _ObservableCollectionAdapterBase<INT64, IObservableCollection_Int64, &IID_IObservableCollection_Int64, foundation::_PropertyValueConverter > CObservableCollection_Int64_Adapter;
typedef _ObservableCollectionAdapterBase<UINT64, IObservableCollection_UInt64, &IID_IObservableCollection_UInt64, foundation::_PropertyValueConverter > CObservableCollection_UInt64_Adapter;

const UINT32 _CORE_BASE_COLLECTION_TYPE_ID = foundation::_ReservedTypeId_Base + 0x100;

//////////////////////////////// ObservableCollection_String ////////////////////////////////////////
// {D954B531-46FC-46B1-BBC5-C34A8A3DA3AD}
const GUID pmod::IID_IObservableCollection_String =
{ 0xd954b531, 0x46fc, 0x46b1, { 0xbb, 0xc5, 0xc3, 0x4a, 0x8a, 0x3d, 0xa3, 0xad } };
const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_String(
    _CORE_BASE_COLLECTION_TYPE_ID + 1,
    IID_IObservableCollection_String,
    U("ObservableCollection_String"),
    &foundation::_StringTypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_Int32 ////////////////////////////////////////
// {693E3BA1-26B3-4189-8FF2-D3760CDC0756}
const GUID pmod::IID_IObservableCollection_Int32 =
{ 0x693e3ba1, 0x26b3, 0x4189, { 0x8f, 0xf2, 0xd3, 0x76, 0xc, 0xdc, 0x7, 0x56 } };
const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_Int32(
    _CORE_BASE_COLLECTION_TYPE_ID + 2,
    IID_IObservableCollection_Int32,
    U("ObservableCollection_Int32"),
    &foundation::_Int32TypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_UInt32 ////////////////////////////////////////
// {A4E26C12-139A-4C10-806C-CF440371D363}
const GUID pmod::IID_IObservableCollection_UInt32 =
{ 0xa4e26c12, 0x139a, 0x4c10, { 0x80, 0x6c, 0xcf, 0x44, 0x3, 0x71, 0xd3, 0x63 } };
const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_UInt32(
    _CORE_BASE_COLLECTION_TYPE_ID + 3,
    IID_IObservableCollection_UInt32,
    U("ObservableCollection_UInt32"),
    &foundation::_UInt32TypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_Int64 ////////////////////////////////////////
// {1490D47E-2A09-49B1-B95B-6E7A1BE66A1A}
const GUID pmod::IID_IObservableCollection_Int16 =
{ 0x1490d47e, 0x2a09, 0x49b1, { 0xb9, 0x5b, 0x6e, 0x7a, 0x1b, 0xe6, 0x6a, 0x1a } };

const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_Int64(
    _CORE_BASE_COLLECTION_TYPE_ID + 4,
    IID_IObservableCollection_Int16,
    U("ObservableCollection_Int64"),
    &foundation::_Int64TypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_UInt64 ////////////////////////////////////////
// {B07252DD-4FF3-4070-A710-C81D00437621}
const GUID pmod::IID_IObservableCollection_UInt16 =
{ 0xb07252dd, 0x4ff3, 0x4070, { 0xa7, 0x10, 0xc8, 0x1d, 0x0, 0x43, 0x76, 0x21 } };

const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_UInt64(
    _CORE_BASE_COLLECTION_TYPE_ID + 5,
    IID_IObservableCollection_UInt16,
    U("ObservableCollection_UInt64"),
    &foundation::_UInt64TypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_Int16 ////////////////////////////////////////
// {D5655172-F824-4B67-8FA5-E3F07E0EBBAE}
const GUID pmod::IID_IObservableCollection_Int64 =
{ 0xd5655172, 0xf824, 0x4b67, { 0x8f, 0xa5, 0xe3, 0xf0, 0x7e, 0xe, 0xbb, 0xae } };

const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_Int16(
    _CORE_BASE_COLLECTION_TYPE_ID + 6,
    IID_IObservableCollection_Int64,
    U("ObservableCollection_Int16"),
    &foundation::_Int16TypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_UInt16 ////////////////////////////////////////
// {66E06332-F610-44F3-AB9D-D806C33062DF}
const GUID pmod::IID_IObservableCollection_UInt64 =
{ 0x66e06332, 0xf610, 0x44f3, { 0xab, 0x9d, 0xd8, 0x6, 0xc3, 0x30, 0x62, 0xdf } };

const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_UInt16(
    _CORE_BASE_COLLECTION_TYPE_ID + 7,
    IID_IObservableCollection_UInt64,
    U("ObservableCollection_UInt16"),
    &foundation::_UInt16TypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_UInt8 ////////////////////////////////////////
// {27F81E39-9EFD-40B1-8A27-710E3AFC7E72}
const GUID pmod::IID_IObservableCollection_UInt8 =
{ 0x27f81e39, 0x9efd, 0x40b1, { 0x8a, 0x27, 0x71, 0xe, 0x3a, 0xfc, 0x7e, 0x72 } };

const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_UInt8(
    _CORE_BASE_COLLECTION_TYPE_ID + 8,
    IID_IObservableCollection_UInt8,
    U("ObservableCollection_UInt8"),
    &foundation::_UInt8TypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_DateTime ////////////////////////////////////////
// {28FEE3E8-8A85-426C-8F19-D5A63F7F5D57}
const GUID pmod::IID_IObservableCollection_DateTime =
{ 0x28fee3e8, 0x8a85, 0x426c, { 0x8f, 0x19, 0xd5, 0xa6, 0x3f, 0x7f, 0x5d, 0x57 } };

const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_DateTime(
    _CORE_BASE_COLLECTION_TYPE_ID + 9,
    IID_IObservableCollection_DateTime,
    U("ObservableCollection_DateTime"),
    &foundation::_DateTimeTypeInfo,
    TypeInfo_Collection);

//////////////////////////////// ObservableCollection_Bool ////////////////////////////////////////
// {11375EA1-3F71-4910-9795-CB5D066BA59C}
const GUID pmod::IID_IObservableCollection_Boolean =
{ 0x11375ea1, 0x3f71, 0x4910, { 0x97, 0x95, 0xcb, 0x5d, 0x6, 0x6b, 0xa5, 0x9c } };

const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_Boolean(
    _CORE_BASE_COLLECTION_TYPE_ID + 10,
    IID_IObservableCollection_Boolean,
    U("ObservableCollection_Boolean"),
    &foundation::_BoolTypeInfo,
    TypeInfo_Collection);

//////////////////////////////// CollectionModelInfo_Object ////////////////////////////////////////
const foundation::EnumerableTypeInfo pmod::_CollectionTypeInfo_Inspectable(
    _CORE_BASE_COLLECTION_TYPE_ID + 11,
    IID_IObservableCollection,
    U("ObservableCollection_Object"),
    &foundation::_InspectableTypeInfo,
    TypeInfo_Collection);

class CCollectionModelClassInfoCache_Type : public CCollectionModelClassInfoCache::_TypeModelInfoCacheType
{
public:
    CCollectionModelClassInfoCache_Type()
    {
        const foundation::EnumerableTypeInfo *factoryModelInfos[] = {
            &_CollectionTypeInfo_String,
            &_CollectionTypeInfo_Int32,
            &_CollectionTypeInfo_UInt32,
            &_CollectionTypeInfo_Int16,
            &_CollectionTypeInfo_UInt16,
            &_CollectionTypeInfo_UInt8,
            &_CollectionTypeInfo_DateTime,
            &_CollectionTypeInfo_Boolean,
            &_CollectionTypeInfo_Int64,
            &_CollectionTypeInfo_UInt64,
            &_CollectionTypeInfo_Inspectable,
        };
        for (int index = 0;
            index < sizeof(factoryModelInfos) / sizeof(const foundation::EnumerableTypeInfo *);
            index++)
        {
            IGNOREHR(RegisterCollectionModelTypeInfoInternal(factoryModelInfos[index]));
        }

        const foundation::library::_TypeAdapterEntry _modelFactoryEntries[] = {
            // IObservableCollection factory interfaces
            { IID_IObservableCollection_Boolean, IID_IObservableCollection_Boolean, foundation::CreateAdapter<CObservableCollection_Boolean_Adapter> },
            { IID_IObservableCollection_String, IID_IObservableCollection_String, foundation::CreateAdapter<CObservableCollection_String_Adapter> },
            { IID_IObservableCollection_Int32, IID_IObservableCollection_Int32, foundation::CreateAdapter<CObservableCollection_Int32_Adapter> },
            { IID_IObservableCollection_UInt32, IID_IObservableCollection_UInt32, foundation::CreateAdapter<CObservableCollection_UInt32_Adapter> },
            { IID_IObservableCollection_Int16, IID_IObservableCollection_Int16, foundation::CreateAdapter<CObservableCollection_Int16_Adapter> },
            { IID_IObservableCollection_UInt16, IID_IObservableCollection_UInt16, foundation::CreateAdapter<CObservableCollection_UInt16_Adapter> },
            { IID_IObservableCollection_Int64, IID_IObservableCollection_Int64, foundation::CreateAdapter<CObservableCollection_Int64_Adapter> },
            { IID_IObservableCollection_UInt64, IID_IObservableCollection_UInt64, foundation::CreateAdapter<CObservableCollection_UInt64_Adapter> },

            { foundation_GUID_NULL, foundation_GUID_NULL, 0L }
        };
        foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spModelAdapterFactory;
        foundation::library::CreateTypeAdapterFactory(_modelFactoryEntries, spModelAdapterFactory.GetAddressOf());
        foundation::library::RegisterInterfaceAdapterFactory(spModelAdapterFactory);

    }
private:
    HRESULT RegisterCollectionModelTypeInfoInternal(_In_ const foundation::EnumerableTypeInfo *pCollectionModelTypeInfo)
    {
        CCollectionModelTypeInfo *pCollectionModelTypeInfoClass = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CCollectionModelTypeInfo>::CreateInstance(&pCollectionModelTypeInfoClass));
        IFR_ASSERT(pCollectionModelTypeInfoClass->_Initialize(pCollectionModelTypeInfo));

        this->AddToCache(pCollectionModelTypeInfo->get_iid_type(), pCollectionModelTypeInfoClass);
        return S_OK;
    }
};

CCollectionModelClassInfoCache::_TypeModelInfoCacheType& CCollectionModelClassInfoCache::GetInstance()
{
	return *_GetPtrInstance<CCollectionModelClassInfoCache_Type>();
}

