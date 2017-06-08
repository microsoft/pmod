/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:fire_event_support.h
****/
#pragma once

#include <foundation/array_wrapper.h>
#include <foundation/com_ptr.h>
#include <foundation/ctl/com_library.h>
#include <foundation/library/interfaces/fire_event.h>
#include "event_handler_container.h"
#include "critical_section_lock.h"

#if _HAS_CPP0X
#include <forward_list>
#endif

#include <vector>
#include <algorithm>
#include <memory>

namespace foundation
{
    namespace library
    {

        //------------------------------------------------------------------------------
        // Class:   _FireEventSupport
        //
        // Purpose: Helper base class to copy & fire event handlers
        //
        //------------------------------------------------------------------------------
        template<
            class _EventHandlerContainer, 
            class _Invoker,
            class _com_ptr_impl = foundation::ctl::_ComBasePtrImpl
        >
        class _FireEventSupport
        {
        public:
            typedef typename _EventHandlerContainer::_TEventHandlerType TEventHandler;
            typedef typename _EventHandlerContainer::_TEventHandlerContainerLockType TLock;

            bool _InitializeFromEventContainer(_EventHandlerContainer &eventHandler)
            {
                this->_event_handler_container_ptr = &eventHandler;

                _CriticalSectionLock<TLock> lock(eventHandler.get_lock());

                const typename _EventHandlerContainer::_VectorContainerType& handlers = eventHandler.GetEventHandlers();

                size_t countHandlers = handlers.size();
                if (countHandlers == 0)
                {
                    return false;
                }
                else if (countHandlers == 1)
                {
                    _handler = handlers[0];
                }
                else
                {
                    // copy the handlers while locked
                    _handlers = handlers;
                }
                // we do have Event handlers to be fired
                return true;
            }

            template <class _TSender, class _TEventArgs>
            HRESULT FireEvent(
                _In_opt_ IFireEventHandlerContainer *pFireEventHandlerContainer,
                _In_ _TSender *pSender,
                _In_ _TEventArgs *pEventArgs)
            {
                HRESULT hr = S_OK;
                if (_handler != nullptr)
                {
                    return _FireEventInvoke(pFireEventHandlerContainer, _handler, pSender, pEventArgs);
                }
                else
                {
                    _VectorEventHandlerPtrContainer<TEventHandler> disconnectHandlers;

                    for (typename _VectorEventHandlerPtrContainer<TEventHandler>::const_iterator iter = _handlers.begin();
                        iter != _handlers.end();
                        ++iter)
                    {
                        TEventHandler* pHandler = (*iter);
                        hr = _Invoker::_Invoke_event_handler(*_event_handler_container_ptr.Get(), pHandler, pSender, pEventArgs);
                        if (IsHandlerDisconnectedError(hr))
                        {
                            disconnectHandlers.push_back(pHandler);
                        }
                        else if (FAILED(hr))
                        {
                            if (pFireEventHandlerContainer != nullptr)
                            {
                                hr = pFireEventHandlerContainer->OnFireEventError(hr, pHandler, pEventArgs);
                            }
                            _IFHR_(hr);
                        }
                    }
                    // Disconnect Handlers that are not connected
                    if (disconnectHandlers.size())
                    {
                        _event_handler_container_ptr->RemoveHandlers(disconnectHandlers);
                    }
                    return S_OK;
                }
            }

            void swap(_FireEventSupport<_EventHandlerContainer,_Invoker,_com_ptr_impl>& from)
            {
                this->_event_handler_container_ptr = from._event_handler_container_ptr;
                from._event_handler_container_ptr.Release();
                // copy handlers
                this->_handler = from._handler;
                this->_handlers = from._handlers;

                // Clean 'from' source
                from._CleanupEventHandlers();
            }

