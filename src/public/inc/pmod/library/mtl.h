/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:mtl.h
****/
#pragma once

#include <foundation/macros.h>
#include <foundation/library/class_base.h>

// Class headers
#include "interfaces/observable_object_class.h"
#include "interfaces/command_class.h"
#include "interfaces/observable_collection_class.h"
#include "interfaces/property_binding_class.h"
#include "interfaces/multi_source_binding_class.h"

#include "factory_util.h"

#include <foundation/library/async_operation_class.h>
#include <foundation/async_operation_ptr.h>
#include <foundation/library/async_operation_class_wrapper.h>
#include <pmod/observable_object_ptr.h>
#include <pmod/command_ptr.h>
#include <pmod/observable_collection_ptr.h>
#include "observable_object_class_wrapper.h"
#include "command_class_wrapper.h"
#include "observable_collection_class_wrapper.h"

namespace pmod {
    namespace library {



        template <class TDelegate, class TEventHandler>
        class _DelegateFoundationBase :
            public TDelegate
        {
        public:
            STDMETHOD(OnRuntimeInitialize)()
            {
                return S_OK;
            }

            STDMETHOD(OnAddEventHandler)(_In_ TEventHandler *pEventHandler)
            {
                UNREFERENCED_PARAMETER(pEventHandler);
                return S_OK;
            }

            STDMETHOD(OnRemoveEventHandler)(_In_ TEventHandler *pEventHandler)
            {
                UNREFERENCED_PARAMETER(pEventHandler);
                return S_OK;
            }

            STDMETHOD(OnStartObserve)()
            {
                return S_OK;
            }

            STDMETHOD(OnStopObserve)()
            {
                return S_OK;
            }
        };

        class _AsyncOperationDelegate :
            public _DelegateFoundationBase<foundation::library::IAsyncOperationDelegate, foundation::ICompletedEventHandler>
        {
        protected:
            STDMETHOD(OnCancel)()
            {
                return S_FALSE;
            }
        };

        template <class TDelegate, class TEventHandler>
        class _SourceBaseModelDelegateBase :
            public _DelegateFoundationBase<TDelegate, TEventHandler>
        {
        public:
            STDMETHOD(OnSourceAttached)()
            {
                return S_OK;
            }
            STDMETHOD(OnSourceDetached)()
            {
                return S_OK;
            }
        };

        template <class TDelegate, class TEventHandler, class TSourceModel>
        class _SingleSourceModelDelegateBase :
            public _SourceBaseModelDelegateBase<TDelegate, TEventHandler>
        {
        public:
            STDMETHOD(OnSourceChanged)(_In_ TSourceModel *pSource)
            {
                return S_OK;
            }
        };

        class _ObservableObjectDelegate :
            public _DelegateFoundationBase<IObservableObjectDelegate, IPropertyChangedEventHandler>
        {
        protected:
            STDMETHOD(OnCreatePropertiesContainer)(
                _In_ IObservableObjectInfo* pPropertyModelInfo,
                _In_ UINT32 size,
                foundation::library::IPropertiesContainer **ppValue)
            {
                UNREFERENCED_PARAMETER(pPropertyModelInfo);
                UNREFERENCED_PARAMETER(size);
                *ppValue = nullptr;
                return S_FALSE;
            }
            STDMETHOD(OnGetProperty)(
                _In_ UINT32 propertyId,
                _Inout_ foundation::IInspectable **ppValue)
            {
                UNREFERENCED_PARAMETER(propertyId);
                UNREFERENCED_PARAMETER(ppValue);
                return S_FALSE;
            }
            STDMETHOD(OnSetProperty)(
                _In_ UINT32 propertyId,
                _In_ bool isPublic,
                _In_opt_ foundation::IInspectable *oldValue,
                _In_opt_ foundation::IInspectable * newValue)
            {
                UNREFERENCED_PARAMETER(propertyId);
                UNREFERENCED_PARAMETER(isPublic);
                UNREFERENCED_PARAMETER(oldValue);
                UNREFERENCED_PARAMETER(newValue);
                return S_OK;
            }
            STDMETHOD(OnInvokeMethod) (
                _In_ foundation::IMethodInfo *pMethodInfo,
                _In_ UINT32 methodId,
                _In_ UINT32 size,
                _In_opt_ foundation::IInspectable **parameters,
                foundation::IInspectable **ppValue)
            {
                IFCPTR_ASSERT(ppValue);

                *ppValue = nullptr;
                UNREFERENCED_PARAMETER(pMethodInfo);
                UNREFERENCED_PARAMETER(methodId);
                UNREFERENCED_PARAMETER(size);
                UNREFERENCED_PARAMETER(parameters);
                return E_NOTIMPL;
            }
        };

