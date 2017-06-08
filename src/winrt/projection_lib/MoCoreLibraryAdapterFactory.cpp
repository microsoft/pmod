/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreLibraryAdapterFactory.cpp
****/

#pragma once
#include "pch.h"

#include "Microsoft.PropertyModel.Library.h"


// Headers for Mo Adapters
#include "MoObjectClassAdapter.h"
#include "MoObjectNodeClassAdapter.h"
#include "MoModelOverridesBaseAdapter.h"
#include "MoViewModelAdapter.h"
#include "MoCollectionViewModelAdapter.h"
#include "MoCommandViewModelAdapter.h"
#include "MoMultiSourceViewModelAdapter.h"
#include "MoMultiSourceCollectionViewModelAdapter.h"
#include "MoViewModelFactoryAdapter.h"

#include "MoObservableObjectClassAdapter.h"
#include "MoCommandClassAdapter.h"
#include "MoObservableCollectionClassAdapter.h"
#include "MoAsyncOperationClassAdapter.h"

EXTERN_C PMOD_API HRESULT  STDAPICALLTYPE RegisterMoAdapterFactory();

EXTERN_C __declspec(dllexport) HRESULT  STDAPICALLTYPE RegisterMoLibraryAdapterFactory();

#pragma warning( disable : 4503 )

template 
<
    class TAdapterClass
>
foundation::library::CreateAdapterInstanceCallback GetAdapterCallBack()
{
    return foundation::CreateAdapter<TAdapterClass>;
}

template 
<
    class TAdapterClass
>
HRESULT AddAddapter(
    foundation::library::IGenericAdapterFactory *pGenericAdapterFactory,
    REFIID iidCoreType,
    REFIID iidMoCoreType,
    REFIID iidCoreInnerType
    )
{
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        iidCoreType,
        iidMoCoreType,
        iidCoreInnerType,
        foundation::CreateAdapter<TAdapterClass>
        ));
    return S_OK;
}


#define ADD_MO_ADAPTER(TCoreInterface,TMoInterface,TCoreInnerInterface,TAdapterClass) \
   hr = AddAddapter<TAdapterClass> \
        (pGenericAdapterFactory,TCoreInterface::GetIID(),__uuidof(TMoInterface),TCoreInnerInterface::GetIID()); \
    IFHR(hr)


//////////////////////////////////// Core Model Support //////////////////////////////////////////
HRESULT RegisterMoCoreModelClassFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{

    IID iidObjectTypes[] = { 
        pmod::IObservableObject::GetIID(),
        pmod::IObservableCollection::GetIID(),
        pmod::ICommand::GetIID(),
        foundation::IAsyncOperation::GetIID(),
    };

    for (int idx = 0; idx < sizeof(iidObjectTypes) / sizeof(IID); ++idx)
    {
        IID iidObjectType = iidObjectTypes[idx];

        // Register IObjectClass Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidObjectType,
            __uuidof(IObjectClass),
            foundation::library::IFoundationClassBase::GetIID(),
            GetAdapterCallBack<CMoObjectClassAdapter>()
            ));

        // Register IObjectOverridesBase Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidObjectType,
            __uuidof(IObjectOverridesBase),
            iidObjectType,
            GetAdapterCallBack<CMoObjectOverridesBaseAdapter>()
            ));

    }

    IID iidCoreModelTypes[] = {
        pmod::IObservableObject::GetIID(),
        pmod::IObservableCollection::GetIID(),
        pmod::ICommand::GetIID(),
    };

    for (int idx = 0; idx < sizeof(iidCoreModelTypes) / sizeof(IID); ++idx)
    {
        IID iidCoreType = iidCoreModelTypes[idx];
        // Register IObjectNodeClass Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidCoreType,
            __uuidof(IObjectNodeClass),
            pmod::library::IObjectNodeClass::GetIID(),
            GetAdapterCallBack<CMoObjectNodeClassAdapter>()
            ));
    }

    return S_OK;
}

//////////////////////////////////// Property Model Class Support //////////////////////////////////////////
HRESULT RegisterMoPropertyModelClassFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IObservableObjectClass Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        IObservableObjectClass,
        pmod::library::IObservableObjectClass,
        CMoObservableObjectClassAdapter);

    // Register IPropertyModelProtected Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        IObservableObjectProtected,
        pmod::library::IObservableObjectClass,
        CMoObservableObjectProtectedAdapter);

    // Register IPropertyModelOverrides Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        IObservableObjectOverrides,
        pmod::library::IObservableObjectClass,
        CMoObservableObjectOverridesAdapter);

    return S_OK;
}

