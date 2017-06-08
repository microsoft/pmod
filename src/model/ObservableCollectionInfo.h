/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionInfo.h
****/

#pragma once

#include <pmod/library/model_library.h>
#include "EnumerableTypeInfo.h"

#include <foundation/com_ptr.h>

//------------------------------------------------------------------------------
// Class:   CCollectionModelTypeInfo
//
// Purpose: A concrete implementation for IObservableCollectionInfo
//
//------------------------------------------------------------------------------
typedef CEnumerableTypeInfo CCollectionModelTypeInfo_BaseType;

class CCollectionModelTypeInfo :
    public CCollectionModelTypeInfo_BaseType
{
 public:
    static HRESULT CreateInstance(
        _In_ const foundation::EnumerableTypeInfo *pCollectionModelTypeInfo,
        _Outptr_ CCollectionModelTypeInfo **ppCollectionModelInfo);
};



