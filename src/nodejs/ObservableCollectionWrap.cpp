/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionWrap.cpp
****/
#include "pch.h"
#include "ObservableCollectionWrap.h"
#include "NodeJsHelper.h"

using namespace v8;
using namespace foundation;
using namespace foundation::library;
using namespace pmod;

class NotifyCollectionChangedEventArgsWrap : public InspectableWrap<INotifyCollectionChangedEventArgs>
{
public:
    static void Init(v8::Handle<v8::Object> exports)
    {
        InspectableBaseWrap::Init(
            exports,
            _constructor,
            _template,
            "NotifyCollectionChangedEventArgs",
            SetPrototypeMethods);
    }

    static void Create(Isolate* isolate, INotifyCollectionChangedEventArgs *p, v8::Local<v8::Object>& instance)
    {
        NotifyCollectionChangedEventArgsWrap* obj = new NotifyCollectionChangedEventArgsWrap(p);
        instance = Local<Function>::New(isolate, _constructor)->NewInstance();
        obj->Wrap(instance);
    }
protected:
    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
    {
        InspectableBaseWrap::SetPrototypeMethods(tpl);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getAction", GetAction);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getNewStartingIndex", GetNewStartingIndex);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getOldStartingIndex", GetOldStartingIndex);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getNewItems", GetNewItems);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getOldItems", GetOldItems);
    }

private:
    explicit NotifyCollectionChangedEventArgsWrap(INotifyCollectionChangedEventArgs *p):
        InspectableWrap<INotifyCollectionChangedEventArgs>(p)
    {}

    static void GetAction(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        NotifyCollectionChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<NotifyCollectionChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        NotifyCollectionModelChangedAction action;
        pWrapper->_ptr->GetAction(&action);
        args.GetReturnValue().Set(Uint32::New(isolate, (uint32_t)action));
    }

    static void GetNewStartingIndex(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        NotifyCollectionChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<NotifyCollectionChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        UINT32 index;
        pWrapper->_ptr->GetNewStartingIndex(&index);
        args.GetReturnValue().Set(Uint32::New(isolate, index));
    }

    static void GetOldStartingIndex(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        NotifyCollectionChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<NotifyCollectionChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        UINT32 index;
        pWrapper->_ptr->GetOldStartingIndex(&index);
        args.GetReturnValue().Set(Uint32::New(isolate, index));
    }

    static void GetOldItems(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        NotifyCollectionChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<NotifyCollectionChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        InspectableArrayWrapper items;
        pWrapper->_ptr->GetOldItems(items.GetSizeAddressOf(), items.GetBufferAddressOf());
        
        ReturnInspectableArray(isolate, items, args);
    }

    static void GetNewItems(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        NotifyCollectionChangedEventArgsWrap* pWrapper = ObjectWrap::Unwrap<NotifyCollectionChangedEventArgsWrap>(args.Holder());
        if (!pWrapper->ThrowIfDisposed(isolate))
        {
            return;
        }
        InspectableArrayWrapper items;
        pWrapper->_ptr->GetNewItems(items.GetSizeAddressOf(), items.GetBufferAddressOf());

        ReturnInspectableArray(isolate, items, args);
    }

    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;
};

Persistent<Function> NotifyCollectionChangedEventArgsWrap::_constructor;
Persistent<v8::FunctionTemplate> NotifyCollectionChangedEventArgsWrap::_template;

Persistent<Function> ObservableCollectionWrap::_constructor;
Persistent<v8::FunctionTemplate> ObservableCollectionWrap::_template;

ObservableCollectionWrap::ObservableCollectionWrap(IObservableCollection *p)
{
    _observableCollectionPtr = p;
    _notifyCollectionChangedEventConnection.SetSink(this, &ObservableCollectionWrap::OnNotifyCollectionChanged);
}

ObservableCollectionWrap::~ObservableCollectionWrap() {
}

v8::Local<v8::Value> ObservableCollectionWrap::Create(IObservableCollection *p)
{
    Isolate* isolate = Isolate::GetCurrent();
    ObservableCollectionWrap* obj = new ObservableCollectionWrap(p);
    auto instance = Local<Function>::New(isolate, _constructor)->NewInstance();
    obj->Wrap(instance);
    return instance;
}

void ObservableCollectionWrap::Init(Handle<Object> exports)
{
    InspectableBaseWrap::Init(
        exports,
        _constructor,
        _template,
        "ObservableCollection",
        SetPrototypeMethods);
    NotifyCollectionChangedEventArgsWrap::Init(exports);
}

void ObservableCollectionWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
    ObjectWrap::SetPrototypeMethods(tpl);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getCount", GetCount);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getItem", GetItem);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addNotifyCollectionChanged", AddNotifyCollectionChanged);
    NODE_SET_PROTOTYPE_METHOD(tpl, "removeNotifyCollectionChanged", RemoveNotifyCollectionChanged);
}

