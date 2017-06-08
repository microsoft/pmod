/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CanExecuteChangedEventArgs.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_CanExecuteChangedEventArgs
#define _Included_com_microsoft_pmod_CanExecuteChangedEventArgs
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_propertymodel_CanExecuteChangedEventArgs
 * Method:    getOldState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_CanExecuteChangedEventArgs_getOldState
  (JNIEnv *, jobject);

/*
 * Class:     com_microsoft_propertymodel_CanExecuteChangedEventArgs
 * Method:    getNewState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_microsoft_pmod_CanExecuteChangedEventArgs_getNewState
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
