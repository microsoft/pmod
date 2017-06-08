/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_object_impl.h
****/
#pragma once
#include <foundation/library/base_object_impl.h>
#include "property_changed_event_args_impl.h"

#include <foundation/library/event_handler_source.h>
#include <foundation/library/object_dispatch_impl.h>
#include <pmod/interfaces/observable_object.h>

namespace pmod {

    namespace library {

        template <
            class TInterface, 
            class TComBase = foundation::ctl::ComInspectableBase,
            class TFireEventHandlerContainer = foundation::library::_DefaultFireEventHandlerContainer
        >
        class _ObservableObjectImplBase :
            public foundation::library::_ObjectImplBase<
                TInterface,
                IObservableObject, 
                TComBase,
                TFireEventHandlerContainer
            >
        {
        public:
            // Interface IObservableObject
            STDMETHOD(GetProperty)(UINT32 propertyId, foundation::IInspectable** value)
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }

            STDMETHOD(SetProperty)(UINT32 propertyId, foundation::IInspectable *value)
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }

            STDMETHOD(GetProperties)(foundation::IInspectable* pProperties, UINT32 *size, foundation::IPropertyIdPair ***ppProperties)
            {
                foundation::ComPtr<foundation::IObjectDispatchInfo> propertyDataInfoPtr;
                IFR_ASSERT(foundation::GetObjectTypeInfo(this, propertyDataInfoPtr.GetAddressOf()));
                return foundation::library::_GetProperties(
                    propertyDataInfoPtr,
                    static_cast<foundation::IObjectDispatch *>(this),
                    nullptr,
                    nullptr,
                    pProperties,
                    size,
                    ppProperties);
            }

            // Methods.
            STDMETHOD(InvokeMethod)(UINT32 methodId, _In_ UINT32 size, foundation::IInspectable **parameters, foundation::IInspectable **ppResult)
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }

            // Event methods.
            STDMETHOD(GetPropertyChangedEventSource)(_COM_Outptr_ IPropertyChangedEventSource** ppEventSource)
            {
                if (_propertyChangedEventSourcePtr == nullptr)
                {
                    IFR_ASSERT(this->CreateEventSourceWithCriticalSection(_propertyChangedEventSourcePtr.GetAddressOf()));
                }
                *ppEventSource = _propertyChangedEventSourcePtr.Get();
                (*ppEventSource)->AddRef();
                return S_OK;
            }

            STDMETHOD(GetEventModelEventSource)(_COM_Outptr_ IEventModelEventSource** ppEventSource)
            {
                if (_eventModelSourcePtr == nullptr)
                {
                    IFR_ASSERT(this->CreateEventSourceWithCriticalSection(_eventModelSourcePtr.GetAddressOf()));
                }
                *ppEventSource = _eventModelSourcePtr.Get();
                (*ppEventSource)->AddRef();
                return S_OK;
            }

            HRESULT _FirePropertyChanged(
                foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
                IPropertyChangedEventArgs *pEventArgs)
            {
                if (_propertyChangedEventSourcePtr != nullptr)
                {
                    _propertyChangedEventSourcePtr->FireEvent(
                        pFireEventHandlerContainer,
                        this->CastToInspectable(),
                        pEventArgs);
                }
                return S_OK;
            }

        protected:
            typedef foundation::library::_ObjectImplBase<TInterface, pmod::IObservableObject, TComBase, TFireEventHandlerContainer> _base_type;

            typedef foundation::library::_EventHandlerSource<
                IPropertyChangedEventSource,
                foundation::library::_EventHandlerContainerWithCriticalSection<IPropertyChangedEventHandler>
            > _PropertyChangedEventSource_Type;

            typedef foundation::library::_EventHandlerSource<
            IEventModelEventSource,
            foundation::library::_EventHandlerContainerWithCriticalSection<IEventModelEventHandler>
            > _EventModelSource_Type;

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == pmod::IObservableObject::GetIID())
                {
                    *ppInterface = static_cast<pmod::IObservableObject *>(this);
                }
                else
                {
                    return _base_type::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            HRESULT FirePropertyChangedWithCallback(UINT32 propertyId)
            {
                if (this->IsObserved())
                {
                    foundation::ComPtr<IPropertyChangedEventArgs> event_args_ptr;
                    IFR_ASSERT(PropertyChangedEventArgsWIthCallbackImpl::CreateInstance(
                        this,
                        propertyId,
                        event_args_ptr.GetAddressOf()));
                    return this->FireWithCallback(
                        this,
                        &_ObservableObjectImplBase::_FirePropertyChanged,
                        event_args_ptr.Get());
                }
                return S_FALSE;
            }

            bool IsObservedInternal() override
            {
                return (_propertyChangedEventSourcePtr != nullptr && _propertyChangedEventSourcePtr->GetEventHandlers().size() > 0) ||
                    (_eventModelSourcePtr != nullptr && _eventModelSourcePtr->GetEventHandlers().size() > 0);
            }

        private:
            foundation::ComPtr<_PropertyChangedEventSource_Type> _propertyChangedEventSourcePtr;
            foundation::ComPtr<_EventModelSource_Type> _eventModelSourcePtr;
        };

        typedef foundation::library::_ObjectWithTypeImplBase<_ObservableObjectImplBase<IObservableObject>, IID_IObservableObject> _ObservableObjectImplBaseType;
        class ObservableObjectImpl :
            public _ObservableObjectImplBaseType,
            protected foundation::library::ObjectDispatchContainerImpl
        {
        public:
            HRESULT _Initialize(const IID& iidType, bool isFreeThreaded)
            {
                _IFR_(_ObservableObjectImplBaseType::_Initialize(&iidType, isFreeThreaded));
                _IFR_(foundation::library::ObjectDispatchContainerImpl::_Initialize_container(iidType));
                return S_OK;
            }

            STDMETHOD(GetProperty)(UINT32 propertyId, foundation::IInspectable** value)
            {
                foundation::library::NoRefCountFoundationCriticalSection cs(this->_GetCriticalSection());
                return this->GetPropertyValue(propertyId, value);
            }

            STDMETHOD(SetProperty)(UINT32 propertyId, foundation::IInspectable *value)
            {
                foundation::library::NoRefCountFoundationCriticalSection cs(this->_GetCriticalSection());
                _IFR_(this->SetPropertyValue(propertyId, value));
                return this->FirePropertyChangedWithCallback(propertyId);
            }
        };

        static inline HRESULT CreateObservableObjectImpl(ObservableObjectImpl **pp, const IID& iidType, bool isFreeThreaded = false)
        {
            return foundation::ctl::inspectable::CreateInstanceWithInitialize(pp, iidType, isFreeThreaded);
        }

    }
}
