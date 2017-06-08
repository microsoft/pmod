/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ErrorsTypeInfo.g.cpp
****/

#include <pmod/errors.h>
#include <pmod/library/library_util.h> 





const IID pmod::ErrorsEnum::IIDType = {0xe6147f75,0xfa3b,0x126a,{0xf4,0xe7,0xf9,0xe2,0x4b,0xe7,0x7b,0x03}};

const foundation::EnumTypeInfo pmod::ErrorsEnum::EnumTypeInfo(
	TypeId,
	IIDType,
	U("pmod.Errors"),
	false,
	&Names[0],
	&Values[0],
	nullptr
	);

const LPCSTR_t pmod::ErrorsEnum::Names[] = {
	U("E_COLLECTION_READ_ONLY"),
	U("E_EVENT_NOT_DEFINED"),
	U("E_METHOD_NOT_DEFINED"),
	U("E_COMMAND_NOT_ENABLED"),
	U("E_PROXY_DISPOSED"),
	U("E_PROXY_NO_SOURCE"),
	U("E_PROXY_NO_FACTORY"),
	nullptr
};

const UINT32 pmod::ErrorsEnum::Values[] = {
	0x80f70100,
	0x80f70101,
	0x80f70102,
	0x80f70103,
	0x80f70104,
	0x80f70105,
	0x80f70106,
	0x80f70107,
	0x80f70400,
	0x80f70401,
	0x80f70402,
	(UINT32)-1
};


static const foundation::TypeInfo* _typeInfoEntries [] = {
	&pmod::ErrorsEnum::EnumTypeInfo,

    nullptr
};

HRESULT  STDAPICALLTYPE register_core_errors_type_info(void)
{
    return foundation::library::RegisterMultipleTypeInfos(_typeInfoEntries);
}
