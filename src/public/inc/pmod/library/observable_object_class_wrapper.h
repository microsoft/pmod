/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_object_class_wrapper.h
****/
#pragma once

#include <foundation/library/persistant_storage_service_util.h>
#include <foundation/library/string_util.h>
#include <foundation/library/async_operation_class_ptr.h>

#include "observable_statics_util.h"
#include "object_node_class_wrapper.h"
#include "interfaces/observable_object_class.h"

namespace pmod
{
    namespace library {
        //------------------------------------------------------------------------------
        // Class:   _ObservableObjectWrapperType
        //
        // Purpose: Define a base class to set/get properties to a Property Model
        //
        //------------------------------------------------------------------------------
        template <class TClass>
        class _ObservableObjectWrapperType
        {
        public:
            template <class T>
            HRESULT SetModelProperty(_In_ UINT32 propertyId, _In_ T value)
            {
                TClass *pT = static_cast<TClass *>(this);
                return pT->set_property_value(propertyId, foundation::CreateValue(value));
            }

            template <class T>
            HRESULT GetModelProperty(_In_ UINT32 propertyId, _Out_ T* value)
            {
                TClass *pT = static_cast<TClass *>(this);
                foundation::InspectablePtr valuePtr;
                _IFR_(pT->GetPropertyValue(propertyId, valuePtr.GetAddressOf()));
                return foundation::pv_util::GetValue(valuePtr, value);
            }

            template <class T>
            T GetModelProperty(_In_ UINT32 propertyId)
            {
                T value;
                THROW_IFR(this->GetModelProperty(propertyId, &value));
                return value;
            }

            template <class T>
            T GetModelPropertyArray(_In_ UINT32 propertyId)
            {
                T arrayWrapper;
                THROW_IFR(this->GetModelProperty(propertyId,
                    arrayWrapper.GetSizeAddressOf(),
                    arrayWrapper.GetBufferAddressOf()));
                return arrayWrapper;
            }

            template <class Q>
            HRESULT GetModelProperty(_In_ UINT32 propertyId, REFIID riid, _COM_Outptr_result_maybenull_ Q ** pp)
            {
                foundation_assert(pp);
                *pp = nullptr;

                TClass *pT = static_cast<TClass*>(this);
                foundation::InspectablePtr valuePtr;
                _IFR_(pT->GetPropertyValue(propertyId, valuePtr.GetAddressOf()));
                if (valuePtr != nullptr)
                {
                    return foundation::QueryInterface(valuePtr, riid, pp);
                }

                return S_OK;
            }

            inline HRESULT GetModelProperty(_In_ UINT32 propertyId, _COM_Outptr_result_maybenull_ foundation::IInspectable* ppValue)
            {
                TClass *pT = static_cast<TClass *>(this);
                return pT->GetPropertyValue(propertyId, ppValue);
            }

            template <class Q>
            inline HRESULT GetModelPropertyType(_In_ UINT32 propertyId, _COM_Outptr_result_maybenull_ Q ** pp)
            {
                return GetModelProperty(propertyId, Q::GetIID(), pp);
            }

            // Array support
            template <class T>
            HRESULT SetModelProperty(_In_ UINT32 propertyId, _In_ UINT32 size, _In_reads_(size) T *values)
            {
                foundation::InspectablePtr spValue = foundation::CreateValue(size, values);
                TClass *pT = static_cast<TClass *>(this);
                return pT->set_property_value(propertyId, spValue);
            }

#pragma warning( push )
#pragma warning( disable : 6001 )
            template <class T>
            HRESULT GetModelProperty(_In_ UINT32 propertyId, _Out_ UINT32 *pSize, _Outptr_result_maybenull_ T **ppValues)
            {
                foundation_assert(ppValues != nullptr);
                *ppValues = nullptr;

                foundation::InspectablePtr spValue;
                TClass *pT = static_cast<TClass*>(this);
                _IFR_(pT->GetPropertyValue(propertyId, spValue.GetAddressOf()));
                _IFR_(foundation::pv_util::GetValue(spValue, pSize, ppValues));
                return S_OK;
            }
#pragma warning( pop )

