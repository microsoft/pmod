/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectWrap.h
****/
#pragma once

#include "ObjectDispatchWrap.h"
#include <pmod/interfaces/observable_object.h>
#include <vector>
#include <pmod/event_connection.h>


class ObservableObjectWrap : public ObjectDispatchWrap
{
public:
    static void Init(v8::Handle<v8::Object> exports);
    static v8::Local<v8::Value> Create(pmod::IObservableObject *p);

    static bool IsInstanceOf(v8::Isolate* isolate, v8::Local<v8::Value>& value)
    {
        return _template.Get(isolate)->HasInstance(value);
    }
private:
    pmod::PropertyChangedConnection _propertyChangedEventConnection;
    std::vector<v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>> _propertyChangesCallbacks;

    explicit ObservableObjectWrap(pmod::IObservableObject *p);
    ~ObservableObjectWrap();

    HRESULT OnPropertyChanged(foundation::IUnknown *pSender, pmod::IPropertyChangedEventArgs *pEventArgs);
    void OnPropertyChangedCallback(pmod::IPropertyChangedEventArgs *pEventArgs);
    static void _OnPropertyChangedCallback(uv_async_t* handle);

    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);
    static void AddPropertyChanged(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void RemovePropertyChanged(const v8::FunctionCallbackInfo<v8::Value>& args);

    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;

    typedef std::pair<ObservableObjectWrap *, pmod::IPropertyChangedEventArgs*> AsyncPropertyChangedDataType;
};

