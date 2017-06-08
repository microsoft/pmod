/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:factory_util.h
****/
#pragma once

#include <foundation/assert.h>

#include "model_library.h"

#include <foundation/pv_util.h>
#include <foundation/delegate_library.h>
#include <foundation/activation_factory.h>
#include <foundation/library/library_util.h>


namespace pmod
{
    namespace library
    {
        inline static IObservableObjectClassFactory *GetObservableObjectClassFactory()
        {
            return GetModelLibraryFactory<IObservableObjectClassFactory>(U("pmod.ObservableObjectClass"));
        }

        inline static IObservableCollectionClassFactory *GetObservableCollectionClassFactory()
        {
            return GetModelLibraryFactory<IObservableCollectionClassFactory>(U("pmod.ObservableCollectionClass"));
        }

        inline static ICommandClassFactory *GetCommandClassFactory()
        {
            return GetModelLibraryFactory<ICommandClassFactory>(U("pmod.CommandClass"));
        }

        inline IViewModelClassFactory *GetViewModelCassFactory()
        {
            return GetModelLibraryFactory<IViewModelClassFactory>(U("pmod.ViewModelClassFactory"));
        }

        inline IPropertyBindingClassFactory *GetPropertyBindingClassFactory()
        {
            return GetModelLibraryFactory<IPropertyBindingClassFactory>(U("pmod.PropertyBindingClass"));
        }

        inline IExpressionBindingClassFactory *GetExpressionBindingClassFactory()
        {
            return GetModelLibraryFactory<IExpressionBindingClassFactory>(U("pmod.ExpressionBindingClass"));
        }

        inline IMultiSourceBindingClassFactory *GetMultiSourceBindingClassFactory()
        {
            return GetModelLibraryFactory<IMultiSourceBindingClassFactory>(U("pmod.MultiSourceBindingClass"));
        }

        inline IViewModelFactoryClassFactory *GetViewModelFactoryClassFactory()
        {
            return GetModelLibraryFactory<IViewModelFactoryClassFactory>(U("pmod.ViewModelFactoryClass"));
        }

        inline ICollectionViewModelClassFactory *GetCollectionViewModelClassFactory()
        {
            return GetModelLibraryFactory<ICollectionViewModelClassFactory>(U("pmod.CollectionViewModelClass"));
        }

        inline ICommandViewModelClassFactory *GetCommandViewModelClassFactory()
        {
            return GetModelLibraryFactory<ICommandViewModelClassFactory>(U("pmod.CommandViewModelClass"));
        }

        inline IMultiSourceCollectionViewModelClassFactory *GetMultiSourceCollectionViewModelClassFactory()
        {
            return GetModelLibraryFactory<IMultiSourceCollectionViewModelClassFactory>(U("pmod.MultiSourceCollectionViewModelClass"));
        }

        inline IMultiSourceViewModelClassFactory *GetMultiSourceViewModelClassFactory()
        {
            return GetModelLibraryFactory<IMultiSourceViewModelClassFactory>(U("pmod.MultiSourceViewModelClass"));
        }

        inline IViewModelClassFactory *GetViewModelClassFactory()
        {
            return GetModelLibraryFactory<IViewModelClassFactory>(U("pmod.ViewModelClass"));
        }

        template
        <
        typename TModelParam,
        typename TModelOptions,
        typename T,
        typename IFactory
        >
        HRESULT CreateClass(
            _In_ const IID& iidType,
            _In_  TModelOptions options,
            IFactory *(*pGetFactory)(),
            HRESULT (STDMETHODCALLTYPE IFactory::*pCreateClass)(const TModelParam*, foundation::IInspectable*, foundation::IInspectable**),
            _COM_Outptr_ T** ppNewInstance)
        {
            IFCPTR_ASSERT(ppNewInstance);
            *ppNewInstance = nullptr;
            
            TModelParam modelParam;
            modelParam.m_iid_type = iidType;
            modelParam.m_options = options;
            
            IFactory *pFactory = (*pGetFactory)();
            IFCPTR_ASSERT(pFactory);

            foundation::InspectablePtr spNewInstance;
            IFR_ASSERT((pFactory->*pCreateClass)(
                                                                       &modelParam,
                                                                       nullptr,
                                                                       spNewInstance.GetAddressOf()));
            IFR_ASSERT(foundation::QueryInterface(spNewInstance,ppNewInstance))
            return S_OK;
        }

