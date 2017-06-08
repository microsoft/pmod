/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterLoggerTypeInfo.cpp
****/

#include "PMODAdapterLoggerTypeInfo.h" 
#include <pmod/library/library_util.h>





const IID nsadapter::NSAdapter_CategoryEnum::IIDType = {0x0b67f789,0x6c2c,0x0932,{0x0f,0x07,0x81,0xd7,0x5b,0x20,0x6c,0xf2}};

const foundation::EnumTypeInfo nsadapter::NSAdapter_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.NSAdapter_Category"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::NSAdapter_CategoryEnum::Names[] = {
	nullptr
};

const UINT32 nsadapter::NSAdapter_CategoryEnum::Values[] = {
	(UINT32)-1
};


const IID nsadapter::NSFoundation_CategoryEnum::IIDType = {0x39196cb5,0x54d8,0xda7e,{0xdb,0x65,0xd4,0x6a,0x56,0x1a,0x92,0xce}};

const foundation::EnumTypeInfo nsadapter::NSFoundation_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.NSFoundation_Category"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::NSFoundation_CategoryEnum::Names[] = {
	U("AddObserver"),
	U("RemoveObserver"),
	nullptr
};

const UINT32 nsadapter::NSFoundation_CategoryEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};


const IID nsadapter::AddObserver_LogRecordEnum::IIDType = {0x8d862691,0x3371,0x0064,{0xb6,0x58,0xb1,0xbb,0x65,0x75,0xc8,0xad}};

const foundation::EnumTypeInfo nsadapter::AddObserver_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.AddObserver_LogRecord"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::AddObserver_LogRecordEnum::Names[] = {
	U("UniqueId"),
	U("KeyPath"),
	U("Options"),
	nullptr
};

const UINT32 nsadapter::AddObserver_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};


const IID nsadapter::RemoveObserver_LogRecordEnum::IIDType = {0x2d65c5db,0xa66a,0x716b,{0xfa,0xff,0x06,0xf3,0xf7,0x93,0xeb,0x05}};

const foundation::EnumTypeInfo nsadapter::RemoveObserver_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.RemoveObserver_LogRecord"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::RemoveObserver_LogRecordEnum::Names[] = {
	U("UniqueId"),
	U("KeyPath"),
	nullptr
};

const UINT32 nsadapter::RemoveObserver_LogRecordEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};


const IID nsadapter::NSObjectBase_CategoryEnum::IIDType = {0xdc4e2d4a,0x9775,0xd1a5,{0xf4,0x44,0x9d,0x8e,0x57,0xaf,0xca,0xe2}};

const foundation::EnumTypeInfo nsadapter::NSObjectBase_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.NSObjectBase_Category"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::NSObjectBase_CategoryEnum::Names[] = {
	U("FinalRelease"),
	nullptr
};

const UINT32 nsadapter::NSObjectBase_CategoryEnum::Values[] = {
	0,
	(UINT32)-1
};


const IID nsadapter::FinalRelease_LogRecordEnum::IIDType = {0x5669149a,0x66b4,0x936d,{0xad,0x6b,0xc2,0x42,0xa3,0xce,0xfe,0xac}};

const foundation::EnumTypeInfo nsadapter::FinalRelease_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.FinalRelease_LogRecord"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::FinalRelease_LogRecordEnum::Names[] = {
	U("UniqueId"),
	nullptr
};

const UINT32 nsadapter::FinalRelease_LogRecordEnum::Values[] = {
	0,
	(UINT32)-1
};


const IID nsadapter::NSPropertyModel_CategoryEnum::IIDType = {0x2ba8ba24,0xd726,0xd8cc,{0x0a,0x93,0xe8,0x33,0xec,0xd0,0xa9,0x00}};

const foundation::EnumTypeInfo nsadapter::NSPropertyModel_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.NSPropertyModel_Category"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::NSPropertyModel_CategoryEnum::Names[] = {
	U("FireNotifyPropertyChanged"),
	nullptr
};

const UINT32 nsadapter::NSPropertyModel_CategoryEnum::Values[] = {
	0,
	(UINT32)-1
};


const IID nsadapter::NSCollectionModel_CategoryEnum::IIDType = {0xfcdd3bd0,0xca97,0x7932,{0x1c,0x00,0xfc,0x1e,0xa9,0x09,0xb7,0xeb}};

const foundation::EnumTypeInfo nsadapter::NSCollectionModel_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.NSCollectionModel_Category"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::NSCollectionModel_CategoryEnum::Names[] = {
	U("FireNotifyCollectionChanged"),
	nullptr
};

const UINT32 nsadapter::NSCollectionModel_CategoryEnum::Values[] = {
	0,
	(UINT32)-1
};


const IID nsadapter::NSFactory_CategoryEnum::IIDType = {0x1acdab92,0xc511,0xc847,{0x07,0x6f,0xdd,0x75,0x06,0xda,0x6a,0xad}};

const foundation::EnumTypeInfo nsadapter::NSFactory_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.NSFactory_Category"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::NSFactory_CategoryEnum::Names[] = {
	U("AddCache"),
	U("RemoveCache"),
	U("RegisterAdapterClass"),
	nullptr
};

const UINT32 nsadapter::NSFactory_CategoryEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};


const IID nsadapter::AddCache_LogRecordEnum::IIDType = {0x803e5d22,0xb628,0x3d91,{0x3b,0x70,0x8e,0xb5,0x57,0xca,0x7c,0x71}};

const foundation::EnumTypeInfo nsadapter::AddCache_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.AddCache_LogRecord"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::AddCache_LogRecordEnum::Names[] = {
	U("UniqueId"),
	U("TypeIId"),
	U("Name"),
	nullptr
};

const UINT32 nsadapter::AddCache_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};


const IID nsadapter::RemoveCache_LogRecordEnum::IIDType = {0x8948c52b,0x686c,0x5bf9,{0x96,0x86,0xce,0xae,0x0d,0xab,0x8a,0xce}};

const foundation::EnumTypeInfo nsadapter::RemoveCache_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.RemoveCache_LogRecord"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::RemoveCache_LogRecordEnum::Names[] = {
	U("UniqueId"),
	nullptr
};

const UINT32 nsadapter::RemoveCache_LogRecordEnum::Values[] = {
	0,
	(UINT32)-1
};


const IID nsadapter::RegisterAdapterClass_LogRecordEnum::IIDType = {0x845bea56,0xea44,0x5522,{0xba,0x01,0x07,0xed,0x61,0x9f,0x61,0xf1}};

const foundation::EnumTypeInfo nsadapter::RegisterAdapterClass_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("nsadapter.RegisterAdapterClass_LogRecord"),
	false,
	&Names[0],
	&Values[0]
	);

const LPCSTR_t nsadapter::RegisterAdapterClass_LogRecordEnum::Names[] = {
	U("ClassName"),
	U("TypeId"),
	nullptr
};

const UINT32 nsadapter::RegisterAdapterClass_LogRecordEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};


static const foundation::TypeInfo* _typeInfoEntries [] = {
	&nsadapter::NSAdapter_CategoryEnum::EnumTypeInfo,
	&nsadapter::NSFoundation_CategoryEnum::EnumTypeInfo,
	&nsadapter::AddObserver_LogRecordEnum::EnumTypeInfo,
	&nsadapter::RemoveObserver_LogRecordEnum::EnumTypeInfo,
	&nsadapter::NSObjectBase_CategoryEnum::EnumTypeInfo,
	&nsadapter::FinalRelease_LogRecordEnum::EnumTypeInfo,
	&nsadapter::NSPropertyModel_CategoryEnum::EnumTypeInfo,
	&nsadapter::NSCollectionModel_CategoryEnum::EnumTypeInfo,
	&nsadapter::NSFactory_CategoryEnum::EnumTypeInfo,
	&nsadapter::AddCache_LogRecordEnum::EnumTypeInfo,
	&nsadapter::RemoveCache_LogRecordEnum::EnumTypeInfo,
	&nsadapter::RegisterAdapterClass_LogRecordEnum::EnumTypeInfo,

    nullptr
};

HRESULT  STDAPICALLTYPE register_nsadapter_logger_typeinfo(void)
{
    return foundation::library::RegisterMultipleTypeInfos(_typeInfoEntries);
}
