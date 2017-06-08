/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectTypeInfo.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_ModelTypeInfo
#define _Included_com_microsoft_pmod_ModelTypeInfo
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_ModelTypeInfo
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_ObjectTypeInfo_getName
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_ModelTypeInfo
 * Method:    getType
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_ObjectTypeInfo_getType
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_ModelTypeInfo
 * Method:    getBaseType
 * Signature: ()Lcom/microsoft/pmod/ModelTypeInfo;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObjectTypeInfo_getBaseType
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
