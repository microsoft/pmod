/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NotifyCollectionChangedEventArgs.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_NotifyCollectionChangedEventArgs
#define _Included_com_microsoft_pmod_NotifyCollectionChangedEventArgs
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_NotifyCollectionChangedEventArgs
 * Method:    getAction
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getAction
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_NotifyCollectionChangedEventArgs
 * Method:    getNewItems
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getNewItems
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_NotifyCollectionChangedEventArgs
 * Method:    getOldItems
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getOldItems
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_NotifyCollectionChangedEventArgs
 * Method:    getNewStartingIndex
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getNewStartingIndex
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_NotifyCollectionChangedEventArgs
 * Method:    getOldStartingIndex
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getOldStartingIndex
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
