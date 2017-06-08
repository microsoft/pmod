/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectFactoryImpl.cpp
****/
#include "ObjectFactoryImpl.h"

#include "JniCoreUtils.h"

// pmod public headers
#include <foundation/library/object_factory_util.h>
using namespace foundation;

JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ObjectFactory_getObjectFactoryInstanceInterface
    (JNIEnv *, jclass)
{
    foundation::IObjectFactory *pObjectFactory = foundation::library::GetObjectFactory();
    // Note that we need to Addref since the JNI call will return a reference Java
    // class that will Release this Object when the JVM is gone
    pObjectFactory->AddRef();
    return (jlong)pObjectFactory;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObjectFactory_tryCreateObjectInternal
    (JNIEnv *env, jobject _this, jstring typeId, jobject context)
{
    foundation::IObjectFactory *pObjectFactory = extractInterfaceExpectedAndCast<foundation::IObjectFactory>(env, _this);
    if (!pObjectFactory)
    {
        return (jobject)nullptr;
    }
    foundation::IInspectable *pContextValue = extractInterfaceIf(env, context);

    IID iidType = toIIDType(env,typeId);
    foundation::ComPtr<foundation::IObject> spObject;
    HRESULT hr = pObjectFactory->TryCreateObject(iidType, pContextValue, spObject.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObjectFactory::tryCreateObject");
        return (jobject)nullptr;
    }

    return spObject != nullptr ? returnValue(env, spObject) : (jobject)nullptr;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObjectFactory_activateObjectInternal
  (JNIEnv *env,jclass,jstring typeId, jobject context)
{
    IID iidType = toIIDType(env,typeId);
    foundation::ComPtr<foundation::IObject> spObject;

    foundation::IInspectable *pContextValue = extractInterfaceIf(env, context);

    HRESULT hr = library::ActivateObject(iidType, pContextValue, spObject.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObjectFactory::activateObjectInternal");
        return (jobject)nullptr;
    }
    return spObject != nullptr ? returnValue(env, spObject) : (jobject)nullptr;
}
