/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectWrap.cpp
****/
#include "pch.h"
#include "ObservableObjectWrap.h"
#include "NodeJsHelper.h"

using namespace v8;
using namespace foundation;
using namespace foundation::library;
using namespace pmod;

class PropertyChangedEventArgsWrap : public InspectableWrap<IPropertyChangedEventArgs>
{
public:
    static void Init(v8::Handle<v8::Object> exports)
    {
        InspectableBaseWrap::Init(
            exports,
            _constructor,
            _template,
            "PropertyChangedEventArgs",
            SetPrototypeMethods);
    }

    static void Create(Isolate* isolate, IPropertyChangedEventArgs *p, v8::Local<v8::Object>& instance)
    {
        PropertyChangedEventArgsWrap* obj = new PropertyChangedEventArgsWrap(p);
        instance = Local<Function>::New(isolate, _constructor)->NewInstance();
        obj->Wrap(instance);
    }
protected:
    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
    {
        InspectableBaseWrap::SetPrototypeMethods(tpl);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getProperty", GetProperty);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getOldValue", GetOldValue);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getNewValue", GetNewValue);
    }

private:
    explicit PropertyChangedEventArgsWrap(IPropertyChangedEventArgs *p):
        InspectableWrap<IPropertyChangedEventArgs>(p)
    {}

    static void GetProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        PropertyChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<PropertyChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        UINT32 propertyId;
        pWrapper->_ptr->GetProperty(&propertyId);
        args.GetReturnValue().Set(Uint32::New(isolate, propertyId));
    }

    static void GetOldValue(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        PropertyChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<PropertyChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        InspectablePtr valuePtr;
        pWrapper->_ptr->GetOldValue(valuePtr.GetAddressOf());
        _NODEJS_RETURN_VALUE_EXCP_(valuePtr);
    }

    static void GetNewValue(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        PropertyChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<PropertyChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        InspectablePtr valuePtr;
        pWrapper->_ptr->GetNewValue(valuePtr.GetAddressOf());
        _NODEJS_RETURN_VALUE_EXCP_(valuePtr);
    }

    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;
};
Persistent<Function> PropertyChangedEventArgsWrap::_constructor;
Persistent<v8::FunctionTemplate> PropertyChangedEventArgsWrap::_template;

Persistent<Function> ObservableObjectWrap::_constructor;
Persistent<v8::FunctionTemplate> ObservableObjectWrap::_template;

ObservableObjectWrap::ObservableObjectWrap(IObservableObject *p):
    ObjectDispatchWrap(p)
{
    _propertyChangedEventConnection.SetSink(this, &ObservableObjectWrap::OnPropertyChanged);
}

ObservableObjectWrap::~ObservableObjectWrap() {
}

v8::Local<v8::Value> ObservableObjectWrap::Create(IObservableObject *p)
{
    Isolate* isolate = Isolate::GetCurrent();
    ObservableObjectWrap* obj = new ObservableObjectWrap(p);
    auto instance = Local<Function>::New(isolate, _constructor)->NewInstance();
    obj->Wrap(instance);
    return instance;
}

void ObservableObjectWrap::Init(Handle<Object> exports)
{
    InspectableBaseWrap::Init(
        exports,
        _constructor,
        _template,
        "ObservableObject",
        SetPrototypeMethods);
    PropertyChangedEventArgsWrap::Init(exports);
}

void ObservableObjectWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
    ObjectDispatchWrap::SetPrototypeMethods(tpl);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addPropertyChanged", AddPropertyChanged);
    NODE_SET_PROTOTYPE_METHOD(tpl, "removePropertyChanged", RemovePropertyChanged);
}

HRESULT ObservableObjectWrap::OnPropertyChanged(foundation::IUnknown *pSender, pmod::IPropertyChangedEventArgs *pEventArgs)
{
    if (IsMainThread())
    {
        this->OnPropertyChangedCallback(pEventArgs);
    }
    else
    {
        auto asyncData = new AsyncPropertyChangedDataType(this, pEventArgs);
        pEventArgs->AddRef();

        auto asyncHandle = new uv_async_t();
        asyncHandle->data = asyncData;
        uv_async_init(uv_default_loop(), asyncHandle, _OnPropertyChangedCallback);

        uv_async_send(asyncHandle);
    }
    return S_OK;
}

void ObservableObjectWrap::OnPropertyChangedCallback(pmod::IPropertyChangedEventArgs *pEventArgs)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Object> eventArgs;
    PropertyChangedEventArgsWrap::Create(isolate, pEventArgs, eventArgs);
    const unsigned argc = 1;
    Local<Value> argv[argc] = { eventArgs };

    for (auto iter = _propertyChangesCallbacks.begin(); iter != _propertyChangesCallbacks.end(); ++iter)
    {
        auto cb = Local<Function>::New(isolate, (*iter));
        cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
}

void ObservableObjectWrap::_OnPropertyChangedCallback(uv_async_t* handle)
{
    auto asyncData = reinterpret_cast<AsyncPropertyChangedDataType *>(handle->data);
    ObservableObjectWrap* pObservableObjectWrap = asyncData->first;
    IPropertyChangedEventArgs *pEventArgs = asyncData->second;
    pObservableObjectWrap->OnPropertyChangedCallback(pEventArgs);
    pEventArgs->Release();
    delete asyncData;
    CloseAndDeleteAsyncHandler(handle);
}

void ObservableObjectWrap::AddPropertyChanged(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Function> localFunc;
    if (!ThrowIfArgumentFunction(isolate, args, 0, localFunc))
    {
        return;
    }
    ObservableObjectWrap* pWrapper = ObjectWrap::Unwrap<ObservableObjectWrap>(args.Holder());

    IObservableObject *pObservableObject;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObservableObject))
    {
        return;
    }

    pWrapper->_propertyChangesCallbacks.push_back(Persistent<Function>(isolate, localFunc));
    pWrapper->_propertyChangedEventConnection.Attach(pObservableObject);
}

void ObservableObjectWrap::RemovePropertyChanged(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Function> localFunc;
    if (!ThrowIfArgumentFunction(isolate, args, 0, localFunc))
    {
        return;
    }
    ObservableObjectWrap* pWrapper = ObjectWrap::Unwrap<ObservableObjectWrap>(args.Holder());

    IObservableObject *pObservableObject;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObservableObject))
    {
        return;
    }

    auto iter = std::find(
        pWrapper->_propertyChangesCallbacks.begin(),
        pWrapper->_propertyChangesCallbacks.end(),
        Persistent<Function>(isolate, localFunc));

    if (iter != pWrapper->_propertyChangesCallbacks.end())
    {
        pWrapper->_propertyChangesCallbacks.erase(iter);
        if (pWrapper->_propertyChangesCallbacks.size() == 0)
        {
            pWrapper->_propertyChangedEventConnection.Detach();
        }
    }
}
