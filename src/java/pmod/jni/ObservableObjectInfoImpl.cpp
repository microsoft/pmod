/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectInfoImpl.cpp
****/
#include "ObservableObjectInfoImpl.h"

#include "JniCoreUtils.h"

// propertymodel public headers
#include <foundation/hstring_wrapper.h>
#include <foundation/array_wrapper.h>

using namespace pmod;

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectInfo_getPropertyById
    (JNIEnv *env, jobject _this,jint propertyId)
{
    IObservableObjectInfo *pPropertyModelInfo = extractInterfaceExpectedAndCast<IObservableObjectInfo>(env,_this);
    if(!pPropertyModelInfo)
    {
        return (jobject)nullptr;
    }
    foundation::IPropertyInfo *pPropertyInfo = nullptr;
    HRESULT hr = pPropertyModelInfo->GetPropertyById(propertyId,&pPropertyInfo);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableObjectInfo::GetPropertyById");
        return (jobject)nullptr;
    }
    return createInspectableObject(env,"com/microsoft/pmod/PropertyInfo",pPropertyInfo);
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectInfo_getPropertyByName
    (JNIEnv *env, jobject _this, jstring propertyName)
{
    IObservableObjectInfo *pPropertyModelInfo = extractInterfaceExpectedAndCast<IObservableObjectInfo>(env,_this);
    if(!pPropertyModelInfo)
    {
        return (jobject)nullptr;
    }
    foundation::HStringPtr hPropertyName = toHStringPtr(env, propertyName);

	foundation::IPropertyInfo *pPropertyInfo = nullptr;
    HRESULT hr = pPropertyModelInfo->GetPropertyByName(hPropertyName, &pPropertyInfo);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableObjectInfo::GetPropertyByName");
        return (jobject)nullptr;
    }
    return createInspectableObject(env,"com/microsoft/pmod/PropertyInfo",pPropertyInfo);
}

JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_ObservableObjectInfo_getPropertiesInternal
    (JNIEnv *env, jobject _this,jint flags)
{
    IObservableObjectInfo *pPropertyModelInfo = extractInterfaceExpectedAndCast<IObservableObjectInfo>(env,_this);
    if(!pPropertyModelInfo)
    {
        return (jobjectArray)nullptr;
    }
    foundation::ArrayRefCountWrapper<foundation::IPropertyInfo> properties;
    HRESULT hr = pPropertyModelInfo->GetProperties((UINT32)flags,properties.GetSizeAddressOf(),properties.GetBufferAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableObjectInfo::GetProperties");
        return (jobjectArray)nullptr;
    }
    // create an array of PropertyInfo's
    jclass propertyInfoClass = env->FindClass("com/microsoft/pmod/PropertyInfo");
    jobjectArray jPropertyInfoArray = env->NewObjectArray(properties.GetSize(), propertyInfoClass, 0);
    // fill them in
    for(UINT32 index = 0;index < properties.GetSize();++index) {

        jobject javaPropertyInfo = createInspectableObject(env,"com/microsoft/pmod/PropertyInfo",properties[index]);
        // put in object array
        env->SetObjectArrayElement(jPropertyInfoArray, index, javaPropertyInfo);
    }
    return jPropertyInfoArray;
}
