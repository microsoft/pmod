/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectFactoryImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_ModelFactory
#define _Included_com_microsoft_pmod_ModelFactory
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_ModelFactory
 * Method:    getModelFactoryInstanceInterface
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ObjectFactory_getObjectFactoryInstanceInterface
  (JNIEnv *, jclass);

/*
 * Class:     com_microsoft_propertymodel_ModelFactory
 * Method:    tryCreateModelInternal
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)Lcom/microsoft/pmod/Model;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObjectFactory_tryCreateObjectInternal
  (JNIEnv *, jobject, jstring, jobject);

/*
 * Class:     com_microsoft_propertymodel_ModelFactory
 * Method:    activateModelInternal
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)Lcom/microsoft/pmod/Model;
 */
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObjectFactory_activateObjectInternal
  (JNIEnv *, jclass,jstring, jobject);


#ifdef __cplusplus
}
#endif
#endif
