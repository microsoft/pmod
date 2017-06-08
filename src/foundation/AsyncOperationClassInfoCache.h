/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationClassInfoCache.h
****/

#pragma once

#include "TypeInfoCache.h"
#include "AsyncOperationTypeInfo.h"

//------------------------------------------------------------------------------
// Class:   CAsyncOperationClassInfoCache 
//
// Purpose: Define a class that allow caching of AsyncOperationModelInfo
//
//------------------------------------------------------------------------------

class CAsyncOperationClassInfoCache:
    public _TStaticTypeInfoCache
    <
        CAsyncOperationClassInfoCache,
        CAsyncOperationTypeInfo
    >
{
public:
    static CAsyncOperationClassInfoCache::_TypeModelInfoCacheType& GetInstance();

    // lookup an Async Operation Type Info by it's Result Type Info
    static CAsyncOperationTypeInfo *LookupTypeByPropertyTypeInfo(foundation::IPropertyTypeInfo *pPropertyTypeInfo)
    {
        return GetInstanceInternal().LookupTypeByPropertyTypeInfoInternal(pPropertyTypeInfo);
    }

    static void AddToResultTypeInfoCache(CAsyncOperationTypeInfo *pTypeModelClassInfo)
    {
        return GetInstanceInternal().AddToResultTypeInfoCacheInternal(pTypeModelClassInfo);
    }
private:
    static CAsyncOperationClassInfoCache& GetInstanceInternal();

    void AddToResultTypeInfoCacheInternal(CAsyncOperationTypeInfo *pTypeModelClassInfo);
    CAsyncOperationTypeInfo *LookupTypeByPropertyTypeInfoInternal(foundation::IPropertyTypeInfo *pPropertyTypeInfo);

    typedef std::map<foundation::PropertyType, CAsyncOperationTypeInfo *> _ResultPropertyTypeClassInfoMap;
    typedef std::map<IID, CAsyncOperationTypeInfo *, foundation::library::GuidLessThan> _ResultModelTypeClassInfoMap;

    _ResultPropertyTypeClassInfoMap  _resultPropertyTypeCache;
    _ResultModelTypeClassInfoMap     _resultModelTypeCache;
};


