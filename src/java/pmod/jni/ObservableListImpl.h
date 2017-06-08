/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableListImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_ListModelImpl
#define _Included_com_microsoft_pmod_ListModelImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_ListModelImpl
 * Method:    AppendItem
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_AppendItem
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_microsoft_propertymodel_ListModelImpl
 * Method:    InsertItem
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_InsertItem
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     com_microsoft_propertymodel_ListModelImpl
 * Method:    RemoveItem
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_RemoveItem
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_microsoft_propertymodel_ListModelImpl
 * Method:    RemoveAt
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_RemoveAt
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_microsoft_propertymodel_ListModelImpl
 * Method:    RemoveAll
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_RemoveAll
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_SetItem
(JNIEnv *env, jobject _this, jint index, jobject value);

#ifdef __cplusplus
}
#endif
#endif
