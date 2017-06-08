/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreAdapterFactory.cpp
****/

#pragma once
#include "pch.h"

#include "Microsoft.PropertyModel.h"
#include <foundation/library/library_util.h>
#include <pmod/errors.h>
#include <pmod/library/model_library.h>
#include <pmod/library/library_util.h>
#include <foundation/library/adapter_base.h>
#include <winrt/MoAsyncOperationAdapterBase.h>
#include <foundation/com_ptr.h>

// Headers for Mo Adapters
#include "MoObjectAdapter.h"
#include "MoObjectNodeAdapter.h"
#include "MoModelTypeInfoAdapter.h"
#include "MoObjectDispatchAdapter.h"
#include "MoObservableObjectAdapter.h"
#include "MoPropertyDataInfoAdapter.h"
#include "MoObservableObjectInfoAdapter.h"
#include "MoCommandInfoAdapter.h"
#include "MoObservableCollectionInfoAdapter.h"
#include "MoObservableCollectionAdapter.h"
#include "MoCommandAdapter.h"
#include "MoObservableVector.h"
#include "MoBindingBaseAdapter.h"
#include "MoPropertyBindingAdapter.h"
#include "MoMultiSourceBindingAdapter.h"
#include "MoSourceModelAdapter.h"
#include "MoAsyncOperationAdapter.h"
#include "MoModelFreeThreadMarshalAdapter.h"
#include "MoObservableObjectMapAdapter.h"
#include "MoKeyValuePairAdapter.h"
#include "MoModelFactoryAdapter.h"
#include "MoExpressionAdapter.h"
#include "MoExpressionBindingAdapter.h"
#include "MoAsyncOperationTypeInfoAdapter.h"

#include "MoEnumTypeInfoAdapter.h"
#include "MoEnumValueAdapter.h"
#include "MoCoreLoggerAdapter.h"
#include "MoStreamProviderAdapter.h"
#include "MoObjectSerializerAdapter.h"

// Free Thread Apartment support
#include "FreeThreadAdapterFactory.h"


#if !defined(_WINDOWS_PHONE_80)
// for XAML support
#include "MoCustomPropertyProviderAdapter.h"
#include "MoXamlCommandAdapter.h"
#endif

#include "winrt/MoObservableCollectionAdapterBase.h"
#include "winrt/MoEnumValueAdapterBase.h"

// auto generated adapters
#include "../../__generated/winrt/FoundationErrors.winrt.g.cpp"
#include "../../__generated/winrt/Errors.winrt.g.cpp"

#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#include "../../__generated/winrt/Perf.winrt.g.cpp"
#endif

// define factory based AsyncOperation for WinRT
typedef TMoAsyncOperationAdapterBase
    <
        ABI::Windows::Foundation::__FIAsyncOperation_1_IInspectable_t,
        ABI::Windows::Foundation::__FIAsyncOperationCompletedHandler_1_IInspectable_t
    > CMoAsyncOperationAdapter_Inspectable;

EXTERN_C __declspec(dllexport) HRESULT  STDAPICALLTYPE RegisterMoAdapterFactory();

#pragma warning( disable : 4503 )

template <class T,class TModelAdapter>
static HRESULT AddMoCollectionAdapter(
    _In_ foundation::library::ITypeAdapterFactory *pModelTypeAdapterFactory,
    _In_ const IID& iidType)
{
    foundation::library::CreateAdapterInstanceCallback creator = foundation::CreateAdapter<TModelAdapter>;
    // add IVector<T>
    IFR_ASSERT(pModelTypeAdapterFactory->AddTypeEntry(
            __uuidof(ABI::Windows::Foundation::Collections::IVector<T>),
            iidType,
            creator));
    // add IIterable<T>
    IFR_ASSERT(pModelTypeAdapterFactory->AddTypeEntry(
            __uuidof(ABI::Windows::Foundation::Collections::IIterable<T>),
            iidType,
            creator));
    // add IVectorView<T>
    IFR_ASSERT(pModelTypeAdapterFactory->AddTypeEntry(
            __uuidof(ABI::Windows::Foundation::Collections::IVectorView<T>),
            iidType,
            creator));

    return S_OK;
}

typedef TPropertyValueMoObservableCollectionAdapterBase<BYTE> CByteObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<boolean> CBoolObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<INT16> CInt16ObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<UINT16> CUInt16ObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<INT32> CInt32ObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<UINT32> CUInt32ObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<UINT64> CUInt64ObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<INT64> CInt64ObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<HSTRING> CStringObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<DOUBLE> CDoubleObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<FLOAT> CFloatObservableCollectionAdapter;
typedef TPropertyValueMoObservableCollectionAdapterBase<ABI::Windows::Foundation::DateTime> CDateTimeObservableCollectionAdapter;

