/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionImpl.cpp
****/
#include "ObservableCollectionImpl.h"

#include "JniCoreUtils.h"
#include "JniConnection.h"

// propertymodel public headers
#include <pmod/interfaces.h>

using namespace pmod;

struct _NotifyCollectionChangedEventArgsClassName{
    static const char *GetEventArgsClassName()
    {
        return "com/microsoft/pmod/NotifyCollectionChangedEventArgs";
    }
};

typedef _TListenerProxy
<
        IObservableCollection,
        INotifyCollectionChangedEventHandler,
        INotifyCollectionChangedEventArgs,
        ObservableCollectionListenerBase,
        _NotifyCollectionChangedEventArgsClassName
> CNotifyCollectionChangedEventProxy;

extern "C" {
    JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_GetFirst
        (JNIEnv *env, jobject _this);

    JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_BaseIteratorImpl_GetCurrent
        (JNIEnv *env, jobject _this);
    JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_BaseIteratorImpl_MoveNext
        (JNIEnv *env, jobject _this);
    JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_BaseIteratorImpl_GetHasCurrent
        (JNIEnv *env, jobject _this);
}

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_GetCount
  (JNIEnv *env, jobject _this)
{
    IObservableCollection *pObservableCollection = extractInterfaceExpectedAndCast<IObservableCollection>(env,_this);
    if(!pObservableCollection)
    {
        return (jint)0;
    }
    UINT32 count;
    HRESULT hr = pObservableCollection->GetCount(&count);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableCollection::GetCount");
        return (jint)0;
    }
    return (jint)count;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_GetItem
  (JNIEnv *env, jobject _this, jint index)
{
    IObservableCollection *pObservableCollection = extractInterfaceExpectedAndCast<IObservableCollection>(env,_this);
    if(!pObservableCollection)
    {
        return (jobject)nullptr;
    }

    foundation::InspectablePtr spItemValue;
    HRESULT hr = pObservableCollection->GetItem((UINT32)index,spItemValue.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableCollection::GetItem");
        return (jobject)nullptr;
    }
    return returnValue(env,spItemValue);
}

JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_GetItems
  (JNIEnv *env, jobject _this)
{
    IObservableCollection *pObservableCollection = extractInterfaceExpectedAndCast<IObservableCollection>(env,_this);
    if(!pObservableCollection)
    {
        return (jobjectArray)nullptr;
    }

    foundation::InspectableArrayWrapper values;
    HRESULT hr = pObservableCollection->GetItems(values.GetSizeAddressOf(),values.GetBufferAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableCollection::GetItems");
        return (jobjectArray)nullptr;
    }
    return returnValues(env,values.GetSize(),values.GetRawBuffer());
}

JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_createListenerProxy
  (JNIEnv *env, jobject _this, jobject javaSinkObj)
{
    IObservableCollection *pObservableCollection = extractInterfaceExpectedAndCast<IObservableCollection>(env,_this);
    if(!pObservableCollection)
    {
        return (jlong)nullptr;
    }
    return (jlong)CNotifyCollectionChangedEventProxy::CreateInstance<CNotifyCollectionChangedEventProxy>(env,javaSinkObj,pObservableCollection);

}

JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_BaseObservableCollectionImpl_GetFirst
(JNIEnv *env, jobject _this)
{
    IObservableCollection *pObservableCollection = extractInterfaceExpectedAndCast<IObservableCollection>(env, _this);
    if (!pObservableCollection)
    {
        return (jlong)0;
    }
    foundation::IIterator_impl<foundation::IInspectable *> *pIterator;
    HRESULT hr = pObservableCollection->First(&pIterator);
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IObservableCollection::GetFirst");
        return (jlong)0;
    }
    return (jlong)pIterator;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_BaseIteratorImpl_GetCurrent
(JNIEnv *env, jobject _this)
{
    foundation::IIterator_impl<foundation::IInspectable *> *pIterator = extractInterfaceExpectedAndCast<foundation::IIterator_impl<foundation::IInspectable *>>(env, _this);
    if (!pIterator)
    {
        return (jobject)nullptr;
    }
    foundation::InspectablePtr spItemValue;
    HRESULT hr = pIterator->GetCurrent(spItemValue.GetAddressOf());
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IIterator::GetCurrent");
        return (jobject)nullptr;
    }
    return returnValue(env, spItemValue);
}

JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_BaseIteratorImpl_MoveNext
(JNIEnv *env, jobject _this)
{
    foundation::IIterator_impl<foundation::IInspectable *> *pIterator = extractInterfaceExpectedAndCast<foundation::IIterator_impl<foundation::IInspectable *>>(env, _this);
    if (!pIterator)
    {
        return (jboolean)false;
    }
    bool hasCurrent;
    HRESULT hr = pIterator->MoveNext(&hasCurrent);
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IIterator::MoveNext");
        return (jboolean)false;
    }
    return (jboolean)hasCurrent;
}

JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_BaseIteratorImpl_GetHasCurrent
(JNIEnv *env, jobject _this)
{
    foundation::IIterator_impl<foundation::IInspectable *> *pIterator = extractInterfaceExpectedAndCast<foundation::IIterator_impl<foundation::IInspectable *>>(env, _this);
    if (!pIterator)
    {
        return (jboolean)false;
    }
    bool hasCurrent;
    HRESULT hr = pIterator->GetHasCurrent(&hasCurrent);
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IIterator::GetHasCurrent");
        return (jboolean)false;
    }
    return (jboolean)hasCurrent;
}
