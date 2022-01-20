/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandWrap.cpp
****/
#include "pch.h"
#include "CommandWrap.h"
#include "NodeJsHelper.h"

using namespace v8;
using namespace foundation;
using namespace pmod;

class CanExecuteChangedEventArgsWrap : public InspectableWrap<ICanExecuteChangedEventArgs>
{
public:
    static void Init(v8::Handle<v8::Object> exports)
    {
        InspectableBaseWrap::Init(
            exports,
            _constructor,
            _template,
            "CanExecuteChangedEventArgs",
            SetPrototypeMethods);
    }

    static void Create(Isolate* isolate, ICanExecuteChangedEventArgs *p, v8::Local<v8::Object>& instance)
    {
        LocalContext context(isolate);

        CanExecuteChangedEventArgsWrap* obj = new CanExecuteChangedEventArgsWrap(p);
        instance = Local<Function>::New(isolate, _constructor)->NewInstance(context.local()).ToLocalChecked();
        obj->Wrap(instance);
    }
protected:
    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
    {
        InspectableBaseWrap::SetPrototypeMethods(tpl);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getOldState", GetOldState);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getNewState", GetNewState);
    }

private:
    explicit CanExecuteChangedEventArgsWrap(ICanExecuteChangedEventArgs *p) :
        InspectableWrap<ICanExecuteChangedEventArgs>(p)
    {}

    static void GetOldState(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        CanExecuteChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<CanExecuteChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        UINT32 state;
        pWrapper->_ptr->GetOldState(&state);
        args.GetReturnValue().Set(Uint32::New(isolate, state));
    }

    static void GetNewState(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        CanExecuteChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<CanExecuteChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        UINT32 state;
        pWrapper->_ptr->GetNewState(&state);
        args.GetReturnValue().Set(Uint32::New(isolate, state));
    }

    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;
};
Persistent<Function> CanExecuteChangedEventArgsWrap::_constructor;
Persistent<v8::FunctionTemplate> CanExecuteChangedEventArgsWrap::_template;

Persistent<Function> CommandWrap::_constructor;
Persistent<v8::FunctionTemplate> CommandWrap::_template;

CommandWrap::CommandWrap(ICommand *p)
{
    this->_commandPtr = p;
    _canExecuteChangedEventConnection.SetSink(this, &CommandWrap::OnCanExecuteChanged);
}

CommandWrap::~CommandWrap() {
    this->_commandPtr.Release();
}

v8::Local<v8::Value> CommandWrap::Create(ICommand *p)
{
    Isolate* isolate = Isolate::GetCurrent();
    LocalContext context(isolate);

    CommandWrap* obj = new CommandWrap(p);
    auto instance = Local<Function>::New(isolate, _constructor)->NewInstance(context.local()).ToLocalChecked();
    obj->Wrap(instance);
    return instance;
}

void CommandWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
    ObjectWrap::SetPrototypeMethods(tpl);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getState", GetState);
    NODE_SET_PROTOTYPE_METHOD(tpl, "execute", Execute);
    NODE_SET_PROTOTYPE_METHOD(tpl, "canExecute", CanExecute);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addCanExecutedChanged", AddCanExecuteChanged);
    NODE_SET_PROTOTYPE_METHOD(tpl, "removeCanExecutedChanged", RemoveCanExecuteChanged);
}

void CommandWrap::Init(Handle<Object> exports)
{
    InspectableBaseWrap::Init(
        exports,
        _constructor,
        _template,
        "Command",
        SetPrototypeMethods);
    CanExecuteChangedEventArgsWrap::Init(exports);
}

void CommandWrap::GetState(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    CommandWrap* pWrapper = ObjectWrap::Unwrap<CommandWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_commandPtr))
    {
        return;
    }

    UINT32 state;
    pWrapper->_commandPtr->GetState(&state);
    args.GetReturnValue().Set(Uint32::New(isolate, state));
}

void CommandWrap::Execute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    CommandWrap* pWrapper = ObjectWrap::Unwrap<CommandWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_commandPtr))
    {
        return;
    }

    InspectablePtr parameterPtr;
    if (args.Length() > 0 && !ThrowIfArgumentValue(isolate, args, 0, parameterPtr))
    {
        return;
    }

    foundation::InspectablePtr resultPtr;
    HRESULT hr = pWrapper->_commandPtr->Execute(
        parameterPtr,
        resultPtr.GetAddressOf());

    _NODEJS_ERROR_EXCP_(hr, U("ICommand::Execute"));
    _NODEJS_RETURN_VALUE_EXCP_(resultPtr);
}

