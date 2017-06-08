/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationObject.cpp
****/
#include "FoundationObject.h"

#include "jniCoreUtils.h"

using namespace foundation;

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_FoundationObject_getUniqueId
    (JNIEnv *env, jobject _this)
{
    IObject *pObject = extractInterfaceExpectedAndCast<IObject>(env, _this);
    if (!pObject)
    {
        return 0;
    }
    ULONG uniqueId;
    pObject->GetUniqueId(&uniqueId);
    return (jint)uniqueId;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_FoundationObject_getTypeInfo
    (JNIEnv *env, jobject _this)
{
    IObject *pObject = extractInterfaceExpectedAndCast<IObject>(env, _this);
    if (!pObject)
    {
        return (jobject)nullptr;
    }
    foundation::IObjectTypeInfo *pTypeInfo = nullptr;
    pObject->GetTypeInfo(&pTypeInfo);
    if (pTypeInfo)
    {
        return returnObjectTypeInfo(env, pTypeInfo);
    }
    return (jobject)nullptr;
}
