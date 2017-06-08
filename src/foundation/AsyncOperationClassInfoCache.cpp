/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationClassInfoCache.cpp
****/

#include "pch.h"

#include "AsyncOperationClassInfoCache.h"
#include "GenericAdapterFactory.h"

#include "PtrSingleton.h"

#include <foundation/library/foundation_adapter_base.h>

using namespace foundation;

const UINT32 _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID = foundation::_ReservedTypeId_Base + 10;
//////////////////////////////// AsyncOperation_String ////////////////////////////////////////
// {75B864ED-5C10-41AD-BF72-C6CDED0EABF8}
const GUID foundation::IID_IAsyncOperation_String =
{ 0x75b864ed, 0x5c10, 0x41ad, { 0xbf, 0x72, 0xc6, 0xcd, 0xed, 0xe, 0xab, 0xf8 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_String(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 1,
    IID_IAsyncOperation_String,
    U("AsyncOperation_String"),
    &_StringTypeInfo);

//////////////////////////////// AsyncOperation_Int32 ////////////////////////////////////////
// {7DF497EB-F9BB-4152-8677-49F7FF6CAC7E}
const GUID foundation::IID_IAsyncOperation_Int32 =
{ 0x7df497eb, 0xf9bb, 0x4152, { 0x86, 0x77, 0x49, 0xf7, 0xff, 0x6c, 0xac, 0x7e } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Int32(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 2,
    IID_IAsyncOperation_Int32,
    U("AsyncOperation_Int32"),
    &_Int32TypeInfo);

//////////////////////////////// AsyncOperation_UInt32 ////////////////////////////////////////
// {CA832570-D68F-44F3-8CF9-499DB6FD20D4}
const GUID foundation::IID_IAsyncOperation_UInt32 =
{ 0xca832570, 0xd68f, 0x44f3, { 0x8c, 0xf9, 0x49, 0x9d, 0xb6, 0xfd, 0x20, 0xd4 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_UInt32(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 3,
    IID_IAsyncOperation_UInt32,
    U("AsyncOperation_UInt32"),
    &_UInt32TypeInfo);

//////////////////////////////// AsyncOperation_Int16 ////////////////////////////////////////
// {B4401959-40D2-4581-B618-0F57A0531468}
const GUID foundation::IID_IAsyncOperation_Int16 =
{ 0xb4401959, 0x40d2, 0x4581, { 0xb6, 0x18, 0xf, 0x57, 0xa0, 0x53, 0x14, 0x68 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Int16(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 4,
    IID_IAsyncOperation_Int16,
    U("AsyncOperation_Int16"),
    &_Int16TypeInfo);

//////////////////////////////// AsyncOperation_UInt16 ////////////////////////////////////////
// {E1B3A8A5-A211-4C87-B69B-880A3086968D}
const GUID foundation::IID_IAsyncOperation_UInt16 =
{ 0xe1b3a8a5, 0xa211, 0x4c87, { 0xb6, 0x9b, 0x88, 0xa, 0x30, 0x86, 0x96, 0x8d } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_UInt16(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 5,
    IID_IAsyncOperation_UInt16,
    U("AsyncOperation_UInt16"),
    &_UInt16TypeInfo);

//////////////////////////////// AsyncOperation_UInt8 ////////////////////////////////////////
// {982C9E12-FA59-4778-8193-83842F646F3D}
const GUID foundation::IID_IAsyncOperation_UInt8 =
{ 0x982c9e12, 0xfa59, 0x4778, { 0x81, 0x93, 0x83, 0x84, 0x2f, 0x64, 0x6f, 0x3d } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_UInt8(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 6,
    IID_IAsyncOperation_UInt8,
    U("AsyncOperation_UInt8"),
    &_UInt8TypeInfo);

//////////////////////////////// AsyncOperation_DateTime ////////////////////////////////////////
// {074B8A85-037B-4530-A3BA-EEA57B7234F2}
const GUID foundation::IID_IAsyncOperation_DateTime =
{ 0x74b8a85, 0x37b, 0x4530, { 0xa3, 0xba, 0xee, 0xa5, 0x7b, 0x72, 0x34, 0xf2 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_DateTime(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 7,
    IID_IAsyncOperation_DateTime,
    U("AsyncOperation_DateTime"),
    &_DateTimeTypeInfo);

//////////////////////////////// AsyncOperation_Boolean ////////////////////////////////////////
// {981A19CC-937D-42F0-ACB5-56528F7B2D1F}
const GUID foundation::IID_IAsyncOperation_Boolean =
{ 0x981a19cc, 0x937d, 0x42f0, { 0xac, 0xb5, 0x56, 0x52, 0x8f, 0x7b, 0x2d, 0x1f } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Boolean(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 8,
    IID_IAsyncOperation_Boolean,
    U("AsyncOperation_Boolean"),
    &_BoolTypeInfo);

//////////////////////////////// AsyncOperation_Int64 ////////////////////////////////////////
// {665E007B-007F-4F6B-926B-AE50AC1FB675}
const GUID foundation::IID_IAsyncOperation_Int64 =
{ 0x665e007b, 0x7f, 0x4f6b, { 0x92, 0x6b, 0xae, 0x50, 0xac, 0x1f, 0xb6, 0x75 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Int64(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 9,
    IID_IAsyncOperation_Int64,
    U("AsyncOperation_Int64"),
    &_Int64TypeInfo);

//////////////////////////////// AsyncOperation_UInt64 ////////////////////////////////////////
// {46B8A87D-145D-468A-ADEE-486743576C05}
const GUID foundation::IID_IAsyncOperation_UInt64 =
{ 0x46b8a87d, 0x145d, 0x468a, { 0xad, 0xee, 0x48, 0x67, 0x43, 0x57, 0x6c, 0x5 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_UInt64(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 10,
    IID_IAsyncOperation_UInt64,
    U("AsyncOperation_UInt64"),
    &_UInt64TypeInfo);

//////////////////////////////// AsyncOperation_Single ////////////////////////////////////////
// {6E80922E-B7D4-4172-8E1C-F243BA775C71}
const GUID foundation::IID_IAsyncOperation_Single =
{ 0x6e80922e, 0xb7d4, 0x4172, { 0x8e, 0x1c, 0xf2, 0x43, 0xba, 0x77, 0x5c, 0x71 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Single(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 25,
    IID_IAsyncOperation_Single,
    U("AsyncOperation_Single"),
    &_SingleTypeInfo);

//////////////////////////////// AsyncOperation_Double ////////////////////////////////////////
// {EB2EB3DE-0741-4F2C-A0DD-D3A775F2BBAA}
const GUID foundation::IID_IAsyncOperation_Double =
{ 0xeb2eb3de, 0x741, 0x4f2c, { 0xa0, 0xdd, 0xd3, 0xa7, 0x75, 0xf2, 0xbb, 0xaa } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Double(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 26,
    IID_IAsyncOperation_Double,
    U("AsyncOperation_Double"),
    &_DoubleTypeInfo);

//////////////////////////////// _AsyncOperationTypeInfo_Inspectable ////////////////////////////////////////

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Inspectable(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 11,
    IID_IAsyncOperation,
    U("AsyncOperation_Inspectable"),
    &_InspectableTypeInfo);

//////////////////////////////// AsyncOperation_Void ////////////////////////////////////////
// {D0C045BE-8B0E-4155-BF24-93D610ADD0D3}
const GUID foundation::IID_IAsyncOperation_Void =
{ 0xd0c045be, 0x8b0e, 0x4155, { 0xbf, 0x24, 0x93, 0xd6, 0x10, 0xad, 0xd0, 0xd3 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Void(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 12,
    IID_IAsyncOperation_Void,
    U("AsyncOperation_Void"),
    &_VoidTypeInfo);

//////////////////////////////// AsyncOperation_StringArray ////////////////////////////////////////
// {F2FD9E45-1166-44FA-9F54-D113380FCBC0}
const GUID foundation::IID_IAsyncOperation_StringArray =
{ 0xf2fd9e45, 0x1166, 0x44fa, { 0x9f, 0x54, 0xd1, 0x13, 0x38, 0xf, 0xcb, 0xc0 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_StringArray(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 13,
    IID_IAsyncOperation_StringArray,
    U("AsyncOperation_StringArray"),
    &_StringArrayTypeInfo);

//////////////////////////////// AsyncOperation_Int32Array ////////////////////////////////////////
// {7AB7F960-9709-4923-8523-150DF23F0106}
const GUID foundation::IID_IAsyncOperation_Int32Array =
{ 0x7ab7f960, 0x9709, 0x4923, { 0x85, 0x23, 0x15, 0xd, 0xf2, 0x3f, 0x1, 0x6 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Int32Array(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 14,
    IID_IAsyncOperation_Int32Array,
    U("AsyncOperation_Int32Array"),
    &_Int32ArrayTypeInfo);

//////////////////////////////// AsyncOperation_UInt32Array ////////////////////////////////////////
// {C509DE65-2D69-4633-AECC-9EB4592DBFD8}
const GUID foundation::IID_IAsyncOperation_UInt32Array =
{ 0xc509de65, 0x2d69, 0x4633, { 0xae, 0xcc, 0x9e, 0xb4, 0x59, 0x2d, 0xbf, 0xd8 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_UInt32Array(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 15,
    IID_IAsyncOperation_UInt32Array,
    U("AsyncOperation_UInt32Array"),
    &_UInt32ArrayTypeInfo);

//////////////////////////////// AsyncOperation_Int16Array ////////////////////////////////////////
// {9DFF669D-9BA1-4C9E-8FC2-61A360636EDE}
const GUID foundation::IID_IAsyncOperation_Int16Array =
{ 0x9dff669d, 0x9ba1, 0x4c9e, { 0x8f, 0xc2, 0x61, 0xa3, 0x60, 0x63, 0x6e, 0xde } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Int16Array(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 16,
    IID_IAsyncOperation_Int16Array,
    U("AsyncOperation_Int16Array"),
    &_Int16ArrayTypeInfo);

//////////////////////////////// AsyncOperation_UInt16Array ////////////////////////////////////////
// {BFF9DCAF-E62B-4A44-9FCE-659927CC9580}
const GUID foundation::IID_IAsyncOperation_UInt16Array =
{ 0xbff9dcaf, 0xe62b, 0x4a44, { 0x9f, 0xce, 0x65, 0x99, 0x27, 0xcc, 0x95, 0x80 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_UInt16Array(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 17,
    IID_IAsyncOperation_UInt16Array,
    U("AsyncOperation_UInt16Array"),
    &_UInt16ArrayTypeInfo);

//////////////////////////////// AsyncOperation_UInt8Array ////////////////////////////////////////
// {4DDCA179-3B90-4C99-9DA1-B120F1410DED}
const GUID foundation::IID_IAsyncOperation_UInt8Array =
{ 0x4ddca179, 0x3b90, 0x4c99, { 0x9d, 0xa1, 0xb1, 0x20, 0xf1, 0x41, 0xd, 0xed } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_UInt8Array(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 18,
    IID_IAsyncOperation_UInt8Array,
    U("AsyncOperation_UInt8Array"),
    &_UInt8ArrayTypeInfo);

//////////////////////////////// AsyncOperation_BooleanArray ////////////////////////////////////////
// {830CBAE1-9CE8-4ECF-941A-95C1F145A474}
const GUID foundation::IID_IAsyncOperation_BooleanArray =
{ 0x830cbae1, 0x9ce8, 0x4ecf, { 0x94, 0x1a, 0x95, 0xc1, 0xf1, 0x45, 0xa4, 0x74 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_BooleanArray(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 19,
    IID_IAsyncOperation_BooleanArray,
    U("AsyncOperation_BooleanArray"),
    &_BoolArrayTypeInfo);


//////////////////////////////// AsyncOperation_Int64Array ////////////////////////////////////////
// {6792B323-DCBF-427D-8FC5-4F7F16513BE6}
const GUID foundation::IID_IAsyncOperation_Int64Array =
{ 0x6792b323, 0xdcbf, 0x427d, { 0x8f, 0xc5, 0x4f, 0x7f, 0x16, 0x51, 0x3b, 0xe6 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Int64Array(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 20,
    IID_IAsyncOperation_Int64Array,
    U("AsyncOperation_Int64Array"),
    &_Int64ArrayTypeInfo);

//////////////////////////////// AsyncOperation_UInt64Array ////////////////////////////////////////
// {85CF41EF-213F-48F7-90DE-B0CDB25FCF0F}
const GUID foundation::IID_IAsyncOperation_UInt64Array =
{ 0x85cf41ef, 0x213f, 0x48f7, { 0x90, 0xde, 0xb0, 0xcd, 0xb2, 0x5f, 0xcf, 0xf } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_UInt64Array(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 21,
    IID_IAsyncOperation_UInt64Array,
    U("AsyncOperation_UInt64Array"),
    &_UInt64ArrayTypeInfo);

//////////////////////////////// AsyncOperation_DateTimeArray ////////////////////////////////////////
// {290F83F6-702F-4911-A8A1-11B255A210E5}
const GUID foundation::IID_IAsyncOperation_DateTimeArray =
{ 0x290f83f6, 0x702f, 0x4911, { 0xa8, 0xa1, 0x11, 0xb2, 0x55, 0xa2, 0x10, 0xe5 } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_DateTimeArray(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 22,
    IID_IAsyncOperation_DateTimeArray,
    U("AsyncOperation_DateTimeArray"),
    &_DateTimeArrayTypeInfo);

//////////////////////////////// AsyncOperation_InspectableArray ////////////////////////////////////////
// {F2FE8655-C3AE-4483-A496-C8592B778D2E}
const GUID foundation::IID_IAsyncOperation_InspectableArray =
{ 0xf2fe8655, 0xc3ae, 0x4483, { 0xa4, 0x96, 0xc8, 0x59, 0x2b, 0x77, 0x8d, 0x2e } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_InspectableArray(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 23,
    IID_IAsyncOperation_InspectableArray,
    U("AsyncOperation_InspectableArray"),
    &_ObjectArrayTypeInfo);

//////////////////////////////// _AsyncOperationTypeInfo_Base ////////////////////////////////////////

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_Base(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 24,
    IID_IAsyncOperationBase,
    U("AsyncOperation_Base"),
    &_VoidTypeInfo);

//////////////////////////////// AsyncOperation_Dictionary ////////////////////////////////////////
// {BBCDF252-7BB8-494D-919A-C2CEAE29974C}
const GUID foundation::IID_IAsyncOperation_IObject =
{ 0xbbcdf252, 0x7bb8, 0x494d,{ 0x91, 0x9a, 0xc2, 0xce, 0xae, 0x29, 0x97, 0x4c } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_IObject(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 25,
    IID_IAsyncOperation_IObject,
    U("AsyncOperation_Object"),
    &_ObjectPropertyTypeInfo);


//////////////////////////////// AsyncOperation_Dictionary ////////////////////////////////////////
// {257FA9C9-0257-4DE6-9EA7-7C245A3CEB4F}
const GUID foundation::IID_IAsyncOperation_IDictionary =
{ 0x257fa9c9, 0x257, 0x4de6,{ 0x9e, 0xa7, 0x7c, 0x24, 0x5a, 0x3c, 0xeb, 0x4f } };

const AsyncOperationTypeInfo _AsyncOperationTypeInfo_IDictionary(
    _FOUNDATION_BASE_ASYNC_OPERATION_TYPE_ID + 26,
    IID_IAsyncOperation_IDictionary,
    U("AsyncOperation_Dictionary"),
    &_DictionaryPropertyTypeInfo);

// define Factory type adapters
typedef _AsyncOperationAdapterBase<HSTRING,IAsyncOperation_String,&IID_IAsyncOperation_String,_PropertyValueConverter > CAsyncOperation_String_Adapter;
typedef _AsyncOperationAdapterBase<INT32,IAsyncOperation_Int32,&IID_IAsyncOperation_Int32, _PropertyValueConverter > CAsyncOperation_Int32_Adapter;
typedef _AsyncOperationAdapterBase<UINT32,IAsyncOperation_UInt32,&IID_IAsyncOperation_UInt32, _PropertyValueConverter > CAsyncOperation_UInt32_Adapter;
typedef _AsyncOperationAdapterBase<INT16,IAsyncOperation_Int16,&IID_IAsyncOperation_Int16, _PropertyValueConverter > CAsyncOperation_Int16_Adapter;
typedef _AsyncOperationAdapterBase<UINT16,IAsyncOperation_UInt16,&IID_IAsyncOperation_UInt16, _PropertyValueConverter > CAsyncOperation_UInt16_Adapter;
typedef _AsyncOperationAdapterBase<BYTE, IAsyncOperation_UInt8, &IID_IAsyncOperation_UInt8, _PropertyValueConverter > CAsyncOperation_UInt8_Adapter;
typedef _AsyncOperationAdapterBase<bool,IAsyncOperation_Boolean,&IID_IAsyncOperation_Boolean, _PropertyValueConverter > CAsyncOperation_Boolean_Adapter;
typedef _AsyncOperationAdapterBase<INT64,IAsyncOperation_Int64,&IID_IAsyncOperation_Int64, _PropertyValueConverter > CAsyncOperation_Int64_Adapter;
typedef _AsyncOperationAdapterBase<UINT64,IAsyncOperation_UInt64,&IID_IAsyncOperation_UInt64, _PropertyValueConverter > CAsyncOperation_UInt64_Adapter;
typedef _AsyncOperationAdapterBase<FLOAT, IAsyncOperation_Single, &IID_IAsyncOperation_Single, _PropertyValueConverter > CAsyncOperation_Single_Adapter;
typedef _AsyncOperationAdapterBase<DOUBLE, IAsyncOperation_Double, &IID_IAsyncOperation_Double, _PropertyValueConverter > CAsyncOperation_Double_Adapter;

// array types
typedef _AsyncOperationAdapterArrayBase<HSTRING, IAsyncOperation_StringArray, &IID_IAsyncOperation_StringArray > CAsyncOperation_StringArray_Adapter;
typedef _AsyncOperationAdapterArrayBase<INT32, IAsyncOperation_Int32Array, &IID_IAsyncOperation_Int32Array> CAsyncOperation_Int32Array_Adapter;
typedef _AsyncOperationAdapterArrayBase<UINT32, IAsyncOperation_UInt32Array, &IID_IAsyncOperation_UInt32Array> CAsyncOperation_UInt32Array_Adapter;
typedef _AsyncOperationAdapterArrayBase<INT16, IAsyncOperation_Int16Array, &IID_IAsyncOperation_Int16Array> CAsyncOperation_Int16Array_Adapter;
typedef _AsyncOperationAdapterArrayBase<UINT16, IAsyncOperation_UInt16Array, &IID_IAsyncOperation_UInt16Array> CAsyncOperation_UInt16Array_Adapter;
typedef _AsyncOperationAdapterArrayBase<BYTE, IAsyncOperation_UInt8Array, &IID_IAsyncOperation_UInt8Array> CAsyncOperation_UInt8Array_Adapter;
typedef _AsyncOperationAdapterArrayBase<bool, IAsyncOperation_BooleanArray, &IID_IAsyncOperation_BooleanArray > CAsyncOperation_BooleanArray_Adapter;
typedef _AsyncOperationAdapterArrayBase<INT64, IAsyncOperation_Int64Array, &IID_IAsyncOperation_Int64Array> CAsyncOperation_Int64Array_Adapter;
typedef _AsyncOperationAdapterArrayBase<UINT64, IAsyncOperation_UInt64Array, &IID_IAsyncOperation_UInt64Array> CAsyncOperation_UInt64Array_Adapter;
typedef _AsyncOperationAdapterArrayBase<foundation::IInspectable *, IAsyncOperation_InspectableArray, &IID_IAsyncOperation_InspectableArray> CAsyncOperation_IAsyncOperation_InspectableArray_Adapter;

// factory types
typedef _AsyncOperationAdapterBase<IObject *, IAsyncOperation_IObject, &IID_IAsyncOperation_IObject, _InspectableConverter > CAsyncOperation_Object_Adapter;
typedef _AsyncOperationAdapterBase<IDictionary *, IAsyncOperation_IDictionary, &IID_IAsyncOperation_IDictionary, _InspectableConverter > CAsyncOperation_Dictionary_Adapter;

class CAsyncOperationClassInfoCache_Type : public CAsyncOperationClassInfoCache::_TypeModelInfoCacheType
{
public:
    CAsyncOperationClassInfoCache_Type()
    {
        const AsyncOperationTypeInfo *factoryModelInfos[] = {
            &_AsyncOperationTypeInfo_Base,
            &_AsyncOperationTypeInfo_String,
            &_AsyncOperationTypeInfo_Int32,
            &_AsyncOperationTypeInfo_UInt32,
            &_AsyncOperationTypeInfo_Int16,
            &_AsyncOperationTypeInfo_UInt16,
            &_AsyncOperationTypeInfo_UInt8,
            &_AsyncOperationTypeInfo_DateTime,
            &_AsyncOperationTypeInfo_Boolean,
            &_AsyncOperationTypeInfo_Int64,
            &_AsyncOperationTypeInfo_UInt64,
            &_AsyncOperationTypeInfo_Single,
            &_AsyncOperationTypeInfo_Double,
            &_AsyncOperationTypeInfo_Void,
            &_AsyncOperationTypeInfo_Inspectable,

            &_AsyncOperationTypeInfo_StringArray,
            &_AsyncOperationTypeInfo_Int32Array,
            &_AsyncOperationTypeInfo_UInt32Array,
            &_AsyncOperationTypeInfo_Int16Array,
            &_AsyncOperationTypeInfo_UInt16Array,
            &_AsyncOperationTypeInfo_UInt8Array,
            &_AsyncOperationTypeInfo_DateTimeArray,
            &_AsyncOperationTypeInfo_BooleanArray,
            &_AsyncOperationTypeInfo_Int64Array,
            &_AsyncOperationTypeInfo_UInt64Array,
            &_AsyncOperationTypeInfo_InspectableArray,

            &_AsyncOperationTypeInfo_IObject,
            &_AsyncOperationTypeInfo_IDictionary
        };
        for (int index = 0;
            index < sizeof(factoryModelInfos) / sizeof(const AsyncOperationTypeInfo *);
            index++)
        {
            IGNOREHR(RegisterAsyncOperationInfoInternal(factoryModelInfos[index]));
        }

        foundation::ComPtr<library::IGenericAdapterFactory> spGenericAdapterFactory;
        IGNOREHR(CGenericAdapterFactory::CreateInstance(spGenericAdapterFactory.GetAddressOf()));
        foundation_assert(spGenericAdapterFactory != nullptr);

        const library::_TypeAdapterEntry _adapterFactoryEntries [] = {
                { IID_IAsyncOperation_UInt8, IID_IAsyncOperation_UInt8, CreateAdapter<CAsyncOperation_UInt8_Adapter> },
                { IID_IAsyncOperation_Boolean, IID_IAsyncOperation_Boolean, CreateAdapter<CAsyncOperation_Boolean_Adapter> },
                { IID_IAsyncOperation_String,IID_IAsyncOperation_String,CreateAdapter<CAsyncOperation_String_Adapter>},
                { IID_IAsyncOperation_Int32,IID_IAsyncOperation_Int32,CreateAdapter<CAsyncOperation_Int32_Adapter>},
                { IID_IAsyncOperation_UInt32,IID_IAsyncOperation_UInt32,CreateAdapter<CAsyncOperation_UInt32_Adapter>},
                { IID_IAsyncOperation_Int16,IID_IAsyncOperation_Int16,CreateAdapter<CAsyncOperation_Int16_Adapter>},
                { IID_IAsyncOperation_UInt16,IID_IAsyncOperation_UInt16,CreateAdapter<CAsyncOperation_UInt16_Adapter>},
                { IID_IAsyncOperation_Int64,IID_IAsyncOperation_Int64,CreateAdapter<CAsyncOperation_Int64_Adapter>},
                { IID_IAsyncOperation_UInt64,IID_IAsyncOperation_UInt64,CreateAdapter<CAsyncOperation_UInt64_Adapter>},
                { IID_IAsyncOperation_Single, IID_IAsyncOperation_Single, CreateAdapter<CAsyncOperation_Single_Adapter> },
                { IID_IAsyncOperation_Double, IID_IAsyncOperation_Double, CreateAdapter<CAsyncOperation_Double_Adapter> },

                { IID_IAsyncOperation_UInt8Array, IID_IAsyncOperation_UInt8Array, CreateAdapter<CAsyncOperation_UInt8Array_Adapter> },
                { IID_IAsyncOperation_BooleanArray, IID_IAsyncOperation_BooleanArray, CreateAdapter<CAsyncOperation_BooleanArray_Adapter> },
                { IID_IAsyncOperation_StringArray, IID_IAsyncOperation_StringArray, CreateAdapter<CAsyncOperation_StringArray_Adapter> },
                { IID_IAsyncOperation_Int32Array, IID_IAsyncOperation_Int32Array, CreateAdapter<CAsyncOperation_Int32Array_Adapter> },
                { IID_IAsyncOperation_UInt32Array, IID_IAsyncOperation_UInt32Array, CreateAdapter<CAsyncOperation_UInt32Array_Adapter> },
                { IID_IAsyncOperation_Int16Array, IID_IAsyncOperation_Int16Array, CreateAdapter<CAsyncOperation_Int16Array_Adapter> },
                { IID_IAsyncOperation_UInt16Array, IID_IAsyncOperation_UInt16Array, CreateAdapter<CAsyncOperation_UInt16Array_Adapter> },
                { IID_IAsyncOperation_Int64Array, IID_IAsyncOperation_Int64Array, CreateAdapter<CAsyncOperation_Int64Array_Adapter> },
                { IID_IAsyncOperation_UInt64Array, IID_IAsyncOperation_UInt64Array, CreateAdapter<CAsyncOperation_UInt64Array_Adapter> },
                { IID_IAsyncOperation_InspectableArray, IID_IAsyncOperation_InspectableArray, CreateAdapter<CAsyncOperation_IAsyncOperation_InspectableArray_Adapter> },

                { IID_IAsyncOperation_IObject, IID_IAsyncOperation_IObject, CreateAdapter<CAsyncOperation_Object_Adapter> },
                { IID_IAsyncOperation_IDictionary, IID_IAsyncOperation_IDictionary, CreateAdapter<CAsyncOperation_Dictionary_Adapter> },
                
                { foundation_GUID_NULL, foundation_GUID_NULL, 0L }
            };

        for (int index = 0;
            index < sizeof(_adapterFactoryEntries) / sizeof(const library::_TypeAdapterEntry);
            index++)
        {
            spGenericAdapterFactory->AddAdapterEntry(
                IID_IAsyncOperation,
                _adapterFactoryEntries[index]._iidInterface,
                IID_IAsyncOperation,
                _adapterFactoryEntries[index]._creatorCallback);
        }
        foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spInterfaceAdapterFactory;
        foundation::QueryInterface(spGenericAdapterFactory, spInterfaceAdapterFactory.GetAddressOf());

        library::RegisterInterfaceAdapterFactory(spInterfaceAdapterFactory);

    }

    HRESULT RegisterAsyncOperationInfoInternal(_In_ const AsyncOperationTypeInfo *pAsyncOperationTypeInfo)
    {
        CAsyncOperationTypeInfo *pAsyncOperationTypeInfoClass = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CAsyncOperationTypeInfo>::CreateInstance(&pAsyncOperationTypeInfoClass));
        IFR_ASSERT(pAsyncOperationTypeInfoClass->_Initialize(pAsyncOperationTypeInfo));

        this->AddToCache(pAsyncOperationTypeInfo->get_iid_type(), pAsyncOperationTypeInfoClass);
        return S_OK;
    }
};

CAsyncOperationClassInfoCache::_TypeModelInfoCacheType& CAsyncOperationClassInfoCache::GetInstance()
{
	return *_GetPtrInstance<CAsyncOperationClassInfoCache_Type>();
}

CAsyncOperationClassInfoCache& CAsyncOperationClassInfoCache::GetInstanceInternal()
{
    static CAsyncOperationClassInfoCache _instance;
    return _instance;
}

void CAsyncOperationClassInfoCache::AddToResultTypeInfoCacheInternal(CAsyncOperationTypeInfo *pTypeModelClassInfo)
{
    foundation::ComPtr<IPropertyTypeInfo> spResultTypeInfo;
    pTypeModelClassInfo->GetResultTypeInfo(spResultTypeInfo.GetAddressOf());

    foundation_assert(spResultTypeInfo);
    foundation::PropertyType propertType;

    spResultTypeInfo->GetPropertyType(&propertType);
    foundation::ComPtr<IObjectTypeInfo> spTypeInfo;
    spResultTypeInfo->GetTypeInfo(spTypeInfo.GetAddressOf());
    if (spTypeInfo == nullptr)
    {
        _resultPropertyTypeCache[propertType] = pTypeModelClassInfo;
    }
    else
    {
        IID iidType;
        spTypeInfo->GetType(&iidType);
        _resultModelTypeCache[iidType] = pTypeModelClassInfo;
    }
}

CAsyncOperationTypeInfo *CAsyncOperationClassInfoCache::LookupTypeByPropertyTypeInfoInternal(IPropertyTypeInfo *pPropertyTypeInfo)
{
    foundation_assert(pPropertyTypeInfo);

    foundation::PropertyType propertyType;
    pPropertyTypeInfo->GetPropertyType(&propertyType);
    foundation::ComPtr<IObjectTypeInfo> spTypeInfo;
    pPropertyTypeInfo->GetTypeInfo(spTypeInfo.GetAddressOf());

    if (spTypeInfo == nullptr)
    {
        _ResultPropertyTypeClassInfoMap::const_iterator iter = _resultPropertyTypeCache.find(propertyType);
        if(iter != _resultPropertyTypeCache.end())
        {
            return iter->second;
        }
    }
    else
    {
        IID iidType;
        spTypeInfo->GetType(&iidType);
        _ResultModelTypeClassInfoMap::const_iterator iter = _resultModelTypeCache.find(iidType);
        if(iter != _resultModelTypeCache.end())
        {
            return iter->second;
        }
    }
    return nullptr;
}

