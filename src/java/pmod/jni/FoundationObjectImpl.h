/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationObjectImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_FoundationObjectImpl
#define _Included_com_microsoft_pmod_FoundationObjectImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_Model
 * Method:    getUniqueId
 * Signature: ()I
 */
    JNIEXPORT jint JNICALL Java_com_microsoft_pmod_FoundationObjectImpl_getUniqueId
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_Model
 * Method:    getModelTypeInfo
 * Signature: ()Lcom/microsoft/pmod/ModelTypeInfo;
 */
    JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_FoundationObjectImpl_getTypeInfo
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