            template <class TModelWrapper>
            TModelWrapper GetModelPropertyWrapper(_In_ UINT32 propertyId)
            {
                TModelWrapper wrapper;
                IGNOREHR(GetModelPropertyWrapper(propertyId, wrapper));
                return wrapper;
            }

            template <class TModelWrapper>
            HRESULT GetParentWrapper(TModelWrapper& wrapper)
            {
                return GetModelPropertyWrapper(Property_Parent, wrapper);
            }

            template <class TModelWrapper>
            HRESULT GetModelPropertyWrapper(_In_ UINT32 propertyId, TModelWrapper& wrapper)
            {
                TClass *pT = static_cast<TClass *>(this);
                return pT->GetModelPropertyWrapperImpl(propertyId, wrapper);
            }

            template <class TModelWrapper>
            HRESULT SetModelPropertyWrapper(_In_ UINT32 propertyId, const TModelWrapper& wrapper)
            {
                _IFR_(set_property_value(propertyId, wrapper.GetSource()));
                return S_OK;
            }

        protected:
            template <class TModelWrapper>
            HRESULT GetModelPropertyWrapperImpl(_In_ UINT32 propertyId, TModelWrapper& wrapper)
            {
                wrapper.Detach();

                foundation::ComPtr<IObservableObject> spPropertyModel;
                HRESULT hr = this->GetModelPropertyType(propertyId, spPropertyModel.GetAddressOf());
                if (FAILED(hr))
                {
                    wrapper.detach(hr);
                    _IFR_(hr);
                }
                else if (spPropertyModel)
                {
                    _IFR_(wrapper.attach(spPropertyModel));
                }
                return S_OK;
            }
        };

        typedef std::pair<UINT32, foundation::InspectablePtr> _PropertyIdPair_Type;

#if _HAS_CPP0X
        typedef foundation::FuncInvoke5Delegate<IMethodOnInvokeDelegate, foundation::IMethodInfo *, UINT32, UINT32, foundation::IInspectable **, foundation::IInspectable ** > _On_invoke_method_delegate_func;

        template <class _Func>
        static HRESULT _SetMethodOnInvokeDelegate(
            _In_ IObservableObjectClass *pObservableObjectClass,
            _In_ const _Func& func)
        {
            foundation::ComPtr<_On_invoke_method_delegate_func> delegatePtr;
            IFR_ASSERT(CreateFuncDelegate(func, delegatePtr.GetAddressOf()));
            return pObservableObjectClass->SetMethodOnInvokeDelegate(delegatePtr);
        }
#endif

