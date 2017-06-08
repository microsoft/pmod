/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumTypeClassInfoCache.h
****/

#pragma once

#include "TypeInfoCache.h"
#include "EnumTypeInfo.h"

//------------------------------------------------------------------------------
// Class:   CEnumTypeClassInfoCache 
//
// Purpose: Define a class that allow caching of EnumTypeInfo
//
//------------------------------------------------------------------------------

class CEnumTypeClassInfoCache:
    public _TStaticTypeInfoCache<CEnumTypeClassInfoCache, CEnumTypeInfo>
{
public:
    static CEnumTypeClassInfoCache::_TypeModelInfoCacheType& GetInstance();

    // register a enum type info in my cache
	static HRESULT RegisterEnumTypeInfo(_In_ const foundation::EnumTypeInfo *pEnumTypeInfo);

    static HRESULT LookupEnumValue(UINT32 enumValue, foundation::IEnumValue **ppEnumValue);

    // support for external callback logger type info
    static HRESULT register_enum_factory_type_info(foundation::library::IDelegateDispatchable *pCallback);

    // Note : this function does not add a ref 
    static HRESULT GetOrCreateEnumTypeInfo(
        _In_ const foundation::EnumTypeInfo *pEnumTypeInfo,
        _Outptr_ CEnumTypeInfo **ppEnumTypeInfoClass);
private:
    static _Dispatchable_ptr_vector_type& GetCallbacks();

};


