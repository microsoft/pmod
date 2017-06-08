/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandWrap.h
****/
#pragma once

#include "ObjectWrap.h"
#include <pmod/interfaces/command.h>
#include <vector>
#include <pmod/event_connection.h>

class CommandWrap : public ObjectWrap
{
public:
    static void Init(v8::Handle<v8::Object> exports);
    static v8::Local<v8::Value> Create(pmod::ICommand *p);

    static bool IsInstanceOf(v8::Isolate* isolate, v8::Local<v8::Value>& value)
    {
        return _template.Get(isolate)->HasInstance(value);
    }
protected:
    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);

    foundation::IInspectable **GetInspectablePtr() override
    {
        return reinterpret_cast<foundation::IInspectable **>(_commandPtr.GetAddressOfPtr());
    }
    explicit CommandWrap(pmod::ICommand *p);
    ~CommandWrap();

private:
    foundation::ComPtr<pmod::ICommand> _commandPtr;
    pmod::CanExecuteConnection _canExecuteChangedEventConnection;
    std::vector<v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>> _canExecutedChangedCallbacks;

    HRESULT OnCanExecuteChanged(foundation::IUnknown *pSender, pmod::ICanExecuteChangedEventArgs *pEventArgs);
    void OnCanExecuteChangedCallback(pmod::ICanExecuteChangedEventArgs *pEventArgs);
    static void _OnCanExecuteChangedCallback(uv_async_t* handle);

    static void GetState(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Execute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CanExecute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddCanExecuteChanged(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void RemoveCanExecuteChanged(const v8::FunctionCallbackInfo<v8::Value>& args);

    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;

    typedef std::pair<CommandWrap *, pmod::ICanExecuteChangedEventArgs*> AsyncCanExecutedChangedDataType;
};

