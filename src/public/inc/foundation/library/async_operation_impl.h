/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_impl.h
****/
#pragma once

#include "base_object_impl.h"
#include "event_args_base.h"
#include "event_handler_source.h"
#include "interfaces/dispatcher.h"
#include <foundation/type_info_statics_util.h>
#include <foundation/interfaces/async_operation.h>
#include <foundation/async_operation_connection.h>
#include <foundation/ctl/com_weak_reference_source.h>
#include <pal/library/signal.h>

namespace foundation {
   
    namespace library {
   
        typedef _EventArgsBase<ICompletedEventArgs> CompletedEventArgs;

        template <class T>
        class _AsyncOperationBase
        {
        public:
            HRESULT _SetCompleted(_In_ HRESULT hResult, _In_ foundation::IInspectable *result)
            {
                foundation::InspectablePtr result_ptr(result);

                ComPtr<IAsyncOperationTypeInfo> asyncOperationTypeInfoPtr;
                IFR_ASSERT(GetObjectTypeInfo(this->get_host(), asyncOperationTypeInfoPtr.GetAddressOf()));

                foundation::ComPtr<foundation::IPropertyTypeInfo> spResultTypeInfo;
                IFR_ASSERT(asyncOperationTypeInfoPtr->GetResultTypeInfo(spResultTypeInfo.GetAddressOf()));
                _IFR_(VerifyPropertyTypeInfo(
                    spResultTypeInfo,
                    result_ptr.GetAddressOfPtr()));

                NoRefCountFoundationCriticalSection cs(this->get_host()->_GetCriticalSection());

                // provide 'scope' lifetime in case the Fire Completed event remove
                // the last reference to my self

                ComPtr<foundation::IInspectable> refCountPtr(this->get_host()->CastToInspectable());

                if (_resultStatus != ResultStatus::Started)
                {
                    return E_UNEXPECTED;
                }
                _hError = hResult;
                _result = result_ptr;
                _resultStatus = FAILED(hResult) ? ResultStatus::Error : ResultStatus::Completed;

                return this->get_host()->_FireCompleted(cs);
            }
            template <class TResult>
            HRESULT _SetCompleted_t(_In_ HRESULT hResult, TResult value)
            {
                return _SetCompleted(hResult,CreateValue(value));
            }
        protected:
            _AsyncOperationBase() :
                _hError(S_OK),
                _resultStatus(ResultStatus::Started)
            {
            }

            HRESULT _Wait(
                UINT32 timeout, 
                ResultStatus* pStatus)
            {
                NoRefCountFoundationCriticalSection cs(this->get_host()->_GetCriticalSection());
                if (_resultStatus == ResultStatus::Started)
                {
                    pal::library::Signal evt;
                    // Wait for the result
                    AsyncOperationCompletedConnection evt_connection;
                    evt_connection.SetSink([&evt](foundation::IUnknown *, foundation::ICompletedEventArgs *) -> HRESULT {
                        evt.set();
                        return S_OK;
                    });
                    evt_connection.Attach(this->get_host()->_GetAsyncOperation());
                    // unlock first to avoid a deadlock
                    cs.unlock();
                    // now Wait
                    if (timeout == _InfiniteTimeout)
                    {
                        evt.Wait();
                    }
                    else
                    {
                        if (!evt.wait_timed(timeout))
                        {
                            return foundation::E_ASYNC_OPERATION_TIMEOUT;
                        }
                    }
                }
                if (pStatus != nullptr)
                {
                    *pStatus = _resultStatus;
                }
                return S_OK;
            }

            HRESULT _Cancel()
            {
                NoRefCountFoundationCriticalSection cs(this->get_host()->_GetCriticalSection());

                if (this->_resultStatus != ResultStatus::Started)
                {
                    // operation was already completed , no action further
                    return S_OK;
                }
                this->_resultStatus = ResultStatus::Canceled;

                _IFR_(this->get_host()->_OnCancel());

                return this->get_host()->_FireCompleted(cs);
            }

