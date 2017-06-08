/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_AsyncOperationImpl
#define _Included_com_microsoft_pmod_AsyncOperationImpl
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_microsoft_propertymodel_AsyncOperationImpl
 * Method:    GetResult
 * Signature: ()Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_GetResult
(JNIEnv *, jobject, jint);

/*
 * Class:     com_microsoft_propertymodel_AsyncOperationImpl
 * Method:    GetStatus
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_GetStatus
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_AsyncOperationImpl
 * Method:    createListenerProxy
 * Signature: (Lcom/microsoft/pmod/EventSink;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_createListenerProxy
  (JNIEnv *, jobject, jobject);
  
/*
 * Class:     com_microsoft_propertymodel_AsyncOperationImpl
 * Method:    Cancel
 * Signature: 
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_Cancel
  (JNIEnv *, jobject);
  
/*
 * Class:     com_microsoft_propertymodel_AsyncOperationImpl
 * Method:    Wait
 * Signature: 
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_Wait
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_microsoft_propertymodel_AsyncOperationImpl
 * Method:    getError
 * Signature: 
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_getError
  (JNIEnv *, jobject);

  
#ifdef __cplusplus
}
#endif
#endif
