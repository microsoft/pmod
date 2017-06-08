/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:collection_viewmodel_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <pmod/interfaces/observable_collection.h>
#include <pmod/interfaces/observable_object.h>
#include <pmod/interfaces/source_model.h>
#include <foundation/interfaces/expression.h>
#include <foundation/library/interfaces/delegate_base.h>

#include "view_model_parameters.h"
#include "source_model_delegate.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // ICreateItemAdapterDelegate Interface
        BEGIN_DECLARE_INTERFACE(ICreateItemAdapterDelegate, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(_In_ foundation::IInspectable* pItem, _COM_Outptr_ ISourceModel **ppSourceModel) = 0;
        END_DECLARE_INTERFACE()

        // ICompareItemDelegate Interface
        BEGIN_DECLARE_INTERFACE(ICompareItemDelegate, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(_In_ foundation::IInspectable * item1, _In_ foundation::IInspectable * item2, _Out_ int *pResult) = 0;
        END_DECLARE_INTERFACE()
        // IFilterItemDelegate Interface
        BEGIN_DECLARE_INTERFACE(IFilterItemDelegate, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(_In_ foundation::IInspectable * item, _Out_ bool *pValue) = 0;
        END_DECLARE_INTERFACE()

        // ICollectionViewModelClass Interface

        BEGIN_DECLARE_INTERFACE(ICollectionViewModelClass, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(AddSortInfo)(_In_ foundation::IInspectable *pSortInfoPath, bool isAscending) = 0;
            STDMETHOD(SetFilterInfo)(_In_ foundation::IExpression *pFilterInfoExpression) = 0;

            // Property 'FilterItemDelegate'
            STDMETHOD(GetFilterItemDelegate)(_COM_Outptr_ IFilterItemDelegate **ppFilterItemDelegate) = 0;
            STDMETHOD(SetFilterItemDelegate)(_In_ IFilterItemDelegate *pFilterItemDelegate) = 0;

            // Property 'CompareItemDelegate'
            STDMETHOD(GetCompareItemDelegate)(_COM_Outptr_ ICompareItemDelegate **ppCompareItemDelegate) = 0;
            STDMETHOD(SetCompareItemDelegate)(_In_ ICompareItemDelegate *pCompareItemDelegate) = 0;

            // Property 'CreateModelAdapterDelegate'
            STDMETHOD(GetCreateItemAdapterDelegate)(_COM_Outptr_ ICreateItemAdapterDelegate **ppCreateItemAdapterDelegate) = 0;
            STDMETHOD(SetCreateItemAdapterDelegate)(_In_ ICreateItemAdapterDelegate *pCreateItemAdapterDelegate) = 0;

            // Enforce a Refresh for an Item contained in the Source Collection
            STDMETHOD(RefreshSourceItem)(_In_ foundation::IInspectable *item) = 0;
        END_DECLARE_INTERFACE()

        // Delegate Interface for a Collection Model View Impl
        typedef ISingleSourceModelDelegateBase<INotifyCollectionChangedEventHandler, IObservableCollection> ICollectionViewModelDelegateBase;
        BEGIN_DECLARE_INTERFACE(ICollectionViewModelDelegate, ICollectionViewModelDelegateBase, PMOD_LIB_API)
            // When the Source Collection changed
            STDMETHOD(OnSourceCollectionChanged)(_In_ INotifyCollectionChangedEventArgs *pEventArgs) = 0;
            // When a Source Item Property has changed
            STDMETHOD(OnSourceItemPropertyChanged)(
                _In_ IObservableObject *item,
                _In_ UINT32 propertyId,
                _In_ foundation::IInspectable *oldValue,
                _In_ foundation::IInspectable *newValue,
                _Out_ bool *pNeedRefresh) = 0;
            // Return true/false depending if the Item is being Filtered
            STDMETHOD(FilterItem)(_In_ foundation::IInspectable * item, _Out_ bool *pValue) = 0;
            // Compare Item to enable the Sort capabilties of this Collection View
            STDMETHOD(CompareItem)(_In_ foundation::IInspectable * item1, _In_ foundation::IInspectable * item2, _Out_ int *pResult) = 0;
            // Callback when an Item in the Collection has been attached
            STDMETHOD(OnItemSourceAttached)(_In_ IObservableObject *item) = 0;
            // Callback when an Item in the Collection has been detached
            STDMETHOD(OnItemSourceDetached)(_In_ IObservableObject *item) = 0;
            // Callback when an Adapter Model is needed
            STDMETHOD(CreateModelAdapter)(_In_ foundation::IInspectable *item, _Outptr_ ISourceModel **ppSourceModel) = 0;
        END_DECLARE_INTERFACE()

        enum class CollectionViewModelOptions
        {
            None = 0x00,
            UseResetOnBatchNotify = 0x01,
            NoSort = 0x02,
            NoFilter = 0x08,
            NoModelAdapter = 0x10,
            ForceAttachSourceItems = 0x20,
            NoTypeCheck = 0x30,
            IgnoreReadOnly = 0x40,
        };

        struct collection_viewmodel_create_parameters :
            _SingleSourceViewModelCreateParametersBase
            <
            ObservableCollectionCreateParametersBase,
            ICollectionViewModelDelegate,
            CollectionViewModelOptions,
            IObservableCollection
            >
        {
            collection_viewmodel_create_parameters() :
                m_p_filter_item_delegate(nullptr),
                m_p_compare_item_delegate(nullptr),
                m_p_create_item_adapter_delegate(nullptr)
            {
            }

            IFilterItemDelegate               *m_p_filter_item_delegate;
            ICompareItemDelegate              *m_p_compare_item_delegate;
            ICreateItemAdapterDelegate        *m_p_create_item_adapter_delegate;
        };

        BEGIN_DECLARE_INTERFACE(ICollectionViewModelClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateCollectionViewModelInstance)(
                _In_ const collection_viewmodel_create_parameters *pModelImpl,
                _In_opt_ foundation::IInspectable *pOuter,
                _Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()

    }
}

