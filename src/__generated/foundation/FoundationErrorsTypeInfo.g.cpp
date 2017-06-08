/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationErrorsTypeInfo.g.cpp
****/

#include <foundation/errors.h> 
#include <foundation/library/library_util.h>

const IID foundation::ErrorsEnum::IIDType = { 0x49ab6c91,0x5231,0x9c4a,{ 0xdb,0x1a,0x6a,0x32,0x87,0xcf,0x58,0xb6 } };
const foundation::EnumTypeInfo foundation::ErrorsEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("foundation.FoundationErrors"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t foundation::ErrorsEnum::Names[] = {
	U("S_OK_"),
	U("S_FALSE_"),
	U("E_UNEXPECTED_"),
	U("E_NOTIMPL_"),
	U("E_OUTOFMEMORY_"),
	U("E_INVALIDARG_"),
	U("E_NOINTERFACE_"),
	U("E_POINTER_"),
	U("E_HANDLE_"),
	U("E_ABORT_"),
	U("E_FAIL_"),
	U("E_ACCESSDENIED_"),
	U("E_BOUNDS_"),
	U("E_KEY_NOT_FOUND"),
	U("E_ARRAY_EXPECTED"),
	U("E_WRONG_TYPE"),
	U("E_INVALID_GUID"),
	U("E_INVALID_ENUM_VALUE"),
	U("E_INVALID_ENUM_NAME"),
	U("E_INVALID_ENUM_TYPE_INFO"),
	U("E_OBJECT_ENTRY_NOTFOUND"),
	U("E_TYPE_INFO_NOT_REGISTERED"),
	U("E_TYPE_INFO_ALREADY_REGISTERED"),
	U("E_TYPE_INFO_ENUM_NOT_REGISTERED"),
	U("E_ASYNC_OPERATION_CANCELED"),
	U("E_ASYNC_OPERATION_TIMEOUT"),
	U("E_EVENT_HANDLER_EXIST"),
	U("E_EVENT_HANDLER_NOT_FOUND"),
	U("E_PV_E_WRONG_TYPE"),
	U("E_PV_E_CAST_FAILED"),
	U("E_PROPERTY_NOT_FOUND"),
	U("E_PROPERTY_NOT_DEFINED"),
	U("E_PROPERTY_READ_ONLY"),
	U("E_PROPERTY_EXIST"),
	U("E_METHOD_NOT_DEFINED"),
	U("E_TYPEMISMATCH"),
	U("RPC_E_SERVER_NOT_AVAILABLE"),
	nullptr
};

const UINT32 foundation::ErrorsEnum::Values[] = {
	0,
	1,
	0x8000ffff,
	0x80004001,
	0x8007000e,
	0x80070057,
	0x80004002,
	0x80004003,
	0x80070006,
	0x80004004,
	0x80004005,
	0x80070005,
	0x8000000b,
	0x80f60100,
	0x80f60101,
	0x80f60102,
	0x80f60103,
	0x80f60108,
	0x80f60109,
	0x80f6010a,
	0x80f6010b,
	0x80f60300,
	0x80f60301,
	0x80f60302,
	0x80f60305,
	0x80f60306,
	0x80f60200,
	0x80f60201,
	0x80f80100,
	0x80f80101,
	0x80f60310,
	0x80f60311,
	0x80f60312,
	0x80f60313,
	0x80f60314,
	0x80028ca0,
	0x800706ba,
	(UINT32)-1
};


static const foundation::TypeInfo* _typeInfoEntries [] = {
	&foundation::ErrorsEnum::EnumTypeInfo,

    nullptr
};

HRESULT  STDAPICALLTYPE register_foundation_errors_type_info(void)
{
    return foundation::library::RegisterMultipleTypeInfos(_typeInfoEntries);
}
