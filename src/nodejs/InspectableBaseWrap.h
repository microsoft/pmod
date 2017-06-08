/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InspectableBaseWrap.h
****/
#pragma once

#include <uv.h>

class InspectableBaseWrap : public node::ObjectWrap
{
public:
    virtual foundation::IInspectable **GetInspectablePtr() = 0;
protected:
    bool ThrowIfGetInspectablePtr(v8::Isolate* isolate, foundation::IInspectable **pp);

    template <class T>
    bool ThrowIfGetPtr(v8::Isolate* isolate, T **ppT)
    {
        foundation::IInspectable *pp;
        if (!ThrowIfGetInspectablePtr(isolate, &pp))
        {
            return false;
        }
        *ppT = reinterpret_cast<T *>(pp);
        return true;
    }

    virtual void Dispose(bool dispose);

    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);
    static bool ThrowIfDisposed(v8::Isolate* isolate, foundation::IInspectable *pInspectable);
    static void Init(
        v8::Handle<v8::Object> exports,
        v8::Persistent<v8::Function> &ctor,
        v8::Persistent<v8::FunctionTemplate> &function,
        const char *className,
        void(*_InitPrototypes)(v8::Local<v8::FunctionTemplate>& tpl),
        v8::FunctionCallback _new = nullptr);

    static void CloseAndDeleteAsyncHandler(uv_async_t* handle);
private:
    static void Dispose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _CloseAsyncHelper(uv_handle_t* handle);
};

template <class PtrType>
class InspectableWrap : public InspectableBaseWrap
{
protected:
    foundation::IInspectable **GetInspectablePtr() override
    {
        return reinterpret_cast<foundation::IInspectable **>(_ptr.GetAddressOfPtr());
    }
    bool ThrowIfDisposed(v8::Isolate* isolate)
    {
        return InspectableBaseWrap::ThrowIfDisposed(isolate, _ptr);
    }

    foundation::ComPtr<PtrType> _ptr;
    explicit InspectableWrap(PtrType *p) :
        _ptr(p)
    {}
    explicit InspectableWrap()
    {}
    ~InspectableWrap()
    {
        _ptr.Release();
    }
};
