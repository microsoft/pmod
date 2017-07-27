/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger_type_info.h
****/

#pragma once

#include <pmod/interfaces.h> 
#include <pmod/library/pmod_lib_api.h>

namespace foundation {

    struct EnumTypeInfo;
}


namespace pmod { 

	enum class ObjectNode_Category
	{
		SetParent = 0,
		ClearParent = 1,
		BeginModelChanges = 2,
		EndModelChanges = 3,
	};

    const UINT32 ObjectNode_Category_SetParent_Id = 0x2000000;
    const UINT32 ObjectNode_Category_ClearParent_Id = 0x2000001;
    const UINT32 ObjectNode_Category_BeginModelChanges_Id = 0x2000002;
    const UINT32 ObjectNode_Category_EndModelChanges_Id = 0x2000003;

	enum class SetParent_LogRecord
	{
		ParentUniqueId = 0,
		ParentTypeId = 1,
		ChildUniqueId = 2,
		ChildTypeId = 3,
	};

    const UINT32 SetParent_LogRecord_ParentUniqueId_Id = 0x2010000;
    const UINT32 SetParent_LogRecord_ParentTypeId_Id = 0x2010001;
    const UINT32 SetParent_LogRecord_ChildUniqueId_Id = 0x2010002;
    const UINT32 SetParent_LogRecord_ChildTypeId_Id = 0x2010003;

	enum class Model_LogRecord
	{
		UniqueId = 0,
	};

    const UINT32 Model_LogRecord_UniqueId_Id = 0x2020000;

	enum class Command_Category
	{
		CanExecute = 0,
		Execute = 1,
	};

    const UINT32 Command_Category_CanExecute_Id = 0x2030000;
    const UINT32 Command_Category_Execute_Id = 0x2030001;

	enum class CommandCanExecute_LogRecord
	{
		Result = 1,
	};

    const UINT32 CommandCanExecute_LogRecord_Result_Id = 0x2040001;

	enum class CommandExecute_LogRecord
	{
		Parameters = 1,
		Result = 2,
	};

    const UINT32 CommandExecute_LogRecord_Parameters_Id = 0x2050001;
    const UINT32 CommandExecute_LogRecord_Result_Id = 0x2050002;

	enum class Collection_Category
	{
		PushChangeAction = 0,
		GetItem = 1,
		SetItem = 2,
		GetItems = 3,
	};

    const UINT32 Collection_Category_PushChangeAction_Id = 0x2060000;
    const UINT32 Collection_Category_GetItem_Id = 0x2060001;
    const UINT32 Collection_Category_SetItem_Id = 0x2060002;
    const UINT32 Collection_Category_GetItems_Id = 0x2060003;

	enum class CollectionPushChangeAction_LogRecord
	{
		Action = 1,
		Index = 2,
		NItems = 3,
	};

    const UINT32 CollectionPushChangeAction_LogRecord_Action_Id = 0x2070001;
    const UINT32 CollectionPushChangeAction_LogRecord_Index_Id = 0x2070002;
    const UINT32 CollectionPushChangeAction_LogRecord_NItems_Id = 0x2070003;

	enum class CollectionGetItem_LogRecord
	{
		Index = 1,
	};

    const UINT32 CollectionGetItem_LogRecord_Index_Id = 0x2080001;

	enum class CollectionSetItem_LogRecord
	{
		Index = 1,
	};

    const UINT32 CollectionSetItem_LogRecord_Index_Id = 0x2090001;

	enum class CollectionGetItems_LogRecord
	{
	};


	enum class PropertyChangedEventArgs_LogRecord
	{
		PropertyId = 0,
		OldValue = 1,
		NewValue = 2,
	};

    const UINT32 PropertyChangedEventArgs_LogRecord_PropertyId_Id = 0x20b0000;
    const UINT32 PropertyChangedEventArgs_LogRecord_OldValue_Id = 0x20b0001;
    const UINT32 PropertyChangedEventArgs_LogRecord_NewValue_Id = 0x20b0002;

	enum class EventModelEventArgs_LogRecord
	{
		EventId = 0,
		EventArgs = 1,
	};

    const UINT32 EventModelEventArgs_LogRecord_EventId_Id = 0x20c0000;
    const UINT32 EventModelEventArgs_LogRecord_EventArgs_Id = 0x20c0001;

	enum class NotifyCollectionChangedEventArgs_LogRecord
	{
		Action = 0,
		OldStartingIndex = 1,
		NewStartingIndex = 2,
	};

    const UINT32 NotifyCollectionChangedEventArgs_LogRecord_Action_Id = 0x20d0000;
    const UINT32 NotifyCollectionChangedEventArgs_LogRecord_OldStartingIndex_Id = 0x20d0001;
    const UINT32 NotifyCollectionChangedEventArgs_LogRecord_NewStartingIndex_Id = 0x20d0002;

	enum class CanExecuteChangedEventArgs_LogRecord
	{
		OldState = 0,
		NewState = 1,
	};

