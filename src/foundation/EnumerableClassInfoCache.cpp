/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumerableClassInfoCache.cpp
****/

#include "pch.h"

#include "EnumerableClassInfoCache.h"

//// from public
#include <foundation//library/foundation_adapter_base.h>
#
#include "PtrSingleton.h"

using namespace foundation;
using namespace foundation::library;

// define Factory type adapters
typedef _EnumerableAdapterBase<HSTRING, IEnumerable_String,&IID_IEnumerable_String, _PropertyValueConverter > CEnumerable_String_Adapter;
typedef _EnumerableAdapterBase<INT32, IEnumerable_Int32, &IID_IEnumerable_Int32, _PropertyValueConverter > CEnumerable_Int32_Adapter;
typedef _EnumerableAdapterBase<UINT32, IEnumerable_UInt32, &IID_IEnumerable_UInt32, _PropertyValueConverter > CEnumerable_UInt32_Adapter;
typedef _EnumerableAdapterBase<INT16, IEnumerable_Int16, &IID_IEnumerable_Int16, _PropertyValueConverter > CEnumerable_Int16_Adapter;
typedef _EnumerableAdapterBase<UINT16, IEnumerable_UInt16, &IID_IEnumerable_UInt16, _PropertyValueConverter > CEnumerable_UInt16_Adapter;
typedef _EnumerableAdapterBase<BYTE, IEnumerable_UInt8, &IID_IEnumerable_UInt8, _PropertyValueConverter > CEnumerable_Byte_Adapter;
typedef _EnumerableAdapterBase<bool, IEnumerable_Boolean, &IID_IEnumerable_Boolean, _PropertyValueConverter > CEnumerable_Boolean_Adapter;
typedef _EnumerableAdapterBase<INT64, IEnumerable_Int64, &IID_IEnumerable_Int64, _PropertyValueConverter > CEnumerable_Int64_Adapter;
typedef _EnumerableAdapterBase<UINT64, IEnumerable_UInt64, &IID_IEnumerable_UInt64, _PropertyValueConverter > CEnumerable_UInt64_Adapter;

const UINT32 _FOUNDATION_BASE_ENUMERABLE_TYPE_ID = foundation::_ReservedTypeId_Base + 0x50;

//////////////////////////////// Enumerable_String ////////////////////////////////////////
// {50DB27A0-8863-4C31-89E5-E6A6EE520951}
const GUID foundation::IID_IEnumerable_String =
{ 0x50db27a0, 0x8863, 0x4c31,{ 0x89, 0xe5, 0xe6, 0xa6, 0xee, 0x52, 0x9, 0x51 } };
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_String(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 1,
    IID_IEnumerable_String,
    U("Enumerable_String"),
    &foundation::_StringTypeInfo);

//////////////////////////////// Enumerable_Int32 ////////////////////////////////////////
// {877C9C7E-EDA6-4EA0-A6B7-81B43A8F29D9}
const GUID foundation::IID_IEnumerable_Int32 =
{ 0x877c9c7e, 0xeda6, 0x4ea0,{ 0xa6, 0xb7, 0x81, 0xb4, 0x3a, 0x8f, 0x29, 0xd9 } };
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_Int32(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 2,
    IID_IEnumerable_Int32,
    U("Enumerable_Int32"),
    &foundation::_Int32TypeInfo);

//////////////////////////////// Enumerable_UInt32 ////////////////////////////////////////
// {DF637815-6A7A-4FA3-9B65-53B25697495E}
const GUID foundation::IID_IEnumerable_UInt32 =
{ 0xdf637815, 0x6a7a, 0x4fa3,{ 0x9b, 0x65, 0x53, 0xb2, 0x56, 0x97, 0x49, 0x5e } };
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_UInt32(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 3,
    IID_IEnumerable_UInt32,
    U("Enumerable_UInt32"),
    &foundation::_UInt32TypeInfo);

