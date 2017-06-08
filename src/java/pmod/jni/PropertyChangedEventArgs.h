/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyChangedEventArgs.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_PropertyChangedEventArgs
#define _Included_com_microsoft_pmod_PropertyChangedEventArgs
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_PropertyChangedEventArgs
 * Method:    getProperty
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyChangedEventArgs_getProperty
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyChangedEventArgs
 * Method:    getOldValue
 * Signature: ()Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_PropertyChangedEventArgs_getOldValue
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyChangedEventArgs
 * Method:    getNewValue
 * Signature: ()Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_PropertyChangedEventArgs_getNewValue
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
