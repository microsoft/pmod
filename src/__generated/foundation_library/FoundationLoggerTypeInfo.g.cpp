/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationLoggerTypeInfo.g.cpp
****/

#include <foundation/logger_type_info.h> 
#include <foundation/library/library_util.h> 





const IID foundation::LoggingLevelEnum::IIDType = {0x3e202745,0x091d,0x63d8,{0xe9,0x73,0x0c,0xef,0x24,0xe0,0xd2,0xb4}};

const foundation::EnumTypeInfo foundation::LoggingLevelEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.LoggingLevel"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::LoggingLevelEnum::Names[] = {
	U("Off"),
	U("Fatal"),
	U("Error"),
	U("Warning"),
	U("Info"),
	U("Debug"),
	U("Trace"),
	nullptr
};

const UINT32 foundation::LoggingLevelEnum::Values[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	(UINT32)-1
};


const IID foundation::Root_CategoryEnum::IIDType = {0x98da0234,0x60a0,0x6074,{0xe7,0x79,0x26,0x44,0xfc,0x94,0x6b,0xb2}};

const foundation::EnumTypeInfo foundation::Root_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.Root_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::Root_CategoryEnum::Names[] = {
	nullptr
};

const UINT32 foundation::Root_CategoryEnum::Values[] = {
	(UINT32)-1
};


const IID foundation::Error_CategoryEnum::IIDType = {0xf339383e,0x8ed8,0x0dc3,{0x23,0x54,0x67,0xc5,0x02,0xd5,0xb0,0x0d}};

const foundation::EnumTypeInfo foundation::Error_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.Error_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::Error_CategoryEnum::Names[] = {
	U("Failure"),
	U("NullPtr"),
	U("Expected"),
	nullptr
};

const UINT32 foundation::Error_CategoryEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};


const IID foundation::Error_LogRecordEnum::IIDType = {0xb6bf9cdf,0x7a72,0xc153,{0x8c,0x98,0x82,0xe0,0x0b,0x18,0x8a,0x6d}};

const foundation::EnumTypeInfo foundation::Error_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.Error_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::Error_LogRecordEnum::Names[] = {
	U("HResult"),
	U("File"),
	U("Line"),
	U("Source"),
	U("Parameter"),
	nullptr
};

const UINT32 foundation::Error_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	3,
	4,
	(UINT32)-1
};


const IID foundation::Foundation_CategoryEnum::IIDType = {0x721523d3,0x80cc,0xf6f0,{0xd4,0x0b,0x8b,0x2f,0x1e,0xe7,0xf8,0xbe}};

const foundation::EnumTypeInfo foundation::Foundation_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.Foundation_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::Foundation_CategoryEnum::Names[] = {
	U("Message"),
	nullptr
};

const UINT32 foundation::Foundation_CategoryEnum::Values[] = {
	0,
	(UINT32)-1
};


const IID foundation::Message_LogRecordEnum::IIDType = {0x75de42a9,0xbc72,0x2637,{0x59,0xd4,0xd9,0x30,0xb8,0xf9,0x22,0xca}};

const foundation::EnumTypeInfo foundation::Message_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.Message_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::Message_LogRecordEnum::Names[] = {
	U("Id"),
	U("Text"),
	U("Parameter"),
	nullptr
};

const UINT32 foundation::Message_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};


const IID foundation::Object_CategoryEnum::IIDType = {0xf2698f26,0xb2fc,0x4aa3,{0xe7,0xb5,0xdf,0xbc,0xf5,0x8c,0x49,0xe5}};

const foundation::EnumTypeInfo foundation::Object_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.Object_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::Object_CategoryEnum::Names[] = {
	U("Initialize"),
	U("FinalRelease"),
	U("FireEvent"),
	U("IsObserved"),
	nullptr
};

const UINT32 foundation::Object_CategoryEnum::Values[] = {
	0,
	1,
	2,
	3,
	(UINT32)-1
};


const IID foundation::AsyncOperation_CategoryEnum::IIDType = {0x942a85af,0x05cd,0xc073,{0x37,0x46,0x4c,0xcf,0x64,0x12,0xb4,0x1d}};

const foundation::EnumTypeInfo foundation::AsyncOperation_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.AsyncOperation_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::AsyncOperation_CategoryEnum::Names[] = {
	U("SetCompleted"),
	nullptr
};

const UINT32 foundation::AsyncOperation_CategoryEnum::Values[] = {
	10,
	(UINT32)-1
};


const IID foundation::Object_LogRecordEnum::IIDType = {0xc5a18d29,0x9d8c,0x0789,{0x09,0x9e,0x4f,0x96,0x8b,0x44,0xdf,0xb7}};

const foundation::EnumTypeInfo foundation::Object_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.Object_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::Object_LogRecordEnum::Names[] = {
	U("UniqueId"),
	U("TypeId"),
	nullptr
};

const UINT32 foundation::Object_LogRecordEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};


const IID foundation::ObjectInitialize_LogRecordEnum::IIDType = {0xa1c01618,0x5a6f,0xe661,{0xd5,0x8a,0x31,0x32,0xea,0xb9,0x88,0xff}};

const foundation::EnumTypeInfo foundation::ObjectInitialize_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectInitialize_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&foundation::Object_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t foundation::ObjectInitialize_LogRecordEnum::Names[] = {
	U("Options"),
	nullptr
};

const UINT32 foundation::ObjectInitialize_LogRecordEnum::Values[] = {
	2,
	(UINT32)-1
};


const IID foundation::ObjectFinalRelease_LogRecordEnum::IIDType = {0x559b224d,0x8932,0xf41b,{0xe7,0xf2,0x3d,0x44,0x5a,0xe9,0x24,0x63}};

