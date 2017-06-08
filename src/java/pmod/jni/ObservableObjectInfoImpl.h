/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectInfoImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_PropertyModelInfo
#define _Included_com_microsoft_pmod_PropertyModelInfo
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_PropertyModelInfo
 * Method:    getPropertyById
 * Signature: (I)Lcom/microsoft/pmod/PropertyInfo;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectInfo_getPropertyById
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_microsoft_propertymodel_PropertyModelInfo
 * Method:    getPropertyByName
 * Signature: (Ljava/lang/String;)Lcom/microsoft/pmod/PropertyInfo;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectInfo_getPropertyByName
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_microsoft_propertymodel_PropertyModelInfo
 * Method:    getPropertiesInternal
 * Signature: (I)[Lcom/microsoft/pmod/PropertyInfo;
 */
JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_ObservableObjectInfo_getPropertiesInternal
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
