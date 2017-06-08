/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValueImpl.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_PropertyValue
#define _Included_com_microsoft_pmod_PropertyValue
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createEmpty
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createEmpty
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createByte
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createByte
  (JNIEnv *, jobject, jbyte);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createChar
 * Signature: (C)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createChar
  (JNIEnv *, jobject, jchar);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createBoolean
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createBoolean
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createShort
 * Signature: (S)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createShort
  (JNIEnv *, jobject, jshort);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createInt
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createInt
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createLong
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createLong
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createString
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createString
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createSingle
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createSingle
  (JNIEnv *, jobject, jfloat);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    createDouble
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createDouble
  (JNIEnv *, jobject, jdouble);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createDateTime
(JNIEnv *, jobject, jobject);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createByteArray
(JNIEnv *, jobject, jbyteArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createCharArray
(JNIEnv *, jobject, jcharArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createBooleanArray
(JNIEnv *, jobject, jbooleanArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createShortArray
(JNIEnv *, jobject, jshortArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createIntArray
(JNIEnv *, jobject, jintArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createSingleArray
(JNIEnv *, jobject, jfloatArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createDoubleArray
(JNIEnv *, jobject, jdoubleArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createLongArray
(JNIEnv *, jobject, jlongArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createStringArray
(JNIEnv *, jobject, jobjectArray);

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createObjectArray
(JNIEnv *, jobject, jobjectArray);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getTypeInternal
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyValue_getTypeInternal
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getByte
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_com_microsoft_pmod_PropertyValue_getByte
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getBoolean
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_PropertyValue_getBoolean
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getChar
 * Signature: ()C
 */
JNIEXPORT jchar JNICALL Java_com_microsoft_pmod_PropertyValue_getChar
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getShort
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_microsoft_pmod_PropertyValue_getShort
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getInt
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyValue_getInt
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getLong
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_PropertyValue_getLong
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_microsoft_pmod_PropertyValue_getStringInternal
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getSingle
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_com_microsoft_pmod_PropertyValue_getSingle
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_PropertyValue
 * Method:    getDouble
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_microsoft_pmod_PropertyValue_getDouble
  (JNIEnv *, jobject);

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_PropertyValue_getDateTime
(JNIEnv *, jobject);

JNIEXPORT jbyteArray JNICALL Java_com_microsoft_pmod_PropertyValue_getByteArray
(JNIEnv *, jobject);

JNIEXPORT jcharArray JNICALL Java_com_microsoft_pmod_PropertyValue_getCharArray
(JNIEnv *, jobject);

JNIEXPORT jbooleanArray JNICALL Java_com_microsoft_pmod_PropertyValue_getBooleanArray
(JNIEnv *, jobject);

JNIEXPORT jshortArray JNICALL Java_com_microsoft_pmod_PropertyValue_getShortArray
(JNIEnv *, jobject);

JNIEXPORT jintArray JNICALL Java_com_microsoft_pmod_PropertyValue_getIntArray
(JNIEnv *, jobject);

JNIEXPORT jlongArray JNICALL Java_com_microsoft_pmod_PropertyValue_getLongArray
(JNIEnv *, jobject);

JNIEXPORT jfloatArray JNICALL Java_com_microsoft_pmod_PropertyValue_getFloatArray
(JNIEnv *, jobject);

JNIEXPORT jdoubleArray JNICALL Java_com_microsoft_pmod_PropertyValue_getDoubleArray
(JNIEnv *, jobject);

JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_PropertyValue_getStringArrayInternal
(JNIEnv *, jobject);

JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_PropertyValue_getObjectArrayInternal
(JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
