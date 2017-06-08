/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:errors.h
****/

#pragma once

#include <foundation/interfaces.h> 

namespace foundation { 

    struct EnumTypeInfo;

	enum Errors
	{
		S_OK_ = 0,
		S_FALSE_ = 1,
		E_UNEXPECTED_ = (int)0x8000ffff,
		E_NOTIMPL_ = (int)0x80004001,
		E_OUTOFMEMORY_ = (int)0x8007000e,
		E_INVALIDARG_ = (int)0x80070057,
		E_NOINTERFACE_ = (int)0x80004002,
		E_POINTER_ = (int)0x80004003,
		E_HANDLE_ = (int)0x80070006,
		E_ABORT_ = (int)0x80004004,
		E_FAIL_ = (int)0x80004005,
		E_ACCESSDENIED_ = (int)0x80070005,
		E_BOUNDS_ = (int)0x8000000b,
		E_KEY_NOT_FOUND = (int)0x80f60100,
		E_ARRAY_EXPECTED = (int)0x80f60101,
		E_WRONG_TYPE = (int)0x80f60102,
		E_INVALID_GUID = (int)0x80f60103,
		E_INVALID_ENUM_VALUE = (int)0x80f60108,
		E_INVALID_ENUM_NAME = (int)0x80f60109,
		E_INVALID_ENUM_TYPE_INFO = (int)0x80f6010a,
		E_OBJECT_ENTRY_NOTFOUND = (int)0x80f6010b,
		E_TYPE_INFO_NOT_REGISTERED = (int)0x80f60300,
		E_TYPE_INFO_ALREADY_REGISTERED = (int)0x80f60301,
		E_TYPE_INFO_ENUM_NOT_REGISTERED = (int)0x80f60302,
		E_ASYNC_OPERATION_CANCELED = (int)0x80f60305,
		E_ASYNC_OPERATION_TIMEOUT = (int)0x80f60306,
		E_EVENT_HANDLER_EXIST = (int)0x80f60200,
		E_EVENT_HANDLER_NOT_FOUND = (int)0x80f60201,
		E_PV_E_WRONG_TYPE = (int)0x80f80100,
		E_PV_E_CAST_FAILED = (int)0x80f80101,
		E_PROPERTY_NOT_FOUND = (int)0x80f60310,
		E_PROPERTY_NOT_DEFINED = (int)0x80f60311,
		E_PROPERTY_READ_ONLY = (int)0x80f60312,
		E_PROPERTY_EXIST = (int)0x80f60313,
		E_METHOD_NOT_DEFINED = (int)0x80f60314,
		E_TYPEMISMATCH = (int)0x80028ca0,
		RPC_E_SERVER_NOT_AVAILABLE = (int)0x800706ba,
	};


	// uuid(49ab6c91-5231-9c4a-db1a-6a3287cf58b6)
	struct ErrorsEnum
	{
		static const UINT32 TypeId = _ReservedTypeId_Base + 0;
		static FOUNDATION_API const IID IIDType;
		static FOUNDATION_API const foundation::EnumTypeInfo EnumTypeInfo;
		static const LPCSTR_t Names[];
		static const UINT32  Values[];
	};


} 