        template <class T>
        class _ObservableObjectClassWrapper :
            public _ObjectNodeClassWrapper<T, IObservableObjectClass>
        {
        public:
            HRESULT SetPropertyInternal(_In_ UINT32 propertyId, _In_ foundation::IInspectable *newValue)
            {
                return this->GetClassInterfaceInternal()->SetPropertyInternal(propertyId, newValue, true);
            }

            HRESULT GetPropertyValue(_In_ UINT32 propertyId, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppValue)
            {
                return this->GetClassInterfaceInternal()->GetProperty(propertyId, ppValue);
            }

            HRESULT SetMethodOnInvokeDelegate(_In_opt_ IMethodOnInvokeDelegate *pMethodOnInvokeDelegate)
            {
                return this->GetClassInterfaceInternal()->SetMethodOnInvokeDelegate(pMethodOnInvokeDelegate);
            }

            template <class _Func>
            HRESULT SetMethodOnInvokeDelegate(const _Func& func)
            {
                return _SetMethodOnInvokeDelegate(this->GetClassInterfaceInternal(), func);
            }

            HRESULT GetMethodOnInvokeDelegate(_COM_Outptr_result_maybenull_ IMethodOnInvokeDelegate **ppMethodOnInvokeDelegate)
            {
                return this->GetClassInterfaceInternal()->GetMethodOnInvokeDelegate(ppMethodOnInvokeDelegate);
            }

            template <class TInterface>
            HRESULT GetPropertiesContainer(_COM_Outptr_ TInterface **ppPropertiesContainer)
            {
                foundation::ComPtr<foundation::library::IPropertiesContainer> properties_container_ptr;
                IFR_ASSERT(this->GetClassInterfaceInternal()->GetPropertiesContainer(properties_container_ptr.GetAddressOf()));

                return properties_container_ptr.QueryInterface(ppPropertiesContainer);
            }

            HRESULT FirePropertyChanged(_In_ UINT32 propertyId)
            {
                return this->GetClassInterfaceInternal()->FirePropertyChanged(propertyId, false);
            }

            HRESULT FirePropertyChangedWithCallback(_In_ UINT32 propertyId)
            {
                return this->GetClassInterfaceInternal()->FirePropertyChanged(propertyId, true);
            }

            HRESULT FirePropertiesChanged(
                _In_ bool use_callback,
                _In_ UINT32 propertyIdStart,
                ...)
            {
                va_list marker;
                va_start(marker, propertyIdStart);     /* Initialize variable arguments. */

                UINT32 propertyId = propertyIdStart;

                while (propertyId != (UINT32)-1)
                {
                    this->GetClassInterfaceInternal()->FirePropertyChanged(propertyId, use_callback);
                    // go to next property id
                    propertyId = va_arg(marker, UINT32);
                }
                va_end(marker);
                return S_OK;
            }

            HRESULT FireEventModel(_In_ UINT32 eventId, _In_ foundation::IInspectable *pEventArgs)
            {
                return this->GetClassInterfaceInternal()->FireEventModel(eventId, pEventArgs);
            }

            HRESULT SetBinding(_In_ UINT32 propertyId, _In_ IBindingBase *pPropertyBinding)
            {
                return this->GetClassInterfaceInternal()->SetBinding(propertyId, pPropertyBinding);
            }

            HRESULT SetExpressionBinding(
                _In_ UINT32 propertyId,
                _In_ LPCSTR_t pExpressionBinding)
            {
                return this->GetClassInterfaceInternal()->SetExpressionBinding(
                    propertyId,
                    foundation::HStringRef(pExpressionBinding));
            }

            HRESULT SetBinding(
                _In_ UINT32 propertyId,
                _In_ LPCSTR_t pBindingPath)
            {
                foundation::ComPtr<IPropertyBinding> spPropertyBinding;
                IFR_ASSERT(CreateBinding(
                    this->template GetClassInterfaceInternalAs<IObservableObject>(),
                    pBindingPath,
                    PropertyBindingOptions::UseSourceWeakReference,
                    spPropertyBinding.GetAddressOf()));

                return this->SetBinding(propertyId, spPropertyBinding);
            }

            HRESULT ClearBinding(_In_ UINT32 propertyId)
            {
                return this->GetClassInterfaceInternal()->ClearBinding(propertyId);
            }

            HRESULT GetPropertyRuntimeClassOptions(UINT32 propertyId, _Out_ PropertyRuntimeClassOptions* pRuntimeClassOptions)
            {
                return this->GetClassInterfaceInternal()->GetPropertyRuntimeClassOptions(propertyId, pRuntimeClassOptions);
            }

            HRESULT SetPropertyRuntimeClassOptions(UINT32 propertyId, _In_ PropertyRuntimeClassOptions runtimeClassOptions)
            {
                return this->GetClassInterfaceInternal()->SetPropertyRuntimeClassOptions(propertyId, runtimeClassOptions);
            }

            template <class TContainer>
            HRESULT SetPropertiesInternal(
                const TContainer &propertyValues)
            {
                this->BeginModelChanges(library::BatchModelChangesOptions::None);

                for (typename TContainer::const_iterator iter =
                    propertyValues.begin();
                    iter != propertyValues.end();
                    ++iter)
                {
                    SetPropertyInternal((*iter).first, (*iter).second);
                }
                this->EndModelChanges();
                return S_OK;
            }

            HRESULT SetPropertiesInternal(
                _In_ foundation::IDictionary *pDictionary,
                _In_ UINT32 propertyIdStart,
                ...)
            {
                va_list marker;
                va_start(marker, propertyIdStart);     /* Initialize variable arguments. */

                UINT32 propertyId = propertyIdStart;

                foundation::ComPtr<IObservableObjectInfo> spPropertyModelInfo;
                IFR_ASSERT(foundation::GetObjectTypeInfo(
                    this->template GetClassInterfaceInternalAs<IObservableObject>(),
                    spPropertyModelInfo.GetAddressOf()));

                std::vector<_PropertyIdPair_Type> propertyValues;
                while (propertyId != (UINT32)-1)
                {
                    foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
                    IFR_ASSERT(spPropertyModelInfo->GetPropertyById(propertyId, spPropertyInfo.GetAddressOf()));

                    foundation::HStringPtr name;
                    spPropertyInfo->GetName(name.GetAddressOf());

                    foundation::string_t key = foundation::library::string_util::to_lower_case_convention_t(foundation::get_raw_buffer(name));
                    foundation::InspectablePtr spValue;
                    IFR_ASSERT(pDictionary->Lookup(foundation::HStringRef(key.c_str()), spValue.ReleaseAndGetAddressOf()));

                    propertyValues.push_back(std::make_pair(propertyId, spValue));
                    // go to next property id
                    propertyId = va_arg(marker, UINT32);
                }
                va_end(marker);
                return SetPropertiesInternal(propertyValues);
            }

            HRESULT UpdatePersistantProperties(
                UINT32 size_properties,
                _In_ UINT32 *pProperties,
                _In_ foundation::IInspectable *pKeyValue)
            {
                return foundation::library::persistant_storage_service_util::UpdateProperties(
                    this->template GetClassInterfaceInternalAs<IObservableObject>(),
                    size_properties,
                    pProperties,
                    pKeyValue
                    );
            }

            template <class TKeyValue>
            inline HRESULT UpdatePersistantProperty(
                UINT32 propertyId,
                const TKeyValue& key_value)
            {
                return UpdatePersistantProperties(1, &propertyId, foundation::CreateValue(key_value));
            }

            HRESULT LoadPersistantProperties(
                UINT32 size_properties,
                _In_opt_ UINT32 *pProperties,
                _In_ foundation::IInspectable *pKeyValue)
            {
                foundation::ComPtr<foundation::library::ISetPropertyCallback> callback_ptr;
                IFR_ASSERT(this->CreatePropertySetCallback(callback_ptr.GetAddressOf()));
                return foundation::library::persistant_storage_service_util::LoadProperties(
                    this->template GetClassInterfaceInternalAs<IObservableObject>(),
                    callback_ptr,
                    size_properties,
                    pProperties,
                    pKeyValue
                    );
            }

            template <class TKeyValue>
            inline HRESULT LoadPersistantProperty(
                UINT32 propertyId,
                const TKeyValue& key_value)
            {
                return LoadPersistantProperties(1, &propertyId, foundation::CreateValue(key_value));
            }

            HRESULT UpdateAllPersistantProperties(_In_ foundation::IInspectable *pKeyValue)
            {
                return foundation::library::persistant_storage_service_util::UpdateAllProperties(
                    this->template GetClassInterfaceInternalAs<IObservableObject>(),
                    pKeyValue
                    );
            }

            template <class TKeyValue>
            HRESULT UpdateAllPersistantProperties_t(const TKeyValue& key_value)
            {
                return UpdateAllPersistantProperties(foundation::CreateValue(key_value));
            }

            HRESULT LoadAllPersistantProperties(_In_  foundation::IInspectable *pKeyValue)
            {
                foundation::ComPtr<foundation::library::ISetPropertyCallback> callback_ptr;
                IFR_ASSERT(this->CreatePropertySetCallback(callback_ptr.GetAddressOf()));

                return foundation::library::persistant_storage_service_util::LoadAllProperties(
                    this->template GetClassInterfaceInternalAs<IObservableObject>(),
                    callback_ptr,
                    pKeyValue
                    );
            }

            template <class TKeyValue>
            HRESULT LoadAllPersistantProperties_t(const TKeyValue& key_value)
            {
                return LoadAllPersistantProperties(foundation::CreateValue(key_value));
            }

            template <class TKeyValue>
            HRESULT LoadPersistantProperties(
                const TKeyValue& key_value,
                UINT32 propertyIdStart,
                ...)
            {
                va_list marker;
                va_start(marker, propertyIdStart);     /* Initialize variable arguments. */
                std::vector<UINT32> properties;
                CreateProperties(properties, propertyIdStart, marker);
                return LoadPersistantProperties(
                    (UINT32)properties.size(),
                    &properties.front(),
                    foundation::CreateValue(key_value));
            }

            template <class TKeyValue>
            HRESULT UpdatePersistantProperties(
                const TKeyValue& key_value,
                UINT32 propertyIdStart,
                ...)
            {
                va_list marker;
                va_start(marker, propertyIdStart);     /* Initialize variable arguments. */
                std::vector<UINT32> properties;
                CreateProperties(properties, propertyIdStart, marker);
                return UpdatePersistantProperties(
                    (UINT32)properties.size(),
                    &properties.front(),
                    foundation::CreateValue(key_value));
            }
        protected:
            template <class _Func>
            HRESULT GetPropertyInternal(UINT32 propertyId, const _Func& func)
            {
                bool use_working_dispatcher = (this->GetFoundationOptions() &
                    (int)ObservableObjectOptions::UseGetPropertyDispatcher) != 0;

                if (!use_working_dispatcher)
                {
                    // try per property option
                    PropertyRuntimeClassOptions options;
                    IFR_ASSERT(GetPropertyRuntimeClassOptions(propertyId, &options));
                    use_working_dispatcher =
                        ((int)options & (int)PropertyRuntimeClassOptions::UseWorkingDispatcherOnGetProperty) != 0;
                }

                if (use_working_dispatcher)
                {
                    return this->RunWorkingDispatcherSync(func);
                }
                return func();
            }

            template <class _Func, class IAsyncOperation_type>
            HRESULT InvokeAsyncInternal(
                const _Func& func,
                IAsyncOperation_type **ppAsyncOperation)
            {
                // create async operation class 
                foundation::AsyncOperationClassPtr async_operation_class;
                IFR_ASSERT(foundation::library::CreateAsyncOperationClass(
                    IAsyncOperation_type::GetIID(),
                    foundation::AsyncOperationOptions::None,
                    async_operation_class.GetAddressOf()));
                // pass our event dispatcher
                async_operation_class.SetEventDispatcher(this->GetEventDispatcher());
                // return back to caller
                IFR_ASSERT(async_operation_class.QueryInterface(ppAsyncOperation));

                // this ptr to be passed into async lambda
                foundation::InspectablePtr this_ptr(this->GetClassInterfaceInternal());

                // dispatch into our working dispatcher
                return this->RunWorkingDispatcherAsync(
                    [=]() {
                    // keep 'this' alive during async execution
                    foundation::InspectablePtr this_async_ptr(this_ptr);

                    HRESULT hr = func(async_operation_class);
                    // Note: the callback could have return a failure HRESULT but miss
                    // failing the async operation. We will ensure to fail the operation to 
                    // unblock the calling code
                    if (FAILED(hr))
                    {
                        async_operation_class.SetCompleted(hr, nullptr);
                    }
                    return hr;
                }
                );
            }
            HRESULT CreatePropertySetCallback(
                _COM_Outptr_ foundation::library::ISetPropertyCallback **ppCallabck)
            {
                class _Set_property_callback_impl :
                    public foundation::ctl::ComBase,
                    public foundation::library::ISetPropertyCallback
                {
                public:
                    HRESULT _Initialize(IObservableObjectClass *pObservableObjectClass)
                    {
                        foundation_assert(pObservableObjectClass != nullptr);
                        this->_pObservableObjectClass = pObservableObjectClass;
                        return S_OK;
                    }
                protected:
                    STDMETHOD(Invoke)(_In_ UINT32 propertyId, _In_opt_ foundation::IInspectable *pValue)
                    {
                        return this->_pObservableObjectClass->SetPropertyInternal(propertyId, pValue, true);
                    }
                private:
                    IObservableObjectClass *_pObservableObjectClass;
                };
                IFR_ASSERT(foundation::ctl::CreateInstanceWithInitialize<_Set_property_callback_impl>(
                    ppCallabck,
                    this->GetClassInterfaceInternal()));
                return S_OK;
            }
        };
    }
}
