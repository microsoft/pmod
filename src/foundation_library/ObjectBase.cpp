/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectBase.cpp
****/

#include "pch.h"
#include "ObjectBase.h"

static _pal_AtomicIntegerType _countObjects = 0;

void OnObjectBaseInitialize(foundation::IObject *pObject)
{
    UNREFERENCED_PARAMETER(pObject);
    _pal_InterlockedIncrement(&_countObjects);
}

void OnObjectBaseReleased(foundation::IObject *pObject)
{
    UNREFERENCED_PARAMETER(pObject);
    _pal_InterlockedDecrement(&_countObjects);
}

UINT32 GetObjectBaseCount()
{
    return _pal_InterlockedCompareExchange(&_countObjects, 0, 0);
}

