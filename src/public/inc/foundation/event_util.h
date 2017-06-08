/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:event_util.h
****/
#pragma once

#include "macros.h"

#include <foundation/library/interfaces/disposable.h>

#include "delegate_library.h"
#include "delegate_weak_ref_library.h"

#include "array_wrapper.h"
#include "com_ptr.h"
#include "weak_reference.h"

#include <vector> 
#include <algorithm>

#if _HAS_CPP0X
#include <type_traits> 
#endif

namespace foundation
{
    //------------------------------------------------------------------------------
    // Class:   BaseListener template
    //
    // Purpose: Base Template for All Listeners
    //
    //------------------------------------------------------------------------------
    template <class T, class TSource, class TEventHandler, class TEventArgs, class TEventSource>
    class BaseListener :
        public TEventHandler
    {
    public:
        static HRESULT AttachEventHandler(TSource *pSource, TEventHandler *pEventHandler)
        {
            IFCPTR_ASSERT(pSource);
            IFCPTR_ASSERT(pEventHandler);

            ComPtr<TEventSource> spEventSource;
            IFR_ASSERT(T::GetEventSource(pSource, spEventSource.GetAddressOf()));
            IFR_ASSERT(spEventSource->AddHandler(pEventHandler));
            return S_OK;
        }

        static HRESULT DetachEventHandler(TSource *pSource, TEventHandler *pEventHandler)
        {
            IFCPTR_ASSERT(pSource);
            IFCPTR_ASSERT(pEventHandler);

            ComPtr<TEventSource> spEventSource;
            IFR_ASSERT(T::GetEventSource(pSource, spEventSource.GetAddressOf()));
            IFR_ASSERT(spEventSource->RemoveHandler(pEventHandler));
            return S_OK;
        }

        static HRESULT AttachEventHandler(TSource *pSource, foundation::IUnknown *pUnkEventHandler)
        {
            TEventHandler *pEventHandler = nullptr;
            foundation::QueryWeakReference(pUnkEventHandler, &pEventHandler);
            return AttachEventHandler(pSource, pEventHandler);
        }
        static HRESULT DetachEventHandler(TSource *pSource, foundation::IUnknown *pUnkEventHandler)
        {
            TEventHandler *pEventHandler = nullptr;
            foundation::QueryWeakReference(pUnkEventHandler, &pEventHandler);
            return DetachEventHandler(pSource, pEventHandler);
        }
    public:
        HRESULT Attach(TSource *pSource)
        {
            return AttachEventHandler(pSource, this);
        }
        HRESULT Detach(TSource *pSource)
        {
            return DetachEventHandler(pSource, this);
        }
    protected:
        // pure Virtual Methods
        virtual HRESULT InvokeInternal(foundation::IUnknown *pSender, TEventArgs *pArgs) = 0;

        // Interface TEventHandler
        virtual HRESULT STDMETHODCALLTYPE Invoke(
            foundation::IUnknown *pSender,
            TEventArgs *pArgs)
        {
            return InvokeInternal(pSender, pArgs);
        }
    };
//------------------------------------------------------------------------------
// Class:   EventHandlerDelegateFactory
//
// Purpose: Template for a Delegate Factory
//
//------------------------------------------------------------------------------
template
    <
    typename TSink,
    typename TEventHandler,
    typename TEventArgs
    >
class EventHandlerDelegateFactory
{
public:
    typedef EventHandlerDelegateBaseEx
        <
        TSink,
        TEventHandler,
        TEventArgs
        > _type_eventhandler_delegate;

    template < typename T>
    static HRESULT Connect(
        T *pT,
        TSink *pSink,
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *)
        )
    {
        _type_eventhandler_delegate *pEventHandlerDelegate = nullptr;
#if _HAS_CPP0X
        IFR_ASSERT(CreateInstance(pSink, pMethod,&pEventHandlerDelegate));
#else
        IFR_ASSERT(CreateInstance(pSink,pMethod,nullptr,&pEventHandlerDelegate));
#endif
        return pT->SetDelegate(pEventHandlerDelegate);
    }

#if _HAS_CPP0X
    static HRESULT CreateInstance(
        TSink *pSink,
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
        _type_eventhandler_delegate **ppEventHandlerDelegate)
    {
        return _CreateDelegateInternal<TSink>(pSink, pMethod,ppEventHandlerDelegate);
    }
#endif

    static HRESULT CreateInstance(
        TSink *pSink,
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
        foundation::IUnknown *pController,
        _type_eventhandler_delegate **ppEventHandlerDelegate)
    {
        return _type_eventhandler_delegate::CreateDelegateInstance(pSink, pMethod,pController, ppEventHandlerDelegate);
    }

private:
#if _HAS_CPP0X
    // Create the Delegate when foundation::IUnknown type is match
    template <typename TSinkInternal>
    static HRESULT _CreateDelegateInternal(
        TSinkInternal *pSink,
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
        _type_eventhandler_delegate **ppEventHandlerDelegate,
        typename std::enable_if<std::is_base_of<foundation::IUnknown,TSinkInternal>::value>::type* p = nullptr)
    {
        return _CreateDelegateWithControllerInternal(pSink, pMethod,ppEventHandlerDelegate);
    }

    // Create the Delegate when foundation::IUnknown type is NOT match
   template <typename TSinkInternal>
   static HRESULT _CreateDelegateInternal(
       TSinkInternal *pSink,
       HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
       _type_eventhandler_delegate **ppEventHandlerDelegate,
       typename std::enable_if<!std::is_base_of<foundation::IUnknown,TSinkInternal>::value>::type* p = nullptr)
   {
       typename _type_eventhandler_delegate::ComObject *pTypeEventHandlerDelegate = nullptr;
       IFR_ASSERT(_type_eventhandler_delegate::CreateDelegateInstance(
            pSink,
            pMethod,
            nullptr,
            &pTypeEventHandlerDelegate));
       *ppEventHandlerDelegate = pTypeEventHandlerDelegate;
       return S_OK;
   }

   // Create the Delegate when _ComBaseType type is match
   template <typename TSinkInternal>
   static HRESULT _CreateDelegateWithControllerInternal(
       TSinkInternal *pSink,
       HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
       _type_eventhandler_delegate **ppEventHandlerDelegate,
       typename std::enable_if<std::is_base_of<typename TSinkInternal::_ComBaseType, TSinkInternal>::value>::type* p = nullptr)
   {
       typename _type_eventhandler_delegate::ComObject *pTypeEventHandlerDelegate = nullptr;
       IFR_ASSERT(_type_eventhandler_delegate::CreateDelegateInstance(
           pSink,
           pMethod,
           ctl::CastToUnknown(pSink),
           &pTypeEventHandlerDelegate));
       *ppEventHandlerDelegate = pTypeEventHandlerDelegate;
       return S_OK;
   }

   // Create the Delegate when ComInspectableBase type is NOT match
   template <typename TSinkInternal>
   static HRESULT _CreateDelegateWithControllerInternal(
       TSinkInternal *pSink,
       HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
       _type_eventhandler_delegate **ppEventHandlerDelegate,
       typename std::enable_if<!std::is_base_of<typename TSinkInternal::_ComBaseType, TSinkInternal>::value>::type* p = nullptr)
   {
       typename _type_eventhandler_delegate::ComObject *pTypeEventHandlerDelegate = nullptr;
       IFR_ASSERT(_type_eventhandler_delegate::CreateDelegateInstance(
           pSink,
           pMethod,
           &pTypeEventHandlerDelegate));
       *ppEventHandlerDelegate = pTypeEventHandlerDelegate;
       return S_OK;
   }
#endif
};
//------------------------------------------------------------------------------
// Class:   EventHandlerConnection
//
// Purpose: Template for an Event Handler Connection which contains a Delegate reference
//
//------------------------------------------------------------------------------
template 
    <
    typename TEventHandler,
    typename TEventArgs
    >
class EventHandlerConnection
{
protected:
    EventHandlerConnection() :
        m_pDelegate(nullptr)
    {
    }

    ~EventHandlerConnection() 
    {
        ReleaseIfDelegate();
    }
public:
    typedef TEventHandler   _TEventHandlerType;
    typedef TEventArgs      _TEventArgsType;

#if _HAS_CPP0X
    typedef typename foundation::FuncEventHandlerDelegate<TEventHandler,TEventArgs>  _TFuncEventHandlerDelegateType;
#endif

