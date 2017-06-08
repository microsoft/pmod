/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger_type_info.h
****/

#pragma once


#include <foundation/interfaces.h> 
#include <foundation/library/foundation_lib_api.h>


namespace foundation { 

    struct EnumTypeInfo;

	enum class Root_Category
	{
	};


	enum class Error_Category
	{
		Failure = 0,
		NullPtr = 1,
		Expected = 2,
	};

    const UINT32 Error_Category_Failure_Id = 0x1020000;
    const UINT32 Error_Category_NullPtr_Id = 0x1020001;
    const UINT32 Error_Category_Expected_Id = 0x1020002;

	enum class Error_LogRecord
	{
		HResult = 0,
		File = 1,
		Line = 2,
		Source = 3,
		Parameter = 4,
	};

    const UINT32 Error_LogRecord_HResult_Id = 0x1030000;
    const UINT32 Error_LogRecord_File_Id = 0x1030001;
    const UINT32 Error_LogRecord_Line_Id = 0x1030002;
    const UINT32 Error_LogRecord_Source_Id = 0x1030003;
    const UINT32 Error_LogRecord_Parameter_Id = 0x1030004;

	enum class Foundation_Category
	{
		Message = 0,
	};

    const UINT32 Foundation_Category_Message_Id = 0x1040000;

	enum class Message_LogRecord
	{
		Id = 0,
		Text = 1,
		Parameter = 2,
	};

    const UINT32 Message_LogRecord_Id_Id = 0x1050000;
    const UINT32 Message_LogRecord_Text_Id = 0x1050001;
    const UINT32 Message_LogRecord_Parameter_Id = 0x1050002;

	enum class Object_Category
	{
		Initialize = 0,
		FinalRelease = 1,
		FireEvent = 2,
		IsObserved = 3,
	};

    const UINT32 Object_Category_Initialize_Id = 0x1060000;
    const UINT32 Object_Category_FinalRelease_Id = 0x1060001;
    const UINT32 Object_Category_FireEvent_Id = 0x1060002;
    const UINT32 Object_Category_IsObserved_Id = 0x1060003;

	enum class AsyncOperation_Category
	{
		SetCompleted = 10,
	};

    const UINT32 AsyncOperation_Category_SetCompleted_Id = 0x107000a;

	enum class Object_LogRecord
	{
		UniqueId = 0,
		TypeId = 1,
	};

    const UINT32 Object_LogRecord_UniqueId_Id = 0x1080000;
    const UINT32 Object_LogRecord_TypeId_Id = 0x1080001;

	enum class ObjectInitialize_LogRecord
	{
		Options = 2,
	};

    const UINT32 ObjectInitialize_LogRecord_Options_Id = 0x1090002;

	enum class ObjectFinalRelease_LogRecord
	{
	};


	enum class ObjectFireEvent_LogRecord
	{
		EventArgs = 2,
	};

    const UINT32 ObjectFireEvent_LogRecord_EventArgs_Id = 0x10b0002;

	enum class ObjectIsObserved_LogRecord
	{
		Observed = 2,
	};

    const UINT32 ObjectIsObserved_LogRecord_Observed_Id = 0x10c0002;

	enum class ObjectTrace_LogRecord
	{
		ObjectId = 0,
		ParameterName = 1,
		ParameterValue = 2,
	};

    const UINT32 ObjectTrace_LogRecord_ObjectId_Id = 0x10d0000;
    const UINT32 ObjectTrace_LogRecord_ParameterName_Id = 0x10d0001;
    const UINT32 ObjectTrace_LogRecord_ParameterValue_Id = 0x10d0002;

	enum class AsyncOperationSetCompleted_LogRecord
	{
		UniqueId = 0,
		HResult = 1,
		Result = 2,
	};

    const UINT32 AsyncOperationSetCompleted_LogRecord_UniqueId_Id = 0x10e0000;
    const UINT32 AsyncOperationSetCompleted_LogRecord_HResult_Id = 0x10e0001;
    const UINT32 AsyncOperationSetCompleted_LogRecord_Result_Id = 0x10e0002;