            HRESULT _GetResult(UINT32 timeout, foundation::IInspectable **ppResult)
            {
                IFCPTR_ASSERT(ppResult);

                ResultStatus resultStatus;
                _IFR_(_Wait(timeout, &resultStatus));
                return resultStatus == ResultStatus::Completed ?
                    _result.CopyTo(ppResult) :
                    resultStatus == ResultStatus::Error ? _hError : foundation::E_ASYNC_OPERATION_CANCELED;
            }

            HRESULT _GetError(HRESULT* hResult)
            {
                NoRefCountFoundationCriticalSection cs(this->get_host()->_GetCriticalSection());

                *hResult = _hError;
                return S_OK;
            }

            HRESULT _GetStatus(ResultStatus *status)
            {
                NoRefCountFoundationCriticalSection cs(this->get_host()->_GetCriticalSection());

                *status = _resultStatus;
                return S_OK;
            }

            void Cleanup()
            {
                _result.Release();
            }
            inline T * get_host() { return static_cast<T *>(this); }
        protected:
            ResultStatus       _resultStatus;
        private:
            InspectablePtr     _result;
            HRESULT             _hError;
        };

        template <
            class TInterface,
            class TComBase = foundation::ctl::ComWeakReferenceSourceInspectableBase,
            class TFireEventHandlerContainer = foundation::library::_DefaultFireEventHandlerContainer
        >
        class _AsyncOperationImplBase :
            public _ObjectImplBase
            <
                TInterface,
                IAsyncOperation,
                TComBase,
                TFireEventHandlerContainer
            >,
            public _AsyncOperationBase<_AsyncOperationImplBase<TInterface, TComBase, TFireEventHandlerContainer>>
        {
        public:
            // Interface IAsyncOperationBase
            STDMETHOD(GetError)(_Out_ HRESULT* hResult) override
            {
                return this->_GetError(hResult);
            }
            STDMETHOD(GetStatus)(_Out_ ResultStatus* status) override
            {
                return this->_GetStatus(status);
            }
            STDMETHOD(GetIsCancelable)(_Out_ bool* pVal) override
            {
                *pVal = false;
                return S_OK;
            }

            STDMETHOD(Cancel)() override
            {
                return this->_Cancel();
            }
            STDMETHOD(Wait)(UINT32 timeout, _Out_ ResultStatus* status) override
            {
                return this->_Wait(timeout, status);
            }

            // Event methods.
            STDMETHOD(GetCompletedEventSource)(_COM_Outptr_ ICompletedEventSource** ppEventSource) override
            {
                foundation_assert(_completedEventSourcePtr != nullptr);
                *ppEventSource = _completedEventSourcePtr;
                (*ppEventSource)->AddRef();
                return S_OK;
            }

            STDMETHOD(GetProgressEventSource)(_COM_Outptr_ IProgressEventSource** ppEventSource) override
            {
                return S_OK;
            }

            // Interface IAsyncOperation
            STDMETHOD(GetResult)(UINT32 timeout, foundation::IInspectable **ppResult) override
            {
                return this->_GetResult(timeout, ppResult);
            }

            HRESULT GetCancelDelegate(_COM_Outptr_result_maybenull_ IDelegateDispatchable **ppCancelDelegate)
            {
                return _cancelDelegatePtr.CopyTo(ppCancelDelegate);
            }

            HRESULT SetCancelDelegate(_In_ IDelegateDispatchable *pCancelDelegate)
            {
                _cancelDelegatePtr = pCancelDelegate;
                return S_OK;
            }

            template <class _Func>
            HRESULT SetCancelDelegate(const _Func& func)
            {
                typedef foundation::FuncInvoke0Delegate<IDelegateDispatchable> _Cancel_delegate_func_type;
                ComPtr<_Cancel_delegate_func_type> delegate_ptr;
                IFR_ASSERT(CreateFuncDelegate(func, delegate_ptr.GetAddressOf()));
                return SetCancelDelegate(delegate_ptr);
            }

            HRESULT _Initialize()
            {
                IFR_ASSERT(_base_type::_Initialize(true));
                IFR_ASSERT(this->CreateEventSourceWithCriticalSection(_completedEventSourcePtr.GetAddressOf()));
                _completedEventSourcePtr->_Initialize(this);
                return S_OK;
            }

            HRESULT _FireCompletedInternal(
                foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
                ICompletedEventArgs *pEventArgs)
            {
                _completedEventSourcePtr->FireEvent(
                    pFireEventHandlerContainer,
                    this->CastToInspectable(),
                    pEventArgs);
                return S_OK;
            }

        protected:
            typedef _ObjectImplBase<TInterface, IAsyncOperation, TComBase, TFireEventHandlerContainer> _base_type;
            typedef _AsyncOperationImplBase<TInterface, TComBase, TFireEventHandlerContainer> _This_type;
            typedef _EventHandlerSource
                <
                ICompletedEventSource,
                _EventHandlerContainerWithCriticalSection<ICompletedEventHandler>
                > _CompletedEventSourceBaseType;

            class _CompletedEventSource : public _CompletedEventSourceBaseType
            {
            public:
                HRESULT _Initialize(_This_type *pT)
                {
                    return AsWeakRef(_class_weak_ref_ptr, pT->CastToInspectable());
                }
            protected:
                STDMETHOD(AddHandler)(_In_ ICompletedEventHandler* pHandler)
                {
                    IFR_ASSERT(_CompletedEventSourceBaseType::AddHandler(pHandler));
                    ComPtr<IAsyncOperation> classPtr;
                    ResolveWeakRef(_class_weak_ref_ptr, classPtr);
                    if (classPtr != nullptr)
                    {
                        return static_cast<_This_type *>(classPtr.Get())->OnAddHandlerInternal(pHandler);
                    }
                    return S_OK;
                }
            private:
                WeakReferencePtr _class_weak_ref_ptr;
            };

            friend class _CompletedEventSource;
            friend class _AsyncOperationBase<_This_type>;

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == IAsyncOperation::GetIID())
                {
                    *ppInterface = static_cast<IAsyncOperation *>(this);
                }
                else if (iid == IAsyncOperationBase::GetIID())
                {
                    *ppInterface = static_cast<IAsyncOperationBase *>(this);
                }
                else
                {
                    return _base_type::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }
            virtual HRESULT OnCancelInternal()
            {
                if (_cancelDelegatePtr != nullptr)
                {
                    return _cancelDelegatePtr->Invoke();
                }
                return S_OK;
            }
            bool IsObservedInternal() override
            {
                return _completedEventSourcePtr->GetEventHandlers().size() > 0;
            }

