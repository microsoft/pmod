/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionClassInfoCache.h
****/

#pragma once

#include "TypeInfoCache.h"
#include "ObservableCollectionInfo.h"


//------------------------------------------------------------------------------
// Class:   CCollectionModelClassInfoCache 
//
// Purpose: Define a class that allow caching of CollectionTypeInfo
//
//------------------------------------------------------------------------------

class CCollectionModelClassInfoCache:
    public _TStaticTypeInfoCache<CCollectionModelClassInfoCache,CCollectionModelTypeInfo>
{
public:
    static CCollectionModelClassInfoCache::_TypeModelInfoCacheType& GetInstance();

};


