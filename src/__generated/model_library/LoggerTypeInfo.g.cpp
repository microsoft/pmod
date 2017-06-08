/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LoggerTypeInfo.g.cpp
****/

#include <pmod/logger_type_info.h> 
#include <pmod/library/library_util.h> 





const IID pmod::ObjectNode_CategoryEnum::IIDType = {0xc993f9d8,0x7550,0x212d,{0x9d,0x3c,0x2a,0x67,0xd3,0x33,0x6b,0x79}};

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


const IID pmod::SetParent_LogRecordEnum::IIDType = {0x34f25afe,0xf30a,0x408d,{0x19,0x8c,0x44,0x7b,0x9b,0xd1,0x10,0x40}};

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


const IID pmod::Model_LogRecordEnum::IIDType = {0x95162748,0x1d33,0x79f7,{0xb4,0xfe,0xfb,0xd9,0x29,0xe4,0xe7,0x96}};

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


const IID pmod::Command_CategoryEnum::IIDType = {0x829eaf71,0xf5db,0xae04,{0xee,0x92,0xd4,0x35,0x81,0x6a,0xf1,0xdb}};

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


const IID pmod::CommandCanExecute_LogRecordEnum::IIDType = {0xdf80cc7d,0xf3d2,0x48eb,{0x61,0x54,0x31,0xa5,0xfe,0x66,0x37,0x93}};

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


const IID pmod::CommandExecute_LogRecordEnum::IIDType = {0x9bc4b1a4,0xd140,0x2d1b,{0x6b,0x28,0x1e,0xb2,0x2a,0xd8,0xde,0xde}};

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


const IID pmod::Collection_CategoryEnum::IIDType = {0x5b9b7a1f,0x478d,0x8cf0,{0x5e,0xf8,0xfd,0xa2,0x7a,0xb7,0x4a,0xeb}};

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


const IID pmod::CollectionPushChangeAction_LogRecordEnum::IIDType = {0x2f9c811c,0x633e,0xbfb9,{0x35,0x89,0x37,0xc4,0x57,0xee,0xb0,0xbb}};

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


const IID pmod::CollectionGetItem_LogRecordEnum::IIDType = {0xd86f70ab,0x65e0,0x8955,{0xa1,0xca,0xe0,0xbd,0x34,0x18,0x98,0x70}};

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


const IID pmod::CollectionSetItem_LogRecordEnum::IIDType = {0xf0220f8e,0x2dc8,0x3a21,{0xda,0xeb,0x25,0x12,0xf8,0x34,0x6a,0x08}};

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


const IID pmod::CollectionGetItems_LogRecordEnum::IIDType = {0xea3edd4e,0x5d72,0x7b21,{0x9b,0x72,0x31,0x43,0x72,0x61,0x30,0x75}};

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


const IID pmod::PropertyModel_CategoryEnum::IIDType = {0xc9e65f48,0x1fb4,0xf44f,{0x40,0xc2,0x3e,0xbc,0x2f,0xe1,0x8b,0xab}};

const foundation::EnumTypeInfo pmod::PropertyModel_CategoryEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.PropertyModel_Category"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::PropertyModel_CategoryEnum::Names[] = {
	U("Invoke"),
	U("GetProperty"),
	U("SetProperty"),
	nullptr
};

const UINT32 pmod::PropertyModel_CategoryEnum::Values[] = {
	0,
	1,
	2,
	(UINT32)-1
};


const IID pmod::PropertyModelInvoke_LogRecordEnum::IIDType = {0x33c87af3,0x0f9c,0xa27e,{0x41,0xb1,0xf5,0x92,0x60,0xf4,0xdc,0xba}};

const foundation::EnumTypeInfo pmod::PropertyModelInvoke_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.PropertyModelInvoke_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::PropertyModelInvoke_LogRecordEnum::Names[] = {
	U("Parameters"),
	U("MethodId"),
	U("m_parameterName"),
	U("ParamSize"),
	U("Result"),
	nullptr
};

const UINT32 pmod::PropertyModelInvoke_LogRecordEnum::Values[] = {
	1,
	2,
	3,
	4,
	5,
	(UINT32)-1
};


const IID pmod::PropertyModelGetProperty_LogRecordEnum::IIDType = {0xa6db6a47,0x454b,0x86dd,{0xca,0xd7,0xb1,0x55,0x3e,0x0c,0x53,0x27}};

const foundation::EnumTypeInfo pmod::PropertyModelGetProperty_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.PropertyModelGetProperty_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::PropertyModelGetProperty_LogRecordEnum::Names[] = {
	U("PropertyId"),
	nullptr
};

const UINT32 pmod::PropertyModelGetProperty_LogRecordEnum::Values[] = {
	1,
	(UINT32)-1
};


const IID pmod::PropertyModelSetProperty_LogRecordEnum::IIDType = {0x5e3049a6,0x5c40,0x082c,{0x0d,0x27,0x05,0xc4,0x1a,0xc4,0x2f,0x2a}};

const foundation::EnumTypeInfo pmod::PropertyModelSetProperty_LogRecordEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.PropertyModelSetProperty_LogRecord"),
	false,
	&Names[0],
	&Values[0],
	&pmod::Model_LogRecordEnum::EnumTypeInfo
	);

const LPCSTR_t pmod::PropertyModelSetProperty_LogRecordEnum::Names[] = {
	U("PropertyId"),
	nullptr
};

const UINT32 pmod::PropertyModelSetProperty_LogRecordEnum::Values[] = {
	1,
	(UINT32)-1
};


const IID pmod::PropertyChangedEventArgs_LogRecordEnum::IIDType = {0xcb013216,0xe000,0xf071,{0x9e,0x14,0x51,0x0f,0x47,0xed,0x82,0x27}};

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


const IID pmod::EventModelEventArgs_LogRecordEnum::IIDType = {0x4b2e2cdd,0x17c3,0x296d,{0x23,0x7a,0xac,0x54,0xdf,0xd6,0x50,0x44}};

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


const IID pmod::NotifyCollectionChangedEventArgs_LogRecordEnum::IIDType = {0x749b85b3,0x7478,0x8a6b,{0x2a,0xd6,0x2e,0x84,0xbd,0x5d,0xb5,0xd2}};

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


const IID pmod::CanExecuteChangedEventArgs_LogRecordEnum::IIDType = {0xffcfaa80,0x921d,0xff86,{0x87,0x2f,0xc6,0x7d,0x95,0x85,0xb2,0xfa}};

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


const IID pmod::JsonProxyServer_CategoryEnum::IIDType = {0x6b7f2daa,0xa27d,0xb803,{0x4a,0x12,0x49,0xa8,0xc5,0x37,0x59,0x8b}};

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


const IID pmod::JsonProxyServerExecute_LogRecordEnum::IIDType = {0xfa6b5f6e,0xf083,0x926f,{0x68,0x27,0x0a,0xc5,0xec,0x9c,0x1c,0xa1}};

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


const IID pmod::JsonProxyServerSendResult_LogRecordEnum::IIDType = {0xd0343c17,0xe223,0xe0f9,{0xbe,0x9b,0x47,0x2e,0x7b,0x64,0x9c,0x14}};

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
	&pmod::PropertyModel_CategoryEnum::EnumTypeInfo,
	&pmod::PropertyModelInvoke_LogRecordEnum::EnumTypeInfo,
	&pmod::PropertyModelGetProperty_LogRecordEnum::EnumTypeInfo,
	&pmod::PropertyModelSetProperty_LogRecordEnum::EnumTypeInfo,
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
