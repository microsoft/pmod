/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationLoggerTypeInfo.g.cpp
****/

#include <foundation/logger_type_info.h> 
#include <foundation/library/library_util.h> 
#include "FoundationLoggerTypeInfoIIDs.g.cpp" 




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

const foundation::EnumTypeInfo foundation::ObjectDispatch_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectDispatch_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::ObjectDispatch_CategoryEnum::Names[] = {
	U("Invoke"),
	U("GetProperty"),
	U("SetProperty"),
	nullptr
};

const UINT32 foundation::ObjectDispatch_CategoryEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};

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

const foundation::EnumTypeInfo foundation::ObjectDispatchInvoke_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectDispatchInvoke_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&foundation::Object_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t foundation::ObjectDispatchInvoke_LogRecordEnum::Names[] = {
	U("Parameters"),
	U("MethodId"),
	U("MethodInfo"),
	U("ParamSize"),
	U("Result"),
	nullptr
};

const UINT32 foundation::ObjectDispatchInvoke_LogRecordEnum::Values[] = {
	2,
	3,
	4,
	5,
	6,
	(UINT32)-1
};

const foundation::EnumTypeInfo foundation::ObjectDispatchGetProperty_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectDispatchGetProperty_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&foundation::Object_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t foundation::ObjectDispatchGetProperty_LogRecordEnum::Names[] = {
	U("PropertyId"),
	nullptr
};

const UINT32 foundation::ObjectDispatchGetProperty_LogRecordEnum::Values[] = {
	2,
	(UINT32)-1
};

const foundation::EnumTypeInfo foundation::ObjectDispatchSetProperty_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.ObjectDispatchSetProperty_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&foundation::Object_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t foundation::ObjectDispatchSetProperty_LogRecordEnum::Names[] = {
	U("PropertyId"),
	nullptr
};

const UINT32 foundation::ObjectDispatchSetProperty_LogRecordEnum::Values[] = {
	2,
	(UINT32)-1
};

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
	&foundation::ObjectDispatch_CategoryEnum::EnumTypeInfo,
	&foundation::AsyncOperation_CategoryEnum::EnumTypeInfo,
	&foundation::Object_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectInitialize_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectFinalRelease_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectFireEvent_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectIsObserved_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectTrace_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectDispatchInvoke_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectDispatchGetProperty_LogRecordEnum::EnumTypeInfo,
	&foundation::ObjectDispatchSetProperty_LogRecordEnum::EnumTypeInfo,
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