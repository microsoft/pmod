/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionWrap.h
****/
#pragma once

#include "ObjectWrap.h"
#include <pmod/interfaces/observable_collection.h>
#include <vector>
#include <pmod/event_connection.h>


class ObservableCollectionWrap : public ObjectWrap
{
public:
    static void Init(v8::Handle<v8::Object> exports);
    static v8::Local<v8::Value> Create(pmod::IObservableCollection *p);

    static bool IsInstanceOf(v8::Isolate* isolate, v8::Local<v8::Value>& value)
    {
        return _template.Get(isolate)->HasInstance(value);
    }
private:
    pmod::NotifyCollectionChangedConnection _notifyCollectionChangedEventConnection;
    std::vector<v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>> _notifyCollectionChangedCallbacks;
    foundation::ComPtr<pmod::IObservableCollection> _observableCollectionPtr;

    foundation::IInspectable **GetInspectablePtr() override
    {
        return reinterpret_cast<foundation::IInspectable **>(_observableCollectionPtr.GetAddressOfPtr());
    }

    explicit ObservableCollectionWrap(pmod::IObservableCollection *p);
    ~ObservableCollectionWrap();

    HRESULT OnNotifyCollectionChanged(foundation::IUnknown *pSender, pmod::INotifyCollectionChangedEventArgs *pEventArgs);
    void OnNotifyCollectionChangedCallback(pmod::INotifyCollectionChangedEventArgs *pEventArgs);
    static void _OnNotifyCollectionChangedCallback(uv_async_t* handle);

    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);
    static void GetCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetItem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddNotifyCollectionChanged(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void RemoveNotifyCollectionChanged(const v8::FunctionCallbackInfo<v8::Value>& args);

    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;

    typedef std::pair<ObservableCollectionWrap *, pmod::INotifyCollectionChangedEventArgs*> AsyncNotifyCollectionChangedDataType;
};

