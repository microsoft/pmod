/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LoggerTypeInfo.g.cpp
****/

#include <pmod/logger_type_info.h> 
#include <pmod/library/library_util.h> 
#include "LoggerTypeInfoIIDs.g.cpp" 



const foundation::EnumTypeInfo pmod::ObjectNode_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.ObjectNode_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::ObjectNode_CategoryEnum::Names[] = {
	U("SetParent"),
	U("ClearParent"),
	U("BeginModelChanges"),
	U("EndModelChanges"),
	nullptr
};

const UINT32 pmod::ObjectNode_CategoryEnum::Values[] = {
	0,
	1,
	2,
	3,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::SetParent_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.SetParent_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::SetParent_LogRecordEnum::Names[] = {
	U("ParentUniqueId"),
	U("ParentTypeId"),
	U("ChildUniqueId"),
	U("ChildTypeId"),
	nullptr
};

const UINT32 pmod::SetParent_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	3,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::Model_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.Model_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::Model_LogRecordEnum::Names[] = {
	U("UniqueId"),
	nullptr
};

const UINT32 pmod::Model_LogRecordEnum::Values[] = {
	0,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::Command_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.Command_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::Command_CategoryEnum::Names[] = {
	U("CanExecute"),
	U("Execute"),
	nullptr
};

const UINT32 pmod::Command_CategoryEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::CommandCanExecute_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.CommandCanExecute_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::CommandCanExecute_LogRecordEnum::Names[] = {
	U("Result"),
	nullptr
};

const UINT32 pmod::CommandCanExecute_LogRecordEnum::Values[] = {
	1,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::CommandExecute_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.CommandExecute_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::CommandExecute_LogRecordEnum::Names[] = {
	U("Parameters"),
	U("Result"),
	nullptr
};

const UINT32 pmod::CommandExecute_LogRecordEnum::Values[] = {
	1,
	2,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::Collection_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.Collection_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::Collection_CategoryEnum::Names[] = {
	U("PushChangeAction"),
	U("GetItem"),
	U("SetItem"),
	U("GetItems"),
	nullptr
};

const UINT32 pmod::Collection_CategoryEnum::Values[] = {
	0,
	1,
	2,
	3,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::CollectionPushChangeAction_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.CollectionPushChangeAction_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::CollectionPushChangeAction_LogRecordEnum::Names[] = {
	U("Action"),
	U("Index"),
	U("NItems"),
	nullptr
};

const UINT32 pmod::CollectionPushChangeAction_LogRecordEnum::Values[] = {
	1,
	2,
	3,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::CollectionGetItem_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.CollectionGetItem_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::CollectionGetItem_LogRecordEnum::Names[] = {
	U("Index"),
	nullptr
};

const UINT32 pmod::CollectionGetItem_LogRecordEnum::Values[] = {
	1,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::CollectionSetItem_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.CollectionSetItem_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::CollectionSetItem_LogRecordEnum::Names[] = {
	U("Index"),
	nullptr
};

const UINT32 pmod::CollectionSetItem_LogRecordEnum::Values[] = {
	1,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::CollectionGetItems_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.CollectionGetItems_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::CollectionGetItems_LogRecordEnum::Names[] = {
	nullptr
};

const UINT32 pmod::CollectionGetItems_LogRecordEnum::Values[] = {
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::PropertyChangedEventArgs_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.PropertyChangedEventArgs_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::PropertyChangedEventArgs_LogRecordEnum::Names[] = {
	U("PropertyId"),
	U("OldValue"),
	U("NewValue"),
	nullptr
};

const UINT32 pmod::PropertyChangedEventArgs_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::EventModelEventArgs_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.EventModelEventArgs_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::EventModelEventArgs_LogRecordEnum::Names[] = {
	U("EventId"),
	U("EventArgs"),
	nullptr
};

const UINT32 pmod::EventModelEventArgs_LogRecordEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::NotifyCollectionChangedEventArgs_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.NotifyCollectionChangedEventArgs_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::NotifyCollectionChangedEventArgs_LogRecordEnum::Names[] = {
	U("Action"),
	U("OldStartingIndex"),
	U("NewStartingIndex"),
	nullptr
};

const UINT32 pmod::NotifyCollectionChangedEventArgs_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::CanExecuteChangedEventArgs_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.CanExecuteChangedEventArgs_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::CanExecuteChangedEventArgs_LogRecordEnum::Names[] = {
	U("OldState"),
	U("NewState"),
	nullptr
};

const UINT32 pmod::CanExecuteChangedEventArgs_LogRecordEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::JsonProxyServer_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.JsonProxyServer_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::JsonProxyServer_CategoryEnum::Names[] = {
	U("Execute"),
	U("SendPluginResult"),
	nullptr
};

const UINT32 pmod::JsonProxyServer_CategoryEnum::Values[] = {
	0,
	1,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::JsonProxyServerExecute_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.JsonProxyServerExecute_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::JsonProxyServerExecute_LogRecordEnum::Names[] = {
	U("CallbackId"),
	U("Action"),
	U("JsonArgs"),
	nullptr
};

const UINT32 pmod::JsonProxyServerExecute_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};

const foundation::EnumTypeInfo pmod::JsonProxyServerSendResult_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.JsonProxyServerSendResult_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::JsonProxyServerSendResult_LogRecordEnum::Names[] = {
	U("CallbackId"),
	U("KeepCallback"),
	U("HResult"),
	U("JsonEventArgs"),
	nullptr
};

const UINT32 pmod::JsonProxyServerSendResult_LogRecordEnum::Values[] = {
	0,
	1,
	2,
	3,
	(UINT32)-1
};


static const foundation::TypeInfo* _typeInfoEntries [] = {
	&pmod::ObjectNode_CategoryEnum::EnumTypeInfo,
	&pmod::SetParent_LogRecordEnum::EnumTypeInfo,
	&pmod::Model_LogRecordEnum::EnumTypeInfo,
	&pmod::Command_CategoryEnum::EnumTypeInfo,
	&pmod::CommandCanExecute_LogRecordEnum::EnumTypeInfo,
	&pmod::CommandExecute_LogRecordEnum::EnumTypeInfo,
	&pmod::Collection_CategoryEnum::EnumTypeInfo,
	&pmod::CollectionPushChangeAction_LogRecordEnum::EnumTypeInfo,
	&pmod::CollectionGetItem_LogRecordEnum::EnumTypeInfo,
	&pmod::CollectionSetItem_LogRecordEnum::EnumTypeInfo,
	&pmod::CollectionGetItems_LogRecordEnum::EnumTypeInfo,
	&pmod::PropertyChangedEventArgs_LogRecordEnum::EnumTypeInfo,
	&pmod::EventModelEventArgs_LogRecordEnum::EnumTypeInfo,
	&pmod::NotifyCollectionChangedEventArgs_LogRecordEnum::EnumTypeInfo,
	&pmod::CanExecuteChangedEventArgs_LogRecordEnum::EnumTypeInfo,
	&pmod::JsonProxyServer_CategoryEnum::EnumTypeInfo,
	&pmod::JsonProxyServerExecute_LogRecordEnum::EnumTypeInfo,
	&pmod::JsonProxyServerSendResult_LogRecordEnum::EnumTypeInfo,

    nullptr
};

HRESULT  STDAPICALLTYPE register_core_logger_type_info(void)
{
    return foundation::library::RegisterMultipleTypeInfos(_typeInfoEntries);
}
