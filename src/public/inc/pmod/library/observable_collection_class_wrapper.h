/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection_class_wrapper.h
****/
#pragma once

#include "object_node_class_wrapper.h"
#include "interfaces/observable_collection_class.h"

namespace pmod
{
    namespace library {

        template <class T>
        class _ObservableCollectionClassWrapper :
            public _ObjectNodeClassWrapper<T, IObservableCollectionClass>
        {
        public:
            HRESULT GetItemsContainer(_COM_Outptr_ IItemsContainer **ppItemsContainer)
            {
                return this->GetClassInterfaceInternal()->GetItemsContainer(ppItemsContainer);
            }

            HRESULT AppendItemInternal(_In_ foundation::IInspectable *pItem)
            {
                return this->GetClassInterfaceInternal()->AppendItemInternal(pItem);
            }

            HRESULT InsertItemInternal(_In_ UINT32 nIndex, _In_ foundation::IInspectable *pItem)
            {
                return this->GetClassInterfaceInternal()->InsertItemInternal(nIndex, pItem);
            }

            HRESULT ReplaceItemInternal(_In_ UINT32 nIndex, _In_ foundation::IInspectable *pItem)
            {
                return this->GetClassInterfaceInternal()->ReplaceItemInternal(nIndex, pItem);
            }

            template <class TValue>
            HRESULT GetItemContainer(UINT32 index, TValue& value)
            {
                foundation::ComPtr<IItemsContainer> items_container_ptr;
                IFR_ASSERT(GetItemsContainer(items_container_ptr.GetAddressOf()));
                foundation::InspectablePtr valuePtr;
                _IFR_(items_container_ptr->Get(index, valuePtr.GetAddressOf()));
                return foundation::GetValue(valuePtr, value);
            }

            template <class TValue>
            HRESULT AppendItemInternal(TValue value)
            {
                foundation::InspectablePtr valuePtr = foundation::pv_util::CreateValue(value);
                return AppendItemInternal(valuePtr.Get());
            }

            template <class TValue>
            HRESULT InsertItemInternal(_In_ UINT32 nIndex, TValue value)
            {
                foundation::InspectablePtr valuePtr = foundation::pv_util::CreateValue(value);
                return this->InsertItemInternal(nIndex, valuePtr.Get());
            }

            template <class array_refcount_wrapper_t>
            HRESULT InsertItems(_In_ UINT32 nIndex, const array_refcount_wrapper_t& items)
            {
                return this->GetClassInterfaceInternal()->InsertItems(
                    nIndex,
                    items.size(),
                    (foundation::IInspectable **)items.GetBuffer());
            }

            HRESULT InsertItems(_In_ UINT32 nIndex, _In_ foundation::IInspectable *pArray)
            {
                foundation::InspectableArrayWrapper values;
                IFR_ASSERT(foundation::pv_util::GetValueArray(
                    pArray,
                    values.GetSizeAddressOf(),
                    values.GetBufferAddressOf()));
                return InsertItems(nIndex, values);
            }

            template <class other_t>
            HRESULT InsertItemsFrom(_In_ UINT32 nIndex, const other_t& other)
            {
                return InsertItemsFrom(nIndex, other.GetClassInterface());
            }

            template <class TInterface>
            HRESULT InsertItemsFrom(_In_ UINT32 nIndex, TInterface *pT)
            {
                ObservableCollectionInspectablePtr collectionPtr;
                IFR_ASSERT(foundation::QueryInterface(pT, collectionPtr.GetAddressOf()));
                foundation::InspectableArrayWrapper items;
                IFR_ASSERT(collectionPtr.GetItems(items));
                return InsertItems(nIndex, items);
            }

            template <class array_refcount_wrapper_t>
            HRESULT replace_items(_In_ UINT32 nIndex, const array_refcount_wrapper_t& items)
            {
                return this->GetClassInterfaceInternal()->ReplaceItems(
                    nIndex,
                    items.size(),
                    (foundation::IInspectable **)items.GetBuffer());
            }

            HRESULT RemoveItem(_In_ foundation::IInspectable *pItem)
            {
                return this->GetClassInterfaceInternal()->RemoveItemInternal(pItem);
            }

            template <class TValue>
            HRESULT RemoveItem(TValue value)
            {
                foundation::InspectablePtr valuePtr = foundation::pv_util::CreateValue(value);
                return RemoveItem(valuePtr.Get());
            }

            HRESULT RemoveItems(_In_ UINT32 nIndex, _In_ UINT32 count)
            {
                return this->GetClassInterfaceInternal()->RemoveItems(nIndex, count);
            }

            HRESULT RemoveAt(UINT32 nIndex)
            {
                return this->GetClassInterfaceInternal()->RemoveItems(nIndex, 1);
            }

            HRESULT RemoveAll()
            {
                return this->GetClassInterfaceInternal()->RemoveAllInternal();
            }
            // Sort the Collection with a Function Compare parameter
            HRESULT sort(_In_ CompareItemValueFunc compare, void *Context = nullptr)
            {
                return this->GetInterfaceProtected()->Sort(compare, Context);
            }

            // Insert an Item In Order using a Function Compare parameter
            HRESULT InsertInOrder(_In_ foundation::IInspectable *pItem, _In_ CompareItemValueFunc compare, void *Context = nullptr)
            {
                return this->GetInterfaceProtected()->InsertInOrder(pItem, compare, Context, nullptr);
            }
            HRESULT GetItemConverter(_COM_Outptr_result_maybenull_ IValueConverter **ppValueConverter)
            {
                return this->GetClassInterfaceInternal()->GetItemConverter(ppValueConverter);
            }
            HRESULT SetItemConverter(_In_opt_ IValueConverter *pValueConverter)
            {
                return this->GetClassInterfaceInternal()->SetItemConverter(pValueConverter);
            }
            HRESULT FireCollectionChanged(
                foundation::NotifyCollectionModelChangedAction action,
                UINT32 oldStartingIndex,
                UINT32 newStartingIndex,
                UINT32 sizeOldItems,
                _In_opt_ foundation::IInspectable **ppOldItems,
                UINT32 sizeNewItems,
                _In_opt_ foundation::IInspectable **ppNewItems)
            {
                return this->GetClassInterfaceInternal()->FireCollectionChanged(
                    action,
                    oldStartingIndex,
                    newStartingIndex,
                    sizeOldItems,
                    ppOldItems,
                    sizeNewItems,
                    ppNewItems);
            }
        };
    }
}
