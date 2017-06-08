/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_CollectionModelImpl
#define _Included_com_microsoft_pmod_CollectionModelImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_CollectionModelImpl
 * Method:    GetCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_GetCount
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_CollectionModelImpl
 * Method:    GetItem
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_GetItem
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_microsoft_propertymodel_CollectionModelImpl
 * Method:    SetItem
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_SetItem
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     com_microsoft_propertymodel_CollectionModelImpl
 * Method:    GetItems
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_GetItems
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_CollectionModelImpl
 * Method:    createListenerProxy
 * Signature: (Lcom/microsoft/pmod/EventSink;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_createListenerProxy
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif
