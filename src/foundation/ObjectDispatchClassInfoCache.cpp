/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchClassInfoCache.cpp
****/
#include "pch.h"

#include "ObjectDispatchClassInfoCache.h"
#include "PtrSingleton.h"

CObjectDispatchClassInfoCache::_TypeModelInfoCacheType& CObjectDispatchClassInfoCache::GetInstance()
{
	return *_GetPtrInstance<CObjectDispatchClassInfoCache::_TypeModelInfoCacheType>();
}

