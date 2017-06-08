/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection_ptr.h
****/
#pragma once

#include "object_node_ptr.h"
#include <foundation/pv_util.h>
#include <pmod/interfaces.h>
#include <iterator>

namespace pmod
{

template <
    class T, 
    class TPublicInterface,
    class _Item_wrapper_alloc = foundation::_ValueAlloc<typename TPublicInterface::_Item_Type >>
class _ObservableCollectionPtrWrapper : public _ObjectNodePtrWrapper<T, TPublicInterface>
{
public:
    UINT32 GetCount()
    {
        UINT32 count = 0;
        IGNOREHR(this->GetClassInterface()->GetCount(&count));
        return count;
    }

    HRESULT GetItem(UINT32 index, typename TPublicInterface::_Item_Type *item)
    {
        return this->GetClassInterface()->GetItem(index, item);
    }

    template <class array_wrapper_t>
    HRESULT GetItems(array_wrapper_t& items)
    {
        items.clear();
        return this->GetClassInterface()->GetItems(items.GetSizeAddressOf(), items.GetBufferAddressOf());
    }

    HRESULT IndexOf(_In_ typename TPublicInterface::_Item_Type item, _Outptr_ UINT32* pnIndex)
    {
        return this->GetClassInterface()->IndexOf(item, pnIndex);
    }

    class const_item_iterator : public std::iterator<std::forward_iterator_tag, typename TPublicInterface::_Item_Type>
    {
    public:
        inline const_item_iterator(
            TPublicInterface *pT) :
            _current_position(0)
        {
            pT->First(_foundationIterator.GetAddressOf());
        }

        inline const_item_iterator() : _current_position(0)
        {
        }

        inline const_item_iterator(const const_item_iterator& rValue) : _current_position(rValue._current_position)
        {
            (const_cast<const_item_iterator&>(rValue))._foundationIterator.CopyTo(this->_foundationIterator.GetAddressOf());
        }

        typename TPublicInterface::_Item_Type* operator->()
        {
            typename TPublicInterface::_Item_Type *pItem = _current_item.relase_and_get_address_of();
            _foundationIterator->GetCurrent(pItem);
            return pItem;
        }

        inline const typename TPublicInterface::_Item_Type& operator*()
        {
            return *this->operator->();
        }

        inline const_item_iterator& operator++()
        {
            if (!!_foundationIterator)
            {
                bool hasCurrent;
                _foundationIterator->MoveNext(&hasCurrent);
            }
            this->_current_position++;
            return *this;
        }

        inline const_item_iterator operator++(int)
        {
            const_item_iterator tmp = *this;
            this->operator++();
            return tmp;
        }

        bool operator==(const const_item_iterator& rValue) const
        {
            bool lHasCurrent = false;

            if (!!_foundationIterator)
            {
                _foundationIterator->GetHasCurrent(&lHasCurrent);
            }

            bool rHasCurrent = false;
            if (rValue._foundationIterator != nullptr)
            {
                rValue._foundationIterator->GetHasCurrent(&rHasCurrent);
            }

            if (!lHasCurrent && !rHasCurrent) return true;
            if (lHasCurrent != rHasCurrent) return false;

            return this->_current_position == rValue._current_position;
        }

        inline bool operator!=(const const_item_iterator& rValue) const
        {
            return !this->operator==(rValue);
        }

    private:
        // The original collection and the current position data:
        foundation::ComPtr<foundation::IIterator_impl<typename TPublicInterface::_Item_Type> > _foundationIterator;
        unsigned int _current_position;
        foundation::_PropertyValueWrapper<typename TPublicInterface::_Item_Type, _Item_wrapper_alloc> _current_item;
    };

    const_item_iterator begin_items() const
    {
        return const_item_iterator(this->GetClassInterface());
    }

    const_item_iterator end_items() const
    {
        return const_item_iterator();
    }
};

template <class T, class _Item_wrapper_alloc = foundation::_ValueAlloc<typename T::_Item_Type> >
class _ObservableCollectionPtr : 
    public foundation::_ObjectPtr
    <
        _ObservableCollectionPtrWrapper<_ObservableCollectionPtr<T, _Item_wrapper_alloc>, T, _Item_wrapper_alloc>,
        T
    >
{
public:
    typedef foundation::_ObjectPtr
        <
        _ObservableCollectionPtrWrapper<_ObservableCollectionPtr<T, _Item_wrapper_alloc>, T, _Item_wrapper_alloc>,
        T
        > _Base_Type;

    typedef typename _Base_Type::const_item_iterator const_iterator;

    _ObservableCollectionPtr() throw()
    {
    }
    _ObservableCollectionPtr(_Inout_ const _ObservableCollectionPtr<T, _Item_wrapper_alloc>& other) throw() :
        _Base_Type(other)
    {
    }
    _ObservableCollectionPtr(_Inout_ const foundation::ComPtr<T>& lp) throw() :
        _Base_Type(lp)
    {
    }
    _ObservableCollectionPtr(_In_ T* lp) throw() :
        _Base_Type(lp)
    {
    }

    const_iterator begin() const
    {
        return _Base_Type::begin_items();
    }

    const_iterator end() const
    {
        return _Base_Type::end_items();
    }
};

template <class T> 
class _RefCountedObservableCollectionPtr : public _ObservableCollectionPtr<T, foundation::_RefCountedValueAlloc<typename T::_Item_Type>>
{
public:
    typedef _ObservableCollectionPtr<T, foundation::_RefCountedValueAlloc<typename T::_Item_Type>> _My_Base;

    _RefCountedObservableCollectionPtr() throw()
    {
    }
    _RefCountedObservableCollectionPtr(_Inout_ const foundation::_ComPtrBase<T>& lp) throw() :
        _My_Base(lp.get())
    {
    }
    _RefCountedObservableCollectionPtr(_In_ T* lp) throw() :
        _My_Base(lp)
    {
    }
};


typedef _RefCountedObservableCollectionPtr<IObservableCollection> ObservableCollectionInspectablePtr;
typedef _RefCountedObservableCollectionPtr<IObservableList> ObservableListInspectablePtr;
typedef _ObservableCollectionPtr<IObservableCollection_String, foundation::_HStringValueAlloc> ObservableCollectionStringPtr;
typedef _ObservableCollectionPtr<IObservableCollection_Int32> ObservableCollectionInt32Ptr;
typedef _ObservableCollectionPtr<IObservableCollection_UInt32> ObservableCollectionUInt32Ptr;
typedef _ObservableCollectionPtr<IObservableCollection_Int16> ObservableCollectionInt16Ptr;
typedef _ObservableCollectionPtr<IObservableCollection_UInt16> ObservableCollectionUInt16Ptr;
typedef _ObservableCollectionPtr<IObservableCollection_Int64> ObservableCollectionInt64Ptr;
typedef _ObservableCollectionPtr<IObservableCollection_UInt64> ObservableCollectionUInt64Ptr;
typedef _ObservableCollectionPtr<IObservableCollection_UInt8> ObservableCollectionUInt8Ptr;
typedef _ObservableCollectionPtr<IObservableCollection_Boolean> ObservableCollectionBooleanPtr;

}

