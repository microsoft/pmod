/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandClassInfoCache.h
****/

#pragma once
#include "TypeInfoCache.h"
#include "CommandInfo.h"


//------------------------------------------------------------------------------
// Class:   CCommandModelClassInfoCache 
//
// Purpose: Define a class that allow caching of CommandTypeInfo
//
//------------------------------------------------------------------------------

class CCommandModelClassInfoCache:
    public _TStaticTypeInfoCache<CCommandModelClassInfoCache, CCommandModelTypeInfo>
{
public:
    static CCommandModelClassInfoCache::_TypeModelInfoCacheType& GetInstance();
};
