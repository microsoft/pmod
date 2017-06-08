/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:errors.h
****/

#pragma once 

#include <pmod/interfaces.h> 

namespace foundation {
    struct EnumTypeInfo;
}

namespace pmod { 

enum Errors
{
	E_COLLECTION_READ_ONLY = 0x80f70103,
	E_EVENT_NOT_DEFINED = 0x80f70104,
	E_METHOD_NOT_DEFINED = 0x80f70105,
	E_COMMAND_NOT_ENABLED = 0x80f70107,
	E_PROXY_DISPOSED = 0x80f70400,
	E_PROXY_NO_SOURCE = 0x80f70401,
	E_PROXY_NO_FACTORY = 0x80f70402,
};


// uuid(a8cd00ad-94ad-3659-9d76-d2c38ab0b552)
struct ErrorsEnum
{
    static const UINT32 TypeId = foundation::_ReservedTypeId_Base + 1;
    static PMOD_API const IID IIDType;
    static PMOD_API const foundation::EnumTypeInfo EnumTypeInfo;
    static const LPCSTR_t Names[];
    static const UINT32  Values[];
};


} 