            HRESULT OnAddHandlerInternal(_In_ ICompletedEventHandler* pHandler)
            {
                if (this->_resultStatus != ResultStatus::Started)
                {
                    foundation::ComPtr<CompletedEventArgs> eventArgsPtr;
                    IFR_ASSERT(ctl::inspectable::CreateInstance(eventArgsPtr.GetAddressOf()));
                    return pHandler->Invoke(this->CastToInspectable(), eventArgsPtr);
                }
                return S_OK;
            }
        private:
            HRESULT _OnCancel()
            {
                return OnCancelInternal();
            }

            IAsyncOperation *_GetAsyncOperation() { return static_cast<IAsyncOperation *>(this); }

            HRESULT _FireCompleted(NoRefCountFoundationCriticalSection& cs)
            {
                if (this->IsObserved())
                {
                    foundation::ComPtr<CompletedEventArgs> eventArgsPtr;
                    IFR_ASSERT(ctl::inspectable::CreateInstance(eventArgsPtr.GetAddressOf()));

                    return this->FireWithCallback(
                        this,
                        &_AsyncOperationImplBase::_FireCompletedInternal,
                        static_cast<ICompletedEventArgs *>(eventArgsPtr.Get()));
                }
                return S_FALSE;
            }

        private:
            ComPtr<IDelegateDispatchable>      _cancelDelegatePtr;
            ComPtr<_CompletedEventSource>    _completedEventSourcePtr;
        };

        typedef _AsyncOperationImplBase<IAsyncOperation> AsyncOperationInspectableImpl;
        typedef _ObjectWithTypeImplBase<AsyncOperationInspectableImpl, IID_IAsyncOperation> AsyncOperationImpl;

        static inline HRESULT CreateAsyncOperationImpl(const IID& iidType,AsyncOperationImpl **pp)
        {
            return ctl::inspectable::CreateInstanceWithInitialize(pp, &iidType);
        }
    }
}

