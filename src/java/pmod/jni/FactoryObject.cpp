/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FactoryObject.cpp
****/
#include "FactoryObject.h"

#include "JniCoreUtils.h"
#include "JniConnection.h"

using namespace pmod;

JNIEXPORT void JNICALL Java_com_microsoft_pmod_FactoryObject_releaseListenerProxy
    (JNIEnv *, jclass, jlong pEventListenerProxy)
{
    foundation_assert(pEventListenerProxy);
    ((foundation::IUnknown *)pEventListenerProxy)->Release();
}
