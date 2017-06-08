/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FactoryObject.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_FactoryObject
#define _Included_com_microsoft_pmod_FactoryObject
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_FactoryObject
 * Method:    releaseListenerProxy
 * Signature: (J)V
 */
    JNIEXPORT void JNICALL Java_com_microsoft_pmod_FactoryObject_releaseListenerProxy
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
