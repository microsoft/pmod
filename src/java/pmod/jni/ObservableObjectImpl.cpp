/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectImpl.cpp
****/
#include "ObservableObjectImpl.h"

#include "JniCoreUtils.h"
#include "JniConnection.h"

#include <vector>

// pmod public headers
#include <pmod/interfaces.h>
#include <foundation/hstring_wrapper.h>

using namespace pmod;

struct _PropertyChangedEventArgsClassName{
    static const char *GetEventArgsClassName()
    {
        return "com/microsoft/pmod/PropertyChangedEventArgs";
    }
};

typedef _TListenerProxy
<
    IObservableObject,
    IPropertyChangedEventHandler,
    IPropertyChangedEventArgs,
    ObservableObjectListenerBase,
    _PropertyChangedEventArgsClassName
> CPropertyChangedEventProxy;

struct _EventModelEventArgsClassName{
    static const char *GetEventArgsClassName()
    {
        return "com/microsoft/pmod/EventModelEventArgs";
    }
};

typedef  _TListenerProxy
<
    IObservableObject,
    IEventModelEventHandler,
    IEventModelEventArgs,
    EventModelListenerBase,
    _EventModelEventArgsClassName
> CEventModelEventProxy;

JNIEXPORT void JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_setPropertyById
    (JNIEnv *env, jobject _this, jint propertyId, jobject propertyValue)
{
    IObservableObject *pObservableObject = extractInterfaceExpectedAndCast<IObservableObject>(env,_this);
    if(!pObservableObject)
    {
        return;
    }
    foundation::IInspectable *pValue = extractInterfaceIf(env, propertyValue);

    HRESULT hr = pObservableObject->SetProperty((UINT32)propertyId,pValue);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::SetProperty");
    }
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_getPropertyById
    (JNIEnv *env, jobject _this, jint propertyId)
{
    IObservableObject *pObservableObject = extractInterfaceExpectedAndCast<IObservableObject>(env,_this);
    if (!pObservableObject)
    {
        return (jobject)nullptr;
    }

    foundation::InspectablePtr spValue;
    HRESULT hr = pObservableObject->GetProperty((UINT32)propertyId,spValue.GetAddressOf());
    if (FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::GetProperty");
        return (jobject)nullptr;
    }
    
    return returnValue(env,spValue);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_setPropertyByName
  (JNIEnv *env, jobject _this, jstring propertyName, jobject propertyValue)
{
    foundation::IDictionary *pPropertyModelMap = extractInterfaceExpectedAndQI<foundation::IDictionary>(env, _this);
    if (!pPropertyModelMap)
    {
        return;
    }
    
    foundation::IInspectable *pValue = extractInterfaceIf(env, propertyValue);
    foundation::HStringPtr hPropertyName = toHStringPtr(env, propertyName);

    bool replaced;
    HRESULT hr = pPropertyModelMap->Insert(hPropertyName, pValue, &replaced);
    if (FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValueMap::Insert");
    }
}


JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_getPropertyByName
  (JNIEnv *env, jobject _this, jstring propertyName)
{
    foundation::IDictionary *pPropertyModelMap = extractInterfaceExpectedAndQI<foundation::IDictionary>(env, _this);
    if (!pPropertyModelMap)
    {
        return (jobject)nullptr;
    }

    foundation::HStringPtr hPropertyName = toHStringPtr(env, propertyName);

    foundation::InspectablePtr spValue;
    HRESULT hr = pPropertyModelMap->Lookup(hPropertyName, spValue.GetAddressOf());
    if (FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValueMap::Lookup");
        return (jobject)nullptr;
    }
    
    return returnValue(env,spValue);
}

JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_createListenerProxy
    (JNIEnv *env, jobject _this, jobject javaSinkObj)
{
    IObservableObject *pObservableObject = extractInterfaceExpectedAndCast<IObservableObject>(env,_this);
    if (!pObservableObject)
    {
        return (jlong)0;
    }
    
    return (jlong)CPropertyChangedEventProxy::CreateInstance<CPropertyChangedEventProxy>(env,javaSinkObj,pObservableObject);
}

JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_createEventModelListenerProxy
(JNIEnv *env, jobject _this, jobject javaSinkObj)
{
    IObservableObject *pObservableObject = extractInterfaceExpectedAndCast<IObservableObject>(env, _this);
    if (!pObservableObject)
    {
        return (jlong)0;
    }
    
    return (jlong)CEventModelEventProxy::CreateInstance<CEventModelEventProxy>(env, javaSinkObj, pObservableObject);
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObservableObjectImpl_Invoke
    (JNIEnv *env, jobject _this, jint methodId, jobjectArray jObjectArray )
{
    IObservableObject *pObservableObject = extractInterfaceExpectedAndCast<IObservableObject>(env,_this);
    if (!pObservableObject)
    {
        return (jobject)nullptr;
    }

    std::vector< foundation::IInspectable * > parameters;

    jsize len = env->GetArrayLength(jObjectArray);
    for (jsize i = 0;i < len; i++) 
    {
        jobject value = env->GetObjectArrayElement(jObjectArray, i);
        foundation::IInspectable *pValue = extractInterfaceIf(env, value);
        parameters.push_back(pValue);
    }

    foundation::InspectablePtr spResult;
    HRESULT hr = pObservableObject->InvokeMethod(
            (UINT32)methodId,
            (UINT32)parameters.size(),
            parameters.size() > 0 ? &parameters.front() : nullptr,
            spResult.GetAddressOf());
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IObservableObject::Invoke");
        return (jobject)nullptr;
    }
    
    return spResult != nullptr ? returnValue(env,spResult) : (jobject)nullptr;
}