    template<typename TSink>
    class _DelegateFactory :
        public EventHandlerDelegateFactory<TSink,TEventHandler,TEventArgs>
    {};

protected:
#if _HAS_CPP0X
    template < typename T>
    static HRESULT Connect(_In_ T *pT,_In_  const typename _TFuncEventHandlerDelegateType::_FunctionType& func)
    {
        _TFuncEventHandlerDelegateType *pEventHandlerDelegateClass = nullptr;
        IFR_ASSERT(CreateFuncDelegate(func,&pEventHandlerDelegateClass));
        return pT->SetDelegate(pEventHandlerDelegateClass);
    }
#endif

    void ReleaseIfDelegate()
    {
        // This object should be a member of the sink object, so the sink object
        // is also being destroyed.  For added safety clear the delegate callback.
        if (m_pDelegate)
        {
            foundation::library::IDisposable *pDisposable;
            if(SUCCEEDED(foundation::QueryWeakReference(m_pDelegate,&pDisposable)))
            {
                pDisposable->Dispose();
            }
            m_pDelegate->Release();
            m_pDelegate = nullptr;
        }
    }

protected:
    TEventHandler               *m_pDelegate;
};

//-----------------------------------------------------------------------------
// The event handler connection classes encapsulate the management of an event
// source (or multiple sources), an event sink (the class receiving events),
// and the intermediary delegate object.  The single-source connection classes
// are lightweight and simple to use.  The multi-source connection classes allow
// multiple sources to be attached to the same connection.
//
// Note that these helper classes are designed to be "forgiving", in that they
// won't fail or foundation_assert if attempting to Attach to or Detach from an invalid
// source.
//-----------------------------------------------------------------------------

template 
<
    typename TSource,
    typename TEventHandler,
    typename TEventArgs,
    typename TListenerBase,
    typename TRefHolder = StrongReferenceHolder<TSource>