//////////////////////////////// Enumerable_Int64 ////////////////////////////////////////
// {1490D47E-2A09-49B1-B95B-6E7A1BE66A1A}
const GUID foundation::IID_IEnumerable_Int16 =
{ 0x1490d47e, 0x2a09, 0x49b1, { 0xb9, 0x5b, 0x6e, 0x7a, 0x1b, 0xe6, 0x6a, 0x1a } };

const EnumerableTypeInfo foundation::_EnumerableTypeInfo_Int64(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 4,
    IID_IEnumerable_Int16,
    U("Enumerable_Int64"),
    &foundation::_Int64TypeInfo);

//////////////////////////////// Enumerable_UInt64 ////////////////////////////////////////
// {A131111C-CA9D-401D-9A18-4E26C9C271E3}
const GUID foundation::IID_IEnumerable_UInt16 =
{ 0xa131111c, 0xca9d, 0x401d,{ 0x9a, 0x18, 0x4e, 0x26, 0xc9, 0xc2, 0x71, 0xe3 } };
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_UInt64(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 5,
    IID_IEnumerable_UInt16,
    U("Enumerable_UInt64"),
    &foundation::_UInt64TypeInfo);

//////////////////////////////// Enumerable_Int16 ////////////////////////////////////////
// {C534227D-33D0-4E14-AD85-80E036B22CB8}
const GUID foundation::IID_IEnumerable_Int64 =
{ 0xc534227d, 0x33d0, 0x4e14,{ 0xad, 0x85, 0x80, 0xe0, 0x36, 0xb2, 0x2c, 0xb8 } };
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_Int16(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 6,
    IID_IEnumerable_Int64,
    U("Enumerable_Int16"),
    &foundation::_Int16TypeInfo);

//////////////////////////////// Enumerable_UInt16 ////////////////////////////////////////
// {0D7B33A6-18BE-47AD-8A79-0C08684A1941}
const GUID foundation::IID_IEnumerable_UInt64 =
{ 0xd7b33a6, 0x18be, 0x47ad,{ 0x8a, 0x79, 0xc, 0x8, 0x68, 0x4a, 0x19, 0x41 } };
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_UInt16(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 7,
    IID_IEnumerable_UInt64,
    U("Enumerable_UInt16"),
    &foundation::_UInt16TypeInfo);

//////////////////////////////// Enumerable_UInt8 ////////////////////////////////////////
// {15597D92-05F9-4BB6-B9D9-6015D5764F17}
const GUID foundation::IID_IEnumerable_UInt8 =
{ 0x15597d92, 0x5f9, 0x4bb6,{ 0xb9, 0xd9, 0x60, 0x15, 0xd5, 0x76, 0x4f, 0x17 } };
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_UInt8(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 8,
    IID_IEnumerable_UInt8,
    U("Enumerable_UInt8"),
    &foundation::_UInt8TypeInfo);

//////////////////////////////// Enumerable_DateTime ////////////////////////////////////////
// {2BFC40EB-8049-4C54-84C3-C81960E4C7AA}
const GUID foundation::IID_IEnumerable_DateTime =
{ 0x2bfc40eb, 0x8049, 0x4c54,{ 0x84, 0xc3, 0xc8, 0x19, 0x60, 0xe4, 0xc7, 0xaa } };

const EnumerableTypeInfo foundation::_EnumerableTypeInfo_DateTime(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 9,
    IID_IEnumerable_DateTime,
    U("Enumerable_DateTime"),
    &foundation::_DateTimeTypeInfo);

//////////////////////////////// Enumerable_Bool ////////////////////////////////////////
// {DBB380C1-D4E1-406A-8247-BD28A2392C72}
const GUID foundation::IID_IEnumerable_Boolean =
{ 0xdbb380c1, 0xd4e1, 0x406a,{ 0x82, 0x47, 0xbd, 0x28, 0xa2, 0x39, 0x2c, 0x72 } };
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_Boolean(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 10,
    IID_IEnumerable_Boolean,
    U("Enumerable_Boolean"),
    &foundation::_BoolTypeInfo);