        protected:
            template <class _TSender, class _TEventArgs>
            inline HRESULT _FireEventInvoke(
                _In_opt_ IFireEventHandlerContainer *pFireEventHandlerContainer,
                _In_ TEventHandler *pEventHandler,
                _In_ _TSender *pSender,
                _In_ _TEventArgs *pEventArgs)
            {
                foundation_assert(pEventHandler);

                HRESULT hr = _Invoker::_Invoke_event_handler(*_event_handler_container_ptr.Get(), pEventHandler, pSender, pEventArgs);
                if (IsHandlerDisconnectedError(hr))
                {
                    _event_handler_container_ptr->RemoveEventHandler(pEventHandler);
                }
                else if (FAILED(hr))
                {
                    if (pFireEventHandlerContainer != nullptr)
                    {
                        hr = pFireEventHandlerContainer->OnFireEventError(hr, pEventHandler, pEventArgs);
                    }
                    _IFHR_(hr);
                }
                return S_OK;
            }

            HRESULT _CopyTo(_Out_ UINT32 *pSize, _Out_ foundation::IUnknown ***ppHandlers)
            {
                foundation_assert(pSize != nullptr);
                foundation_assert(ppHandlers != nullptr);
                if (_handler != nullptr)
                {
                    foundation::IUnknown *pUnk = _handler.Get();
                    foundation::RefCountCopyTo(1,&pUnk, pSize, ppHandlers);
                }
                else
                {
                    foundation::RefCountCopyTo(
                        (UINT32)_handlers.size(),
                        (foundation::IUnknown **)foundation::GetVectorBufferPtr(_handlers),
                        pSize,
                        ppHandlers);
                }
                return S_OK;
            }

            void _CleanupEventHandlers()
            {
                _handler.Release();
                _handlers.clear();
            }
        private:

            // Return true if the HRESULT is a 'Disconnect' handler error
            static bool IsHandlerDisconnectedError(HRESULT hr)
            {
                return hr == foundation::Errors::RPC_E_SERVER_NOT_AVAILABLE;
            }

        private:
            foundation::ComPtr<_EventHandlerContainer, _com_ptr_impl> _event_handler_container_ptr;
            foundation::ComPtr<TEventHandler>  _handler;
            _VectorEventHandlerPtrContainer<TEventHandler>      _handlers;
        };

        //------------------------------------------------------------------------------
        // Class:   _FireEventSupportWithAdapter
        //
        // Purpose: specialized event support to fire events using an event args adapter class
        //
        //------------------------------------------------------------------------------
        template<
            class _EventHandlerContainer,
            class _Invoker, 
            class _TEventArgs, 
            class _TEventArgsAdapter,
            class _com_ptr_impl = foundation::ctl::_ComBasePtrImpl
        >
        class _FireEventSupportWithAdapter :
            public _FireEventSupport<_EventHandlerContainer, _Invoker, _com_ptr_impl>
        {
        public:
            template <class _TSender, class _TSourceEventArgs>
            HRESULT FireEvent(
                _In_opt_ IFireEventHandlerContainer *pFireEventHandlerContainer,
                _In_ _TSender *pSender,
                _In_ _TSourceEventArgs *pSourceEventArgs)
            {
                foundation::ComPtr<_TEventArgs> spEventArgs;
                IFR_ASSERT(_TEventArgsAdapter::ToEventArgs(pSender, pSourceEventArgs, spEventArgs.GetAddressOf()));
                foundation_assert(spEventArgs != nullptr);
                return _FireEventSupport<_EventHandlerContainer, _Invoker, _com_ptr_impl>::FireEvent(
                    pFireEventHandlerContainer, 
                    pSender, 
                    (_TEventArgs *)spEventArgs);
            }
        };