>
class SingleSourceEventHandlerConnection :
    public EventHandlerConnection<TEventHandler,TEventArgs>
{
protected:
	typedef EventHandlerConnection<TEventHandler,TEventArgs> TBASE;
public:
#if _HAS_CPP0X
    SingleSourceEventHandlerConnection(_In_ const typename TBASE::_TFuncEventHandlerDelegateType::_FunctionType& func) :
        m_IsAttached(false)
    {
        IGNOREHR(SetSink(func));
    }
#endif

    SingleSourceEventHandlerConnection() :
        m_IsAttached(false)
    {
    }

    ~SingleSourceEventHandlerConnection() 
    {
        Detach();
    }

#if _HAS_CPP0X
    HRESULT SetSink(_In_ const typename TBASE::_TFuncEventHandlerDelegateType::_FunctionType& func)
    {
        return TBASE::Connect(this,func);
    }

    template <class _Func>
    static HRESULT SetSinkOnce(
        TSource *p_source,
        const _Func& sink)
    {
        IFCPTR_ASSERT(p_source);
        struct ComConnection : ctl::ComBase
        {
            void FinalRelease() override
            {
                _connection.Detach();
                ctl::ComBase::FinalRelease();
            }
            SingleSourceEventHandlerConnection _connection;
        };
        ComConnection *p_com_connection = nullptr;
        ctl::ComObject<ComConnection>::CreateInstance(&p_com_connection);

        p_com_connection->_connection.SetSink([=]
            (foundation::IUnknown *pSender, TEventArgs *pEventArgs) -> HRESULT {

            HRESULT hr = sink(pEventArgs);
            p_com_connection->Release();
            return hr;
        });
        // sink to events
        ComPtr<foundation::IUnknown> punk_ptr(p_com_connection);
        return p_com_connection->_connection.Attach(p_source);
    }

#endif

    template<typename TSink>
    HRESULT SetSink(
        _In_ TSink *pSink,
        _In_ HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *)
    )
    {
        return EventHandlerDelegateFactory
            <
            TSink,
            TEventHandler,
            TEventArgs
            >::Connect(this, pSink, pMethod);
    }

    template<typename TSinkInterface,typename TSink>
    HRESULT SetSinkWeakRef(
        _In_ TSink *pSink,
        _In_ HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *)
        )
    {
        TEventHandler *pEventHandlerDelegate;
        IFR_ASSERT(CreateEventHandlerDelegateWeakRef<TSinkInterface>(pSink, pMethod, &pEventHandlerDelegate));
        return this->SetDelegate(pEventHandlerDelegate);
    }

    HRESULT SetDelegate(TEventHandler  *pDelegate)
    {
        foundation_assert(pDelegate);
        bool isAttached = this->m_IsAttached;
        TSource *p_source = nullptr;
        if (isAttached)
        {
            p_source = this->GetSource();
            foundation_assert(p_source != nullptr);
            this->Detach();
        }
        this->ReleaseIfDelegate();
        this->m_pDelegate = pDelegate;
        HRESULT hr = S_OK;
        if (isAttached)
        {
            hr = this->Attach(p_source);
        }
        return hr;
    }

    HRESULT Attach(_In_opt_ TSource *pSource)
    {
        HRESULT hr = S_OK;

        if (pSource != GetSource())
        {
            // Detach from any previous source
            Detach();
            if (pSource)
            {
                // Record the source and Attach
                m_RefHolder.Attach(pSource);
                return AttachHandler();
            }
        }        
        return hr;
    }

    HRESULT Attach(_In_ IUnknown *pSource)
    {
        IFCPTR_ASSERT(pSource);

        ComPtr<TSource> sourcePtr;
        IFR_ASSERT(QueryInterface(pSource, sourcePtr.GetAddressOf()));
        return Attach(sourcePtr);
    }

    HRESULT Detach()
    {
        HRESULT hr = S_OK;
        if (m_RefHolder.IsReferenceAttached())
        {
            hr =  DetachHandler();
            m_RefHolder.Clear();
        }
        return hr;
    }

    HRESULT dispose()
    {
        HRESULT hr = Detach();
        this->ReleaseIfDelegate();
        return hr;
    }

    // Attach to existing Source
    HRESULT AttachHandler()
    {
        HRESULT hr = S_OK;
        ComPtr<TSource> spSource;
        hr = ResolveReferenceHolder(m_RefHolder,false,*spSource.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        if(!m_IsAttached)
        {
            foundation_assert(this->m_pDelegate);
            IFCPTR_ASSERT(this->m_pDelegate);

            // Attach the delegate to the current source
            hr = TListenerBase::AttachEventHandler(spSource, this->m_pDelegate);
            if (FAILED(hr))
            {
                return hr;
            }
            m_IsAttached = true;
        }
        return hr;
    }

    HRESULT DetachHandler()
    {
        if(m_IsAttached)
        {
            HRESULT hr;
            ComPtr<TSource> spSource;
            hr = ResolveReferenceHolder(m_RefHolder,true,*spSource.GetAddressOf());
            if (FAILED(hr))
            {
                return hr;
            }
            // Even if we are attached we will tolerate that our 'Source' object could be gone
            if(spSource != nullptr)
            {
                foundation_assert(this->m_pDelegate != nullptr);
                // Note: the delegate can be re-used for attaching to a different source, so
                // we don't bother to release it here.

                HRESULT hr = TListenerBase::DetachEventHandler(spSource, this->m_pDelegate);
                foundation_assert(SUCCEEDED(hr));
            }
            m_IsAttached = false;
            return hr;
        }
        return S_OK;
    }

    TSource *GetSource()
    {
        ComPtr<TSource> spSource;
        IGNOREHR(m_RefHolder.ResolveReference(*spSource.GetAddressOf()));
        return spSource;
    }

    template <class Q>
    Q *GetSourceAs()
    {
        TSource *pSource = GetSource();
        Q *pQ = nullptr;
        if(pSource)
        {
            foundation::QueryWeakReference(pSource,&pQ);
        }
        return pQ;
    }

    void SetSource(TSource *pSource)
    {
        Attach(nullptr);
        m_RefHolder.Attach(pSource);
    }


private:
    TRefHolder                  m_RefHolder;
    bool                        m_IsAttached;
};

template <
    typename TSource,
    typename TEventHandler,
    typename TEventArgs,
    typename TListenerBase