//////////////////////////////// CollectionModelInfo_Object ////////////////////////////////////////
const EnumerableTypeInfo foundation::_EnumerableTypeInfo_Inspectable(
    _FOUNDATION_BASE_ENUMERABLE_TYPE_ID + 11,
    IID_IEnumerable,
    U("Enumerable_Object"),
    &foundation::_InspectableTypeInfo);

class CEnumerableClassInfoCache_Type : public CEnumerableClassInfoCache::_TypeModelInfoCacheType
{
public:
    CEnumerableClassInfoCache_Type()
    {
        const EnumerableTypeInfo *factoryModelInfos[] = {
            &_EnumerableTypeInfo_String,
            &_EnumerableTypeInfo_Int32,
            &_EnumerableTypeInfo_UInt32,
            &_EnumerableTypeInfo_Int16,
            &_EnumerableTypeInfo_UInt16,
            &_EnumerableTypeInfo_UInt8,
            &_EnumerableTypeInfo_DateTime,
            &_EnumerableTypeInfo_Boolean,
            &_EnumerableTypeInfo_Int64,
            &_EnumerableTypeInfo_UInt64,
            &_EnumerableTypeInfo_Inspectable,
        };
        for (int index = 0;
            index < sizeof(factoryModelInfos) / sizeof(const EnumerableTypeInfo *);
            index++)
        {
            IGNOREHR(RegisterEnumerableTypeInfoInternal(factoryModelInfos[index]));
        }

        const foundation::library::_TypeAdapterEntry _modelFactoryEntries[] = {
            { IID_IEnumerable_Boolean, IID_IEnumerable_Boolean, CreateAdapter<CEnumerable_Boolean_Adapter> },
            { IID_IEnumerable_String, IID_IEnumerable_String, CreateAdapter<CEnumerable_String_Adapter> },
            { IID_IEnumerable_Int32, IID_IEnumerable_Int32, CreateAdapter<CEnumerable_Int32_Adapter> },
            { IID_IEnumerable_UInt32, IID_IEnumerable_UInt32, CreateAdapter<CEnumerable_UInt32_Adapter> },
            { IID_IEnumerable_Int16, IID_IEnumerable_Int16, CreateAdapter<CEnumerable_Int16_Adapter> },
            { IID_IEnumerable_UInt16, IID_IEnumerable_UInt16, CreateAdapter<CEnumerable_UInt16_Adapter> },
            { IID_IEnumerable_Int64, IID_IEnumerable_Int64, CreateAdapter<CEnumerable_Int64_Adapter> },
            { IID_IEnumerable_UInt64, IID_IEnumerable_UInt64, CreateAdapter<CEnumerable_UInt64_Adapter> },
            { foundation_GUID_NULL, foundation_GUID_NULL, 0L }
        };
        foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spModelAdapterFactory;
        foundation::library::CreateTypeAdapterFactory(_modelFactoryEntries, spModelAdapterFactory.GetAddressOf());
        foundation::library::RegisterInterfaceAdapterFactory(spModelAdapterFactory);

    }
private:
    HRESULT RegisterEnumerableTypeInfoInternal(_In_ const EnumerableTypeInfo *pEnumerableTypeInfo)
    {
        CEnumerableTypeInfo *pEnumerableTypeInfoClass = nullptr;
        IFR_ASSERT(ctl::ComInspectableObject<CEnumerableTypeInfo>::CreateInstance(&pEnumerableTypeInfoClass));
        IFR_ASSERT(pEnumerableTypeInfoClass->_Initialize(pEnumerableTypeInfo));

        this->AddToCache(pEnumerableTypeInfo->get_iid_type(), pEnumerableTypeInfoClass);
        return S_OK;
    }
};

CEnumerableClassInfoCache::_TypeModelInfoCacheType& CEnumerableClassInfoCache::GetInstance()
{
	return *_GetPtrInstance<CEnumerableClassInfoCache_Type>();
}