// Entry Point to Create the Collection Model Adapter
static HRESULT CreateMoObservableCollectionAdapterFactory(_COM_Outptr_ foundation::library::IInterfaceAdapterFactory **ppModelAdapterFactory)
{
    IFR_ASSERT(foundation::library::CreateTypeAdapterFactory(nullptr,ppModelAdapterFactory));
    foundation::ComPtr<foundation::library::ITypeAdapterFactory> spModelTypeAdapterFactory;
    IFR_ASSERT(foundation::QueryInterface(*ppModelAdapterFactory,spModelTypeAdapterFactory.GetAddressOf()));

   // Define ObservableCollection_Byte
    AddMoCollectionAdapter<BYTE,CByteObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_UInt8);
    // Define ObservableCollection_Bool
    AddMoCollectionAdapter<boolean,CBoolObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_Boolean);
    // Define ObservableCollection_Int16
    AddMoCollectionAdapter<INT16,CInt16ObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_Int16);
    // Define ObservableCollection_UInt16
    AddMoCollectionAdapter<UINT16,CUInt16ObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_UInt16);
    // Define ObservableCollection_Int32
    AddMoCollectionAdapter<INT32,CInt32ObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_Int32);
    // Define ObservableCollection_UInt32
    AddMoCollectionAdapter<UINT32,CUInt32ObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_UInt32);
    // Define ObservableCollection_UInt64
    AddMoCollectionAdapter<UINT64,CUInt64ObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_UInt64);
    // Define ObservableCollection_Int64
    AddMoCollectionAdapter<INT64,CInt64ObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_Int64);
     // Define ObservableCollection_String
    AddMoCollectionAdapter<HSTRING,CStringObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_String);
     // Define ObservableCollection_DateTime
    AddMoCollectionAdapter<ABI::Windows::Foundation::DateTime,CDateTimeObservableCollectionAdapter>(spModelTypeAdapterFactory,pmod::IID_IObservableCollection_DateTime);

    return S_OK;
}

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

//////////////////////////////////// KeyValuePair support Support //////////////////////////////////////////
    HRESULT RegisterMoKeyValuePairFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IKeyValuePair<HSTRING,IInspectable *> Adapter
    IFHR(pGenericAdapterFactory->AddAdapterEntry(
            foundation::IDictionaryPair::GetIID(),
            __uuidof(ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,IInspectable *>),
            foundation::IDictionaryPair::GetIID(),
            GetAdapterCallBack<CMoPropertyNamePair>()
        ));

    // Register IKeyValuePair<UINT32,IInspectable *> Adapter
    IFHR(pGenericAdapterFactory->AddAdapterEntry(
			foundation::IPropertyIdPair::GetIID(),
            __uuidof(ABI::Windows::Foundation::Collections::IKeyValuePair<UINT32,IInspectable *>),
			foundation::IPropertyIdPair::GetIID(),
            GetAdapterCallBack<CMoPropertyIdPair>()
        ));


    // Register IKeyValuePair<IEnumValue,IInspectable *> Adapter
    IFHR(pGenericAdapterFactory->AddAdapterEntry(
        foundation::ILogParameterPair::GetIID(),
        __uuidof(ABI::Windows::Foundation::Collections::IKeyValuePair<IEnumValue *,IInspectable *>),
        foundation::ILogParameterPair::GetIID(),
        GetAdapterCallBack<CMoLogParameterPair>()
        ));
  
    return S_OK;
}
//////////////////////////////////// Core Model Support //////////////////////////////////////////
HRESULT RegisterMoCoreModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    IID iidPublicObjectTypes[] = {
        pmod::IObservableObject::GetIID(),
        pmod::IObservableCollection::GetIID(),
        pmod::ICommand::GetIID(),
        pmod::IPropertyBinding::GetIID(),
        pmod::IMultiSourceBinding::GetIID(),
        foundation::IAsyncOperation::GetIID(),
        foundation::IEnumValue::GetIID(),
    };

    for (int idx = 0; idx < sizeof(iidPublicObjectTypes) / sizeof(IID); ++idx)
    {
        // Register IObject Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidPublicObjectTypes[idx],
            __uuidof(IObject),
            foundation::IObject::GetIID(),
            GetAdapterCallBack<CMoObjectAdapter>()
            ));
    }

    IID iidPublicCoreModelTypes[] = { 
        pmod::IObservableObject::GetIID(),
        pmod::IObservableCollection::GetIID(),
        pmod::ICommand::GetIID(),
        pmod::IPropertyBinding::GetIID(),
        pmod::IMultiSourceBinding::GetIID(),
    };

    for(int idx = 0; idx < sizeof(iidPublicCoreModelTypes)/sizeof(IID); ++idx)
    {
        // Register IObjectNode Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidPublicCoreModelTypes[idx],
            __uuidof(IObjectNode),
            foundation::_IObjectNode::GetIID(),
            GetAdapterCallBack<CMoObjectNodeAdapter>()
            ));
    }

    IID iidObjectTypes[] = { 
        pmod::IObservableObject::GetIID(),
        pmod::IObservableCollection::GetIID(),
        pmod::ICommand::GetIID(),
        foundation::IAsyncOperation::GetIID(),
    };

    for (int idx = 0; idx < sizeof(iidObjectTypes) / sizeof(IID); ++idx)
    {
        IID iidObjectType = iidObjectTypes[idx];

        // Register IEventSourceClass Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidObjectType,
            __uuidof(IEventSourceClass),
            iidObjectType,
            GetAdapterCallBack<CMoEventSourceClassAdapter>()
            ));
    }

	IID iidPropertyDataTypes[] = {
		pmod::IObservableObject::GetIID(),
		foundation::IObjectDispatch::GetIID(),
	};

	for (int idx = 0; idx < sizeof(iidPropertyDataTypes) / sizeof(IID); ++idx)
	{
		IID iidType = iidPropertyDataTypes[idx];

		// Register IObjectDispatch Adapter
		IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
			iidType,
			__uuidof(IObjectDispatch),
			foundation::IObjectDispatch::GetIID(),
			GetAdapterCallBack<CMoObjectDispatchAdapter>()
			));
	}
    // All our Models that support IObservableObject Interfaces
    // and the EnumValue/AsycnOperation as well
    IID iidPropertyModelCoreModelTypes[] = { 
        pmod::IObservableObject::GetIID(),
        pmod::IObservableCollection::GetIID(),
        pmod::ICommand::GetIID(),
        pmod::IPropertyBinding::GetIID(),
        pmod::IMultiSourceBinding::GetIID(),
        foundation::IAsyncOperation::GetIID(),
        foundation::IEnumValue::GetIID(),
    };
    for(int idx = 0; idx < sizeof(iidPropertyModelCoreModelTypes)/sizeof(IID); ++idx)
    {
        IID iidPropertyModelCoreType = iidPropertyModelCoreModelTypes[idx];
        // Register IObservableObject Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidPropertyModelCoreType,
            __uuidof(IObservableObject),
            pmod::IObservableObject::GetIID(),
            GetAdapterCallBack<CMoObservableObjectAdapter>()
            ));
