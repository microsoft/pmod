/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventHandler.h
****/

#pragma once

#include <foundation/library/event_handler_container.h>
#include <foundation/library/fire_event_support.h>

#include <foundation/library/interfaces/disposable.h>
#include <foundation/library/logger_macros.h>

#ifdef _MSC_VER

#pragma warning( push )
#pragma warning( disable : 4503 )

#endif

//------------------------------------------------------------------------------
// Class:   CLockDelegate
//
// Purpose: A class to delegate a concrete lock implementation
//
//------------------------------------------------------------------------------
template <class TLockImpl>
class CLockDelegate
{
public:
	CLockDelegate() :
		m_lockImpl(nullptr)
	{
	}

	void SetLockDelegate(TLockImpl *lock)
	{
		m_lockImpl = lock;
	}

	void lock()
	{
		if (m_lockImpl)
		{
			m_lockImpl->Lock();
		}
	}

	void unlock()
	{
		if (m_lockImpl)
		{
			m_lockImpl->Unlock();
		}
	}
private:
	TLockImpl *m_lockImpl;
};

//------------------------------------------------------------------------------
// Class:   _TEventHandlerContainer
//
// Purpose: A Template class for storing Event Handlers
//
//------------------------------------------------------------------------------

template 
<
    class TEventHandler,
    class TSender,
    class TEventArgs,
	class _TEventHandlerStorage,
    class _TypeEventHandler,
    class TEventHandlerRefCount,
    class TLock
>
class _TEventHandlerContainer :
    public _TEventHandlerStorage,
    protected CLockDelegate<TLock>
{
public:
    // Override from TEventHandlerStorage
    CLockDelegate<TLock>& _GetLock()
    {
        return *this;
    }

    HRESULT _InitializeLock(TLock *lock)
    {
        this->SetLockDelegate(lock);
        return S_OK;
    }

    HRESULT FireEvent(
		foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ TSender *pSender,
        _In_ TEventArgs *pEventArgs)
    {
        _TypeEventHandler *pThis = static_cast<_TypeEventHandler *>(this);
		return foundation::library::_FireEventSupportUtil::_FireWithEventSupport<
			foundation::library::_SimpleInvokeHandler,TEventHandlerRefCount>(
			pEventHandlerContainer,
			pSender,
			pEventArgs,
			*pThis);
    }

	template <class T,class _TEventArgs>
	HRESULT FireEventWithInvoke(
		foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ TSender *pSender,
		_In_ _TEventArgs *pEventArgs)
	{
		T *pThis = static_cast<T *>(this);
		return foundation::library::_FireEventSupportUtil::_FireWithEventSupport<
			foundation::library::_DelegateInvokeHandler,TEventHandlerRefCount>(
			pEventHandlerContainer,
			pSender,
			pEventArgs,
			*pThis);
	}

    template <class _TEventArgs>
    HRESULT FireEventEx(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ TSender *pSender,
        _In_ _TEventArgs *pEventArgs)
    {
        foundation::ComPtr<TEventArgs> spEventArgs;
        HRESULT hr = foundation::QueryInterface(pEventArgs,spEventArgs.GetAddressOf());
        foundation_assert(SUCCEEDED(hr));
        IFHR_ASSERT(hr);
        return FireEvent(pEventHandlerContainer,pSender,spEventArgs);
    }

    template <class TEventArgsAdapter,class _TSourceEventArgs>
    HRESULT FireEventWithAdapter(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ TSender *pSender,
        _In_ _TSourceEventArgs *pEventArgs)
    {
		_TypeEventHandler *pThis = static_cast<_TypeEventHandler *>(this);
		return foundation::library::_FireEventSupportUtil::_FireWithEventSupportAndAdapter<
			foundation::library::_SimpleInvokeHandler,
			TEventHandlerRefCount,
			TEventArgsAdapter,
			TEventArgs>(
			pEventHandlerContainer,
			pSender,
			pEventArgs,
			*pThis);
    }
};
//------------------------------------------------------------------------------
// Class:   TEventSourceImpl
//
// Purpose: A Template class to implement a TEventSource interface
//
//------------------------------------------------------------------------------

template <
    class TEventSource,
    class TEventHandler,
    class TSender,
    class TEventArgs,
    class TLock
>
class TEventSourceImpl :
    public foundation::ctl::ComBase,
    public _TEventHandlerContainer
    <
        TEventHandler,
        TSender,
        TEventArgs,
		foundation::library::_EventHandlerContainerWithLock<TEventHandler, TEventSourceImpl<TEventSource, TEventHandler, TSender, TEventArgs, TLock>, CLockDelegate<TLock>>,
		TEventSourceImpl<TEventSource,TEventHandler,TSender,TEventArgs,TLock>,
        foundation::ctl::_ComBasePtrImpl,
        TLock
    >,
    public TEventSource
{
public:
	typedef CLockDelegate<TLock> _TEventHandlerContainerLockType;

protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == TEventSource::GetIID())
        {
            *ppInterface = static_cast<TEventSource *>(this);
        }
        else
        {
            return foundation::ctl::ComBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // TEventSource
    STDMETHOD(AddHandler)(_In_ TEventHandler* pHandler)
    {
        return this->AddEventHandler(pHandler);
    }

    STDMETHOD(RemoveHandler)(_In_ TEventHandler* pHandler)
    {
        return this->RemoveEventHandler(pHandler);
    }
};

template <
    class TEventSource,
    class TEventHandler,
    class TSender,
    class TEventArgs,
	class TNotifyEventHandlerCallback,
    class TLock>
class TNotifyEventSourceImpl :
    public TEventSourceImpl<TEventSource,TEventHandler,TSender,TEventArgs,TLock>
{
private:
    typedef TEventSourceImpl<TEventSource,TEventHandler,TSender,TEventArgs,TLock> _TBase;
public:
	TNotifyEventSourceImpl():
        m_pNotifyEventHandlerCallback(nullptr)
    {
    }

    template <class T>
    static HRESULT CreateInstance(
        _In_ TNotifyEventHandlerCallback *pNotify,
        _In_ TLock *lock,
        _COM_Outptr_ T **ppNewInstance)
    {
		IFR_ASSERT(foundation::ctl::ComObject<T>::CreateInstance(ppNewInstance));
		IFR_ASSERT((*ppNewInstance)->_InitializeLock(lock));
		
        (*ppNewInstance)->m_pNotifyEventHandlerCallback = pNotify;
        return S_OK;
    }

	HRESULT _DisposeInternal()
	{
		m_pNotifyEventHandlerCallback = nullptr;
		return S_OK;
	}

protected:
	// TEventSource
	STDMETHOD(AddHandler)(_In_ TEventHandler* pHandler) override
	{
		HRESULT hr;

		IFHR(_TBase::AddHandler(pHandler));
		if (m_pNotifyEventHandlerCallback != nullptr)
		{
			m_pNotifyEventHandlerCallback->_OnAddHandler(pHandler);
		}

		return S_OK;
	}

	STDMETHOD(RemoveHandler)(_In_ TEventHandler* pHandler) override
	{
		HRESULT hr;

		IFHR(_TBase::RemoveHandler(pHandler));
		if (m_pNotifyEventHandlerCallback != nullptr)
		{
			m_pNotifyEventHandlerCallback->_OnRemoveHandler(pHandler);
		}

		return S_OK;
	}
private:
	TNotifyEventHandlerCallback *m_pNotifyEventHandlerCallback;
};

