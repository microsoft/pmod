/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventSourceBase.h
****/

#pragma once

#include "EventHandler.h"
#include <foundation/library/interfaces/disposable.h>

template 
    <
    class TEventSource,
    class TEventHandler,
    class TSender,
    class TEventArgs,
	class TNotifyEventHandlerCallback,
    class TLock
    >
class _EventSourceBase
{
protected:
	typedef TNotifyEventSourceImpl<TEventSource, TEventHandler, TSender, TEventArgs, TNotifyEventHandlerCallback, TLock>  _TNotifyEventSourceImpl_Type;

    _EventSourceBase() :
		m_pEventSourceImpl(nullptr)
    {
    }

    ~_EventSourceBase()
    {
        if(m_pEventSourceImpl != nullptr)
        {
			m_pEventSourceImpl->_DisposeInternal();
        }
        foundation::ctl::ReleaseInterface(m_pEventSourceImpl);
    }

	HRESULT _CreateEventSource(
		TNotifyEventHandlerCallback *pNotifyEventHandlerCallback, 
		TLock *lock)
	{
	    return _CreateEventSourceWithType<_TNotifyEventSourceImpl_Type>(pNotifyEventHandlerCallback, lock);
	}

	template <class T>
	HRESULT _CreateEventSourceWithType(
		_In_ TNotifyEventHandlerCallback *pNotifyEventHandlerCallback,
		TLock *lock)
	{
		foundation_assert(m_pEventSourceImpl == nullptr);
		foundation_assert(pNotifyEventHandlerCallback != nullptr);
		T *pT = nullptr;
		IFR_ASSERT(T::CreateInstance(pNotifyEventHandlerCallback, lock,&pT));
		this->m_pEventSourceImpl = pT;
		return S_OK;
	}

    bool IsEventHandlerSource() { return m_pEventSourceImpl != nullptr; }

	_TNotifyEventSourceImpl_Type *GetEventSourceImpl() const
    {
        return m_pEventSourceImpl;
    }

    size_t GetCountHandlerStorage() const
    {
        return m_pEventSourceImpl != nullptr ? m_pEventSourceImpl->GetEventHandlers().size():0;
    }

protected:
	_TNotifyEventSourceImpl_Type *m_pEventSourceImpl;
};
