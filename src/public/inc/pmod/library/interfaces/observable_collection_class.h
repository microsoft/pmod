/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <pmod/interfaces/observable_collection.h>
#include <foundation/com_ptr.h>
#include <foundation/library/interfaces/delegate_base.h>
#include <foundation/library/interfaces/disposable.h>
#include <foundation/library/type_info_create_parameter.h>

#include "object_node_class.h"
#include "value_converter.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        typedef  int(__cdecl *CompareItemValueFunc)(foundation::IInspectable *elem1, foundation::IInspectable *elem2, void *Context);

        // IItemsContainer Interface
        BEGIN_DECLARE_INTERFACE(IItemsContainer, foundation::library::IDisposable, PMOD_LIB_API)
            STDMETHOD(GetCount)(_Out_ UINT32 *pSize) = 0;
            STDMETHOD(Get)(_In_ UINT32 index, _COM_Outptr_ foundation::IInspectable **value) = 0;
            STDMETHOD(Set)(_In_ UINT32 index, _In_ foundation::IInspectable *value) = 0;
            STDMETHOD(GetItems)(_Out_ UINT32 *pSize, _COM_Outptr_ foundation::IInspectable ***items) = 0;
            STDMETHOD(Append)(_In_ foundation::IInspectable *value) = 0;
            STDMETHOD(Insert)(_In_ UINT32 index, _In_ foundation::IInspectable *value) = 0;
            STDMETHOD(Remove)(_In_ foundation::IInspectable *value) = 0;
            STDMETHOD(RemoveAt)(_In_ UINT32 index) = 0;
            STDMETHOD(InsertPositionOf)(_In_ foundation::IInspectable *value, _In_ CompareItemValueFunc compare, _In_opt_ void *Context, _Out_ UINT32 *index) = 0;
            STDMETHOD(Find)(_In_ foundation::IInspectable *value, _In_ CompareItemValueFunc compare, _In_opt_ void *Context, _Out_ UINT32 *index) = 0;
            STDMETHOD(Sort)(_In_ CompareItemValueFunc compare, _In_opt_ void *Context) = 0;
            STDMETHOD(Clear)() = 0;
            STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **first) = 0;
            STDMETHOD(Clone)(_COM_Outptr_ IItemsContainer **ppItemsContainer) = 0;
        END_DECLARE_INTERFACE()

        // IObservableCollectionClass Interface

        BEGIN_DECLARE_INTERFACE(IObservableCollectionClass, IObjectNodeClass, PMOD_LIB_API)
            // access to items container
            STDMETHOD(GetItemsContainer)(_COM_Outptr_ IItemsContainer **ppItemsContainer) = 0;
            // Property 'ItemConverter'
            STDMETHOD(GetItemConverter)(_COM_Outptr_result_maybenull_ IValueConverter **ppValueConverter) = 0;
            STDMETHOD(SetItemConverter)(_In_opt_ IValueConverter *pValueConverter) = 0;

            STDMETHOD(Sort)(_In_ CompareItemValueFunc compare, _In_opt_ void *Context) = 0;
            STDMETHOD(InsertInOrder)(_In_ foundation::IInspectable *newValue, _In_ CompareItemValueFunc compare, _In_ void *Context, _Out_ UINT32 *index) = 0;
            // Internal Append/Insert/Replace Item
            STDMETHOD(AppendItemInternal)(_In_ foundation::IInspectable *item) = 0;
            STDMETHOD(InsertItemInternal)(_In_ UINT32 nIndex, _In_ foundation::IInspectable *item) = 0;
            STDMETHOD(ReplaceItemInternal)(_In_ UINT32 nIndex, _In_ foundation::IInspectable *item) = 0;
            STDMETHOD(RemoveItemInternal)(_In_ foundation::IInspectable *item) = 0;

            STDMETHOD(RemoveAllInternal)() = 0;
            STDMETHOD(IndexOfInternal)(_In_ foundation::IInspectable *item, _Outptr_ UINT32* pnIndex) = 0;

            // batch method support
            STDMETHOD(InsertItems)(_In_ UINT32 nIndex, _In_ UINT32 size, _In_ foundation::IInspectable **items) = 0;
            STDMETHOD(ReplaceItems)(_In_ UINT32 nIndex, _In_ UINT32 size, _In_ foundation::IInspectable **items) = 0;
            STDMETHOD(RemoveItems)(_In_ UINT32 nIndex, _In_ UINT32 count) = 0;
            // Fire a notification
            STDMETHOD(FireCollectionChanged)(
                _In_ foundation::NotifyCollectionModelChangedAction action,
                _In_ UINT32 oldStartingIndex,
                _In_ UINT32 newStartingIndex,
                _In_ UINT32 sizeOldItems, _In_opt_ foundation::IInspectable **ppOldItems,
                _In_ UINT32 sizeNewItems, _In_opt_ foundation::IInspectable **ppNewItems
                ) = 0;
        END_DECLARE_INTERFACE()

        // Delegate Interface for IFastItemsContainer
        BEGIN_DECLARE_INTERFACE(IFastItemsContainer, IItemsContainer, PMOD_LIB_API)
            STDMETHOD(ReadCustomData)(_In_ UINT32 index, LPVOID pCustomData) = 0;
            STDMETHOD(WriteCustomData)(_In_ UINT32 index, LPVOID pCustomData) = 0;
            STDMETHOD(GetSlotItem)(_In_ UINT32 index, _COM_Outptr_ foundation::IInspectable **value) = 0;
            STDMETHOD(SetSlotItem)(_In_ UINT32 index, _In_ foundation::IInspectable *value) = 0;
            STDMETHOD(ResizeSlotItems)(_In_ UINT32 size) = 0;
            STDMETHOD(InsertSlotItems)(_In_ UINT32 index, _In_ UINT32 size) = 0;
            STDMETHOD(RemoveSlotItems)(_In_ UINT32 index, _In_ UINT32 size) = 0;
        END_DECLARE_INTERFACE()

        // Delegate Interface for IFastItemsContainer
        BEGIN_DECLARE_INTERFACE(IFastItemsContainerDelegate, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(OnClearSlotItem)(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr) = 0;
            STDMETHOD(OnGetSlotItem)(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr, _COM_Outptr_result_maybenull_ foundation::IInspectable **value) = 0;
            STDMETHOD(OnSetSlotItem)(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr, _In_opt_ foundation::IInspectable *value) = 0;
        END_DECLARE_INTERFACE()

        // Delegate Interface for a Collection Model Impl
        BEGIN_DECLARE_INTERFACE(IObservableCollectionDelegate, foundation::library::IDelegateBase<INotifyCollectionChangedEventHandler>, PMOD_LIB_API)
            STDMETHOD(OnCreateItemsContainer)(_COM_Outptr_ IItemsContainer **ppValue) = 0;
            // The Collection was changed
            STDMETHOD(OnCollectionChanged)(_In_ INotifyCollectionChangedEventArgs *pEventArgs) = 0;
        END_DECLARE_INTERFACE()


        enum class ObservableCollectionOptions
        {
            None = 0x00,
            IsReadOnly = 0x01,
            IsSet = 0x02,
            UseFastItemsContainer = 0x20,
        };

        struct ObservableCollectionCreateParametersBase :
            public foundation::library::_TypeInfoCreateParameterBase
            <
            IObservableCollectionDelegate,
            ObservableCollectionOptions
            >
        {
        };

        struct ObservableCollectionCreateParameters :
            public ObservableCollectionCreateParametersBase
        {
            // value array of initial objects
            foundation::InspectablePtr m_initial_values_ptr;
        };

        BEGIN_DECLARE_INTERFACE(IObservableCollectionClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateObservableCollectionInstance)(
                _In_ const ObservableCollectionCreateParameters *pCreateParams,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