void ObservableCollectionWrap::GetCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    ObservableCollectionWrap* pWrapper = ObjectWrap::Unwrap<ObservableCollectionWrap>(args.Holder());

    IObservableCollection *pObservableCollection;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObservableCollection))
    {
        return;
    }
    UINT32 count;
    pObservableCollection->GetCount(&count);
    args.GetReturnValue().Set(Uint32::New(isolate, count));
}

void ObservableCollectionWrap::GetItem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    ObservableCollectionWrap* pWrapper = ObjectWrap::Unwrap<ObservableCollectionWrap>(args.Holder());

    IObservableCollection *pObservableCollection;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObservableCollection))
    {
        return;
    }

    UINT32 index;
    if (!GetArgumentValue(isolate, args, 0, index))
    {
        return;
    }

    foundation::InspectablePtr valuePtr;
    HRESULT hr = pObservableCollection->GetItem(
        index,
        valuePtr.GetAddressOf());

    _NODEJS_ERROR_EXCP_(hr, U("IObservableCollection::GetItem"));
    _NODEJS_RETURN_VALUE_EXCP_(valuePtr);
}

HRESULT ObservableCollectionWrap::OnNotifyCollectionChanged(foundation::IUnknown *pSender, INotifyCollectionChangedEventArgs *pEventArgs)
{
    if (IsMainThread())
    {
        this->OnNotifyCollectionChangedCallback(pEventArgs);
    }
    else
    {
        auto asyncData = new AsyncNotifyCollectionChangedDataType(this, pEventArgs);
        pEventArgs->AddRef();

        auto asyncHandle = new uv_async_t();
        asyncHandle->data = asyncData;
        uv_async_init(uv_default_loop(), asyncHandle, _OnNotifyCollectionChangedCallback);

        uv_async_send(asyncHandle);
    }
    return S_OK;
}

void ObservableCollectionWrap::OnNotifyCollectionChangedCallback(INotifyCollectionChangedEventArgs *pEventArgs)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Object> eventArgs;
    NotifyCollectionChangedEventArgsWrap::Create(isolate, pEventArgs, eventArgs);
    const unsigned argc = 1;
    Local<Value> argv[argc] = { eventArgs };

    for (auto iter = _notifyCollectionChangedCallbacks.begin(); iter != _notifyCollectionChangedCallbacks.end(); ++iter)
    {
        auto cb = Local<Function>::New(isolate, (*iter));
        cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
}

void ObservableCollectionWrap::_OnNotifyCollectionChangedCallback(uv_async_t* handle)
{
    auto asyncData = reinterpret_cast<AsyncNotifyCollectionChangedDataType *>(handle->data);
    ObservableCollectionWrap* pObservableCollectionWrap = asyncData->first;
    INotifyCollectionChangedEventArgs *pEventArgs = asyncData->second;
    pObservableCollectionWrap->OnNotifyCollectionChangedCallback(pEventArgs);
    pEventArgs->Release();
    delete asyncData;
    CloseAndDeleteAsyncHandler(handle);
}

void ObservableCollectionWrap::AddNotifyCollectionChanged(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Function> localFunc;
    if (!ThrowIfArgumentFunction(isolate, args, 0, localFunc))
    {
        return;
    }
    ObservableCollectionWrap* pWrapper = ObjectWrap::Unwrap<ObservableCollectionWrap>(args.Holder());

    IObservableObject *pObservableObject;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObservableObject))
    {
        return;
    }

    pWrapper->_notifyCollectionChangedCallbacks.push_back(Persistent<Function>(isolate, localFunc));
    pWrapper->_notifyCollectionChangedEventConnection.Attach(pObservableObject);
}

void ObservableCollectionWrap::RemoveNotifyCollectionChanged(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Function> localFunc;
    if (!ThrowIfArgumentFunction(isolate, args, 0, localFunc))
    {
        return;
    }
    ObservableCollectionWrap* pWrapper = ObjectWrap::Unwrap<ObservableCollectionWrap>(args.Holder());

    IObservableObject *pObservableObject;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObservableObject))
    {
        return;
    }

    auto iter = std::find(
        pWrapper->_notifyCollectionChangedCallbacks.begin(),
        pWrapper->_notifyCollectionChangedCallbacks.end(),
        Persistent<Function>(isolate, localFunc));

    if (iter != pWrapper->_notifyCollectionChangedCallbacks.end())
    {
        pWrapper->_notifyCollectionChangedCallbacks.erase(iter);
        if (pWrapper->_notifyCollectionChangedCallbacks.size() == 0)
        {
            pWrapper->_notifyCollectionChangedEventConnection.Detach();
        }
    }
}
