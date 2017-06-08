/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:perf_type_info.h
****/

#pragma once


#include <pmod/interfaces.h> 
#include <pmod/type_info_declare.h> 


namespace pmod { 

	enum class Core_PerfId
	{
		Object_GetContent = 0x300,
		PropertyModel_GetProperty = 769,
		PropertyModel_SetProperty = 770,
		PropertyModel_SetPropertyInternal = 771,
		PropertyModel_Invoke = 772,
		PropertyModel_InvokeCallback = 773,
		PropertyModel_GetProperties = 774,
		PropertyModel_FirePropertyChanged = 775,
		PropertyModel_FireBatchPropertyChanged = 776,
		PropertyModel_FireEventModel = 777,
		CollectionModel_GetCount = 778,
		CollectionModel_GetItem = 779,
		CollectionModel_FireNotifyCollectionChanged = 780,
		CommandModel_Execute = 781,
		CommandModel_ExecuteCallback = 782,
		CommandModel_CanExecute = 783,
		CommandModel_FireCanExecuteChanged = 784,
		BindingBase_GetValue = 785,
		BindingBase_FireValueChanged = 786,
	};

    const UINT32 Core_PerfId_Object_GetContent_Id = 0x4500300;
    const UINT32 Core_PerfId_PropertyModel_GetProperty_Id = 0x4500301;
    const UINT32 Core_PerfId_PropertyModel_SetProperty_Id = 0x4500302;
    const UINT32 Core_PerfId_PropertyModel_SetPropertyInternal_Id = 0x4500303;
    const UINT32 Core_PerfId_PropertyModel_Invoke_Id = 0x4500304;
    const UINT32 Core_PerfId_PropertyModel_InvokeCallback_Id = 0x4500305;
    const UINT32 Core_PerfId_PropertyModel_GetProperties_Id = 0x4500306;
    const UINT32 Core_PerfId_PropertyModel_FirePropertyChanged_Id = 0x4500307;
    const UINT32 Core_PerfId_PropertyModel_FireBatchPropertyChanged_Id = 0x4500308;
    const UINT32 Core_PerfId_PropertyModel_FireEventModel_Id = 0x4500309;
    const UINT32 Core_PerfId_CollectionModel_GetCount_Id = 0x450030a;
    const UINT32 Core_PerfId_CollectionModel_GetItem_Id = 0x450030b;
    const UINT32 Core_PerfId_CollectionModel_FireNotifyCollectionChanged_Id = 0x450030c;
    const UINT32 Core_PerfId_CommandModel_Execute_Id = 0x450030d;
    const UINT32 Core_PerfId_CommandModel_ExecuteCallback_Id = 0x450030e;
    const UINT32 Core_PerfId_CommandModel_CanExecute_Id = 0x450030f;
    const UINT32 Core_PerfId_CommandModel_FireCanExecuteChanged_Id = 0x4500310;
    const UINT32 Core_PerfId_BindingBase_GetValue_Id = 0x4500311;
    const UINT32 Core_PerfId_BindingBase_FireValueChanged_Id = 0x4500312;

	enum class ProxyFactory_PerfId
	{
		Object_GetContent = 0x350,
		PropertyModel_GetProperty = 849,
		PropertyModel_SetProperty = 850,
		PropertyModel_Invoke = 851,
		PropertyModel_GetProperties = 852,
		CollectionModel_GetCount = 853,
		CollectionModel_GetItem = 854,
		CommandModel_Execute = 855,
		CommandModel_CanExecute = 856,
	};

    const UINT32 ProxyFactory_PerfId_Object_GetContent_Id = 0x4510350;
    const UINT32 ProxyFactory_PerfId_PropertyModel_GetProperty_Id = 0x4510351;
    const UINT32 ProxyFactory_PerfId_PropertyModel_SetProperty_Id = 0x4510352;
    const UINT32 ProxyFactory_PerfId_PropertyModel_Invoke_Id = 0x4510353;
    const UINT32 ProxyFactory_PerfId_PropertyModel_GetProperties_Id = 0x4510354;
    const UINT32 ProxyFactory_PerfId_CollectionModel_GetCount_Id = 0x4510355;
    const UINT32 ProxyFactory_PerfId_CollectionModel_GetItem_Id = 0x4510356;
    const UINT32 ProxyFactory_PerfId_CommandModel_Execute_Id = 0x4510357;
    const UINT32 ProxyFactory_PerfId_CommandModel_CanExecute_Id = 0x4510358;

	enum class CoreProxyFactory_PerfId
	{
		Object_GetContent = 0x3a0,
		PropertyModel_GetProperty = 929,
		PropertyModel_SetProperty = 930,
		PropertyModel_Invoke = 931,
		PropertyModel_GetProperties = 932,
		CollectionModel_GetCount = 933,
		CollectionModel_GetItem = 934,
		CommandModel_Execute = 935,
		CommandModel_CanExecute = 936,
	};

    const UINT32 CoreProxyFactory_PerfId_Object_GetContent_Id = 0x45203a0;
    const UINT32 CoreProxyFactory_PerfId_PropertyModel_GetProperty_Id = 0x45203a1;
    const UINT32 CoreProxyFactory_PerfId_PropertyModel_SetProperty_Id = 0x45203a2;
    const UINT32 CoreProxyFactory_PerfId_PropertyModel_Invoke_Id = 0x45203a3;
    const UINT32 CoreProxyFactory_PerfId_PropertyModel_GetProperties_Id = 0x45203a4;
    const UINT32 CoreProxyFactory_PerfId_CollectionModel_GetCount_Id = 0x45203a5;
    const UINT32 CoreProxyFactory_PerfId_CollectionModel_GetItem_Id = 0x45203a6;
    const UINT32 CoreProxyFactory_PerfId_CommandModel_Execute_Id = 0x45203a7;
    const UINT32 CoreProxyFactory_PerfId_CommandModel_CanExecute_Id = 0x45203a8;


	// uuid(08c4cd53-8542-b912-c70f-b0ad4ebb069b)
	struct Core_PerfIdEnum
	{
		static const UINT32 TypeId = 0x450;
		static PMOD_API const IID IIDType;
		static PMOD_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(4a9ad9e7-ef3a-b9ec-a420-28cc1a8377c2)
	struct ProxyFactory_PerfIdEnum
	{
		static const UINT32 TypeId = 0x451;
		static PMOD_API const IID IIDType;
		static PMOD_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(eb78abaa-5af3-9451-7393-5e5aa1647674)
	struct CoreProxyFactory_PerfIdEnum
	{
		static const UINT32 TypeId = 0x452;
		static PMOD_API const IID IIDType;
		static PMOD_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};


} 
