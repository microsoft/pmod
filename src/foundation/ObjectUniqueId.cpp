/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectUniqueId.cpp
****/

#include "pch.h"

static UINT32 NextObjectUniqueId()
{
    static _pal_AtomicIntegerType _objectId = 0;

    return _pal_InterlockedIncrement(&_objectId);
}

EXTERN_C FOUNDATION_API UINT32 STDAPICALLTYPE GetNextObjectUniqueId()
{
    return NextObjectUniqueId();
}
