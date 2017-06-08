/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:command_impl.h
****/
#pragma once
#include <foundation/value_ptr.h>
#include <foundation/library/base_object_impl.h>
#include "can_execute_changed_event_args_impl.h"
#include <foundation/library/event_handler_source.h>
#include <foundation/library/boxing_value_adapters.h>
#include <pmod/interfaces/command.h>
#include <foundation/weak_reference.h>

#include <vector>
#include <set>

namespace pmod {

    namespace library {

        template <
            class TInterface, 
            class TComBase = foundation::ctl::ComInspectableBase,
            class TFireEventHandlerContainer = foundation::library::_DefaultFireEventHandlerContainer
            >
        class _CommandBaseImplBase :
            public foundation::library::_ObjectImplBase<TInterface, ICommand, TComBase, TFireEventHandlerContainer>,
            public ICommand
        {
        public:
            // Interface ICommandBase
            STDMETHOD(GetState)(_Out_ UINT32* pVal)
            {
                *pVal = this->_state;
                return S_OK;
            }

            // Event methods.
            STDMETHOD(GetCanExecuteChangedEventSource)(_COM_Outptr_ ICanExecuteChangedEventSource** ppEventSource)
            {
                if (_canExecuteChangedSourcePtr == nullptr)
                {
                    IFR_ASSERT(this->CreateEventSourceWithCriticalSection(_canExecuteChangedSourcePtr.GetAddressOf()));
                }
                *ppEventSource = _canExecuteChangedSourcePtr.Get();
                (*ppEventSource)->AddRef();
                return S_OK;
            }

            // Interface ICommand
            STDMETHOD(GetUniqueId)(UINT32 *pUniqueId) override
            {
                return _base_type::GetUniqueId(pUniqueId);
            }
            STDMETHOD(GetProcessId)(UINT32 *pProcessId) override
            {
                return _base_type::GetProcessId(pProcessId);
            }

            STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue) override
            {
                return _base_type::GetTypeInfo(ppValue);
            }

            STDMETHOD(CanExecute)(foundation::IInspectable *parameter, _Out_ bool *pCanExecute)
            {
                UNREFERENCED_PARAMETER(parameter);
                *pCanExecute = this->_state == CommandStateEnabled;
                return S_OK;
            }

            HRESULT set_command_state(UINT32 newState)
            {
                UINT32 oldState = this->_state;
                this->_state = newState;

                if (oldState != newState && this->IsObserved())
                {
                    foundation::ComPtr<ICanExecuteChangedEventArgs> event_args_ptr;
                    IFR_ASSERT(CanExecuteChangedEventArgsImpl::CreateInstance(
                        oldState,
                        newState,
                        event_args_ptr.GetAddressOf()));

                    return this->FireWithCallback(
                        this,
                        &_CommandBaseImplBase::FireCanExecuteChanged,
                        event_args_ptr.Get());
                }
                return S_OK;
            }

            HRESULT set_command_enabled(bool enable)
            {
                return set_command_state(enable ? CommandStateEnabled : CommandStateDisabled);
            }

        protected:
            _CommandBaseImplBase():
                _state(CommmandModelState::CommandStateUnknown)
            {}

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == ICommandBase::GetIID())
                {
                    *ppInterface = static_cast<ICommandBase *>(static_cast<_base_type *>(this));
                }
                else if (iid == ICommand::GetIID())
                {
                    *ppInterface = static_cast<ICommand *>(this);
                }
                else
                {
                    return _base_type::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            HRESULT FireCanExecuteChanged(
                foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
                ICanExecuteChangedEventArgs *pEventArgs)
            {
                if (_canExecuteChangedSourcePtr != nullptr)
                {
                    _canExecuteChangedSourcePtr->FireEvent(
                        pFireEventHandlerContainer,
                        this->CastToInspectable(),
                        pEventArgs);
                }
                return S_OK;
            }

            bool IsObservedInternal() override
            {
                return _canExecuteChangedSourcePtr != nullptr && _canExecuteChangedSourcePtr->GetEventHandlers().size() > 0;
            }

            typedef foundation::library::_ObjectImplBase<TInterface, ICommand, TComBase, TFireEventHandlerContainer> _base_type;

            typedef foundation::library::_EventHandlerSource<
                ICanExecuteChangedEventSource,
                foundation::library::_EventHandlerContainerWithCriticalSection<ICanExecuteChangedEventHandler>
            > _CanExecuteChangedEventSource_Type;

        private:
            foundation::ComPtr<_CanExecuteChangedEventSource_Type> _canExecuteChangedSourcePtr;
            UINT32 _state;
        };

