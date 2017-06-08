/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterLoggerTypeInfo.h
****/

#pragma once


#include <foundation/pal.h> 
namespace foundation {
struct EnumTypeInfo;
}


namespace nsadapter { 

	enum NSAdapter_Category
	{
	};


	enum NSFoundation_Category
	{
		NSFoundation_Category_AddObserver = 0,
		NSFoundation_Category_RemoveObserver = 1,
	};

    const UINT32 NSFoundation_Category_AddObserver_Id = 0x5510000;
    const UINT32 NSFoundation_Category_RemoveObserver_Id = 0x5510001;

	enum AddObserver_LogRecord
	{
		AddObserver_LogRecord_UniqueId = 0,
		AddObserver_LogRecord_KeyPath = 1,
		AddObserver_LogRecord_Options = 2,
	};

    const UINT32 AddObserver_LogRecord_UniqueId_Id = 0x5520000;
    const UINT32 AddObserver_LogRecord_KeyPath_Id = 0x5520001;
    const UINT32 AddObserver_LogRecord_Options_Id = 0x5520002;

	enum RemoveObserver_LogRecord
	{
		RemoveObserver_LogRecord_UniqueId = 0,
		RemoveObserver_LogRecord_KeyPath = 1,
	};

    const UINT32 RemoveObserver_LogRecord_UniqueId_Id = 0x5530000;
    const UINT32 RemoveObserver_LogRecord_KeyPath_Id = 0x5530001;

	enum NSObjectBase_Category
	{
		NSObjectBase_Category_FinalRelease = 0,
	};

    const UINT32 NSObjectBase_Category_FinalRelease_Id = 0x5540000;

	enum FinalRelease_LogRecord
	{
		FinalRelease_LogRecord_UniqueId = 0,
	};

    const UINT32 FinalRelease_LogRecord_UniqueId_Id = 0x5550000;

	enum NSPropertyModel_Category
	{
		NSPropertyModel_Category_FireNotifyPropertyChanged = 0,
	};

    const UINT32 NSPropertyModel_Category_FireNotifyPropertyChanged_Id = 0x5560000;

	enum NSCollectionModel_Category
	{
		NSCollectionModel_Category_FireNotifyCollectionChanged = 0,
	};

    const UINT32 NSCollectionModel_Category_FireNotifyCollectionChanged_Id = 0x5570000;

	enum NSFactory_Category
	{
		NSFactory_Category_AddCache = 0,
		NSFactory_Category_RemoveCache = 1,
		NSFactory_Category_RegisterAdapterClass = 2,
	};

    const UINT32 NSFactory_Category_AddCache_Id = 0x5580000;
    const UINT32 NSFactory_Category_RemoveCache_Id = 0x5580001;
    const UINT32 NSFactory_Category_RegisterAdapterClass_Id = 0x5580002;

	enum AddCache_LogRecord
	{
		AddCache_LogRecord_UniqueId = 0,
		AddCache_LogRecord_TypeIId = 1,
		AddCache_LogRecord_Name = 2,
	};

    const UINT32 AddCache_LogRecord_UniqueId_Id = 0x5590000;
    const UINT32 AddCache_LogRecord_TypeIId_Id = 0x5590001;
    const UINT32 AddCache_LogRecord_Name_Id = 0x5590002;

	enum RemoveCache_LogRecord
	{
		RemoveCache_LogRecord_UniqueId = 0,
	};

    const UINT32 RemoveCache_LogRecord_UniqueId_Id = 0x55a0000;

	enum RegisterAdapterClass_LogRecord
	{
		RegisterAdapterClass_LogRecord_ClassName = 0,
		RegisterAdapterClass_LogRecord_TypeId = 1,
	};

    const UINT32 RegisterAdapterClass_LogRecord_ClassName_Id = 0x55b0000;
    const UINT32 RegisterAdapterClass_LogRecord_TypeId_Id = 0x55b0001;


	// uuid(0b67f789-6c2c-0932-0f07-81d75b206cf2)
	struct NSAdapter_CategoryEnum
	{
		static const UINT32 TypeId = 0x550;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(39196cb5-54d8-da7e-db65-d46a561a92ce)
	struct NSFoundation_CategoryEnum
	{
		static const UINT32 TypeId = 0x551;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(8d862691-3371-0064-b658-b1bb6575c8ad)
	struct AddObserver_LogRecordEnum
	{
		static const UINT32 TypeId = 0x552;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(2d65c5db-a66a-716b-faff-06f3f793eb05)
	struct RemoveObserver_LogRecordEnum
	{
		static const UINT32 TypeId = 0x553;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(dc4e2d4a-9775-d1a5-f444-9d8e57afcae2)
	struct NSObjectBase_CategoryEnum
	{
		static const UINT32 TypeId = 0x554;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(5669149a-66b4-936d-ad6b-c242a3cefeac)
	struct FinalRelease_LogRecordEnum
	{
		static const UINT32 TypeId = 0x555;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(2ba8ba24-d726-d8cc-0a93-e833ecd0a900)
	struct NSPropertyModel_CategoryEnum
	{
		static const UINT32 TypeId = 0x556;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(fcdd3bd0-ca97-7932-1c00-fc1ea909b7eb)
	struct NSCollectionModel_CategoryEnum
	{
		static const UINT32 TypeId = 0x557;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(1acdab92-c511-c847-076f-dd7506da6aad)
	struct NSFactory_CategoryEnum
	{
		static const UINT32 TypeId = 0x558;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(803e5d22-b628-3d91-3b70-8eb557ca7c71)
	struct AddCache_LogRecordEnum
	{
		static const UINT32 TypeId = 0x559;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(8948c52b-686c-5bf9-9686-ceae0dab8ace)
	struct RemoveCache_LogRecordEnum
	{
		static const UINT32 TypeId = 0x55a;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};

	// uuid(845bea56-ea44-5522-ba01-07ed619f61f1)
	struct RegisterAdapterClass_LogRecordEnum
	{
		static const UINT32 TypeId = 0x55b;
		static  const IID IIDType;
		static  const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};


} 
