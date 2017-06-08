/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_factory.h
****/
#pragma once

#include <foundation/library/class_options.h>
#include <foundation/library/class_base.h>
#include <foundation/library/object_factory_util.h>


namespace pmod {
    namespace library {

        template
            <
                class T,
                const IID& _TypeIId,
                const ObservableObjectOptions _Options = ObservableObjectOptions::None,
                bool isFreeThreaded = false
            >
        class ObjectFactory
        {
        public:
            static IID GetType()
            {
                return _TypeIId;
            }

            template <typename... Arguments>
            static HRESULT CreateInstanceWithParent(
                foundation::IObject *pParent,
                T **ppT,
                const Arguments&... params)
            {
                return foundation::library::CreateObjectClassWithParent(
                    GetType(), 
                    (ObservableObjectOptions)((UINT32)_Options | (isFreeThreaded ? foundation::library::FoundationClassOptions_IsFreeThreadApartment:0)),
                    ppT,
                    pParent,
                    params...);
            }

            template <typename... Arguments>
            static HRESULT CreateInstance(
                T **ppT,
                const Arguments&... params)
            {
                return CreateInstanceWithParent(nullptr, ppT, params...);
            }

            template <typename Q,typename... Arguments>
            static HRESULT CreateInstanceAs(
                Q **ppQ,
                const Arguments&... params)
            {
                foundation::ComPtr<T, foundation::ctl::_ComBasePtrImpl> ptr;
                IFR_ASSERT(CreateInstance(ptr.GetAddressOf(), params...));
                return foundation::QueryInterface(ptr->CastToInspectable(), ppQ);
            }

            // register a callback that does not need any paramater
            static HRESULT RegisterObjectCreator(const IID&iidType = _TypeIId)
            {
                IFR_ASSERT(foundation::library::RegisterObjectFactory(iidType, CreateCallback));
                return S_OK;
            }

            template <class _ValueType>
            static HRESULT RegisterObjectCreatorWithContextValue(const IID&iidType = _TypeIId)
            {
                IFR_ASSERT(foundation::library::RegisterObjectFactory(
                    iidType,
                    CreateCallbackWithContextValue<_ValueType>));
                return S_OK;
            }

            template <class Q>
            static HRESULT RegisterObjectCreatorWithContextType(const IID&iidType = _TypeIId)
            {
                IFR_ASSERT(foundation::library::RegisterObjectFactory(
                    iidType,
                    CreateCallbackWithContextType<Q>));
                return S_OK;
            }

            class _RegisterObjectCreator
            {
            public:
                _RegisterObjectCreator(const IID&iidType = _TypeIId)
                {
                    RegisterObjectCreator(iidType);
                }
            };

            template <class Q>
            class _RegisterObjectCreatorWithContextType
            {
            public:
                _RegisterObjectCreatorWithContextType(const IID&iidType = _TypeIId)
                {
                    RegisterObjectCreatorWithContextType<Q>(iidType);
                }
            };
        private:
            static HRESULT STDMETHODCALLTYPE CreateCallback(
                _In_ foundation::IInspectable *pContextParameter,
                _COM_Outptr_ foundation::IObject **ppObject)
            {
                IFCPTR_ASSERT(ppObject);
                *ppObject = nullptr;

                UNREFERENCED_PARAMETER(pContextParameter);

                T *pT = nullptr;
                IFR_ASSERT(CreateInstance(&pT));
                *ppObject = pT->GetInterface();

                return S_OK;
            }

            template <class _ValueType>
            static HRESULT STDMETHODCALLTYPE CreateCallbackWithContextValue(
                _In_ foundation::IInspectable *pContextParameter,
                _COM_Outptr_ foundation::IObject **ppObject)
            {
                IFCPTR_ASSERT(ppObject);
                *ppObject = nullptr;

                _ValueType value;
                IFR_ASSERT(foundation::GetValue(pContextParameter, value));

                T *pT = nullptr;
                IFR_ASSERT(CreateInstance(&pT, value));
                *ppObject = pT->GetInterface();
                return S_OK;
            }

            template <class Q>
            static HRESULT STDMETHODCALLTYPE CreateCallbackWithContextType(
                _In_ foundation::IInspectable *pContextParameter,
                _COM_Outptr_ foundation::IObject **ppObject)
            {
                IFCPTR_ASSERT(ppObject);
                *ppObject = nullptr;

                foundation::ComPtr<Q> context_parameter_ptr;
                if (pContextParameter != nullptr &&
                    !foundation::pv_util::IsValueEmpty(pContextParameter))
                {
                    IFR_ASSERT(foundation::QueryInterface(
                        pContextParameter,
                        context_parameter_ptr.GetAddressOf()));
                }
                T *pT = nullptr;
                IFR_ASSERT(CreateInstance(&pT, context_parameter_ptr.Get()));
                *ppObject = pT->GetInterface();
                return S_OK;
            }
        };
    }
}