        template
            <
            typename TModelParam,
            typename T,
            typename IFactory
            >
        HRESULT CreateClass(
            _In_  const TModelParam& params,
            IFactory *(*pGetFactory)(),
            HRESULT(STDMETHODCALLTYPE IFactory::*pCreateClass)(const TModelParam*, foundation::IInspectable*, foundation::IInspectable**),
            _COM_Outptr_ T** ppNewInstance)
        {
            IFCPTR_ASSERT(ppNewInstance);
            *ppNewInstance = nullptr;

            IFactory *pFactory = (*pGetFactory)();
            IFCPTR_ASSERT(pFactory);

            foundation::InspectablePtr spNewInstance;
            IFR_ASSERT((pFactory->*pCreateClass)(
                &params,
                nullptr,
                spNewInstance.GetAddressOf()));
            IFR_ASSERT(foundation::QueryInterface(spNewInstance, ppNewInstance))
            return S_OK;
        }
                
        template
            <
            typename T,
            typename TViewModelParam,
            typename TModelOptions,
            typename TViewModelOptions,
            typename TSource,
            typename IFactory
            >
        HRESULT CreateSingleSourceViewModel(
            _In_ TSource                    *pSource,
            _In_ const IID&                 iidType,
            _In_  TModelOptions             modelOptions,
            _In_  TViewModelOptions         viewModelOptions,
            IFactory *(*pGetFactory)(),
            HRESULT  (STDMETHODCALLTYPE IFactory::*pCreateViewModelClass)(const TViewModelParam*, foundation::IInspectable*, foundation::IInspectable**),
            _COM_Outptr_ T **ppNewInstance)
        {
            IFCPTR_ASSERT(ppNewInstance);
            *ppNewInstance = nullptr;
            
            TViewModelParam         viewModelParam;
            viewModelParam.m_p_source = pSource;
            viewModelParam.m_iid_type = iidType;
            viewModelParam.m_options = modelOptions;
            viewModelParam.m_view_model_options = viewModelOptions;
            
            foundation::InspectablePtr spNewInstance;

            IFactory *pFactory = (*pGetFactory)();
            IFCPTR_ASSERT(pFactory);

            IFR_ASSERT((pFactory->*pCreateViewModelClass)(
                &viewModelParam,
                nullptr,
                spNewInstance.GetAddressOf()));
            IFR_ASSERT(foundation::QueryInterface(spNewInstance,ppNewInstance))
            return S_OK;
        }
 

        template <class T>
        static HRESULT CreateObservableObject(
            _In_ const IID&                   iidType,
            _In_  ObservableObjectOptions        options,
            _COM_Outptr_ T   **ppNewInstance)
        {
            return CreateClass<ObservableObjectCreateParameters>(
                iidType,
                options,
                &GetObservableObjectClassFactory,
                &IObservableObjectClassFactory::CreateObservableObjectInstance,
                ppNewInstance);
        }

        inline static HRESULT CreateObservableObjectDictionary(foundation::IDictionary **ppCoreDictionary)
        {
            return CreateObservableObject(foundation_GUID_NULL, ObservableObjectOptions::None, ppCoreDictionary);
        }

        template <class T>
        static HRESULT CreateObservableCollection(
            _In_ const IID&                 iidType,
            _In_  ObservableCollectionOptions    options,
            _COM_Outptr_ T **ppNewInstance)
        {
            IFCPTR_ASSERT(ppNewInstance);
            *ppNewInstance = nullptr;

            ObservableCollectionCreateParameters params;
            params.m_iid_type = iidType;
            params.m_options = options;

            return CreateClass<ObservableCollectionCreateParameters>(
                iidType,
                options,
                &GetObservableCollectionClassFactory,
                &IObservableCollectionClassFactory::CreateObservableCollectionInstance,
                ppNewInstance);
        }

        template <class T>
        static HRESULT CreateObservableCollection(
            _In_  const ObservableCollectionCreateParameters& params,
            _COM_Outptr_ T** ppNewInstance)
        {
            return CreateClass<ObservableCollectionCreateParameters>(
                params,
                &GetObservableCollectionClassFactory,
                &IObservableCollectionClassFactory::CreateObservableCollectionInstance,
                ppNewInstance);
        }

        template <class T>
        static HRESULT CreateCommand(
            _In_ const IID&                 iidType,
            _In_  CommandOptions       options,
            _COM_Outptr_ T **ppNewInstance)
        {
            return CreateClass<CommandCreateParameters, CommandOptions>(
                iidType,
                options,
                &GetCommandClassFactory,
                &ICommandClassFactory::CreateCommandInstance,
                ppNewInstance);
        }