        typedef _CommandBaseImplBase<ICommandBase> _CommandInspectableBaseImplBase;

        class _SimpleCommandImplBase :
            public _CommandBaseImplBase<ISimpleCommand>
        {
        public:
            STDMETHOD(CanExecute)(_Out_ bool *pCanExecute)
            {
                return this->_CommandBaseImplBase<ISimpleCommand>::CanExecute(nullptr, pCanExecute);
            }
            STDMETHOD(Execute)()
            {
                return E_NOTIMPL;
            }
        protected:
            STDMETHOD(Execute)(foundation::IInspectable *parameter, foundation::IInspectable **ppResult) override
            {
                UNREFERENCED_PARAMETER(parameter);
                UNREFERENCED_PARAMETER(ppResult);
                return this->Execute();
            }
        };

        template <class TInterface>
        class _CommandParameterImplBase :
            public _CommandBaseImplBase<TInterface>
        {
        public:
            STDMETHOD(CanExecute)(typename TInterface::_TParameter_Type parameter,_Out_ bool *pCanExecute)
            {
                UNREFERENCED_PARAMETER(parameter);
                return this->_CommandBaseImplBase<TInterface>::CanExecute(nullptr, pCanExecute);
            }

            STDMETHOD(Execute)(typename TInterface::_TParameter_Type parameter)
            {
                return E_NOTIMPL;
            }
        protected:
            STDMETHOD(Execute)(foundation::IInspectable *parameter, foundation::IInspectable **ppResult) override
            {
                UNREFERENCED_PARAMETER(ppResult);
                return this->Execute(foundation::ValuePtr(parameter).GetValue<typename TInterface::_TParameter_Type>());
            }
        };

        template <class TInterface>
        class _CommandResultValueImplBase :
            public _CommandBaseImplBase<TInterface>
        {
        public:
            STDMETHOD(CanExecute)(_Out_ bool *pCanExecute)
            {
                return this->_CommandBaseImplBase<TInterface>::CanExecute(nullptr, pCanExecute);
            }

            STDMETHOD(Execute)(typename TInterface::_TResult_Type *pResult)
            {
                return E_NOTIMPL;
            }
        protected:
            STDMETHOD(Execute)(foundation::IInspectable *parameter, foundation::IInspectable **ppResult) override
            {
                UNREFERENCED_PARAMETER(parameter);
                typename  TInterface::_TResult_Type result;
                _IFR_(this->Execute(&result));
                *ppResult = foundation::CreateValue(result).Detach();
                return S_OK;
            }
        };

        template <class TInterface>
        class _CommandResultPtrImplBase :
            public _CommandBaseImplBase<TInterface>
        {
        public:
            STDMETHOD(CanExecute)(_Out_ bool *pCanExecute)
            {
                return this->_CommandBaseImplBase<TInterface>::CanExecute(nullptr, pCanExecute);
            }

            STDMETHOD(Execute)(typename TInterface::_TResult_Type *pResult)
            {
                return E_NOTIMPL;
            }
        protected:
            STDMETHOD(Execute)(foundation::IInspectable *parameter, foundation::IInspectable **ppResult) override
            {
                UNREFERENCED_PARAMETER(parameter);
                return this->Execute(reinterpret_cast<typename  TInterface::_TResult_Type *>(ppResult));
            }
        };

        template <class T,class TBASE, class TInterface = IObservableObject>
        class _WeakRefCommandImpl : public TBASE
        {
        public:
            HRESULT _Initialize(_In_ T *pT)
            {
                IFR_ASSERT(foundation::AsWeakRef(_weak_ref_ptr, pT->CastToInspectable()));
                return S_OK;
            }
        protected:
            T *get_target()
            {
                return foundation::ResolveAndCastWeakRefWeak<T, TInterface>(_weak_ref_ptr);
            }
        private:
            foundation::WeakReferencePtr _weak_ref_ptr;
        };

    }
}
