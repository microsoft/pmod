/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumerableClassInfoCache.h
****/

#pragma once

#include "TypeInfoCache.h"
#include "EnumerableTypeInfo.h"


//------------------------------------------------------------------------------
// Class:   CEnumerableClassInfoCache 
//
// Purpose: Define a class that allow caching of CollectionTypeInfo
//
//------------------------------------------------------------------------------

class CEnumerableClassInfoCache :
    public _TStaticTypeInfoCache<CEnumerableClassInfoCache,CEnumerableTypeInfo>
{
public:
    static CEnumerableClassInfoCache::_TypeModelInfoCacheType& GetInstance();

};


