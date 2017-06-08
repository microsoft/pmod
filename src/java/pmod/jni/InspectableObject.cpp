/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InspectableObject.cpp
****/
#include "InspectableObject.h"

#include "JniCoreUtils.h"


JNIEXPORT void JNICALL Java_com_microsoft_pmod_InspectableObject_release
    (JNIEnv *env, jobject _this)
{
    foundation::IInspectable *pInspectable = extractInterface(env,_this);
    foundation_assert(pInspectable);
    if(pInspectable)
    {
        pInspectable->Release();
    }
    initInterface(env,_this,nullptr);
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_InspectableObject_createObjectInternal
(JNIEnv *env, jclass, jlong pInterface)
{
    return returnValue(env, reinterpret_cast<foundation::IInspectable *>(pInterface));
}