        class _ObservableCollectionDelegate :
            public _DelegateFoundationBase<IObservableCollectionDelegate, INotifyCollectionChangedEventHandler>
        {
            STDMETHOD(OnCreateItemsContainer)(IItemsContainer **ppValue)
            {
                IFCPTR_ASSERT(ppValue);
                *ppValue = nullptr;
                // do not create any custom items container
                return S_OK;
            }

            STDMETHOD(OnCollectionChanged)(_In_ INotifyCollectionChangedEventArgs *pEventArgs)
            {
                foundation_assert(pEventArgs);
                IFCPTR_ASSERT(pEventArgs);
                return S_OK;
            }
        };

        class _CommandDelegate :
            public _DelegateFoundationBase<ICommandDelegate, ICanExecuteChangedEventHandler>
        {
        protected:
            STDMETHOD(OnCanExecute)(_In_opt_ foundation::IInspectable *parameter, _Out_ bool *pValue)
            {
                UNREFERENCED_PARAMETER(parameter);
                *pValue = true;
                return S_FALSE;
            }

            STDMETHOD(OnExecute)(_In_opt_ foundation::IInspectable *parameter, foundation::IInspectable **ppResult)
            {
                UNREFERENCED_PARAMETER(parameter);
                UNREFERENCED_PARAMETER(ppResult);
                return S_FALSE;
            }
        };

        class _PropertyBindingDelegate :
            public _SingleSourceModelDelegateBase<IPropertyBindingDelegate, IBindingValueChangedEventHandler, IObservableObject>
        {
            STDMETHOD(OnValueChanged)(_In_opt_ foundation::IInspectable *value, _In_ bool isValueSet)
            {
                return S_OK;
            }
        };

        class _MultiSourcePropertyBindingDelegate :
            public _SourceBaseModelDelegateBase<IMultiSourceBindingDelegate, IBindingValueChangedEventHandler>
        {
            STDMETHOD(OnBindingValuesChanged)(
                _In_ UINT32 size,
                _In_ foundation::IInspectable **pBindingValues,
                foundation::IInspectable **ppValue)
            {
                return S_FALSE;
            }
        };

        class _AsyncOperationClassBase :
            public foundation::library::_FoundationClassBase
            <
            foundation::IAsyncOperation,
            foundation::library::IAsyncOperationClass,
            _AsyncOperationDelegate,
            foundation::_AsyncOperationPtrWrapper<_AsyncOperationClassBase,foundation::IAsyncOperation>,
            foundation::_AsyncOperationClassWrapper<_AsyncOperationClassBase,foundation::IInspectable *>
            >
        {
        public:
            HRESULT _InitializeModel()
            {
                return S_OK;
            }

            HRESULT _CreateInnerInstance(
                const IID& iidType,
                _In_opt_ foundation::IInspectable *pOuter,
                foundation::AsyncOperationOptions options)
            {
                foundation::library::async_operation_create_parameters params;
                params.m_iid_type = iidType;
                params.m_p_delegate = this;
                params.m_options = foundation::library::ToDelegateWeakRefOptions(options);
                IFR_ASSERT(foundation::library::GetAsyncOperationFactory()->CreateAsyncOperation(
                    &params,
                    pOuter,
                    this->GetInnerAddressOf()));
                IFR_ASSERT(this->QueryHostInstance(this->GetInnerRaw()));
                return S_OK;
            }
        };

        class _ObservableObjectClassBase:
            public foundation::library::_FoundationClassBase
            <
                IObservableObject,
                IObservableObjectClass,
                _ObservableObjectDelegate,
                _Observable_object_ptr_wrapper<_ObservableObjectClassBase>,
                _ObservableObjectClassWrapper<_ObservableObjectClassBase>
            >,
            public _ObservableObjectWrapperType<_ObservableObjectClassBase>
        {
        public:
            HRESULT _InitializeModel()
            {
                return _InitializeProperties();
            }

            HRESULT _CreateInnerInstance(
                const IID& iidType,
                _In_opt_ foundation::IInspectable *pOuter,
                ObservableObjectOptions options)
            {
                ObservableObjectCreateParameters params;
                params.m_iid_type = iidType;
                params.m_p_delegate = this;
                params.m_options = foundation::library::ToDelegateWeakRefOptions(options);
                IFR_ASSERT(GetObservableObjectClassFactory()->CreateObservableObjectInstance(
                    &params, 
                    pOuter, 
                    this->GetInnerAddressOf()));
                IFR_ASSERT(this->QueryHostInstance(this->GetInnerRaw()));
                return S_OK;
            }
        protected:
            virtual HRESULT _InitializeProperties()
            {
                return S_OK;
            }
        };
        