        //------------------------------------------------------------------------------
        // Class:   _FireEventSupportImplBase
        //
        // Purpose: Base class for IFireEventSupport implementation
        //
        //------------------------------------------------------------------------------
        template <class TSender, class TEventArgs>
        class _FireEventSupportImplBase :
            public foundation::ctl::ComBase,
            public IFireEventSupport
        {
        public:
            template <class T>
            static HRESULT CreateInstance(
                const IID& iid,
                _In_ TSender *pSender,
                _In_ TEventArgs *pEventArgs,
                _Outptr_ T **ppNewInstance)
            {
                IFR_ASSERT(foundation::ctl::ComObject<T>::CreateInstance(ppNewInstance));
                IFR_ASSERT((*ppNewInstance)->_Initialize(iid, pSender, pEventArgs));
                return S_OK;
            }
        private:
            HRESULT _Initialize(const IID& iid, TSender *pSender, TEventArgs *pEventArgs)
            {
                foundation_assert(pSender != nullptr);
                foundation_assert(pEventArgs != nullptr);
                _type = iid;
                _sender_ptr = pSender;
                _eventArgs_ptr = pEventArgs;
                return S_OK;
            }
        protected:
            void FinalRelease() override
            {
                // explicit release
                _sender_ptr.Release();
                _eventArgs_ptr.Release();

                foundation::ctl::ComBase::FinalRelease();
            }

            // Override ComBase
            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == IFireEventSupport::GetIID())
                {
                    *ppInterface = static_cast<IFireEventSupport *>(this);
                }
                else
                {
                    return foundation::ctl::ComBase::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            // Interface IFireEventSupport
            STDMETHOD(GetType)(_Out_ IID *pType)
            {
                IFCPTR_ASSERT(pType);
                *pType = _type;
                return S_OK;
            }
        protected:
            IID                              _type;
            foundation::ComPtr<TSender>     _sender_ptr;
            foundation::ComPtr<TEventArgs>  _eventArgs_ptr;
        };

        //------------------------------------------------------------------------------
        // Class:   _FireEventSupportImpl
        //
        // Purpose: Implements both the IFireEventSupport & the stack based firing
        //
        //------------------------------------------------------------------------------
        template <
            class _TSender, 
            class _TEventArgs, 
            class TBASE
        >
        class _FireEventSupportImpl :
            public _FireEventSupportImplBase<_TSender, _TEventArgs>,
            public TBASE
        {
        public:
            typedef TBASE _Fire_event_support_type;
        protected:
            typedef _FireEventSupportImplBase<_TSender, _TEventArgs> _Fire_event_support_impl_base_type;

            void FinalRelease() override
            {
                this->_CleanupEventHandlers();
                _Fire_event_support_impl_base_type::FinalRelease();
            }

            // interface IFireEventSupport
            STDMETHOD(FireEventHandlers)(_In_ IFireEventHandlerContainer *pContainer)
            {
                return this->FireEvent(
                    pContainer, 
                    this->_sender_ptr.Get(), 
                    this->_eventArgs_ptr.Get());
            }

            STDMETHOD(GetEventHandlers)(_Out_ UINT32 *pSize, foundation::IUnknown ***ppHandlers)
            {
                return TBASE::_CopyTo(pSize, ppHandlers);
            }

            STDMETHOD(FireEventHandler)(
                _In_ IFireEventHandlerContainer *pFireEventHandlerContainer,
                _In_ foundation::IUnknown *pEventHandler)
            {
                return TBASE::_FireEventInvoke(
                    pFireEventHandlerContainer,
                    reinterpret_cast<typename TBASE::TEventHandler *>(pEventHandler),
                    this->_sender_ptr.Get(),
                    this->_eventArgs_ptr.Get());
            }
        };
        //------------------------------------------------------------------------------
        // Class:   _FireEventSupportWithAdapterImpl
        //
        // Purpose: Implements both the IFireEventSupport & the stack based firing
        //
        //------------------------------------------------------------------------------
        template <
            class _TSender, 
            class _TSourceEventArgs, 
            class _TEventArgs, 
            class TBASE,
            class _TEventArgsAdapter
        >
        class _FireEventSupportWithAdapterImpl :
            public _FireEventSupportImplBase<_TSender, _TSourceEventArgs>,
            public TBASE
        {
        public:
            typedef TBASE _Fire_event_support_type;
        protected:
            // interface IFireEventSupport
            STDMETHOD(FireEventHandlers)(_In_ IFireEventHandlerContainer *pFireEventHandlerContainer)
            {
                foundation::ComPtr<_TEventArgs> eventArgsPtr;
                IFR_ASSERT(_TEventArgsAdapter::ToEventArgs(
                    this->_sender_ptr, 
                    this->_eventArgs_ptr, 
                    eventArgsPtr.GetAddressOf()));
                foundation_assert(eventArgsPtr != nullptr);
                return this->FireEvent(
                    pFireEventHandlerContainer, 
                    this->_sender_ptr.Get(),
                    (_TEventArgs *)eventArgsPtr.Get());
            }

            STDMETHOD(GetEventHandlers)(_Out_ UINT32 *pSize, foundation::IUnknown ***ppHandlers)
            {
                return TBASE::_CopyTo(pSize, ppHandlers);
            }

            STDMETHOD(FireEventHandler)(
                _In_ IFireEventHandlerContainer *pFireEventHandlerContainer,
                _In_ foundation::IUnknown *pEventHandler)
            {
                foundation::ComPtr<_TEventArgs> eventArgsPtr;
                IFR_ASSERT(_TEventArgsAdapter::ToEventArgs(
                    this->_sender_ptr, 
                    this->_eventArgs_ptr, 
                    eventArgsPtr.GetAddressOf()));
                foundation_assert(eventArgsPtr != nullptr);
                return TBASE::_FireEventInvoke(
                    pFireEventHandlerContainer,
                    reinterpret_cast<typename TBASE::TEventHandler *>(pEventHandler),
                    this->_sender_ptr.Get(),
                    eventArgsPtr.Get());
            }

        };