void CommandWrap::CanExecute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    CommandWrap* pWrapper = ObjectWrap::Unwrap<CommandWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_commandPtr))
    {
        return;
    }

    InspectablePtr parameterPtr;
    if (args.Length() > 0 && !ThrowIfArgumentValue(isolate, args, 0, parameterPtr))
    {
        return;
    }

    bool canExecute;
    HRESULT hr = pWrapper->_commandPtr->CanExecute(
        parameterPtr,
        &canExecute);

    _NODEJS_ERROR_EXCP_(hr, U("ICommand::CanExecute"));
    args.GetReturnValue().Set(Boolean::New(isolate, canExecute));
}

HRESULT CommandWrap::OnCanExecuteChanged(foundation::IUnknown *pSender, ICanExecuteChangedEventArgs *pEventArgs)
{
    if (IsMainThread())
    {
        this->OnCanExecuteChangedCallback(pEventArgs);
    }
    else
    {
        auto asyncData = new AsyncCanExecutedChangedDataType(this, pEventArgs);
        pEventArgs->AddRef();

        auto asyncHandle = new uv_async_t();
        asyncHandle->data = asyncData;
        uv_async_init(uv_default_loop(), asyncHandle, _OnCanExecuteChangedCallback);

        uv_async_send(asyncHandle);
    }
    return S_OK;
}

void CommandWrap::OnCanExecuteChangedCallback(ICanExecuteChangedEventArgs *pEventArgs)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Object> eventArgs;
    CanExecuteChangedEventArgsWrap::Create(isolate, pEventArgs, eventArgs);
    const unsigned argc = 1;
    Local<Value> argv[argc] = { eventArgs };
    Local<Value> rcv = Undefined(isolate);

    for (auto iter = _canExecutedChangedCallbacks.begin(); iter != _canExecutedChangedCallbacks.end(); ++iter)
    {
        auto cb = Local<Function>::New(isolate, (*iter));
        cb->Call(isolate->GetCurrentContext(), rcv, argc, argv);
    }
}

void CommandWrap::_OnCanExecuteChangedCallback(uv_async_t* handle)
{
    auto asyncData = reinterpret_cast<AsyncCanExecutedChangedDataType *>(handle->data);
    CommandWrap* pObservableCollectionWrap = asyncData->first;
    ICanExecuteChangedEventArgs *pEventArgs = asyncData->second;
    pObservableCollectionWrap->OnCanExecuteChangedCallback(pEventArgs);
    pEventArgs->Release();
    delete asyncData;
    CloseAndDeleteAsyncHandler(handle);
}

void CommandWrap::AddCanExecuteChanged(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Function> localFunc;
    if (!ThrowIfArgumentFunction(isolate, args, 0, localFunc))
    {
        return;
    }
    CommandWrap* pWrapper = ObjectWrap::Unwrap<CommandWrap>(args.Holder());

    IObservableObject *pObservableObject;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObservableObject))
    {
        return;
    }

    pWrapper->_canExecutedChangedCallbacks.push_back(Persistent<Function>(isolate, localFunc));
    pWrapper->_canExecuteChangedEventConnection.Attach(pObservableObject);
}

void CommandWrap::RemoveCanExecuteChanged(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Function> localFunc;
    if (!ThrowIfArgumentFunction(isolate, args, 0, localFunc))
    {
        return;
    }
    CommandWrap* pWrapper = ObjectWrap::Unwrap<CommandWrap>(args.Holder());

    IObservableObject *pObservableObject;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObservableObject))
    {
        return;
    }

    auto iter = std::find(
        pWrapper->_canExecutedChangedCallbacks.begin(),
        pWrapper->_canExecutedChangedCallbacks.end(),
        Persistent<Function>(isolate, localFunc));

    if (iter != pWrapper->_canExecutedChangedCallbacks.end())
    {
        pWrapper->_canExecutedChangedCallbacks.erase(iter);
        if (pWrapper->_canExecutedChangedCallbacks.size() == 0)
        {
            pWrapper->_canExecuteChangedEventConnection.Detach();
        }
    }
}
