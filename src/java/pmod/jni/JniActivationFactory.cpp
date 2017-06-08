/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JniActivationFactory.cpp
****/

#include "JniCoreUtils.h"
#include "JVMEnv.h"

#include <foundation/activation_factory.h>

using namespace foundation;

// exported entry points
extern "C" {
    JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ActivationFactory_getFactory
        (JNIEnv *env, jclass jclass, jstring activationId);
}

JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ActivationFactory_getFactory
(JNIEnv *env, jclass thiz, jstring activationId)
{
    HStringPtr hActivationId = toHStringPtr(env, activationId);
    foundation::IUnknown *pFactory = GetActivationFactoryInstance(hActivationId.GetRawBuffer());
    if (pFactory == nullptr)
    {
        return (jlong)0;
    }
    pFactory->AddRef();
    return (jlong)pFactory;
}