    const UINT32 CanExecuteChangedEventArgs_LogRecord_OldState_Id = 0x20e0000;
    const UINT32 CanExecuteChangedEventArgs_LogRecord_NewState_Id = 0x20e0001;

	enum class JsonProxyServer_Category
	{
		Execute = 0,
		SendPluginResult = 1,
	};

    const UINT32 JsonProxyServer_Category_Execute_Id = 0x20f0000;
    const UINT32 JsonProxyServer_Category_SendPluginResult_Id = 0x20f0001;

	enum class JsonProxyServerExecute_LogRecord
	{
		CallbackId = 0,
		Action = 1,
		JsonArgs = 2,
	};

    const UINT32 JsonProxyServerExecute_LogRecord_CallbackId_Id = 0x2100000;
    const UINT32 JsonProxyServerExecute_LogRecord_Action_Id = 0x2100001;
    const UINT32 JsonProxyServerExecute_LogRecord_JsonArgs_Id = 0x2100002;

	enum class JsonProxyServerSendResult_LogRecord
	{
		CallbackId = 0,
		KeepCallback = 1,
		HResult = 2,
		JsonEventArgs = 3,
	};

    const UINT32 JsonProxyServerSendResult_LogRecord_CallbackId_Id = 0x2110000;
    const UINT32 JsonProxyServerSendResult_LogRecord_KeepCallback_Id = 0x2110001;
    const UINT32 JsonProxyServerSendResult_LogRecord_HResult_Id = 0x2110002;
    const UINT32 JsonProxyServerSendResult_LogRecord_JsonEventArgs_Id = 0x2110003;


	// uuid(c993f9d8-7550-212d-9d3c-2a67d3336b79)
	struct ObjectNode_CategoryEnum
	{
		static const UINT32 TypeId = 0x200;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(34f25afe-f30a-408d-198c-447b9bd11040)
	struct SetParent_LogRecordEnum
	{
		static const UINT32 TypeId = 0x201;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(95162748-1d33-79f7-b4fe-fbd929e4e796)
	struct Model_LogRecordEnum
	{
		static const UINT32 TypeId = 0x202;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(829eaf71-f5db-ae04-ee92-d435816af1db)
	struct Command_CategoryEnum
	{
		static const UINT32 TypeId = 0x203;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(df80cc7d-f3d2-48eb-6154-31a5fe663793)
	struct CommandCanExecute_LogRecordEnum
	{
		static const UINT32 TypeId = 0x204;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(9bc4b1a4-d140-2d1b-6b28-1eb22ad8dede)
	struct CommandExecute_LogRecordEnum
	{
		static const UINT32 TypeId = 0x205;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(5b9b7a1f-478d-8cf0-5ef8-fda27ab74aeb)
	struct Collection_CategoryEnum
	{
		static const UINT32 TypeId = 0x206;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(2f9c811c-633e-bfb9-3589-37c457eeb0bb)
	struct CollectionPushChangeAction_LogRecordEnum
	{
		static const UINT32 TypeId = 0x207;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(d86f70ab-65e0-8955-a1ca-e0bd34189870)
	struct CollectionGetItem_LogRecordEnum
	{
		static const UINT32 TypeId = 0x208;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(f0220f8e-2dc8-3a21-daeb-2512f8346a08)
	struct CollectionSetItem_LogRecordEnum
	{
		static const UINT32 TypeId = 0x209;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(ea3edd4e-5d72-7b21-9b72-314372613075)
	struct CollectionGetItems_LogRecordEnum
	{
		static const UINT32 TypeId = 0x20a;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(cb013216-e000-f071-9e14-510f47ed8227)
	struct PropertyChangedEventArgs_LogRecordEnum
	{
		static const UINT32 TypeId = 0x20b;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(4b2e2cdd-17c3-296d-237a-ac54dfd65044)
	struct EventModelEventArgs_LogRecordEnum
	{
		static const UINT32 TypeId = 0x20c;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(749b85b3-7478-8a6b-2ad6-2e84bd5db5d2)
	struct NotifyCollectionChangedEventArgs_LogRecordEnum
	{
		static const UINT32 TypeId = 0x20d;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(ffcfaa80-921d-ff86-872f-c67d9585b2fa)
	struct CanExecuteChangedEventArgs_LogRecordEnum
	{
		static const UINT32 TypeId = 0x20e;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(6b7f2daa-a27d-b803-4a12-49a8c537598b)
	struct JsonProxyServer_CategoryEnum
	{
		static const UINT32 TypeId = 0x20f;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(fa6b5f6e-f083-926f-6827-0ac5ec9c1ca1)
	struct JsonProxyServerExecute_LogRecordEnum
	{
		static const UINT32 TypeId = 0x210;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(d0343c17-e223-e0f9-be9b-472e7b649c14)
	struct JsonProxyServerSendResult_LogRecordEnum
	{
		static const UINT32 TypeId = 0x211;
		static PMOD_LIB_API const IID IIDType;
		static PMOD_LIB_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};


} 
