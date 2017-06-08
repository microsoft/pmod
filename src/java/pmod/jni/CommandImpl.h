/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_CommandModelImpl
#define _Included_com_microsoft_pmod_CommandModelImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_CommandModelImpl
 * Method:    Execute
 * Signature: (Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_BaseCommandImpl_Execute
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_microsoft_propertymodel_CommandModelImpl
 * Method:    CanExecute
 * Signature: (Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_BaseCommandImpl_CanExecute
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_microsoft_propertymodel_CommandModelImpl
 * Method:    GetState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_BaseCommandImpl_GetState
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_CommandModelImpl
 * Method:    createListenerProxy
 * Signature: (Lcom/microsoft/pmod/EventSink;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_BaseCommandImpl_createListenerProxy
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif
