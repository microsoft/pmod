/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Dictionary.cpp
****/

#include "JniCoreUtils.h"
#include <foundation/interfaces/dictionary.h>
#include <foundation/library/dictionary_class.h>

using namespace foundation;
// exported entry points
extern "C" {
    JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dictionary_createDictionary
        (JNIEnv *env, jobject _this);
    JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_Dictionary_hasKeyInternal
        (JNIEnv *env, jobject _this, jstring jkey);
    JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_Dictionary_lookupInternal
        (JNIEnv *env, jobject _this, jstring jkey);
    JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_Dictionary_insertInternal
        (JNIEnv *env, jobject _this, jstring jkey, jobject value);
    JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dictionary_removeInternal
        (JNIEnv *env, jobject _this, jstring jkey);
    JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dictionary_clearInternal
        (JNIEnv *env, jobject _this);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dictionary_createDictionary
(JNIEnv *env, jobject _this)
{
    IDictionary *pDictionary = nullptr;
    if (!ThrowIfComFail(
        env,
        library::CreateDictionaryClass(&pDictionary),
        "library::CreateDictionaryClass"))
    {
        initInterface(env, _this, pDictionary);
    }
}

JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_Dictionary_hasKeyInternal
(JNIEnv *env, jobject _this, jstring jkey)
{
    HStringPtr key = toHStringPtr(env, jkey);

    IDictionary *pDictionary = extractInterfaceExpectedAndCast<IDictionary>(env, _this);
    foundation::InspectablePtr valuePtr;
    bool found = false;
    ThrowIfComFail(
        env,
        pDictionary->HasKey(key, &found),
        "IDictionary::HasKey");
    return found ? (jboolean)1 : (jboolean)0;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_Dictionary_lookupInternal
    (JNIEnv *env, jobject _this,jstring jkey)
{
    HStringPtr key = toHStringPtr(env, jkey);

    IDictionary *pDictionary = extractInterfaceExpectedAndCast<IDictionary>(env, _this);
    foundation::InspectablePtr valuePtr;
    if (ThrowIfComFail(
        env,
        pDictionary->Lookup(key, valuePtr.GetAddressOf()),
        "IDictionary::Lookup"))
    {
        return (jobject)nullptr;
    }
    return returnValue(env, valuePtr);
}

JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_Dictionary_insertInternal
(JNIEnv *env, jobject _this, jstring jkey, jobject value)
{
    HStringPtr key = toHStringPtr(env, jkey);

    IDictionary *pDictionary = extractInterfaceExpectedAndCast<IDictionary>(env, _this);
    foundation::IInspectable *pValue = extractInterfaceIf(env, value);
    foundation::InspectablePtr valuePtr;
    bool replaced;
    ThrowIfComFail(
        env,
        pDictionary->Insert(key, pValue, &replaced),
        "IDictionary::Insert");
    return replaced ? (jboolean)1 : (jboolean)0;
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dictionary_removeInternal
(JNIEnv *env, jobject _this, jstring jkey)
{
    HStringPtr key = toHStringPtr(env, jkey);

    IDictionary *pDictionary = extractInterfaceExpectedAndCast<IDictionary>(env, _this);
    foundation::InspectablePtr valuePtr;
    ThrowIfComFail(
        env,
        pDictionary->Remove(key),
        "IDictionary::Remove");
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dictionary_clearInternal
(JNIEnv *env, jobject _this)
{
    IDictionary *pDictionary = extractInterfaceExpectedAndCast<IDictionary>(env, _this);
    foundation::InspectablePtr valuePtr;
    ThrowIfComFail(
        env,
        pDictionary->Clear(),
        "IDictionary::Clear");
}