#if !defined(_WINDOWS_PHONE_80)
        // Register ICustomPropertyProvider Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidPropertyModelCoreType,
            __uuidof(ABI::Windows::UI::Xaml::Data::ICustomPropertyProvider),
            pmod::IObservableObject::GetIID(),
            GetAdapterCallBack<CMoCustomPropertyProviderAdapter>()
            ));
        // Register ICustomPropertyProvider Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidPropertyModelCoreType,
            __uuidof(ABI::Windows::UI::Xaml::Data::INotifyPropertyChanged),
            pmod::IObservableObject::GetIID(),
            GetAdapterCallBack<CMoCustomPropertyProviderAdapter>()
            ));
#endif
    }
    return S_OK;
}
//////////////////////////////////// Source Model base Support //////////////////////////////////////////
HRESULT RegisterMoSourceModelBaseFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    IID iidSourceModelBaseTypes[] = { 
        pmod::IPropertyBinding::GetIID(),
        pmod::IMultiSourceBinding::GetIID(),
        pmod::IObservableObject::GetIID(),      // for PropertyViewModel
        pmod::IObservableCollection::GetIID(),    // for CollectionViewModel
        pmod::ICommand::GetIID(),     // for CommandViewModel
        foundation::IAsyncOperation::GetIID(),     // wildcard entry for the Proxy
    };

    for(int idx = 0; idx < sizeof(iidSourceModelBaseTypes)/sizeof(IID); ++idx)
    {
        IID iidSourceModelBaseType = iidSourceModelBaseTypes[idx];
        // Register ISourceModelBase Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidSourceModelBaseType,
            __uuidof(ISourceModelBase),
            pmod::ISourceModelBase::GetIID(),
            GetAdapterCallBack<CMoSourceModelBaseAdapter>()
            ));


        // Register ISourceModelBaseOverrides Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidSourceModelBaseType,
            __uuidof(ISourceModelBaseOverrides),
            pmod::ISourceModelBase::GetIID(),
            GetAdapterCallBack<CMoSourceModelBaseOverridesAdapter>()
            ));


    }
    return S_OK;
}

