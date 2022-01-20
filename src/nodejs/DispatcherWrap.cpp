/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DispatcherWrap.cpp
****/
#include "pch.h"
#include "DispatcherWrap.h"
#include "NodeJsHelper.h"
#include <foundation/ctl/com_library.h>
#include <foundation/library/interfaces/disposable.h>
#include <foundation/library/async_operation_impl.h>

#include <uv.h>

using namespace v8;
using namespace foundation;
using namespace foundation::library;

Persistent<Function> DispatcherWrap::_constructor;
v8::Persistent<v8::FunctionTemplate> DispatcherWrap::_template;

class CUvDispatcher :
    public ctl::ComInspectableBase,
    public IDispatcher,
    public IDisposable
{
public:
    HRESULT _Initialize(uv_loop_t*loop)
    {
        this->_loop = loop;
        return S_OK;
    }
protected:
    CUvDispatcher():
        _loop(nullptr)
    {}

    // Interface IDispatcher
    STDMETHOD(HasThreadAccess)(_Out_ bool *pValue) override
    {
        *pValue = false;
        return S_OK;
    }

    STDMETHOD(RunAsync)(
        _In_ IDelegateDispatchable* pDelegateDispatchable,
        IAsyncOperation **ppAsyncAction) override
    {
        Baton *baton = new Baton(pDelegateDispatchable);
        int status = uv_queue_work(_loop,
            &baton->_request,
            Run, 
            (uv_after_work_cb)RunAfter);
        assert(status == 0);
        *ppAsyncAction = baton->_asyncOperationPtr;
        (*ppAsyncAction)->AddRef();
        return S_OK;
    }

    // Interface IDisposable
    STDMETHOD(Dispose)() override
    {
        return S_OK;
    }

    // ComBase overrides
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
    {
        if (iid == IDispatcher::GetIID())
        {
            *ppInterface = static_cast<IDispatcher *>(this);
        }
        else if (iid == IDisposable::GetIID())
        {
            *ppInterface = static_cast<library::IDisposable *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

protected:
    void FinalRelease() override
    {

    }

private:
    struct Baton {
        uv_work_t _request;
        IDelegateDispatchable* _dispatchable;
        ComPtr<AsyncOperationImpl> _asyncOperationPtr;
        HRESULT _hr;

        Baton(IDelegateDispatchable* dispatchable) :
            _dispatchable(dispatchable), _hr(0){
            _request.data = this;
            _dispatchable->AddRef();

            CreateAsyncOperationImpl(IAsyncOperation::GetIID(), _asyncOperationPtr.GetAddressOf());
        }
        virtual ~Baton() {
            _dispatchable->Release();
        }
    };

    static void Run(uv_work_t *req)
    {
        Baton* baton = static_cast<Baton*>(req->data);
        baton->_hr = baton->_dispatchable->Invoke();
    }

    static void RunAfter(uv_work_t *req,int status)
    {
        Baton* baton = static_cast<Baton*>(req->data);
        baton->_asyncOperationPtr->_SetCompleted(baton->_hr, nullptr);
        delete baton;
    }
private:
    uv_loop_t* _loop;
};

DispatcherWrap::DispatcherWrap() 
{
    ctl::inspectable::CreateInstanceWithInitialize<CUvDispatcher>(
        _ptr.GetAddressOf(),
        uv_default_loop());
}

DispatcherWrap::~DispatcherWrap()
{
}

void DispatcherWrap::Init(Handle<Object> exports)
{
    InspectableBaseWrap::Init(
        exports,
        _constructor,
        _template,
        "Dispatcher",
        SetPrototypeMethods,
        New);
}

void DispatcherWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
}

void DispatcherWrap::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    LocalContext context(isolate);

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new DispatcherWrap()`
        DispatcherWrap* obj = new DispatcherWrap();
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    }
    else {
        // Invoked as plain function `MyObject(...)`, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, _constructor);
        args.GetReturnValue().Set(cons->NewInstance(context.local(), argc, argv).ToLocalChecked());
    }
}
