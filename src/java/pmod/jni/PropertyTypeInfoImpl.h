/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyTypeInfoImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_PropertyTypeInfo
#define _Included_com_microsoft_pmod_PropertyTypeInfo
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_PropertyTypeInfo
 * Method:    getPropertyTypeInternal
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyTypeInfo_getPropertyTypeInternal
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyTypeInfo
 * Method:    getModelType
 * Signature: ()Lcom/microsoft/pmod/ModelTypeInfo;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_PropertyTypeInfo_getModelType
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