const foundation::EnumTypeInfo foundation::ObjectFinalRelease_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectFinalRelease_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&foundation::Object_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t foundation::ObjectFinalRelease_LogRecordEnum::Names[] = {
	nullptr
};

const UINT32 foundation::ObjectFinalRelease_LogRecordEnum::Values[] = {
	(UINT32)-1
};


const IID foundation::ObjectFireEvent_LogRecordEnum::IIDType = {0xa4e9468e,0xb566,0x4952,{0x87,0x00,0x8c,0x3b,0xd1,0xd3,0x00,0xa9}};

const foundation::EnumTypeInfo foundation::ObjectFireEvent_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectFireEvent_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&foundation::Object_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t foundation::ObjectFireEvent_LogRecordEnum::Names[] = {
	U("EventArgs"),
	nullptr
};

const UINT32 foundation::ObjectFireEvent_LogRecordEnum::Values[] = {
	2,
	(UINT32)-1
};


const IID foundation::ObjectIsObserved_LogRecordEnum::IIDType = {0xca8a0938,0xf8c3,0x62ae,{0x14,0x3b,0x71,0xb4,0x1a,0x12,0xc3,0x87}};

const foundation::EnumTypeInfo foundation::ObjectIsObserved_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectIsObserved_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&foundation::Object_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t foundation::ObjectIsObserved_LogRecordEnum::Names[] = {
	U("Observed"),
	nullptr
};

const UINT32 foundation::ObjectIsObserved_LogRecordEnum::Values[] = {
	2,
	(UINT32)-1
};


const IID foundation::ObjectTrace_LogRecordEnum::IIDType = {0xf515f064,0xdf1c,0x1590,{0xea,0x15,0x86,0x7d,0x01,0xf2,0x96,0x33}};

const foundation::EnumTypeInfo foundation::ObjectTrace_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectTrace_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::ObjectTrace_LogRecordEnum::Names[] = {
	U("ObjectId"),
	U("ParameterName"),
	U("ParameterValue"),
	nullptr
};

const UINT32 foundation::ObjectTrace_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};


const IID foundation::AsyncOperationSetCompleted_LogRecordEnum::IIDType = {0xfe208879,0x0386,0xb70d,{0xd9,0x9a,0x58,0xcb,0x29,0xc4,0x2d,0x02}};

const foundation::EnumTypeInfo foundation::AsyncOperationSetCompleted_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.AsyncOperationSetCompleted_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::AsyncOperationSetCompleted_LogRecordEnum::Names[] = {
	U("UniqueId"),
	U("HResult"),
	U("Result"),
	nullptr
};

const UINT32 foundation::AsyncOperationSetCompleted_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};


const IID foundation::BaseEventArgs_LogRecordEnum::IIDType = {0x2a98a20f,0x9b5d,0x5897,{0x87,0x52,0xf5,0x76,0xf9,0x6e,0x89,0x37}};

const foundation::EnumTypeInfo foundation::BaseEventArgs_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.BaseEventArgs_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::BaseEventArgs_LogRecordEnum::Names[] = {
	U("TypeIID"),
	nullptr
};

const UINT32 foundation::BaseEventArgs_LogRecordEnum::Values[] = {
	0,
	(UINT32)-1
};


const IID foundation::ObjectFactory_CategoryEnum::IIDType = {0x7b5d44b2,0x83bc,0xc950,{0x46,0x5b,0x80,0xa5,0x76,0x02,0x01,0x90}};

const foundation::EnumTypeInfo foundation::ObjectFactory_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectFactory_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::ObjectFactory_CategoryEnum::Names[] = {
	U("RegisterObject"),
	U("CreateObject"),
	nullptr
};

const UINT32 foundation::ObjectFactory_CategoryEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};


const IID foundation::ObjectFactory_LogRecordEnum::IIDType = {0xb60cbdc2,0x52cf,0x71b2,{0xb2,0x4b,0x3a,0xac,0x5e,0x43,0x05,0xef}};

const foundation::EnumTypeInfo foundation::ObjectFactory_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectFactory_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::ObjectFactory_LogRecordEnum::Names[] = {
	U("TypeIID"),
	U("Context"),
	nullptr
};

const UINT32 foundation::ObjectFactory_LogRecordEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};


static const foundation::TypeInfo* _typeInfoEntries [] = {
	&foundation::LoggingLevelEnum::EnumTypeInfo,
	&foundation::Root_CategoryEnum::EnumTypeInfo,
	&foundation::Error_CategoryEnum::EnumTypeInfo,
	&foundation::Error_LogRecordEnum::EnumTypeInfo,
	&foundation::Foundation_CategoryEnum::EnumTypeInfo,
	&foundation::Message_LogRecordEnum::EnumTypeInfo,
	&foundation::Object_CategoryEnum::EnumTypeInfo,
	&foundation::AsyncOperation_CategoryEnum::EnumTypeInfo,
	&foundation::Object_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectInitialize_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectFinalRelease_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectFireEvent_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectIsObserved_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectTrace_LogRecordEnum::EnumTypeInfo,
	&foundation::AsyncOperationSetCompleted_LogRecordEnum::EnumTypeInfo,
	&foundation::BaseEventArgs_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectFactory_CategoryEnum::EnumTypeInfo,
	&foundation::ObjectFactory_LogRecordEnum::EnumTypeInfo,

    nullptr
};

HRESULT register_foundation_logger_type_info(void)
{
    return foundation::library::RegisterMultipleTypeInfos(_typeInfoEntries);
}