//////////////////////////////////// Source Model Support //////////////////////////////////////////
HRESULT RegisterMoSourceModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    IID iidSourceModelTypes[] = { 
        pmod::IPropertyBinding::GetIID(),
        pmod::IObservableObject::GetIID(),        // for PropertyViewModel
        pmod::IObservableCollection::GetIID(),      // for CollectionViewModel
        pmod::ICommand::GetIID(),         // for CommandViewModel
        foundation::IAsyncOperation::GetIID(),  // wildcard entry for the Proxy
    };

    for(int idx = 0; idx < sizeof(iidSourceModelTypes)/sizeof(IID); ++idx)
    {
        IID iidSourceModelType = iidSourceModelTypes[idx];
        // Register ISourceModel Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidSourceModelType,
            __uuidof(ISourceModel),
            pmod::ISourceModel::GetIID(),
            GetAdapterCallBack<CMoSourceModelAdapter>()
            ));


        // Register ISourceModelOverrides Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidSourceModelType,
            __uuidof(ISourceModelOverrides),
            pmod::ISourceModelBase::GetIID(),
            GetAdapterCallBack<CMoSourceModelOverridesAdapter>()
            ));
    }
    return S_OK;
}
//////////////////////////////////// Multi Source Model Support //////////////////////////////////////////
HRESULT RegisterMultiSourceModelBaseFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    IID iidMultiSourceModelTypes[] = { 
        pmod::IMultiSourceBinding::GetIID(),
        pmod::IObservableObject::GetIID(),            // for MultiSourcePropertyModel
        pmod::IObservableCollection::GetIID(),          // for MultiSourceObservableCollection
    };

    for(int idx = 0; idx < sizeof(iidMultiSourceModelTypes)/sizeof(IID); ++idx)
    {
        IID iidMultiSourceModelType = iidMultiSourceModelTypes[idx];
        // Register IMultiSourceModel Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidMultiSourceModelType,
            __uuidof(IMultiSourceModel),
            pmod::IMultiSourceModel::GetIID(),
            GetAdapterCallBack<CMoMultiSourceModelAdapter>()
            ));


        // Register IMultiSourceModelOverrides Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidMultiSourceModelType,
            __uuidof(IMultiSourceModelOverrides),
            pmod::ISourceModelBase::GetIID(),
            GetAdapterCallBack<CMoMultiSourceModelOverridesAdapter>()
            ));
    }
    return S_OK;
}
//////////////////////////////////// Core Dictionary Support //////////////////////////////////////////
HRESULT RegisterMoCoreDictionaryFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IMap<HSTRING,IInspectable *> Adapter
    ADD_MO_ADAPTER(
        foundation::IDictionary,
        ABI::Windows::Foundation::Collections::__FIMap_2_HSTRING_IInspectable_t,
        foundation::IDictionary,
        CMoObservableObjectMapAdapter);

    typedef foundation::AdapterBase
        <
        foundation::IDictionary,
        IDictionary,
        &__uuidof(IDictionary)
        > CMoDictionaryAdapter;

    ADD_MO_ADAPTER(
        foundation::IDictionary,
        IDictionary,
        foundation::IDictionary,
        CMoDictionaryAdapter);

    return S_OK;
}
//////////////////////////////////// Property Model Support //////////////////////////////////////////
HRESULT RegisterMoObservableObjectFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IPropertyChangedEventArgs Adapter
    ADD_MO_ADAPTER(
        pmod::IPropertyChangedEventArgs,
        IPropertyChangedEventArgs,
        pmod::IPropertyChangedEventArgs,
        CMoPropertyChangedEventArgsAdapter);

    // Register IBatchPropertyChangedEventArgs Adapter
    ADD_MO_ADAPTER(
        pmod::IBatchPropertyChangedEventArgs,
        IBatchPropertyChangedEventArgs,
        pmod::IBatchPropertyChangedEventArgs,
        CMoBatchPropertyChangedEventArgsAdapter);

    // Register IPropertyChangedWithCallbackEventArgs Adapter
    ADD_MO_ADAPTER(
        pmod::IPropertyChangedEventArgs,
        IPropertyChangedWithCallbackEventArgs,
        pmod::library::IPropertyChangedWithCallbackEventArgs,
        CMoPropertyChangedEventWithCallbackArgsAdapter);

    // Register IPropertyChangedEventArgs Adapter
    ADD_MO_ADAPTER(
        pmod::IEventModelEventArgs,
        IEventModelEventArgs,
        pmod::IEventModelEventArgs,
        CMoEventModelEventArgsAdapter);

    // Register IMap<HSTRING,IInspectable *> Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        ABI::Windows::Foundation::Collections::__FIMap_2_HSTRING_IInspectable_t,
        foundation::IDictionary,
        CMoObservableObjectMapAdapter);

    // Register IObservableMap<HSTRING,IInspectable *> Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableObject,
        ABI::Windows::Foundation::Collections::__FIObservableMap_2_HSTRING_IInspectable_t,
        foundation::IDictionary,
        CMoPropertyModelObservableMapAdapter);

    return S_OK;
}

HRESULT RegisterMoObservableCollectionFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;
    // Register IObservableCollection Adapter
    ADD_MO_ADAPTER(
        pmod::IObservableCollection,
        IObservableCollection,
        pmod::IObservableCollection,
        CMoObservableCollectionAdapter);

    // Register INotifyCollectionChangedEventArgs Adapter
    ADD_MO_ADAPTER(
        pmod::INotifyCollectionChangedEventArgs,
        INotifyCollectionChangedEventArgs,
        pmod::INotifyCollectionChangedEventArgs,
        CMoNotifyCollectionChangedEventArgsAdapter);

    // Register CInspectableObservableVector Adapter

    IID iidObservableVectorTypes[] = { 
        __uuidof(ABI::Windows::Foundation::Collections::IObservableVector<IInspectable *>),
        __uuidof(ABI::Windows::Foundation::Collections::IVector<IInspectable *>),
        __uuidof(ABI::Windows::Foundation::Collections::IVectorView<IInspectable *>),
        __uuidof(ABI::Windows::Foundation::Collections::IIterable<IInspectable *>)
    };
    for(int idx = 0; idx < sizeof(iidObservableVectorTypes)/sizeof(IID); ++idx)
    {
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            pmod::IObservableCollection::GetIID(),
            iidObservableVectorTypes[idx],
            pmod::IObservableCollection::GetIID(),
            GetAdapterCallBack<CInspectableObservableVector<IInspectable *>>()
        ));
    }

    return S_OK;
}

HRESULT RegisterMoCommandFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register ICommand Adapter

    IID iidCommandModelTypes[] = { 
        __uuidof(ICommandBase),
        __uuidof(ISimpleCommand),
        __uuidof(ISimpleAsyncCommand),
        __uuidof(ICommand),
    };
    for(int idx = 0; idx < sizeof(iidCommandModelTypes)/sizeof(IID); ++idx)
    {
        IFHR(pGenericAdapterFactory->AddAdapterEntry(
            pmod::ICommand::GetIID(),
            iidCommandModelTypes[idx],
            pmod::ICommand::GetIID(),
            GetAdapterCallBack<CMoCommandAdapter>()
            ));
    }

    // Register ICanExecuteChangedEventArgs Adapter
    ADD_MO_ADAPTER(
        pmod::ICanExecuteChangedEventArgs,
        ICanExecuteChangedEventArgs,
        pmod::ICanExecuteChangedEventArgs,
        CMoCanExecuteChangedEventArgsAdapter);

#if !defined(_WINDOWS_PHONE_80)
     // Register ICommand Adapter
     IFHR(pGenericAdapterFactory->AddAdapterEntry(
            pmod::ICommand::GetIID(),
            __uuidof(ABI::Windows::UI::Xaml::Input::ICommand),
            pmod::ICommand::GetIID(),
            GetAdapterCallBack<CMoXamlCommandAdapter>()
            ));
#endif
    return S_OK;
}

//////////////////////////////////// Property Binding Support //////////////////////////////////////////
HRESULT RegisterMoPropertyBindingFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

     // Register IBindingBase Adapter
    ADD_MO_ADAPTER(
        pmod::IPropertyBinding,
        IBindingBase,
        pmod::IBindingBase,
        CMoBindingBaseAdapter);

     // Register IPropertyBinding Adapter
    ADD_MO_ADAPTER(
        pmod::IPropertyBinding,
        IPropertyBinding,
        pmod::IPropertyBinding,
        CMoPropertyBindingAdapter);

     // Register IBindingValueChangedEventArgs Adapter
    ADD_MO_ADAPTER(
        pmod::IBindingValueChangedEventArgs,
        IBindingValueChangedEventArgs,
        pmod::IBindingValueChangedEventArgs,
        CMoBindingValueChangedEventArgsAdapter);    

    return S_OK;
}
//////////////////////////////////// Multi Source Binding Support //////////////////////////////////////////
HRESULT RegisterMoMultiSourceBindingFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

     // Register IBindingBase Adapter
    ADD_MO_ADAPTER(
        pmod::IMultiSourceBinding,
        IBindingBase,
        pmod::IBindingBase,
        CMoBindingBaseAdapter);

     // Register IMultiSourceBinding Adapter
    ADD_MO_ADAPTER(
        pmod::IMultiSourceBinding,
        IMultiSourceBinding,
        pmod::IMultiSourceBinding,
        CMoMultiSourceBindingAdapter);

     // Register IMultiSourceBindingProtected Adapter
    // TODO: move this specific to the projection lib
    //ADD_MO_ADAPTER(
    //    pmod::IMultiSourceBinding,
    //    IMultiSourceBindingProtected,
    //    pmod::library::IMultiSourceBindingClass,
    //    CMoMultiSourceBindingProtectedAdapter);

    return S_OK;
}
//////////////////////////////////// Async Operation Model Support //////////////////////////////////////////
HRESULT RegisterMoAsyncOperationModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    HRESULT hr;

    // Register IAsyncOperation Adapter
    ADD_MO_ADAPTER(
        foundation::IAsyncOperation,
        IAsyncOperation,
        foundation::IAsyncOperation,
        CMoAsyncOperationAdapter);

    // Register IResultCompletedEventArgs Adapter
    ADD_MO_ADAPTER(
        foundation::ICompletedEventArgs,
        IResultCompletedEventArgs,
        foundation::ICompletedEventArgs,
        CMoResultCompletedEventArgsAdapter);

    // Register ABI::Windows::Foundation::__FIAsyncOperation_1_IInspectable_t Adapter
    ADD_MO_ADAPTER(
        foundation::IAsyncOperation,
        ABI::Windows::Foundation::__FIAsyncOperation_1_IInspectable_t,
        foundation::IAsyncOperation,
        CMoAsyncOperationAdapter_Inspectable);
    // Register ABI::Windows::Foundation::IAsyncInfo Adapter
    ADD_MO_ADAPTER(
        foundation::IAsyncOperation,
        ABI::Windows::Foundation::IAsyncInfo,
        foundation::IAsyncOperation,
        CMoAsyncOperationAdapter_Inspectable);

    // Register IAgileObject Adapter
    ADD_MO_ADAPTER(
        foundation::IAsyncOperation,
        IAgileObject,
        foundation::IAsyncOperation,
        CMoObjectFreeThreadMarshalAdapter);

    // Register IAgileObject Adapter
    ADD_MO_ADAPTER(
        foundation::IAsyncOperation,
        IMarshal,
        foundation::IAsyncOperation,
        CMoObjectFreeThreadMarshalAdapter);

    return S_OK;
}

HRESULT RegisterMoModelTypeInfoFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    //////////////////////////////////// Type Info Support //////////////////////////////////////////
    
    // Register IObjectTypeInfo Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IObjectTypeInfo::GetIID(),
        __uuidof(IObjectTypeInfo),
        foundation::IObjectTypeInfo::GetIID(),
        GetAdapterCallBack<CMoModelTypeInfoAdapter>()
        ));
 
    // Register IMethodInfo Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IMethodInfo::GetIID(),
        __uuidof(IMethodInfo),
        foundation::IMethodInfo::GetIID(),
        GetAdapterCallBack<CMoMethodInfoAdapter>()
        ));

	// Register IObjectDispatchInfo Adapter
	IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
		foundation::IObjectTypeInfo::GetIID(),
		__uuidof(IObjectDispatchInfo),
		foundation::IObjectDispatchInfo::GetIID(),
		GetAdapterCallBack<CMoPropertyDataInfoAdapter>()
		));

	// Register IObservableObjectInfo Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IObjectTypeInfo::GetIID(),
        __uuidof(IObservableObjectInfo),
        pmod::IObservableObjectInfo::GetIID(),
        GetAdapterCallBack<CMoObservableObjectInfoAdapter>()
        ));

    // Register IViewModelInfo Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IObjectTypeInfo::GetIID(),
        __uuidof(IViewModelInfo),
        pmod::IViewModelInfo::GetIID(),
        GetAdapterCallBack<CMoViewModelInfoAdapter>()
        ));

    // Register IEnumTypeInfo Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IObjectTypeInfo::GetIID(),
        __uuidof(IEnumTypeInfo),
        foundation::IEnumTypeInfo::GetIID(),
        GetAdapterCallBack<CMoEnumTypeInfoAdapter>()
        ));

    // Register IObservableCollectionInfo Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IObjectTypeInfo::GetIID(),
        __uuidof(IEnumerableTypeInfo),
        foundation::IEnumerableTypeInfo::GetIID(),
        GetAdapterCallBack<CMoEnumerableTypeInfoAdapter>()
        ));

    // Register ICommandInfo Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IObjectTypeInfo::GetIID(),
        __uuidof(ICommandInfo),
        pmod::ICommandInfo::GetIID(),
        GetAdapterCallBack<CMoCommandInfoAdapter>()
        ));

    // Register IAsyncOperationTypeInfo Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IObjectTypeInfo::GetIID(),
        __uuidof(IAsyncOperationTypeInfo),
        foundation::IAsyncOperationTypeInfo::GetIID(),
        GetAdapterCallBack<CMoAsyncOperationTypeInfoAdapter>()
        ));

    return S_OK;
}

HRESULT RegisterMoExpression(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    // Register IExpression Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IExpression::GetIID(),
        __uuidof(IExpression),
        foundation::IExpression::GetIID(),
        GetAdapterCallBack<CMoExpressionAdapter>()
        ));
    return S_OK;
}