        struct _SimpleInvokeHandler
        {
            template <
                class _EventHandlerContainer,
                class _TEventHandler,
                class _TSender,
                class _TEventArgs>
            static inline HRESULT _Invoke_event_handler(
                _EventHandlerContainer &eventHandlerContainer,
                _TEventHandler *pEventHandler,
                _TSender *pSender,
                _TEventArgs *pEventArgs)
                {
                    UNREFERENCED_PARAMETER(eventHandlerContainer);
                    return pEventHandler->Invoke(pSender, pEventArgs);
                }
        };

        struct _DelegateInvokeHandler
        {
            template <
                class _EventHandlerContainer,
                class _TEventHandler,
                class _TSender,
                class _TEventArgs>
            static inline HRESULT _Invoke_event_handler(
                _EventHandlerContainer &eventHandlerContainer,
                _TEventHandler *pEventHandler,
                _TSender *pSender,
                _TEventArgs *pEventArgs)
                {
                    return eventHandlerContainer._Invoke_event_handler(
                        pEventHandler, 
                        pSender, 
                        pEventArgs);
                }
        };

        typedef foundation::ArrayRefCountWrapper<IFireEventSupport> _FireEventSupportArrayType;
        //------------------------------------------------------------------------------
        // Class:   _FireEventSupportDispatchableImpl
        //
        // Purpose: Implements both the IFireEventSupportDispatchable/IDelegateDispatchable
        //
        //------------------------------------------------------------------------------
        class _FireEventSupportDispatchableImpl :
            public ctl::ComBase,
            public IFireEventSupportDispatchable,
            public IDelegateDispatchable
        {
        public:
            HRESULT _Initialize(
                _In_ IFireEventHandlerContainer *pFireEventHandlerContainer
                )
            {
                foundation_assert(pFireEventHandlerContainer != nullptr);
                _pFireEventHandlerContainer = pFireEventHandlerContainer;
                ADDREF_INTERFACE(pFireEventHandlerContainer);

                return pFireEventHandlerContainer->Detach(
                    _eventSupportArray.GetSizeAddressOf(),
                    _eventSupportArray.GetBufferAddressOf());
            }

            static HRESULT _Fire_event_handlers(
                const _FireEventSupportArrayType& array,
                _In_ IFireEventHandlerContainer *pFireEventHandlerContainer)
            {
                for (UINT32 index = 0; index < array.GetSize();
                ++index)
                {
                    _IFR_(array[index]->FireEventHandlers(pFireEventHandlerContainer));
                }
                return S_OK;
            }

        protected:
            _FireEventSupportDispatchableImpl() :
                _pFireEventHandlerContainer(nullptr)
            {}

            void FinalRelease() override
            {
                RELEASE_INTERFACE(_pFireEventHandlerContainer);
                _eventSupportArray.Clear();
                ctl::ComBase::FinalRelease();
            }

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == IFireEventSupportDispatchable::GetIID())
                {
                    *ppInterface = static_cast<IFireEventSupportDispatchable *>(this);
                }
                else if (iid == IDelegateDispatchable::GetIID())
                {
                    *ppInterface = static_cast<IDelegateDispatchable *>(this);
                }
                else
                {
                    return ctl::ComBase::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }
            // Interface IDelegateDispatchable
            STDMETHOD(Invoke)()
            {
                return _Fire_event_handlers(
                    this->_eventSupportArray,
                    _pFireEventHandlerContainer
                    );
            }
            // Interface IFireEventSupportDispatchable
            STDMETHOD(GetFireEvents)(_Out_ UINT32 *pSize, IFireEventSupport ***ppFireEvents)
            {
                IFCPTR_ASSERT(pSize);
                IFCPTR_ASSERT(ppFireEvents);

                _eventSupportArray.CopyTo(pSize, ppFireEvents);
                return S_OK;
            }

