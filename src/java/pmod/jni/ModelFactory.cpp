/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelFactory.cpp
****/
#include "ModelFactory.h"

#include "jniCoreUtils.h"

// propertymodel public headers
#include <pmod/model_util.h>
#include <pmod/library/library_util.h>
using namespace pmod;


JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ModelFactory_getModelFactoryInstanceInterface
    (JNIEnv *, jclass)
{
    IModelFactory *pModelFactory = pmod::library::get_model_factory();
    // Note that we need to Addref since the JNI call will return a reference Java
    // class that will Release this Object when the JVM is gone
    pModelFactory->AddRef();
    return (jlong)pModelFactory;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ModelFactory_tryCreateModelInternal
    (JNIEnv *env, jobject _this, jstring typeId, jobject context)
{
    IModelFactory *pModelFactory = extractInterfaceExpectedAndCast<IModelFactory>(env,_this);
    if(!pModelFactory)
    {
        return (jobject)nullptr;
    }
    foundation::IInspectable *pContextValue = extractInterfaceExpected(env,context);
    if(!pContextValue)
    {
        return (jobject)nullptr;
    }

    IID iidType = toIIDType(env,typeId);
    foundation::ComPtr<IModel> spModel;
    HRESULT hr = pModelFactory->TryCreateModel(iidType,pContextValue,spModel.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IModelFactory::TryCreateModel");
        return (jobject)nullptr;
    }

    return spModel != nullptr ? returnValue(env,spModel) : (jobject)nullptr;
}
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ModelFactory_activateModelInternal
  (JNIEnv *env,jclass,jstring typeId, jobject context)
{
    IID iidType = toIIDType(env,typeId);
    foundation::ComPtr<IModel> spModel;

    foundation::IInspectable *pContextValue = nullptr;
    if(context != nullptr)
    {
        pContextValue = extractInterfaceExpected(env,context);
    }

    HRESULT hr = core_util::activate_model(iidType,pContextValue,spModel.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IModelFactory::activateModelInternal");
        return (jobject)nullptr;
    }
    return spModel != nullptr ? returnValue(env,spModel) : (jobject)nullptr;
}