>
class MultiSourceEventHandlerConnection:
    public EventHandlerConnection<TEventHandler,TEventArgs>
{
protected:
    typedef EventHandlerConnection<TEventHandler, TEventArgs> TBASE;
public:
#if _HAS_CPP0X
    MultiSourceEventHandlerConnection(_In_ const typename TBASE::_TFuncEventHandlerDelegateType::_FunctionType& func)
    {
        IGNOREHR(SetSink(func));
    }
#endif

    MultiSourceEventHandlerConnection()
    {
    }

    ~MultiSourceEventHandlerConnection() 
    {
        DetachAll();
    }

    template<typename TSink>
    HRESULT SetSink(
        _In_ TSink *pSink,
        _In_ HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *)
        )
    {
        return EventHandlerDelegateFactory
                    <
                    TSink,
                    TEventHandler,
                    TEventArgs
                    >::Connect(this, pSink, pMethod);
    }

#if _HAS_CPP0X
    HRESULT SetSink(_In_ const typename TBASE::_TFuncEventHandlerDelegateType::_FunctionType& func)
    {
        return TBASE::Connect(this, func);
    }
#endif

    HRESULT SetDelegate(TEventHandler  *pDelegate)
    {
        foundation_assert(pDelegate);

        this->DetachAllEventHandlers();
        // release previous delegate
        this->ReleaseIfDelegate();
        this->m_pDelegate = pDelegate;
        // now Attach all our existing sources to the new delegate
        HRESULT hr = S_OK;
        for(typename std::vector<ComPtr<TSource> >::iterator
            iter = m_Sources.begin();
            iter != m_Sources.end();
        ++iter)
        {
            hr = TListenerBase::AttachEventHandler(*iter, this->m_pDelegate);
            foundation_assert(SUCCEEDED(hr));
        }
        return hr;
    }

    HRESULT Attach(_In_ TSource *pSource)
    {
        HRESULT hr = S_OK;

        if (pSource)
        {
            // Verify that we haven't already attached to this source
            if (std::find(m_Sources.begin(),m_Sources.end(), pSource)==m_Sources.end())
            {
                foundation_assert(this->m_pDelegate);
                IFCPTR_ASSERT(this->m_pDelegate);

                // Attach the delegate to the source
                hr = TListenerBase::AttachEventHandler(pSource, this->m_pDelegate);
                IFHR_ASSERT(hr);
                // Record this source
                m_Sources.push_back(pSource);
            }
        }

        return S_OK;
    }

    HRESULT Detach(_In_ TSource *pSource)
    {
        HRESULT hr = S_OK;
        if (pSource)
        {
            typename std::vector<ComPtr<TSource> >::iterator iter =
                std::find(m_Sources.begin(),m_Sources.end(), pSource);
            if (iter != m_Sources.end())
            {
                // Note: the delegate can be re-used for attaching to a different source, so
                // we don't bother to release it here.

                foundation_assert(this->m_pDelegate);
                hr = TListenerBase::DetachEventHandler(pSource, this->m_pDelegate);
                foundation_assert(SUCCEEDED(hr));
                // remove from our vector
                m_Sources.erase(iter);
                return S_OK;
            }
        }
        return S_FALSE;
    }

    HRESULT DetachAll()
    {
        _IFR_(DetachAllEventHandlers());
        // clear our sources
        m_Sources.clear();
        return S_OK;
    }
private:
    HRESULT DetachAllEventHandlers()
    {
        HRESULT hr = S_OK;
        for (typename std::vector<ComPtr<TSource> >::iterator
            iter = m_Sources.begin();
            iter != m_Sources.end();
        ++iter)
        {
            hr = TListenerBase::DetachEventHandler(*iter, this->m_pDelegate);
            foundation_assert(SUCCEEDED(hr));
        }
        return hr;
    }

private:
    std::vector<ComPtr<TSource> > m_Sources;
};

template 
    <
    typename TBASE,
    typename TSink
    >
class SinkEventHandlerConnection :
    public TBASE
{
public:
    SinkEventHandlerConnection(
        TSink *pSink, 
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, typename TBASE::_TEventArgsType *)
        )
    {
        EventHandlerDelegateFactory
        <
            TSink,
            typename TBASE::_TEventHandlerType,
            typename TBASE::_TEventArgsType
        >::Connect(this, pSink, pMethod);
    }
};

}