            STDMETHOD(GetContainer)(IFireEventHandlerContainer **ppContainer)
            {
                IFCPTR_ASSERT(ppContainer);
                *ppContainer = _pFireEventHandlerContainer;
                (*ppContainer)->AddRef();
                return S_OK;
            }

        private:
            IFireEventHandlerContainer *_pFireEventHandlerContainer;
            _FireEventSupportArrayType _eventSupportArray;
        };


        //------------------------------------------------------------------------------
        // Class:   _FireEventSupportUtil
        //
        // Purpose: static helpers for firing events
        //
        //------------------------------------------------------------------------------
        class _FireEventSupportUtil
        {
        public:
            template
            <
                class TLocker,
                class T,
                class _TEventArgs
            >
            static HRESULT _FireEventWithCallback(
                        TLocker& csl,
                        T* pThis,
                        _In_ HRESULT(T::*_FireEventDelegate)(IFireEventHandlerContainer *, _TEventArgs *),
                        _In_ _TEventArgs *pEventArgs)
            {
                IFireEventHandlerContainer	*_pFireEventHandlerContainer =
                    pThis->_Get_fire_event_handler_container();

                _FireEventHandlerContainerContext containerContext(_pFireEventHandlerContainer);
                HRESULT hr;

                _IFHR_(_pFireEventHandlerContainer->BeginSupport());

                // Use Callback to Fire first 
                hr = (pThis->*_FireEventDelegate)(_pFireEventHandlerContainer, pEventArgs);
                IFHR_ASSERT(hr);

                // Overridable FireEvent support
                // Note: we use logger macros without 'foundation_assert'
                _IFHR_(pThis->_OnCustomFireEvent(_pFireEventHandlerContainer, pEventArgs));

                bool bValue;
                // If Event Support is not enabled the previous call already delivered
                // and dispatched
                _pFireEventHandlerContainer->IsEventSupportEnabled(&bValue);
                if (!bValue)
                {
                    return S_OK;
                }
                // If the Event Support Array is empty no handlers are needed to be fired
                _pFireEventHandlerContainer->IsEmpty(&bValue);
                if (bValue)
                {
                    return S_OK;
                }
                // if EndSupport returns S_FALSE we are continuing our container
                // support until we reach level '0'
                if (containerContext.EndSupport() == S_FALSE)
                {
                    return S_OK;
                }

                // if we arrive here we will commit the Event Handler Container
                // either by:
                // 1) an Event Dispatcher will be used 
                // 2) sync firing all the 'IFireEventSupport' objects in the container

                foundation::ComPtr<IDispatcher> event_dispatcher_ptr;
                _pFireEventHandlerContainer->GetFireEventDispatcher(event_dispatcher_ptr.GetAddressOf());

                bool bUseEventDispatcher = false;
                if (event_dispatcher_ptr != nullptr)
                {
                    bool hasThreadAccess;
                    event_dispatcher_ptr->HasThreadAccess(&hasThreadAccess);
                    bUseEventDispatcher = !hasThreadAccess;
                }

                if (bUseEventDispatcher)
                {
                    // create an IDispatchable entity with all the IFireEventSupport 
                    foundation::ComPtr<_FireEventSupportDispatchableImpl, foundation::ctl::_ComBasePtrImpl> dispatchablePtr;
                    IFR_ASSERT(foundation::ctl::CreateInstanceWithInitialize(dispatchablePtr.GetAddressOf(), _pFireEventHandlerContainer));

                    if (pThis->_IsFreeThreaded())
                    {
                        // before dispatching we should unlock to allow any QI into host adapters
                        csl.unlock();
                    }
                    // run event dispatcher async
                    foundation::ComPtr<foundation::IAsyncOperation> spAsyncAction;
                    return event_dispatcher_ptr->RunAsync(dispatchablePtr, spAsyncAction.GetAddressOf());
                }
                else
                {
                    if (pThis->_IsFreeThreaded())
                    {
                        // swap the array and Fire
                        _FireEventSupportArrayType eventSupportArray;
                        _pFireEventHandlerContainer->Detach(
                            eventSupportArray.GetSizeAddressOf(),
                            eventSupportArray.GetBufferAddressOf());

                        // Note: we will unlock to avoid any possible contention
                        csl.unlock();

                        _IFR_(_FireEventSupportDispatchableImpl::_Fire_event_handlers(
                            eventSupportArray,
                            _pFireEventHandlerContainer));
                    }
                    else
                    {
                        // sync Fire
                        _IFR_(_pFireEventHandlerContainer->FireAllEventHandlers());
                    }
                }
                return S_OK;
            }

