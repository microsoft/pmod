/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationObjectImpl.cpp
****/
#include "FoundationObjectImpl.h"

#include "JniCoreUtils.h"

using namespace foundation;

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_FoundationObjectImpl_getUniqueId
    (JNIEnv *env, jobject _this)
{
    IObject *pObject = extractInterfaceExpectedAndCast<IObject>(env, _this);
    if (!pObject)
    {
        return 0;
    }
    UINT32 uniqueId;
    pObject->GetUniqueId(&uniqueId);
    return (jint)uniqueId;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_FoundationObjectImpl_getTypeInfo
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