        class _CommandClassBase :
            public foundation::library::_FoundationClassBase
            <
                ICommand,
                ICommandClass,
                _CommandDelegate,
                _CommandPtrWrapper<_CommandClassBase, ICommand>,
                _CommandClassWrapper<_CommandClassBase>
            >
        {
        public:
            HRESULT _CreateInnerInstance(
                const IID& iidType,
                _In_opt_ foundation::IInspectable *pOuter,
                CommandOptions options)
            {
                CommandCreateParameters params;
                params.m_iid_type = iidType;
                params.m_p_delegate = this;
                params.m_options = foundation::library::ToDelegateWeakRefOptions(options);
                IFR_ASSERT(GetCommandClassFactory()->CreateCommandInstance(
                    &params,
                    pOuter,
                    this->GetInnerAddressOf()));
                IFR_ASSERT(this->QueryHostInstance(this->GetInnerRaw()));
                return S_OK;
            }
        };


        class _ObservableCollectionClassBase :
            public foundation::library::_FoundationClassBase
            <
                IObservableCollection,
                IObservableCollectionClass,
                _ObservableCollectionDelegate,
                _ObservableCollectionPtrWrapper<_ObservableCollectionClassBase, IObservableCollection, foundation::_RefCountedValueAlloc<foundation::IInspectable *>>,
                _ObservableCollectionClassWrapper<_ObservableCollectionClassBase>
            >
        {
        public:
            HRESULT _CreateInnerInstance(
                const IID& iidType,
                _In_opt_ foundation::IInspectable *pOuter,
                ObservableCollectionOptions options)
            {
                ObservableCollectionCreateParameters params;
                params.m_iid_type = iidType;
                params.m_p_delegate = this;
                params.m_options = foundation::library::ToDelegateWeakRefOptions(options);
                IFR_ASSERT(GetObservableCollectionClassFactory()->CreateObservableCollectionInstance(
                    &params,
                    pOuter,
                    this->GetInnerAddressOf()));
                IFR_ASSERT(this->QueryHostInstance(this->GetInnerRaw()));
                return S_OK;
            }
        };
        
        template <class TBASE, class TSourceModel, class TSourceModelBase>
        class _SourceModelBase :
            public TBASE
        {

        protected:
            _SourceModelBase() :
                _pSourceModel(nullptr)
            {
            }

        public:
            HRESULT SetForceAttached(bool newValue)
            {
                foundation_assert(_pSourceModel);
                return _pSourceModel->SetForceAttached(newValue);
            }

            HRESULT set_attach_enabled(bool newValue)
            {
                foundation_assert(_pSourceModel);
                return _pSourceModel->SetAttachEnabled(newValue);
            }

            TSourceModelBase *get_source_model_base()
            {
                return _pSourceModel;
            }
        protected:
            HRESULT QueryHostInstance(foundation::IUnknown *pInnerUnk)
            {
                IFR_ASSERT(TBASE::QueryHostInstance(pInnerUnk));
                IFR_ASSERT(foundation::QueryWeakReference(pInnerUnk, &_pSourceModel));
                return S_OK;
            }
        protected:
            TSourceModelBase *_pSourceModel;
        };

        template <class TBASE, class TSourceModel>
        class _SingleSourceModelBase :
            public _SourceModelBase<TBASE, TSourceModel, ISourceModel>
        {

        protected:
            _SingleSourceModelBase() :
                _pSource(nullptr)
            {
            }
        public:
            TSourceModel *GetSource()
            {
                return _pSource;
            }

            HRESULT SetSource(TSourceModel *pSource)
            {
                foundation_assert(this->_pSource);
                return this->_pSource->SetSource(pSource);
            }

            HRESULT Disconnect()
            {
                return this->SetSource(nullptr);
            }

        public:
            // ISourceModelDelegateBase Implementation
            STDMETHOD(OnSourceChanged)(_In_opt_ TSourceModel *pSource)
            {
                _pSource = pSource;
                return S_OK;
            }
        protected:
            TSourceModel *_pSource; // TODO: check if a weak reference is appropiate
        };