            template <
                class _Invoker,
                class _com_ptr_impl,
                class _EventHandlerContainer,
                class _TSender,
                class _TEventArgs
            >
            static HRESULT _FireWithEventSupport(
                _In_opt_ IFireEventHandlerContainer *pEventHandlerContainer,			
                _In_ _TSender *pSender,
                _In_ _TEventArgs *pEventArgs,
                _EventHandlerContainer &eventHandlerContainer
                )
            {
                typedef _FireEventSupport<_EventHandlerContainer, _Invoker, _com_ptr_impl> _Fire_event_support_type;
                typedef _FireEventSupportImpl< _TSender, _TEventArgs, _Fire_event_support_type > _Fire_event_support_impl_type;

                return _FireWithEventSupportImpl
                    <
                    _Fire_event_support_type,
                    _Fire_event_support_impl_type>
                    (pEventHandlerContainer,
                        pSender,
                        pEventArgs,
                        eventHandlerContainer);
            }

            template <
                class _Invoker,
                class _com_ptr_impl,
                class TEventArgsAdapter,
                class _TEventArgs,
                class _EventHandlerContainer,
                class _TSender,
                class _TSourceEventArgs
            >
            static HRESULT _FireWithEventSupportAndAdapter(
                _In_opt_ IFireEventHandlerContainer *pEventHandlerContainer,
                _In_ _TSender *pSender,
                _In_ _TSourceEventArgs *pSourceEventArgs,
                _EventHandlerContainer &eventHandlerContainer
                )
            {

                typedef _FireEventSupport<_EventHandlerContainer, _Invoker, _com_ptr_impl> _Fire_event_support_type;
                typedef _FireEventSupportWithAdapterImpl< _TSender, _TSourceEventArgs, _TEventArgs, _Fire_event_support_type, TEventArgsAdapter > _Fire_event_support_impl_type;
                typedef _FireEventSupportWithAdapter<_EventHandlerContainer, _Invoker, _TEventArgs, TEventArgsAdapter, _com_ptr_impl> _Fire_event_support_with_adapter_type;

                return _FireWithEventSupportImpl<
                    _Fire_event_support_with_adapter_type,
                    _Fire_event_support_impl_type
                >
                    (pEventHandlerContainer,
                        pSender,
                        pSourceEventArgs,
                        eventHandlerContainer);
            }
        private:
            class _FireEventHandlerContainerContext
            {
            public:
                _FireEventHandlerContainerContext(IFireEventHandlerContainer *pFireEventHandlerContainer) :
                    _pFireEventHandlerContainer(pFireEventHandlerContainer)
                {
                }