        template <class T>
        static HRESULT CreateBinding(
            _In_ IObservableObject                *pSource,
            _In_ foundation::IInspectable      *pPath,
            _In_ PropertyBindingOptions        options,
            _COM_Outptr_ T                     **ppNewInstance)
        {
            IFCPTR_ASSERT(ppNewInstance);
            *ppNewInstance = nullptr;

            PropertyBindingCreateParameters propertyBindingParam;
            propertyBindingParam.m_options = options;
            propertyBindingParam.m_p_path = pPath;
            propertyBindingParam.m_p_source = pSource;

            foundation::InspectablePtr spNewInstance;
            IFR_ASSERT(GetPropertyBindingClassFactory()->CreatePropertyBindingInstance(
                    &propertyBindingParam,
                    nullptr,
                    spNewInstance.GetAddressOf()));
            IFR_ASSERT(foundation::QueryInterface(spNewInstance, ppNewInstance))
            return S_OK;
        }

        template <class T>
        inline HRESULT CreateBinding(
            _In_ IObservableObject                 *pSource,
            _In_ LPCSTR_t                       pPropertyPath,
            _In_ PropertyBindingOptions         options,
            _COM_Outptr_ T     **ppNewInstance)
        {
            foundation::InspectablePtr path = foundation::pv_util::CreateValue(pPropertyPath);
            return CreateBinding(pSource,path,options,ppNewInstance);
        }


        static HRESULT CreateExpressionBinding(
                _In_ IObservableObject                *pSource,
                _In_ foundation::IExpression                   *pBindingExpression,
                _In_ PropertyBindingOptions        bindingOptions,
                IBindingBase                        **ppBindingBase)
        {
            IFR_ASSERT(GetExpressionBindingClassFactory()->CreateExpressionBindingInstance(
                    pSource,
                    pBindingExpression,
                    bindingOptions,
                    ppBindingBase));
            return S_OK;
        }

        inline HRESULT CreateExpressionBinding(
            _In_ IObservableObject                *pSource,
            _In_ LPCSTR_t bindingExpression,
            _In_ PropertyBindingOptions        bindingOptions,
            IBindingBase                   **ppBindingBase)
        {
            foundation::ComPtr<foundation::IExpression> spExpression;
            IFR_ASSERT(foundation::library::CreateExpression(bindingExpression, spExpression.GetAddressOf()));
            return CreateExpressionBinding(pSource, spExpression, bindingOptions, ppBindingBase);
        }

        template <class T,class TSoure>
        HRESULT CreateCollectionViewModel(
                _In_opt_ TSoure               *pSourceCollection,
                _In_ const IID&                     iidType,
                _In_  ObservableCollectionOptions        modelOptions,
                _In_  CollectionViewModelOptions    viewModelOptions,
                _COM_Outptr_ T           **ppNewInstance)
        {
            foundation::ComPtr<IObservableCollection> spSourceCollection;
            if (pSourceCollection != nullptr)
            {
                IFR_ASSERT(foundation::QueryInterface(pSourceCollection, spSourceCollection.GetAddressOf()));
            }
            return CreateSingleSourceViewModel
                <
                    T,
                    collection_viewmodel_create_parameters,
                    ObservableCollectionOptions,
                    CollectionViewModelOptions,
                    IObservableCollection
                >(
                    spSourceCollection,
                    iidType,
                    (ObservableCollectionOptions)((UINT32)modelOptions | (UINT32)ObservableCollectionOptions::IsReadOnly),
                    viewModelOptions,
                    &GetCollectionViewModelClassFactory,
                    &ICollectionViewModelClassFactory::CreateCollectionViewModelInstance,
                    ppNewInstance
                 );
        }

        inline HRESULT CreateViewModelFactory(
            const ObjectCreatorEntry *pObjectCreatorEntries,
            const SingleSourceCreatorEntry *pSingleSourceModelCreatorEntries,
            _COM_Outptr_ IViewModelFactory **ppViewModelFactory)
        {
            ViewModelFactoryCreateParameters viewModelFactoryParam;
            viewModelFactoryParam.m_p_object_creator_entries = pObjectCreatorEntries;
            viewModelFactoryParam.m_p_single_source_model_factory_entries = pSingleSourceModelCreatorEntries;

            foundation::InspectablePtr spInstance;
            IFR_ASSERT(GetViewModelFactoryClassFactory()->CreateViewModelFactoryInstance(
                &viewModelFactoryParam,
                nullptr,
                spInstance.GetAddressOf()));
            IFR_ASSERT(foundation::QueryInterface(spInstance,ppViewModelFactory));

            return S_OK;
        }
    }
}
