/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEventHandler.h
****/

#pragma once

#include <map>
#include "EventHandler.h"
#include "CriticalSection.h"

template <class TEventHandler,class TLock>
class MoEventHandlerStorage :
	public foundation::library::_EventHandlerContainerWithLock<TEventHandler, MoEventHandlerStorage<TEventHandler, TLock>,TLock>
{
private:
    typedef std::map<INT64,TEventHandler *> EventHandlerMap;
public:
    virtual TLock& _GetLock() = 0;
protected:

    HRESULT AddHandlerInternal(_In_ TEventHandler *pEventHandler,_In_ EventRegistrationToken *token)
    {
        __super::AddEventHandler(pEventHandler);

        token->value = InterlockedIncrement64(&s_eventHandlerCounter);
        m_EventHandlers[token->value] = pEventHandler;

        return S_OK;
    }

    HRESULT RemoveHandlerInternal(_In_ EventRegistrationToken token,__out_opt TEventHandler **ppEventHandler = nullptr)
    {
        EventHandlerMap::iterator iter = m_EventHandlers.find(token.value);
        if(iter != m_EventHandlers.end())
        {
            if(ppEventHandler) 
            {
                *ppEventHandler = iter->second;
                (*ppEventHandler)->AddRef();
            }
            __super::RemoveEventHandler(iter->second);
            m_EventHandlers.erase(token.value);
        }
        else
        {
            // TODO:
            // trigger error?
        }
        return S_OK;
    }
private:
    static INT64 s_eventHandlerCounter;

    EventHandlerMap m_EventHandlers;
};

template <class TEventHandler,class TLock>
INT64 MoEventHandlerStorage<TEventHandler,TLock>::s_eventHandlerCounter = 0;

template <
    class TEventHandler,
    class TSender,
    class TEventArgs,
    class TLock = foundation_lib::CFakeCriticalSection
>
class MoEventHandler:
    public _TEventHandlerContainer
    <
        TEventHandler,
        TSender,
        TEventArgs,
        MoEventHandlerStorage< TEventHandler,CLockDelegate<TLock> >, // TEventHandlerStorage
        MoEventHandler<TEventHandler,TSender,TEventArgs,TLock>,      // _TypeEventHandler
		foundation::_NoRefCountPtrImpl,
        TLock
    >
{
public:
	typedef CLockDelegate<TLock> _TEventHandlerContainerLockType;
	typedef TSender _TSenderType;
protected:
    MoEventHandler()
    {
        this->_InitializeLock(&m_fakeCriticalSection);
    }
private:
    // TODO: my protection needs to be enabled
    foundation_lib::CFakeCriticalSection m_fakeCriticalSection;
};
