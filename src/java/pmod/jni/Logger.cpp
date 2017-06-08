/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Logger.cpp
****/
#include "JniCoreUtils.h"

// foundation public headers
#include <foundation/library/logger_util.h>
using namespace foundation;

extern "C" {
    JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_Logger_getRootCategory
        (JNIEnv *env, jobject _this);

    JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_LoggerUtil_getFoundationLogger
        (JNIEnv *env, jclass);

    JNIEXPORT jint JNICALL Java_com_microsoft_pmod_LogCategory_GetLevel
        (JNIEnv *env, jobject _this);
    JNIEXPORT void JNICALL Java_com_microsoft_pmod_LogCategory_SetLevel
        (JNIEnv *env, jobject _this, jint level);
}


JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_Logger_getRootCategory
(JNIEnv *env, jobject _this)
{
    ILogger *pLogger = extractInterfaceExpectedAndCast<ILogger>(env, _this);
    if (!pLogger)
    {
        return (jobject)nullptr;
    }
    foundation::ComPtr<ILogCategory> spLogCategory;
    pLogger->GetRootCategory(spLogCategory.GetAddressOf());
    return createInspectableObject(env, "com/microsoft/pmod/LogCategory", spLogCategory);
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_LoggerUtil_getFoundationLogger
(JNIEnv *env, jclass)
{
    return createInspectableObject(env, "com/microsoft/pmod/Logger", logger_util::GetFoundationLogger());
}

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_LogCategory_GetLevel
(JNIEnv *env, jobject _this)
{
    ILogCategory *pLogCategory = extractInterfaceExpectedAndCast<ILogCategory>(env, _this);
    if (!pLogCategory)
    {
        return -1;
    }
    LoggingLevel level;
    pLogCategory->GetLevel(&level);
    return (jint)level;
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_LogCategory_SetLevel
(JNIEnv *env, jobject _this,jint level)
{
    ILogCategory *pLogCategory = extractInterfaceExpectedAndCast<ILogCategory>(env, _this);
    pLogCategory->SetLevel((LoggingLevel)level);
}
