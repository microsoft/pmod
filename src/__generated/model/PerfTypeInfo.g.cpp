/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PerfTypeInfo.g.cpp
****/

#include <pmod/perf_type_info.h> 
#include <pmod/library/library_util.h> 





const IID pmod::Core_PerfIdEnum::IIDType = {0x08c4cd53,0x8542,0xb912,{0xc7,0x0f,0xb0,0xad,0x4e,0xbb,0x06,0x9b}};

const foundation::EnumTypeInfo pmod::Core_PerfIdEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.Core_PerfId"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::Core_PerfIdEnum::Names[] = {
	U("Object_GetContent"),
	U("PropertyModel_GetProperty"),
	U("PropertyModel_SetProperty"),
	U("PropertyModel_SetPropertyInternal"),
	U("PropertyModel_Invoke"),
	U("PropertyModel_InvokeCallback"),
	U("PropertyModel_GetProperties"),
	U("PropertyModel_FirePropertyChanged"),
	U("PropertyModel_FireBatchPropertyChanged"),
	U("PropertyModel_FireEventModel"),
	U("CollectionModel_GetCount"),
	U("CollectionModel_GetItem"),
	U("CollectionModel_FireNotifyCollectionChanged"),
	U("CommandModel_Execute"),
	U("CommandModel_ExecuteCallback"),
	U("CommandModel_CanExecute"),
	U("CommandModel_FireCanExecuteChanged"),
	U("BindingBase_GetValue"),
	U("BindingBase_FireValueChanged"),
	nullptr
};

const UINT32 pmod::Core_PerfIdEnum::Values[] = {
	0x300,
	769,
	770,
	771,
	772,
	773,
	774,
	775,
	776,
	777,
	778,
	779,
	780,
	781,
	782,
	783,
	784,
	785,
	786,
	(UINT32)-1
};


const IID pmod::ProxyFactory_PerfIdEnum::IIDType = {0x4a9ad9e7,0xef3a,0xb9ec,{0xa4,0x20,0x28,0xcc,0x1a,0x83,0x77,0xc2}};

const foundation::EnumTypeInfo pmod::ProxyFactory_PerfIdEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.ProxyFactory_PerfId"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::ProxyFactory_PerfIdEnum::Names[] = {
	U("Object_GetContent"),
	U("PropertyModel_GetProperty"),
	U("PropertyModel_SetProperty"),
	U("PropertyModel_Invoke"),
	U("PropertyModel_GetProperties"),
	U("CollectionModel_GetCount"),
	U("CollectionModel_GetItem"),
	U("CommandModel_Execute"),
	U("CommandModel_CanExecute"),
	nullptr
};

const UINT32 pmod::ProxyFactory_PerfIdEnum::Values[] = {
	0x350,
	849,
	850,
	851,
	852,
	853,
	854,
	855,
	856,
	(UINT32)-1
};


const IID pmod::CoreProxyFactory_PerfIdEnum::IIDType = {0xeb78abaa,0x5af3,0x9451,{0x73,0x93,0x5e,0x5a,0xa1,0x64,0x76,0x74}};

const foundation::EnumTypeInfo pmod::CoreProxyFactory_PerfIdEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.CoreProxyFactory_PerfId"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::CoreProxyFactory_PerfIdEnum::Names[] = {
	U("Object_GetContent"),
	U("PropertyModel_GetProperty"),
	U("PropertyModel_SetProperty"),
	U("PropertyModel_Invoke"),
	U("PropertyModel_GetProperties"),
	U("CollectionModel_GetCount"),
	U("CollectionModel_GetItem"),
	U("CommandModel_Execute"),
	U("CommandModel_CanExecute"),
	nullptr
};

const UINT32 pmod::CoreProxyFactory_PerfIdEnum::Values[] = {
	0x3a0,
	929,
	930,
	931,
	932,
	933,
	934,
	935,
	936,
	(UINT32)-1
};


static const foundation::TypeInfo* _typeInfoEntries [] = {
	&pmod::Core_PerfIdEnum::EnumTypeInfo,
	&pmod::ProxyFactory_PerfIdEnum::EnumTypeInfo,
	&pmod::CoreProxyFactory_PerfIdEnum::EnumTypeInfo,

    nullptr
};

HRESULT  STDAPICALLTYPE register_perf_logger_type_info(void)
{
    return foundation::library::RegisterMultipleTypeInfos(_typeInfoEntries);
}