//////////////////////////////////// Property View Model Support //////////////////////////////////////////
HRESULT RegisterMoPropertyViewModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IPropertyViewModel Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        IViewModel,
        pmod::library::IViewModelClass,
        CMoPropertyViewModelAdapter);

    // Register IPropertyViewModelProtected Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        IViewModelProtected,
        pmod::library::IViewModelClass,
        CMoPropertyViewModelProtectedAdapter);

    // Register IPropertyViewModelOverrides Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        IViewModelOverrides,
        pmod::library::IViewModelClass,
        CMoPropertyViewModelOverridesAdapter);
    return S_OK;
}
//////////////////////////////////// MultiSource Property View Model Support //////////////////////////////////////////
HRESULT RegisterMoMultiSourcePropertyViewModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IMultiSourcePropertyViewModel Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        IMultiSourceViewModel,
        pmod::library::IObservableObjectClass,
        CMoMultiSourcePropertyViewModelAdapter);

    // Register IMultiSourcePropertyViewModelOverrides Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        IMultiSourceViewModelOverrides,
        pmod::library::IObservableObjectClass,
        CMoMultiSourcePropertyViewModelOverridesAdapter);

    return S_OK;
}

//////////////////////////////////// Collection Model Class Support //////////////////////////////////////////
HRESULT RegisterMoCollectionModelClassFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IObservableCollectionClass Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        IObservableCollectionClass,
        pmod::library::IObservableCollectionClass,
        CMoObservableCollectionClassAdapter);

    // Register ICollectionModelProtected Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        IObservableCollectionProtected,
        pmod::library::IObservableCollectionClass,
        CMoObservableCollectionProtectedAdapter);

    // Register ICollectionModelBaseOverrides Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        IObservableCollectionBaseOverrides,
        pmod::library::IObservableCollectionClass,
        CMoObservableCollectionBaseOverridesAdapter);

    // Register ICollectionModelOverrides Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        IObservableCollectionOverrides,
        pmod::library::IObservableCollectionClass,
        CMoCollectionModelOverridesAdapter);

    return S_OK;
}
//////////////////////////////////// Collection View Model Support //////////////////////////////////////////
HRESULT RegisterMoCollectionViewModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;
    // Register ICollectionViewModel Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        ICollectionViewModel,
        pmod::library::ICollectionViewModelClass,
        CMoCollectionViewModelAdapter);

    // Register ICollectionViewModelClass Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        ICollectionViewModelClass,
        pmod::library::ICollectionViewModelClass,
        CMoCollectionViewModelClassAdapter);

    // Register ICollectionViewModelProtected Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        ICollectionViewModelProtected,
        pmod::library::ICollectionViewModelClass,
        CMoCollectionViewModelProtectedAdapter);

    // Register ICollectionViewModelBaseOverrides Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        ICollectionViewModelBaseOverrides,
        pmod::library::ICollectionViewModelClass,
        CMoCollectionViewModelOverridesAdapter);

    return S_OK;
}
//////////////////////////////////// MultiSource Collection View Model Support //////////////////////////////////////////
HRESULT RegisterMoMultiSourceCollectionViewModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IMultiSourcePropertyViewModel Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        IMultiSourceCollectionViewModel,
        pmod::library::IObservableCollectionClass,
        CMoMultiSourceCollectionViewModelAdapter);

    // Register IMultiSourceCollectionViewModelOverrides Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        IMultiSourceCollectionViewModelOverrides,
        pmod::library::IObservableCollectionClass,
        CMoMultiSourceCollectionViewModelOverridesAdapter);

    return S_OK;
}

//////////////////////////////////// Command Model Class Support //////////////////////////////////////////
HRESULT RegisterMoCommandModelClassFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register ICommandClass Adapter
    ADD_MO_ADAPTER(
        pmod::ICommand,
        ICommandClass,
        pmod::library::ICommandClass,
        CMoCommandClassAdapter);

    // Register ICommandModelProtected Adapter
    ADD_MO_ADAPTER(
        pmod::ICommand,
        ICommandProtected,
        pmod::library::ICommandClass,
        CMoCommandModelProtectedAdapter);

    // Register ICommandClass Adapter
    ADD_MO_ADAPTER(
        pmod::ICommand,
        ICommandOverrides,
        pmod::library::ICommandClass,
        CMoCommandModelOverridesAdapter);

    return S_OK;
}
//////////////////////////////////// Command View Model Support //////////////////////////////////////////
HRESULT RegisterMoCommandViewModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;
    // Register ICommandViewModel Adapter
    ADD_MO_ADAPTER(
        pmod::ICommand,
        ICommandViewModel,
        pmod::library::ICommandViewModelClass,
        CMoCommandViewModelAdapter);

    // Register ICommandViewModel Adapter
    ADD_MO_ADAPTER(
        pmod::ICommand,
        ICommandViewModelProtected,
        pmod::library::ICommandViewModelClass,
        CMoCommandViewModelProtectedAdapter);

    // Register ICommandViewModel Adapter
    ADD_MO_ADAPTER(
        pmod::ICommand,
        ICommandViewModelOverrides,
        pmod::library::ICommandViewModelClass,
        CMoCommandViewModelOverridesAdapter);

    return S_OK;
}

