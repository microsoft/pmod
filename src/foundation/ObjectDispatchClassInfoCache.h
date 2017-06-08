/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchClassInfoCache.h
****/

#pragma once

#include "TypeInfoCache.h"
#include "ObjectDispatchInfo.h"

//------------------------------------------------------------------------------
// Class:   CObjectDispatchClassInfoCache 
//
// Purpose: Define a class that allow caching of ObjectDispatchTypeInfo
//
//------------------------------------------------------------------------------
class CObjectDispatchClassInfoCache :
	public _TStaticTypeInfoCache<CObjectDispatchClassInfoCache, CObjectDispatchInfoClass>
{
public:
	static _TypeModelInfoCacheType& GetInstance();
};