                HRESULT EndSupport()
                {
                    foundation_assert(_pFireEventHandlerContainer != nullptr);
                    HRESULT hr = _pFireEventHandlerContainer->EndSupport(false);
                    _pFireEventHandlerContainer = nullptr;
                    return hr;
                }

                ~_FireEventHandlerContainerContext()
                {
                    // if this code execute an abnormal failure was raised during the context lifetime
                    if (_pFireEventHandlerContainer != nullptr)
                    {
                        _pFireEventHandlerContainer->EndSupport(true);
                    }
                }
            private:
                IFireEventHandlerContainer *_pFireEventHandlerContainer;
            };

                template
                <
                    class TFireEventSupport,
                    class TFireEventSupportImpl,
                    class TSender,
                    class TEventArgs,
                    class _EventHandlerContainer
                >
                static HRESULT _FireWithEventSupportImpl(
                        _In_opt_ IFireEventHandlerContainer *pEventHandlerContainer,
                        _In_ TSender *pSender,
                        _In_ TEventArgs *pEventArgs,
                        _EventHandlerContainer &eventHandlerContainer)
                {
                    foundation_assert(pSender != nullptr);
                    foundation_assert(pEventArgs != nullptr);

                    HRESULT hr = S_OK;

                    TFireEventSupport fireEventSupportInternal;
                    if (fireEventSupportInternal._InitializeFromEventContainer(eventHandlerContainer))
                    {
                        bool isEventSupportEnabled = false;
                        if (pEventHandlerContainer != nullptr)
                        {
                            IGNOREHR(pEventHandlerContainer->IsEventSupportEnabled(&isEventSupportEnabled));
                        }

                        if (isEventSupportEnabled)
                        {
                            TFireEventSupportImpl *pFireEventSupport = nullptr;
                            IFR_ASSERT(TFireEventSupportImpl::CreateInstance(
                                _EventHandlerContainer::_GetEventHandlerType(),
                                pSender, 
                                pEventArgs, 
                                &pFireEventSupport));

                            pFireEventSupport->swap(fireEventSupportInternal);
                            // Let the Event Handler Container perform the 'Fire' when appropriate
                            hr = pEventHandlerContainer->AddFireEventSupport(pFireEventSupport);
                            // Release the FireEvent Support Instance
                            foundation::ctl::ReleaseInterface(pFireEventSupport);
                            IFHR_ASSERT(hr);
                        }
                        else
                        {
                            // Fire Event without Event Handler Container support
                            IFR_ASSERT(fireEventSupportInternal.FireEvent(pEventHandlerContainer, pSender, pEventArgs));
                        }
                    }
                    return S_OK;
                }
        };

        typedef foundation::ComPtr<IFireEventSupport> _Fire_event_support_ptr_type;

#if _HAS_CPP0X
        typedef std::forward_list< _Fire_event_support_ptr_type > _Fire_event_support_container_type;
#else
        typedef std::vector< _Fire_event_support_ptr_type > _Fire_event_support_container_type;
#endif