//////////////////////////////////// Async Operation Class Support //////////////////////////////////////////
HRESULT RegisterMoAsyncOperationClassFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IAsyncOperationModelClass Adapter
    ADD_MO_ADAPTER(
        foundation::IAsyncOperation,
        IAsyncOperationClass,
        foundation::library::IAsyncOperationClass,
        CMoAsyncOperationClassAdapter);

    // Register IPropertyResultModelProtected Adapter
    ADD_MO_ADAPTER(
        foundation::IAsyncOperation,
        IAsyncOperationProtected,
        foundation::library::IAsyncOperationClass,
        CMoAsyncOperationModelProtectedAdapter);

    // Register IPropertyModelOverrides Adapter
    ADD_MO_ADAPTER(
        foundation::IAsyncOperation,
        IAsyncOperationOverrides,
        foundation::library::IAsyncOperationClass,
        CMoAsyncOperationModelOverridesAdapter);

    return S_OK;
}
//////////////////////////////////// View Model Factory Support //////////////////////////////////////////
HRESULT RegisterMoViewModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

     // Register IViewModelFactory Adapter
    ADD_MO_ADAPTER(
        foundation::IObjectFactory,
        ABI::Microsoft::PropertyModel::IViewModelFactory,
        pmod::library::IViewModelFactoryClass,
        CMoViewModelFactoryAdapter);

     // Register IViewModelFactoryProtected Adapter
    ADD_MO_ADAPTER(
        foundation::IObjectFactory,
        IViewModelFactoryProtected,
        pmod::library::IViewModelFactoryClass,
        CMoViewModelFactoryProtectedAdapter);

     // Register IViewModelFactoryOverrides Adapter
    ADD_MO_ADAPTER(
        foundation::IObjectFactory,
        IViewModelFactoryOverrides,
        pmod::library::IViewModelFactoryClass,
        CMoViewModelFactoryOverridesAdapter);

     // Register IViewModelCreatedEventArgs Adapter
    ADD_MO_ADAPTER(
        pmod::IViewModelCreatedEventArgs,
        IViewModelCreatedEventArgs,
        pmod::IViewModelCreatedEventArgs,
        CMoViewModelCreatedEventArgsAdapter);

    return S_OK;
}

HRESULT  STDAPICALLTYPE RegisterMoLibraryAdapterFactory()
{
    static bool bRegister = false;

    _IFR_(RegisterMoAdapterFactory());
    if(!bRegister)
    {
        foundation::ComPtr<foundation::library::IGenericAdapterFactory> spGenericAdapterFactory;
        IFR_ASSERT(foundation::library::CreateGenericAdapterFactory(spGenericAdapterFactory.GetAddressOf()));

       
        // Core Class support
        IFR_ASSERT(RegisterMoCoreModelClassFactory(spGenericAdapterFactory));

        // Models
        IFR_ASSERT(RegisterMoPropertyModelClassFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoCollectionModelClassFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoCommandModelClassFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoAsyncOperationClassFactory(spGenericAdapterFactory));

        // View Models
        IFR_ASSERT(RegisterMoPropertyViewModelFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoCollectionViewModelFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoCommandViewModelFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoMultiSourcePropertyViewModelFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoMultiSourceCollectionViewModelFactory(spGenericAdapterFactory));
        
        // View Model Factory
        IFR_ASSERT(RegisterMoViewModelFactory(spGenericAdapterFactory));

        // QI for foundation::library::IInterfaceAdapterFactory to register
        foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spMoAdapterFactory;
        IFR_ASSERT(foundation::QueryInterface(spGenericAdapterFactory, spMoAdapterFactory.GetAddressOf()));

        IFR_ASSERT(foundation::library::RegisterInterfaceAdapterFactory(spMoAdapterFactory));

        bRegister = true;
    }

    return S_OK;
}
