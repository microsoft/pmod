/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:delegate_library.h
****/
#pragma once

#if _HAS_CPP0X
#include <functional>
#endif

#include "ctl/com_library.h"
#include "com_ptr.h"
#include "macros.h"
#include <foundation/library/interfaces/disposable.h>
#include <foundation/library/interfaces/dispatcher.h>

namespace foundation
{
// A helper object that automatically calls BeginCallback/EndCallback
// around the scope in which it is declared.
template <typename TDelegate>
class CallbackScope
{
public:

    CallbackScope(TDelegate *pDelegate) :
        m_Controller(nullptr)
    {
        if (pDelegate)
        {
            m_Controller = pDelegate->GetController();
            BeginCallback();
        }
    }

    ~CallbackScope()
    {
        EndCallback();
    }

private:
    void BeginCallback()
    {
        if (m_Controller)
        {
            m_Controller->AddRef();
        }
    }
    
    void EndCallback()
    {
        if (m_Controller)
        {
            m_Controller->Release();
        }
    }

private:
    foundation::IUnknown *m_Controller;
};



// Prologue code for methods which delegate to a callback, which does the following:
//  - Verifies the callback is non-null
//  - Declares a local strong reference which lasts the duration of the callback

#define _DELEGATE_CALLBACK_PROLOGUE(TSink,TDelegate)           \
    if (!this->HasCallback())                                   \
    {                                                           \
        return S_FALSE;                                         \
    }                                                           \
    foundation::CallbackScope< foundation::SinkDelegateBase<TSink, TDelegate> > callbackScope(this)

//////////////////////////////////////////////////////////////////////////
// DelegateBase Class
//////////////////////////////////////////////////////////////////////////
template <class TDelegate>
class DelegateBase:
    public ctl::ComInspectableBase,
    public TDelegate,
    public foundation::library::IDisposable
{
public:
    typedef TDelegate _TDelegate_type;

    foundation::IUnknown *GetController()
    {
        return m_Controller;
    }

protected:
    DelegateBase():
        m_Controller(nullptr)
    {
    }

    virtual ~DelegateBase()
    {
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == TDelegate::GetIID())
        {
            *ppInterface = static_cast<TDelegate *>(this);
        }
        else if (iid == foundation::library::IDisposable::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IDisposable *>(this);
        }
        else
        {
            return ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // Interface IDisposable
    STDMETHOD (Dispose)()
    {
        m_Controller = nullptr;
        return S_OK;
    }

protected:
    foundation::IUnknown *m_Controller; // Optional weak reference to the object we should addref/release when making the callback
};

//////////////////////////////////////////////////////////////////////////
// SinkDelegateBase Class
//////////////////////////////////////////////////////////////////////////
template <class TSink,class TDelegate>
class SinkDelegateBase: public DelegateBase<TDelegate>
{
public:
    typedef TSink _TSink_type;

    void SetCallback(TSink* instance, foundation::IUnknown *controller = nullptr)
    {
        this->m_Callback = instance;
        this->m_Controller = controller;
    }

    bool HasCallback()
    {
        return m_Callback != nullptr;
    }

    TSink *GetCallback() { return m_Callback; }
protected:
    SinkDelegateBase():
         m_Callback(nullptr)
    {
    }

    // Interface IDisposable
    STDMETHOD (Dispose)()
    {
        m_Callback = nullptr;
        return DelegateBase<TDelegate>::Dispose();
    }

protected:
    TSink*  m_Callback; // Weak reference to the object we should make the callback on
};

template <class T, class TBASE>
static HRESULT CreateDelegate(T* instance, TBASE **ppDelegate)
{
    ctl::ComInspectableObject<TBASE>::CreateInstance(ppDelegate);
    (*ppDelegate)->SetCallback(instance);
    return S_OK;
}

template <class T, class TBASE>
static HRESULT CreateDelegate(T* instance, foundation::IUnknown *controller, TBASE **ppDelegate)
{
    ctl::ComInspectableObject<TBASE>::CreateInstance(ppDelegate);
    (*ppDelegate)->SetCallback(instance, controller);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// EventHandlerDelegateBase Class
// Summary : Template for Event Handler Delegates
//////////////////////////////////////////////////////////////////////////
template <class TSink,class TDelegate,class TEventArgs>
class EventHandlerDelegateBase:
    public SinkDelegateBase<TSink,TDelegate>
{
public:
    typedef ctl::ComObject<EventHandlerDelegateBase<TSink,TDelegate,TEventArgs> >  ComObject;

protected:
    // EventHandler Interface
    STDMETHOD(Invoke)(foundation::IUnknown* pSender,TEventArgs* pArgs)
    {
        _DELEGATE_CALLBACK_PROLOGUE(TSink,TDelegate);
        return this->m_Callback->InvokeInternal(pSender,pArgs);
    }
};


//-----------------------------------------------------------------------------
// The DelegateEx classes take as a template parameter the method which is to
// be called when the even is received.
//
// NOTE: To help reduce code bloat due to template specialization, the methods
// here should be kept as simple as possible and avoid use of macro expansions
// such error-handling macros (IFR, etc).
//-----------------------------------------------------------------------------

template <
    typename TSink,
    typename TDelegate,
    typename TEventArgs
    >
class EventHandlerDelegateBaseEx :
    public SinkDelegateBase<TSink,TDelegate>
{
public:

    typedef ctl::ComObject<EventHandlerDelegateBaseEx<TSink, TDelegate, TEventArgs> > ComObject;

    template <
        typename _TSink,
        typename _TDelegate
        >
    static HRESULT CreateDelegateInstance(
        _In_ _TSink *pSink,
        _In_ HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
        _COM_Outptr_ _TDelegate **ppDelegate
        )
    {
        // If the sink object Implements foundation::IUnknown, then pass it in as the controller
        // (second parameter) as well as the callback (first parameter).  This will
        // ensure that AddRef/Release is called around the callback invocation.
        
        foundation::IUnknown *pController = nullptr;
            
        if (pSink)
        {
            pSink->QueryInterface(foundation_IID_IUnknown, reinterpret_cast<void **>(&pController));
            // we want a Weak reference to be passed into 'SetCallback'
            pController->Release();
        }
        return CreateDelegateInstance(pSink, pMethod,pController, ppDelegate);
    }

    template <
        typename _TSink,
        typename _TDelegate
        >
    static HRESULT CreateDelegateInstance(
        _In_ _TSink *pSink,
        _In_ HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
        _In_opt_ foundation::IUnknown *pController,
        _COM_Outptr_ _TDelegate **ppDelegate
        )
    {
        foundation_assert(pSink);
        foundation_assert(pMethod);

        HRESULT hr = (ctl::ComInspectableObject<_TDelegate>::CreateInstance(ppDelegate));
        IFR_ASSERT(hr);
        (*ppDelegate)->SetCallback(pSink, pController);
        (*ppDelegate)->_pMethod = pMethod;
        return S_OK;
    }

protected:
    EventHandlerDelegateBaseEx() :
        _pMethod(nullptr)
    {}
    // EventHandler Interface
    STDMETHOD(Invoke)(
        _In_ foundation::IUnknown *pSender,
        _In_ TEventArgs *pArgs
        )
    {
        _DELEGATE_CALLBACK_PROLOGUE(TSink,TDelegate);
        return (this->m_Callback->*this->_pMethod)(pSender, pArgs);
    }
private:
    HRESULT(TSink::*_pMethod)(foundation::IUnknown *, TEventArgs *);
};

//+-----------------------------------------------------------------------------
// Class:   FuncDelegateBase
//
// Purpose: Template to implement a delegate based on C++ func template
//------------------------------------------------------------------------------
template <class TDelegate,class _Func>
class FuncDelegateBase: public DelegateBase<TDelegate>
{
public:
    void SetFunc(const _Func& func)
    {
        _func = func;
    }

protected:
    FuncDelegateBase():
        _disposed(false)
    {
    }

    // Interface IDisposable
    STDMETHOD (Dispose)()
    {
        _disposed = true;
        return S_OK;
    }
protected:
    bool    _disposed;
    _Func   _func;
};

//+-----------------------------------------------------------------------------
// Function:   CreateFuncDelegate
//
// Purpose: Method template to create a new Delegate based on a Function Type
//------------------------------------------------------------------------------
template <class T>
static HRESULT CreateFuncDelegate(
    _In_ const typename T::_FunctionType& func,
    _COM_Outptr_ T **ppDelegateClass
    )
{
    T *pDelegateClass = nullptr;
    HRESULT hr = (ctl::ComInspectableObject<T>::CreateInstance(&pDelegateClass));
    if (SUCCEEDED(hr))
    {
        pDelegateClass->SetFunc(func);
    }
    *ppDelegateClass = pDelegateClass;
    return hr;
}

//+-----------------------------------------------------------------------------
// Class:   Dispatchable
//
// Purpose: Implements IDelegateDispatchable using a SinkDelegateBase class
//------------------------------------------------------------------------------
template <
    typename TInvokeParams,
    typename T,
    typename TSink
>
class Dispatchable : public 
    SinkDelegateBase<TSink,foundation::library::IDelegateDispatchable>
{
public:
    template <
        typename _TSink,
        typename _TDelegate
    >
    static HRESULT CreateDelegateInstance(
    _In_ _TSink *pSink,
    _In_ foundation::IUnknown *pController,
    _In_ const TInvokeParams& invokeParams,
        _COM_Outptr_ _TDelegate **ppDelegate
    )
    {
        HRESULT hr = (ctl::ComInspectableObject<_TDelegate>::CreateInstance(ppDelegate));

        if (SUCCEEDED(hr))
        {
            (*ppDelegate)->SetCallback(pSink, nullptr);
            if(pController)
            {
                pController->AddRef();
                (*ppDelegate)->m_pController = pController;
            }
            (*ppDelegate)->m_InvokeParams = invokeParams;
        }
        return hr;
    }
    TInvokeParams& GetInvokeParams()
    {
        return m_InvokeParams;
    }
protected:
    Dispatchable():
        m_pController(nullptr)
    {
    }

    ~Dispatchable()
    {
        RELEASE_INTERFACE(m_pController);
    }

    STDMETHOD(Invoke)()
    {
        if (!this->HasCallback())
        {
            return S_FALSE;
        }
        CallbackScope< DelegateBase<foundation::library::IDelegateDispatchable> > callbackScope(this);
        T *pThis = static_cast<T *>(this);
        return pThis->InvokeInternal(m_InvokeParams);
    }

private:
    foundation::IUnknown        *m_pController;
    TInvokeParams   m_InvokeParams;
};

//+-----------------------------------------------------------------------------
// Class:   InvokeEventDispatchable
//
// Purpose: Implements IDelegateDispatchable for an Event Notification
//------------------------------------------------------------------------------
template <typename TEventArgs>
struct _InvokeEventParams
{
    ComPtr<foundation::IUnknown>  m_spSender;
    ComPtr<TEventArgs> m_spEventArgs;
};

template <
    typename TSink,
    typename TEventArgs,
    HRESULT (TSink::*pMethod)(foundation::IUnknown *, TEventArgs *)
>
class InvokeEventDispatchable : public 
    Dispatchable<
        _InvokeEventParams<TEventArgs>,
        InvokeEventDispatchable<TSink,TEventArgs,pMethod>,
        TSink
    >
{
public:
    HRESULT InvokeInternal(const _InvokeEventParams<TEventArgs>& invokeParams)
    {
        return (this->m_Callback->*pMethod)(invokeParams.m_spSender,invokeParams.m_spEventArgs);
    }
};

#if _HAS_CPP0X
//+-----------------------------------------------------------------------------
// Class:   FuncInvoke0Delegate
//
// Purpose: Specialization of a FuncDelegateBase class that use the std::function
//------------------------------------------------------------------------------
template <class TDelegate>
class FuncInvoke0Delegate: public FuncDelegateBase<TDelegate,std::function<HRESULT(void)>>
{
public:
    typedef std::function<HRESULT(void)> _FunctionType;
protected:
    //  TDelegate Interface
    STDMETHOD(Invoke)()
    {
        if(this->_disposed)
        {
            return S_FALSE;
        }
        CallbackScope< FuncInvoke0Delegate<TDelegate> > callbackScope(this);

        return this->_func();
    }
};
//+-----------------------------------------------------------------------------
// Class:   FuncInvoke2Delegate
//
// Purpose: Specialization of a FuncDelegateBase class that use the std::function
//------------------------------------------------------------------------------
template <class TDelegate,class TParam1,class TParam2>
class FuncInvoke2Delegate: public FuncDelegateBase<TDelegate,std::function<HRESULT(TParam1,TParam2)>>
{
public:
    typedef std::function<HRESULT(TParam1,TParam2)> _FunctionType;
protected:
    //  TDelegate Interface
    STDMETHOD(Invoke)(TParam1 param1,TParam2 param2)
    {
        if(this->_disposed)
        {
            return S_FALSE;
        }
        CallbackScope< FuncInvoke2Delegate<TDelegate,TParam1,TParam2> > callbackScope(this);

        return this->_func(param1,param2);
    }
};

//+-----------------------------------------------------------------------------
// Class:   FuncInvoke5Delegate
//
// Purpose: Specialization of a FuncDelegateBase class that use the std::function
//------------------------------------------------------------------------------
template <class TDelegate, class TParam1, class TParam2, class TParam3, class TParam4, class TParam5>
class FuncInvoke5Delegate : public FuncDelegateBase<TDelegate, std::function<HRESULT(TParam1, TParam2, TParam3, TParam4, TParam5)>>
{
public:
    typedef std::function<HRESULT(TParam1, TParam2, TParam3, TParam4, TParam5)> _FunctionType;
protected:
    //  TDelegate Interface
    STDMETHOD(Invoke)(TParam1 param1, TParam2 param2, TParam3 param3, TParam4 param4, TParam5 param5)
    {
        if (this->_disposed)
        {
            return S_FALSE;
        }
        CallbackScope< FuncInvoke5Delegate<TDelegate, TParam1, TParam2, TParam3, TParam4, TParam5> > callbackScope(this);

        return this->_func(param1, param2, param3, param4, param5);
    }
};


//+-----------------------------------------------------------------------------
// Class:   FuncEventHandlerDelegate
//
// Purpose: Specialization of a FuncInvoke2Delegate for Event Handler delegates
//------------------------------------------------------------------------------
template <class TDelegate,class TEventArgs>
class FuncEventHandlerDelegate: public FuncInvoke2Delegate<TDelegate,foundation::IUnknown*,TEventArgs*>
{
};

#endif

}