        //------------------------------------------------------------------------------
        // Class:   _FireEventHandlerContainerImpl
        //
        // Purpose: Implements IFireEventHandlerContainer
        //
        //------------------------------------------------------------------------------
        template <class T>
        class _FireEventHandlerContainerImpl :
            public IFireEventHandlerContainer
        {
        protected:
            _FireEventHandlerContainerImpl() :
                _supportLevel(0)
            {
            }
            // Interface IFireEventHandlerContainer
            STDMETHOD(IsEventSupportEnabled)(bool *value)
            {
                *value = this->_Get_host()->_IsEventSupportEnabled();
                return S_OK;
            }

            STDMETHOD(AddFireEventSupport)(_In_ IFireEventSupport *pFireEventSupport) override
            {
#if _HAS_CPP0X
                this->_eventSupportContainer.push_front(pFireEventSupport);
#else
                this->_eventSupportContainer.push_back(pFireEventSupport);
#endif
                return S_OK;
            }

            STDMETHOD(FireAllEventHandlers)()
            {
                _Fire_event_support_container_type fireEventSupportArray;
                fireEventSupportArray.swap(this->_eventSupportContainer);
                for (_Fire_event_support_container_type::const_iterator iter = fireEventSupportArray.begin();
                iter != fireEventSupportArray.end();
                    ++iter)
                {
                    // Note: will bail out when the first 'Fire' failed
                    _IFR_((*iter)->FireEventHandlers(this));
                }
                return S_OK;

            }

            STDMETHOD(IsEmpty)(bool *value)
            {
                *value = this->_eventSupportContainer.empty();
                return S_OK;
            }
            STDMETHOD(Detach)(UINT32 *size, IFireEventSupport ***ppFireEventSupport)
            {
                _Fire_event_support_container_type fireEventSupportArray;
                fireEventSupportArray.swap(this->_eventSupportContainer);
                std::vector<_Fire_event_support_ptr_type> eventSupportVector;
                eventSupportVector.resize(std::distance(fireEventSupportArray.begin(), fireEventSupportArray.end()));
                std::copy(fireEventSupportArray.begin(), fireEventSupportArray.end(), eventSupportVector.begin());
                foundation::RefCountCopyTo(eventSupportVector, size, ppFireEventSupport);
                return S_OK;
            }

            STDMETHOD(BeginSupport)()
            {
#ifdef DEBUG
                if (_supportLevel == 0 && !this->_eventSupportContainer.empty())
                {
                    foundation_assert(false);
                }
#endif
                ++_supportLevel;
                return S_OK;
            }
            STDMETHOD(EndSupport)(bool clearEventSupport)
            {
                foundation_assert(_supportLevel > 0);
                --_supportLevel;
                if (clearEventSupport)
                {
                    this->_eventSupportContainer.clear();
                }
                return _supportLevel == 0 ? S_OK : S_FALSE;
            }

            STDMETHOD(OnFireEventError)(
                HRESULT hr,
                IUnknown *pEventHandler,
                IUnknown *pEventArgs)
            {
                UNREFERENCED_PARAMETER(pEventHandler);
                UNREFERENCED_PARAMETER(pEventArgs);

                /*
                foundation::logger_util::LogTrace(
                    foundation::LoggingLevel::Error,
                    foundation::Error_Category::Failure,
                    hr,
                    __WFILE__,
                    __LINE__,
                    U("OnFireEventError"));
                */
                return this->_Get_host()->_IsContinueOnFireEventError() ? S_OK : hr;
            }

            STDMETHOD(GetFireEventDispatcher)(_COM_Outptr_result_maybenull_ IDispatcher **ppEventDispatcher)
            {
                *ppEventDispatcher = this->_Get_host()->_GetEventDispatcher();
                if (*ppEventDispatcher)
                {
                    (*ppEventDispatcher)->AddRef();
                }
                return S_OK;
            }
        protected:
            T *_Get_host()
            {
                return static_cast<T *>(this);
            }
        private:
            _Fire_event_support_container_type		_eventSupportContainer;
            UINT8									_supportLevel;
        };

        template <class T>
        class _FireEventHandlerContainerWithCacheImpl:
            public _FireEventHandlerContainerImpl<T>
        {
        public:
            IFireEventHandlerContainer *_Get_fire_event_handler_container()
            {
                T* pThis = static_cast<T *>(this);

                if (_pFireEventHandlerContainer == nullptr)
                {
                    if (pThis->IsComposed())
                    {
                        // this QI require the Aggregated object to be ready
                        HRESULT hr = foundation::QueryWeakReference(
                            pThis->CastToInspectable(),
                            &_pFireEventHandlerContainer);
                        foundation_assert(SUCCEEDED(hr));
                    }
                    else
                    {
                        _pFireEventHandlerContainer = static_cast<IFireEventHandlerContainer *>(pThis);
                    }
                }
                return _pFireEventHandlerContainer;
            }

        protected:
            _FireEventHandlerContainerWithCacheImpl():
                _pFireEventHandlerContainer(nullptr)
            {}

        private:
            IFireEventHandlerContainer				*_pFireEventHandlerContainer;
        };
    }

}