	enum class BaseEventArgs_LogRecord
	{
		TypeIID = 0,
	};

    const UINT32 BaseEventArgs_LogRecord_TypeIID_Id = 0x10f0000;

	enum class ObjectFactory_Category
	{
		RegisterObject = 0,
		CreateObject = 1,
	};

    const UINT32 ObjectFactory_Category_RegisterObject_Id = 0x1100000;
    const UINT32 ObjectFactory_Category_CreateObject_Id = 0x1100001;

	enum class ObjectFactory_LogRecord
	{
		TypeIID = 0,
		Context = 1,
	};

    const UINT32 ObjectFactory_LogRecord_TypeIID_Id = 0x1110000;
    const UINT32 ObjectFactory_LogRecord_Context_Id = 0x1110001;


	// uuid(3e202745-091d-63d8-e973-0cef24e0d2b4)
	struct LoggingLevelEnum
	{
		static const UINT32 TypeId = 0x100;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(98da0234-60a0-6074-e779-2644fc946bb2)
	struct Root_CategoryEnum
	{
		static const UINT32 TypeId = 0x101;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(f339383e-8ed8-0dc3-2354-67c502d5b00d)
	struct Error_CategoryEnum
	{
		static const UINT32 TypeId = 0x102;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(b6bf9cdf-7a72-c153-8c98-82e00b188a6d)
	struct Error_LogRecordEnum
	{
		static const UINT32 TypeId = 0x103;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(721523d3-80cc-f6f0-d40b-8b2f1ee7f8be)
	struct Foundation_CategoryEnum
	{
		static const UINT32 TypeId = 0x104;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(75de42a9-bc72-2637-59d4-d930b8f922ca)
	struct Message_LogRecordEnum
	{
		static const UINT32 TypeId = 0x105;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(f2698f26-b2fc-4aa3-e7b5-dfbcf58c49e5)
	struct Object_CategoryEnum
	{
		static const UINT32 TypeId = 0x106;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(942a85af-05cd-c073-3746-4ccf6412b41d)
	struct AsyncOperation_CategoryEnum
	{
		static const UINT32 TypeId = 0x107;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(c5a18d29-9d8c-0789-099e-4f968b44dfb7)
	struct Object_LogRecordEnum
	{
		static const UINT32 TypeId = 0x108;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(a1c01618-5a6f-e661-d58a-3132eab988ff)
	struct ObjectInitialize_LogRecordEnum
	{
		static const UINT32 TypeId = 0x109;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(559b224d-8932-f41b-e7f2-3d445ae92463)
	struct ObjectFinalRelease_LogRecordEnum
	{
		static const UINT32 TypeId = 0x10a;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(a4e9468e-b566-4952-8700-8c3bd1d300a9)
	struct ObjectFireEvent_LogRecordEnum
	{
		static const UINT32 TypeId = 0x10b;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(ca8a0938-f8c3-62ae-143b-71b41a12c387)
	struct ObjectIsObserved_LogRecordEnum
	{
		static const UINT32 TypeId = 0x10c;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(f515f064-df1c-1590-ea15-867d01f29633)
	struct ObjectTrace_LogRecordEnum
	{
		static const UINT32 TypeId = 0x10d;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(fe208879-0386-b70d-d99a-58cb29c42d02)
	struct AsyncOperationSetCompleted_LogRecordEnum
	{
		static const UINT32 TypeId = 0x10e;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(2a98a20f-9b5d-5897-8752-f576f96e8937)
	struct BaseEventArgs_LogRecordEnum
	{
		static const UINT32 TypeId = 0x10f;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(7b5d44b2-83bc-c950-465b-80a576020190)
	struct ObjectFactory_CategoryEnum
	{
		static const UINT32 TypeId = 0x110;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(b60cbdc2-52cf-71b2-b24b-3aac5e4305ef)
	struct ObjectFactory_LogRecordEnum
	{
		static const UINT32 TypeId = 0x111;
		static FOUNDATION_LIB_API const IID IIDType;
		static FOUNDATION_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};


} 