        template <class TBASE, class TSourceModel>
        class _MultiSourceModelBase :
            public _SourceModelBase<TBASE, TSourceModel, IMultiSourceModel>
        {

        protected:
            _MultiSourceModelBase()
            {
            }
        public:
            HRESULT AddSource(TSourceModel *pSource)
            {
                foundation_assert(this->_pSourceModel);
                return this->_pSourceModel->AddSource(pSource);
            }
            HRESULT RemoveSource(TSourceModel *pSource)
            {
                foundation_assert(this->_pSourceModel);
                return this->_pSourceModel->RemoveSource(pSource);
            }
            HRESULT RemoveAllSource()
            {
                foundation_assert(this->_pSourceModel);
                return this->_pSourceModel->RemoveAllSources();
            }
            HRESULT Disconnect()
            {
                return E_NOTIMPL;
            }
        };

        class _MultiSourceBindingClassBase :
            public _MultiSourceModelBase
            <
                foundation::library::_FoundationClassBase
                <
                    IBindingBase,
                    IMultiSourceBindingClass,
                    _MultiSourcePropertyBindingDelegate,
                    foundation::_PtrWrapperBase<_MultiSourceBindingClassBase, IBindingBase>,
                    foundation::_FoundationClassWrapper<_MultiSourceBindingClassBase, IMultiSourceBindingClass>
                >,
                foundation::IObject
            >
        {
        public:
            HRESULT GetValue(foundation::IInspectable **pValue)
            {
                foundation_assert(_interface != nullptr);
                IFCEXPECT_ASSERT(_interface != nullptr);

                return _interface->GetValue(pValue);
            }

            HRESULT AddSource(_In_ IBindingBase *pBindingBase)
            {
                IFCPTR_ASSERT(pBindingBase);

                foundation::ComPtr<foundation::IObject> spObject;
                IFR_ASSERT(foundation::QueryInterface(pBindingBase, spObject.GetAddressOf()));
                IFR_ASSERT(_pSourceModel->AddSource(spObject));
                return S_OK;
            }

            HRESULT RemoveSource(_In_ IBindingBase *pBindingBase)
            {
                IFCPTR_ASSERT(pBindingBase);

                foundation::ComPtr<foundation::IObject> spObject;
                IFR_ASSERT(foundation::QueryInterface(pBindingBase, spObject.GetAddressOf()));
                IFR_ASSERT(_pSourceModel->RemoveSource(spObject));
                return S_OK;
            }

            HRESULT _CreateInnerInstance(
                const IID& iidType,
                _In_opt_ foundation::IInspectable *pOuter,
                MultiSourceBindingOptions options)
            {
                UNREFERENCED_PARAMETER(iidType);

                MultiSourcePropertyBindingCreateParameters params;
                params.m_p_delegate = this;
                params.m_options = foundation::library::ToDelegateWeakRefOptions(options);
                IFR_ASSERT(GetMultiSourceBindingClassFactory()->CreateMultiSourceBindingInstance(&params, pOuter, this->GetInnerAddressOf()));
                IFR_ASSERT(QueryHostInstance(this->GetInnerRaw()));
                return S_OK;
            }
        };

        typedef foundation::library::_AggregateObjectClassBase<_AsyncOperationClassBase> _AsyncOperationClass;

        typedef foundation::library::_AggregateObjectClassBase<_ObservableObjectClassBase> _ObservableObjectClass;
        typedef foundation::library::_AggregateObjectClassBase<_CommandClassBase> _CommandClass;
        typedef foundation::library::_AggregateObjectClassBase<_ObservableCollectionClassBase> _ObservableCollectionClass;

        typedef foundation::library::_AggregateClassBase<_MultiSourceBindingClassBase> _MultiSourceBindingClass;

        template <class T, class TInterface = IObservableObject>
        class _WeakRefCommandClass : public _CommandClass
        {
        public:
            HRESULT _InitializeModel(_In_ T *pT)
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

        template <class T>
        class _ChildCommandClass : public _CommandClass
        {
        public:
            HRESULT _InitializeModel(_In_ T *pT)
            {
                this->_pParentTarget = pT;
                return S_OK;
            }
        protected:
            _ChildCommandClass() :
                _pParentTarget(nullptr)
            {}

            T *get_target()
            {
                foundation::ComPtr<foundation::IObject> spParent;
                IGNOREHR(GetParent(spParent.GetAddressOf()));
                if (spParent == nullptr)
                {
                    // our parent is gone so we will invalidate our weak reference
                    _pParentTarget = nullptr;
                }
                return _pParentTarget;
            }
        private:
            T *_pParentTarget;
        };

    }
}
