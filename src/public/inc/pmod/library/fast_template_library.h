/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:fast_template_library.h
****/
#pragma once

#include "mtl.h"
#include <foundation/weak_reference.h>
#include <foundation/library/critical_section.h>
#include "type_adapter_base.h"


namespace pmod {
    namespace library {

        // template:_base_helper
        // Allow deriving two classes
        template <class TBase1, class TBase2>
        class _FastBaseHelper :
            public TBase1,
            public TBase2
        {
        };

        //------------------------------------------------------------------------------
        // Class:   _FastPropertiesContainer
        //
        // Purpose: a base class for a 'fast' model container implementation
        //
        //------------------------------------------------------------------------------
        template <class T>
        class _FastPropertiesContainer :
            public foundation::ctl::ImplementsInspectable
            <
                foundation::library::IPropertiesContainer,
                &foundation::library::IID_IPropertiesContainer
            >
        {
        public:
            // used internally
            void _SetOuter(T *pOuter)
            {
                _pOuter = pOuter;
            }
        protected:
            _FastPropertiesContainer() :
                _pOuter(nullptr)
            {
            }
            // interface foundation::library::IPropertiesContainer
            STDMETHOD(HasProperty)(_In_ UINT32 /*propertyId*/, _Out_ bool *found)
            {
                IFCPTR_ASSERT(found);
                // TODO: we would need to code generate the property bounds
                *found = true;
                return S_OK;
            }

            STDMETHOD(GetValue)(_In_ UINT32 propertyId, foundation::IInspectable **ppValue)
            {
                return E_NOTIMPL;
            }

            STDMETHOD(SetValue)(_In_ UINT32 propertyId, _In_opt_ foundation::IInspectable *pValue)
            {
                return E_NOTIMPL;
            }

            STDMETHOD(RemoveProperty)(_In_ UINT32 propertyId)
            {
                return E_NOTIMPL;
            }

            STDMETHOD(GetSize)(_Out_ UINT32* size)
            {
                return E_NOTIMPL;
            }

            // interface IDisposable
            STDMETHOD(Dispose)()
            {
                return S_FALSE;
            }
            T *GetOuter()
            {
                return _pOuter;
            }
        private:
            T *_pOuter;
        };

        // helper to create a fast properties container
        template <class T,class TOuter>
        HRESULT _CreateFastPropertiesContainer(
            _In_ TOuter *pOuter,
            _COM_Outptr_ foundation::library::IPropertiesContainer **ppValue)
        {
            T *pT = nullptr;
            IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
            pT->_SetOuter(pOuter);
            *ppValue = static_cast<foundation::library::IPropertiesContainer *>(pT);
            return S_OK;
        }

        template <class TBASE = _ObservableObjectClass>
        class _FastModelBase :
            public TBASE
        {
        protected:
            // helper to allow settings properties using the 'core' infra
            template <class TValue>
            HRESULT SetPropertyInternal(UINT32 propertyId, TValue value)
            {
                foundation::InspectablePtr spValue = foundation::pv_util::CreateValue(value);
                return this->GetInterfaceProtected()->SetPropertyInternal(propertyId, spValue, true);
            }

            // helper to allow settings enum properties using the 'core' infra
            template <class TValue>
            HRESULT SetEnumPropertyInternal(UINT32 propertyId, UINT32 enumTypeId, TValue value)
            {
                foundation::ComPtr<foundation::IEnumValue> spEnumValue;
                IFR_ASSERT(foundation::GetEnumValue(enumTypeId, (UINT32)value, spEnumValue.GetAddressOf()));
                return this->GetInterfaceProtected()->SetPropertyInternal(propertyId, spEnumValue, true);
            }

            // helper to allow settings array properties using the 'core' infra
            template <class TValue>
            HRESULT SetPropertyInternal(UINT32 propertyId, UINT32 size, TValue value)
            {
                foundation::InspectablePtr spValue = foundation::pv_util::CreateValue(size, value);
                return this->GetInterfaceProtected()->SetPropertyInternal(propertyId, spValue, true);
            }

            IObservableObject *GetInner()
            {
                return this->GetInterface();
            }
        };

        template <class TBASE, class TInterface>
        class _DelegateFastModelBase :
            public TBASE,
            public TInterface
        {
        public:
#if defined(_VARIADIC_SUPPORT) 
            template <typename... Arguments>
            _DelegateFastModelBase(_In_ Arguments&... params)
                :TBASE(params...)
            {
            }
#else 
            template <typename Argument>
            _DelegateFastModelBase(_In_ Argument& parameter)
                : TBASE(parameter)
            {
            }
            _DelegateFastModelBase()
            {
            }
#endif
        protected:
            // Interface foundation::IObject
            STDMETHOD(GetUniqueId)(UINT32 *uniqueId) override
            {
                return TBASE::GetUniqueId(uniqueId);
            }
            STDMETHOD(GetProcessId)(UINT32 *processId) override
            {
                return TBASE::GetProcessId(processId);
            }

            STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue) override
            {
                return TBASE::GetTypeInfo(ppValue);
            }

            // IObservableObject Interface
            STDMETHOD(GetProperty)(_In_ UINT32 propertyId, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppValue) override
            {
                return TBASE::GetProperty(propertyId, ppValue);
            }
            STDMETHOD(SetProperty)(_In_ UINT32 propertyId, _In_opt_ foundation::IInspectable *pValue) override
            {
                return TBASE::SetProperty(propertyId, pValue);
            }

            STDMETHOD(GetProperties)(foundation::IInspectable *pProperties, _Out_ UINT32* size, foundation::IPropertyIdPair ***ppProperties) override
            {
                return TBASE::GetProperties(pProperties, size, ppProperties);
            }

            STDMETHOD(GetPropertyChangedEventSource)(_COM_Outptr_ IPropertyChangedEventSource** ppEventSource) override
            {
                return TBASE::GetPropertyChangedEventSource(ppEventSource);
            }

            STDMETHOD(GetEventModelEventSource)(_COM_Outptr_ IEventModelEventSource** ppEventSource) override
            {
                return TBASE::GetEventModelEventSource(ppEventSource);
            }

            STDMETHOD(InvokeMethod) (_In_ UINT32 methodId, _In_ UINT32 size, _In_opt_ foundation::IInspectable **parameters, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppResult) override
            {
                return TBASE::InvokeMethod(methodId, size, parameters, ppResult);
            }

        };

    }
}