HRESULT RegisterMoExpressionBinding(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    // Register IExpressionBinding Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        pmod::IMultiSourceBinding::GetIID(),
        __uuidof(IExpressionBinding),
        pmod::IBindingBase::GetIID(),
        GetAdapterCallBack<CMoExpressionBindingAdapter>()
        ));
    // Register ISourceModel Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        pmod::IMultiSourceBinding::GetIID(),
        __uuidof(ISourceModel),
        pmod::ISourceModel::GetIID(),
        GetAdapterCallBack<CMoSourceModelAdapter>()
        ));
    return S_OK;
}

HRESULT RegisterMoModelFactory(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    // Register IModelFactory Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IObjectFactory::GetIID(),
        __uuidof(IObjectFactory),
        foundation::IObjectFactory::GetIID(),
        GetAdapterCallBack<CMoObjectFactoryAdapter>()
        ));
    return S_OK;
}

HRESULT RegisterMoEnumValue(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    // Register IEnumValue Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IEnumValue::GetIID(),
        __uuidof(IEnumValue),
        foundation::IEnumValue::GetIID(),
        GetAdapterCallBack<CMoEnumValueAdapter>()
        ));

    // Register the 'generic' IReference<UINT32> that winRT may be looking for
    typedef TMoEnumValueAdapterBase<ABI::Windows::Foundation::IReference<UINT32>, UINT32> CMoEnumValueAsReference;

    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::IEnumValue::GetIID(),
        __uuidof(ABI::Windows::Foundation::IReference<UINT32>),
        foundation::IEnumValue::GetIID(),
        GetAdapterCallBack<CMoEnumValueAsReference>()
        ));
    return S_OK;
}

HRESULT RegisterMoCoreLogger(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    // Register ICoreLogger Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::ILogger::GetIID(),
        __uuidof(ILogger),
        foundation::ILogger::GetIID(),
        GetAdapterCallBack<CMoCoreLoggerAdapter>()
        ));

    // Register ILogCategory Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::ILogCategory::GetIID(),
        __uuidof(ILogCategory),
        foundation::ILogCategory::GetIID(),
        GetAdapterCallBack<CMoLogCategoryAdapter>()
        ));

    // Register ILogCreatedEventArgs Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::ILogCreatedEventArgs::GetIID(),
        __uuidof(ILogCreatedEventArgs),
        foundation::ILogCreatedEventArgs::GetIID(),
        GetAdapterCallBack<CMoLogCreatedEventArgsAdapter>()
        ));

    // Register ILogParameter Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
        foundation::ILogParameterPair::GetIID(),
        __uuidof(ILogParameter),
        foundation::ILogParameterPair::GetIID(),
        GetAdapterCallBack<CMoLogParameterAdapter>()
    ));

    // Register ILogRecord Adapter
    IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            foundation::ILogRecord::GetIID(),
            __uuidof(ILogRecord),
            foundation::ILogRecord::GetIID(),
            GetAdapterCallBack<CMoLogRecordAdapter>()
        ));
   return S_OK;
}

HRESULT RegisterMoObjectSerializer(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    // Register IObjectSerializer Adapter
    IID iidObjectTypes[] = {
        pmod::IObservableObject::GetIID(),
        pmod::IObservableCollection::GetIID(),
        pmod::ICommand::GetIID(),
        foundation::IAsyncOperation::GetIID(),
    };

    for (int idx = 0; idx < sizeof(iidObjectTypes) / sizeof(IID); ++idx)
    {
        IID iidObjectType = iidObjectTypes[idx];
        // Register IObjectSerializer Adapter
        IFR_ASSERT(pGenericAdapterFactory->AddAdapterEntry(
            iidObjectType,
            __uuidof(IObjectSerializer),
            foundation::IObjectSerializer::GetIID(),
            GetAdapterCallBack<CMoObjectSerializerAdapter>()
            ));
    }
    return S_OK;
}

template <const IID* piid>
static HRESULT AddNotSupportedAdapter(
    foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    return pGenericAdapterFactory->AddAdapterEntry(
        GUID_NULL,
        *piid,
        IID_IInspectable,
        GetAdapterCallBack<foundation::NotSupportedAdapter<piid>>()
        );
}

EXTERN_C const IID IID_IProvideClassInfo;
EXTERN_C const IID IID_IProvideClassInfo2;

HRESULT RegisterMoNotSupported(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    AddNotSupportedAdapter<&IID_IProvideClassInfo>(pGenericAdapterFactory);
    AddNotSupportedAdapter<&IID_IProvideClassInfo2>(pGenericAdapterFactory);

    return S_OK;
}
// to register IAsyncOperation<T> factory support
extern HRESULT RegisterAsyncOperationAdapterFactories();


