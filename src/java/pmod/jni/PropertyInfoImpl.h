/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyInfoImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_PropertyInfo
#define _Included_com_microsoft_pmod_PropertyInfo
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_PropertyInfo
 * Method:    getId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyInfo_getId
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyInfo
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_PropertyInfo_getName
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
