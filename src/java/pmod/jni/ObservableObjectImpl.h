/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_PropertyModelImpl_PropertyModelImpl
#define _Included_com_microsoft_PropertyModelImpl_PropertyModelImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_PropertyModelImpl_PropertyModelImpl
 * Method:    setPropertyById
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_setPropertyById
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     com_microsoft_PropertyModelImpl_PropertyModelImpl
 * Method:    getPropertyById
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_getPropertyById
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_microsoft_PropertyModelImpl_PropertyModelImpl
 * Method:    setPropertyByName
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_setPropertyByName
  (JNIEnv *, jobject, jstring, jobject);

/*
 * Class:     com_microsoft_PropertyModelImpl_PropertyModelImpl
 * Method:    getPropertyByName
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_getPropertyByName
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_microsoft_PropertyModelImpl_PropertyModelImpl
 * Method:    Invoke
 * Signature: (I[Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_Invoke
  (JNIEnv *, jobject, jint, jobjectArray);

/*
 * Class:     com_microsoft_PropertyModelImpl_PropertyModelImpl
 * Method:    createListenerProxy
 * Signature: (Lcom/microsoft/pmod/EventSink;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_createListenerProxy
  (JNIEnv *, jobject, jobject);

JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_createEventModelListenerProxy
(JNIEnv *env, jobject _this, jobject javaSinkObj);

#ifdef __cplusplus
}
#endif
#endif