HRESULT  STDAPICALLTYPE RegisterMoAdapterFactory()
{
    static bool bRegister = false;
    if(!bRegister)
    {
        foundation::ComPtr<foundation::library::IGenericAdapterFactory> spGenericAdapterFactory;
        IFR_ASSERT(foundation::library::CreateGenericAdapterFactory(spGenericAdapterFactory.GetAddressOf()));

        IFR_ASSERT(RegisterMoModelTypeInfoFactory(spGenericAdapterFactory));

        // KeyValuePair Support
        IFR_ASSERT(RegisterMoKeyValuePairFactory(spGenericAdapterFactory));

        // EnumValue Support
        IFR_ASSERT(RegisterMoEnumValue(spGenericAdapterFactory));

        // CoreLogger Support
        IFR_ASSERT(RegisterMoCoreLogger(spGenericAdapterFactory));
       
        // Core support
        IFR_ASSERT(RegisterMoCoreModelFactory(spGenericAdapterFactory));

        // Source Model Base
        IFR_ASSERT(RegisterMoSourceModelBaseFactory(spGenericAdapterFactory));

        // Single Source Model
        IFR_ASSERT(RegisterMoSourceModelFactory(spGenericAdapterFactory));

        // Multi Source Model
        IFR_ASSERT(RegisterMultiSourceModelBaseFactory(spGenericAdapterFactory));

        // Expression
        IFR_ASSERT(RegisterMoExpression(spGenericAdapterFactory));

        // Bindings
        IFR_ASSERT(RegisterMoPropertyBindingFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoMultiSourceBindingFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoExpressionBinding(spGenericAdapterFactory));

        // Core Dictionary
        IFR_ASSERT(RegisterMoCoreDictionaryFactory(spGenericAdapterFactory));

        // Models
        IFR_ASSERT(RegisterMoObservableObjectFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoObservableCollectionFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoCommandFactory(spGenericAdapterFactory));
        IFR_ASSERT(RegisterMoAsyncOperationModelFactory(spGenericAdapterFactory));
        
        // Model Factory
        IFR_ASSERT(RegisterMoModelFactory(spGenericAdapterFactory));

        // Object Serializer Factory
        IFR_ASSERT(RegisterMoObjectSerializer(spGenericAdapterFactory));

        // Register IStreamProvider Adapter
        IFR_ASSERT(spGenericAdapterFactory->AddAdapterEntry(
            foundation::IStreamProvider::GetIID(),
            __uuidof(IStreamProvider),
            foundation::IStreamProvider::GetIID(),
            GetAdapterCallBack<CMoStreamProviderAdapter>()
            ));

        // QI for foundation::library::IInterfaceAdapterFactory to register
        foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spMoAdapterFactory;
        IFR_ASSERT(foundation::QueryInterface(spGenericAdapterFactory,spMoAdapterFactory.GetAddressOf()));

        IFR_ASSERT(foundation::library::RegisterInterfaceAdapterFactory(spMoAdapterFactory));

        // Factory Mo Collection support
        IFR_ASSERT(CreateMoObservableCollectionAdapterFactory(spMoAdapterFactory.ReleaseAndGetAddressOf()));
        IFR_ASSERT(foundation::library::RegisterInterfaceAdapterFactory(spMoAdapterFactory));

        // Register Free Threaded Apartment support
        foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spFreeThreadAdapterFactory;
        IFR_ASSERT(CFreeThreadAdapterFactory::CreateInstance(spFreeThreadAdapterFactory.GetAddressOf()));
        IFR_ASSERT(foundation::library::RegisterInterfaceAdapterFactory(spFreeThreadAdapterFactory));

        // Register auto generated entriesIMultiSourceBindingClass
        foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spModelAdapterFactory;
        IFR_ASSERT(foundation::library::CreateTypeAdapterFactory(_foundationErrorsFactoryEntries, spModelAdapterFactory.ReleaseAndGetAddressOf()));
        IFR_ASSERT(foundation::library::RegisterInterfaceAdapterFactory(spModelAdapterFactory));

        IFR_ASSERT(foundation::library::CreateTypeAdapterFactory(_coreErrorsFactoryEntries, spModelAdapterFactory.ReleaseAndGetAddressOf()));
        IFR_ASSERT(foundation::library::RegisterInterfaceAdapterFactory(spModelAdapterFactory));

#if defined(DEBUG)
        IFR_ASSERT(foundation::library::CreateTypeAdapterFactory(_corePerfFactoryEntries, spModelAdapterFactory.ReleaseAndGetAddressOf()));
        IFR_ASSERT(foundation::library::RegisterInterfaceAdapterFactory(spModelAdapterFactory));
#endif

        // register runtime namespaces to be mapped
        foundation::HStringPtr hcore_namespace(U("Microsoft.PropertyModel"));

        IFR_ASSERT(foundation::library::register_runtime_namespace(
            foundation::HStringPtr(U("foundation")),
            hcore_namespace));
        IFR_ASSERT(foundation::library::register_runtime_namespace(
            foundation::HStringPtr(U("pmod")),
            hcore_namespace));

        IFR_ASSERT(RegisterAsyncOperationAdapterFactories());

        bRegister = true;
    }

    return S_OK;
}
