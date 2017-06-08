/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_factory_util.h
****/
#pragma once

#if _HAS_CPP0X
#include <functional>
#endif

#include <foundation/com_ptr.h>
#include <foundation/pv_util.h>
#include <foundation/activation_factory.h>
#include <foundation/interfaces/object_factory.h>

#include "callback_delegate_adapter_base.h"
#include <foundation/interfaces/object_factory.h>
#include <foundation/foundation_factory.h>

namespace foundation
{
    namespace library
    {
        // Object Creator Callback
        typedef HRESULT(STDMETHODCALLTYPE *CreateObjectCallback)(
            _In_opt_ IInspectable* contextParameter,     // the Context Parameter
            _COM_Outptr_ IObject **ppNewInstance);     // the new View Model instance

        // create_model_callback_adapter Class
        class _CreateObjectCallbackAdapter :
            public _CallbackDelegateAdapterBase
            <
            _CreateObjectCallbackAdapter,
            CreateObjectCallback,
            ICreateObjectDelegate,
            &IID_ICreateObjectDelegate
            >
        {
        protected:
            STDMETHOD(Invoke)(_In_ foundation::IInspectable* pContextParameter, foundation::IObject **ppNewInstance)
            {
                return (*this->GetCallback())(pContextParameter, ppNewInstance);
            }
        };
#if _HAS_CPP0X

        typedef std::function<HRESULT(foundation::IInspectable*, IObject **)> _CreatorFuncCallbackType;
        template <class _Func>
        class _CreateObjectFuncAdapter :
            public ctl::ImplementsInspectable<ICreateObjectDelegate, &IID_ICreateObjectDelegate>
        {
        public:
            HRESULT _Initialize(const _CreatorFuncCallbackType& func)
            {
                this->_callback = func;
                return S_OK;
            }
        protected:
            _CreateObjectFuncAdapter()
            {
            }

            STDMETHOD(Invoke)(_In_ foundation::IInspectable* pContextParameter, foundation::IObject **ppNewInstance)
            {
                return _callback(pContextParameter, ppNewInstance);
            }
        private:
            _CreatorFuncCallbackType _callback;
        };
#endif

        inline IObjectFactory *GetObjectFactory()
        {
            return GetFoundationFactory<IObjectFactory>(U("foundation.ObjectFactory"));
        }

        inline IObjectFactoryFactory *GetObjectFactoryFactory()
        {
            return GetFoundationFactory<IObjectFactoryFactory>(U("foundation.ObjectFactoryFactory"));
        }

        template <class T, class _ContextParamterType>
        HRESULT CreateObjectFromFactory(
            _In_ GUID iidType,
            _In_ _ContextParamterType context_parameter,
            T **ppT)
        {
            InspectablePtr context_parameter_ptr = pv_util::CreateValue(context_parameter);

            ComPtr<foundation::IObject> spObject;
            _IFR_(GetObjectFactory()->TryCreateObject(
                iidType,
                context_parameter_ptr,
                spObject.GetAddressOf()));
            if (spObject == nullptr)
            {
                return E_UNEXPECTED;
            }

            IFR_ASSERT(foundation::QueryInterface(spObject, ppT));
            return S_OK;
        }

        template <class T>
        HRESULT CreateObjectFromFactory(
            _In_ GUID iidType,
            T **pT)
        {
            return CreateObjectFromFactory(iidType, (IInspectable *)nullptr, pT);
        }

        template <class T>
        static HRESULT ActivateObject(
            _In_ GUID iidType,
            _In_ foundation::IInspectable *pContextParameter,
            _COM_Outptr_ T **ppT)
        {
            IFCPTR_ASSERT(ppT);
            *ppT = nullptr;

            foundation::ComPtr<foundation::IObject> spObject;
            _IFR_(GetObjectFactory()->ActivateObject(iidType, pContextParameter, spObject.GetAddressOf()));
            foundation_assert(spObject != nullptr);
            return foundation::QueryInterface(spObject, ppT);
        }

        inline HRESULT RegisterObjectFactory(
            _In_ GUID iidClassType,
            _In_ ICreateObjectDelegate *pCreateModelDelegate)
        {
            return GetObjectFactory()->RegisterObjectFactory(iidClassType, pCreateModelDelegate);
        }

        inline HRESULT UnregisterObjectFactory(
            _In_ GUID iidClassType)
        {
            return GetObjectFactory()->UnregisterObjectFactory(iidClassType);
        }

        inline HRESULT RegisterObjectFactory(
            _In_ IObjectFactory *pObjectFactory,
            _In_ GUID iidClassType,
            _In_ library::CreateObjectCallback pCreateObjectCallback)
        {
            foundation_assert(pObjectFactory);
            foundation_assert(pCreateObjectCallback);

            ComPtr<ICreateObjectDelegate> spCreator;
            IFR_ASSERT(_CreateObjectCallbackAdapter::CreateInstance(
                pCreateObjectCallback,
                spCreator.GetAddressOf()));
            return pObjectFactory->RegisterObjectFactory(iidClassType, spCreator);
        }

        inline HRESULT CreateObjectCallbackAdapter(
            _In_ library::CreateObjectCallback pCreateObjectCallback,
            _COM_Outptr_ ICreateObjectDelegate **ppCreateObjectDelegate)
        {
            return _CreateObjectCallbackAdapter::CreateInstance(pCreateObjectCallback, ppCreateObjectDelegate);
        }

#if _HAS_CPP0X
        template <class _Func>
        inline HRESULT CreateObjectCallbackAdapter(
            _In_ const _Func& callback,
            _COM_Outptr_ ICreateObjectDelegate **ppCreateObjectDelegate)
        {
            typedef _CreateObjectFuncAdapter<_Func> _Creator_type;
            return ctl::inspectable::CreateInstanceWithInitialize<_Creator_type>(ppCreateObjectDelegate, callback);
        }
#endif

        inline HRESULT RegisterObjectFactory(
            _In_ GUID iidClassType,
            _In_ library::CreateObjectCallback pCreateObjectCallback)
        {
            return RegisterObjectFactory(GetObjectFactory(), iidClassType, pCreateObjectCallback);
        }

        template <class T, class TContext>
        static HRESULT STDMETHODCALLTYPE CreateFactoryObjectInstanceWithContext(
            foundation::IInspectable *pContext,
            foundation::IObject **ppObject)
        {
            ComPtr<TContext> context_ptr;
            IFR_ASSERT(foundation::QueryInterface(pContext, context_ptr.GetAddressOf()));

            ComPtr<ctl::ComInspectableObject<T>> p_ptr;
            _IFR_(ctl::inspectable::CreateInstanceWithInitialize(
                p_ptr.GetAddressOf(),
                context_ptr.Get()));
            IFR_ASSERT(foundation::QueryInterface(
                ctl::inspectable::CastToInspectable(p_ptr.Get()),
                ppObject));
            return S_OK;
        }

        template <class T, class TContext>
        inline HRESULT RegisterObjectCreatorWithContext(GUID iidType)
        {
            return RegisterObjectCreator(
                iidType,
                CreateFactoryObjectInstanceWithContext<T, TContext>);
        }

        inline HRESULT CreateObjectFactory(_COM_Outptr_ IObjectFactory **pp)
        {
            return GetObjectFactoryFactory()->CreateObjectFactory(pp);
        }

    }

}

