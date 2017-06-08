/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableListImpl.cpp
****/
#include "ObservableListImpl.h"

#include "JniCoreUtils.h"
#include "JniConnection.h"

// propertymodel public headers
#include <pmod/interfaces.h>

using namespace pmod;


JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_SetItem
  (JNIEnv *env, jobject _this, jint index, jobject value)
{
    IObservableList *pListModel = extractInterfaceExpectedAndCast<IObservableList>(env,_this);
    if(!pListModel)
    {
        return;
    }

    foundation::IInspectable *pValue = extractInterfaceIf(env, value);

    HRESULT hr = pListModel->SetItem((UINT32)index,pValue);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableList::SetItem");
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_AppendItem
  (JNIEnv *env, jobject _this, jobject item)
{
    IObservableList *pListModel = extractInterfaceExpectedAndCast<IObservableList>(env,_this);
    if(!pListModel)
    {
        return;
    }

    foundation::IInspectable *pValue = extractInterfaceIf(env, item);

    HRESULT hr = pListModel->AppendItem(pValue);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableList::AppendItem");
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_InsertItem
  (JNIEnv *env, jobject _this, jint index, jobject item)
{
    IObservableList *pListModel = extractInterfaceExpectedAndCast<IObservableList>(env,_this);
    if(!pListModel)
    {
        return;
    }

    foundation::IInspectable *pValue = extractInterfaceIf(env, item);

    HRESULT hr = pListModel->InsertItem((UINT32)index,pValue);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableList::InsertItem");
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_RemoveItem
    (JNIEnv *env, jobject _this, jobject item)
{
    IObservableList *pListModel = extractInterfaceExpectedAndCast<IObservableList>(env,_this);
    if(!pListModel)
    {
        return;
    }

    foundation::IInspectable *pValue = extractInterfaceIf(env, item);

    HRESULT hr = pListModel->RemoveItem(pValue);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableList::RemoveItem");
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_RemoveAt
  (JNIEnv *env, jobject _this, jint index)
{
    IObservableList *pListModel = extractInterfaceExpectedAndCast<IObservableList>(env,_this);
    if(!pListModel)
    {
        return;
    }

    HRESULT hr = pListModel->RemoveAt((UINT32)index);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableList::RemoveAt");
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_BaseObservableListImpl_RemoveAll
  (JNIEnv *env, jobject _this)
{
    IObservableList *pListModel = extractInterfaceExpectedAndCast<IObservableList>(env,_this);
    if(!pListModel)
    {
        return;
    }

    HRESULT hr = pListModel->RemoveAll();
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObservableList::RemoveAll");
    }
}
