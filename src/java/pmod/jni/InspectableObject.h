/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InspectableObject.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_InspectableObject
#define _Included_com_microsoft_pmod_InspectableObject
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_InspectableObject
 * Method:    release
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_InspectableObject_release
  (JNIEnv *, jobject);

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_InspectableObject_createObjectInternal
(JNIEnv *env, jclass, jlong pInterface);

#ifdef __cplusplus
}
#endif
#endif
