/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FastItemsContainer.cpp
****/

#include "pch.h"

#include "ArrayHelper.h"

#include "FastItemsContainer.h"
#include "FastContainerUtil.h"
#include <foundation/library/logger_macros.h>
#include <pmod/library/pmod_lib_api.h>

#include <algorithm>

using namespace foundation;
using namespace pmod;
using namespace pmod::library;

// {1C417ED7-82EA-43D3-A239-99F1D0D1CD82}
const GUID pmod::library::IID_IFastItemsContainer =
{ 0x1c417ed7, 0x82ea, 0x43d3,{ 0xa2, 0x39, 0x99, 0xf1, 0xd0, 0xd1, 0xcd, 0x82 } };

// Private export
EXTERN_C PMOD_LIB_API HRESULT  STDAPICALLTYPE CreateFastItemsContainer(
    const FastItemsContainerParameters *pParameters,
    pmod::library::IFastItemsContainer **ppPropertiesContainer)
{
    return CFastItemsContainer::CreateInstance(pParameters, ppPropertiesContainer);
}

static int __cdecl _CompareInspectablesValuesAdapter(
    foundation::IInspectable *elem1, 
    foundation::IInspectable *elem2, 
    void *Context)
{
    UNREFERENCED_PARAMETER(Context);
    return pv_util::CompareInspectableValues(elem1, elem2);
}
//------------------------------------------------------------------------------
// Class:   CFastItemsContainerIterator
// foundation::IIterator_impl<IInspectable*> implementation using CFastItemsContainer
//------------------------------------------------------------------------------
class CFastItemsContainerIterator :
    public foundation::ctl::ComInspectableBase,
    public foundation::IIterator_impl<foundation::IInspectable*>
{
public:
    static HRESULT CreateInstance(
        CFastItemsContainer *pItemsContainer,
        foundation::IIterator_impl<foundation::IInspectable*> **ppIterator)
    {
        foundation_assert(pItemsContainer);
        CFastItemsContainerIterator *pIterator = nullptr;
        foundation::ctl::ComInspectableObject<CFastItemsContainerIterator>::CreateInstance(&pIterator);
        pIterator->_pItemsContainer = pItemsContainer;
        // add ref
        foundation::ctl::AddRefInterface(pItemsContainer);

        *ppIterator = pIterator;
        return S_OK;
    }
protected:
    CFastItemsContainerIterator() :
        _pItemsContainer(nullptr),
        _current_pos(0)
    {}
    void FinalRelease()
    {
        // release our items container
        foundation::ctl::ReleaseInterface(_pItemsContainer);
    }
    STDMETHOD(GetCurrent)(_Outptr_ IInspectable* *current)
    {
        IFCPTR_ASSERT(current);

        return _pItemsContainer->Get(_current_pos, current);
    }
    STDMETHOD(GetHasCurrent)(_Outptr_ bool *hasCurrent)
    {
        IFCPTR_ASSERT(hasCurrent);
        *hasCurrent = _current_pos < _pItemsContainer->_itemsSize;
        return S_OK;
    }
    STDMETHOD(MoveNext)(_Outptr_ bool *hasCurrent)
    {
        IFCPTR_ASSERT(hasCurrent);
        ++_current_pos;
        *hasCurrent = _current_pos < _pItemsContainer->_itemsSize;
        return S_OK;
    }
private:
    CFastItemsContainer         *_pItemsContainer;
    UINT32                      _current_pos;
};

HRESULT CFastItemsContainer::CreateInstance(
    const FastItemsContainerParameters *pParameters,
    pmod::library::IFastItemsContainer **ppItemsContainerBase)
{
    CFastItemsContainer *pFastItemsContainer = nullptr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CFastItemsContainer>::CreateInstance(&pFastItemsContainer));
    IFR_ASSERT(pFastItemsContainer->_Initialize(pParameters));
    *ppItemsContainerBase = static_cast<pmod::library::IFastItemsContainer *>(pFastItemsContainer);
    return S_OK;
}

HRESULT CFastItemsContainer::_Initialize(
    const FastItemsContainerParameters *pParameters)
{
    _itemPropertyType = pParameters->_propertyType;
    _itemSlotPrefixSize = pParameters->_slotPrefixSize;
    _itemSlotSize = _itemSlotPrefixSize + CFastContainerUtil::GetSlotSize(_itemPropertyType);
    _itemCapacitySlotSize = pParameters->_itemCapacitySlotSize;
    if (pParameters->_initialSize)
    {
        IFR_ASSERT(AllocateSlotItems(pParameters->_initialSize));
    }
    _isDelegateWeakRef = IsOptionEnabled(pParameters->m_options, FastItemsContainerOptions::IsDelegateWeakRef);

    _pDelegate = pParameters->m_p_delegate;
    if (!_isDelegateWeakRef)
    {
        _pDelegate->AddRef();
    }

    return S_OK;
}

HRESULT CFastItemsContainer::AllocateSlotItems(UINT32 size)
{
    foundation_assert(_itemsSlotsAlloc == nullptr);

    // allocate our items slots
    LPVOID allocate_slots = AllocateSlots(size + _itemCapacitySlotSize, true);
    if (allocate_slots == nullptr)
    {
        return E_POINTER_;
    }
    _itemsSlotsAlloc = allocate_slots;
    _itemsSize = size;
    _itemsCapacitySize = size + _itemCapacitySlotSize;
    return S_OK;
}

LPVOID CFastItemsContainer::AllocateSlots(UINT32 size,bool zeroFlag)
{
    size_t alloc_size = _itemSlotSize * size;

    LPVOID alloc_buffer = _pal_MemAlloc((UINT32)alloc_size);
    if (alloc_buffer == nullptr)
    {
        // allocation has failed
        return nullptr;
    }
    // zero allocation
    if (zeroFlag)
    {
        memset(alloc_buffer, 0, alloc_size);
    }
    return alloc_buffer;
}


HRESULT CFastItemsContainer::DisposeInternal()
{
    if (_itemsSlotsAlloc)
    {
        for (UINT32 index = 0;
            index < _itemsSize;
            ++index)
        {
            LPVOID pItemSlotPtr = GetItemSlotValuePtr(index);

            OnClearSlotItem(index, pItemSlotPtr);
            CFastContainerUtil::ClearPropertySlot(
                _itemPropertyType, 
                pItemSlotPtr);
        }

        _pal_MemFree(_itemsSlotsAlloc);
        _itemsSlotsAlloc = nullptr;
        _itemsSize = 0;
        _itemsCapacitySize = 0;
    }
    return S_OK;
}

LPVOID CFastItemsContainer::GetItemSlotPtr(_In_ UINT32 index) const
{
    foundation_assert(_itemsSlotsAlloc != nullptr);
    return reinterpret_cast<UINT8 *>(_itemsSlotsAlloc)+(index*_itemSlotSize);
}

LPVOID CFastItemsContainer::GetItemSlotValuePtr(_In_ UINT32 index) const
{
    return reinterpret_cast<UINT8 *>(GetItemSlotPtr(index)) + _itemSlotPrefixSize;
}

STDMETHODIMP CFastItemsContainer::GetSlotItem(_In_ UINT32 index, _COM_Outptr_ foundation::IInspectable **value)
{
    if (!CheckBounds(index))
    {
        return E_BOUNDS_;
    }
    return CFastContainerUtil::GetPropertyValue(_itemPropertyType, GetItemSlotValuePtr(index), value);
}

STDMETHODIMP CFastItemsContainer::SetSlotItem(_In_ UINT32 index, _In_ foundation::IInspectable *value)
{
    if (!CheckBounds(index))
    {
        return E_BOUNDS_;
    }
    return CFastContainerUtil::SetPropertyValue(_itemPropertyType, GetItemSlotValuePtr(index), value);
}

HRESULT CFastItemsContainer::OnClearSlotItem(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr)
{
    if (_pDelegate != nullptr)
    {
        return _pDelegate->OnClearSlotItem(index, ToCustomSlotPtr(pItemSlotPtr));
    }
    return S_OK;
}

HRESULT CFastItemsContainer::GetItemInternal(_In_ UINT32 index,LPVOID pItemSlotPtr, foundation::IInspectable **value)
{
    if (_pDelegate != nullptr)
    {
        HRESULT hr = _pDelegate->OnGetSlotItem(index, ToCustomSlotPtr(pItemSlotPtr), value);
        IFHR(hr);
        if (hr == S_OK)
        {
            return S_OK;
        }
    }
    return CFastContainerUtil::GetPropertyValue(_itemPropertyType, pItemSlotPtr, value);
}

HRESULT CFastItemsContainer::SetItemInternal(_In_ UINT32 index,LPVOID pItemSlotPtr, foundation::IInspectable *value)
{
    if (_pDelegate != nullptr)
    {
        HRESULT hr = _pDelegate->OnSetSlotItem(index, ToCustomSlotPtr(pItemSlotPtr), value);
        IFHR(hr);
        if (hr == S_OK)
        {
            return S_OK;
        }
    }
    return CFastContainerUtil::SetPropertyValue(_itemPropertyType, pItemSlotPtr, value);
}

STDMETHODIMP CFastItemsContainer::GetCount(_Outptr_ UINT32 *pSize)
{
    IFCPTR_ASSERT(pSize);
    *pSize = _itemsSize;
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::Get(_In_ UINT32 index, foundation::IInspectable **value)
{
    if (!CheckBounds(index))
    {
        return E_BOUNDS_;
    }
    return GetItemInternal(index,GetItemSlotValuePtr(index), value);
}

STDMETHODIMP CFastItemsContainer::Set(_In_ UINT32 index, foundation::IInspectable *value)
{
    if (!CheckBounds(index))
    {
        return E_BOUNDS_;
    }
    LPVOID pItemSlotPtr = GetItemSlotValuePtr(index);
    OnClearSlotItem(index, pItemSlotPtr);
    return SetItemInternal(index, pItemSlotPtr, value);
}

STDMETHODIMP CFastItemsContainer::GetItems(_Outptr_ UINT32 *pSize, foundation::IInspectable ***items)
{
    _Inspectable_ptr_vector_type vector_items;
    ToVectorItems(vector_items);
    ContainerRefCountToArray(vector_items, pSize, items);
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::Append(_In_ foundation::IInspectable *value)
{
    IFR_ASSERT(InsertSlotItems(_itemsSize, 1));
    return Set(_itemsSize - 1, value);
}

STDMETHODIMP CFastItemsContainer::Insert(_In_ UINT32 index, _In_ foundation::IInspectable *value)
{
    IFR_ASSERT(InsertSlotItems(index, 1));
    return Set(index, value);
}

STDMETHODIMP CFastItemsContainer::Remove(_In_ foundation::IInspectable *value)
{
    UINT32 index = NINDEX;
    this->Find(value,nullptr,nullptr,&index);
    if (index != NINDEX)
    {
        return RemoveAt(index);
    }
    return S_FALSE;
}

STDMETHODIMP CFastItemsContainer::RemoveAt(_In_ UINT32 index)
{
    return RemoveSlotItems(index,1);
}

STDMETHODIMP CFastItemsContainer::Clear()
{
    IFR_ASSERT(DisposeInternal());
    _itemsSize = 0;
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::InsertPositionOf(
    _In_ foundation::IInspectable *pValue,
    _In_ pmod::library::CompareItemValueFunc compare,
    _In_ void *Context,
    _Outptr_ UINT32 *pIndex)
{
    const_iterator iter = std::lower_bound(
        begin(),
        end(),
        pValue,
        [this, pValue,compare, Context](LPVOID item1, LPVOID ){

        foundation::InspectablePtr itemPtr1;
        CFastContainerUtil::GetPropertyValue(
            this->_itemPropertyType,
            item1,
            itemPtr1.GetAddressOf());

        int cmp = (*compare)(itemPtr1, pValue, Context);
        return cmp < 0;
    });

    *pIndex = (UINT32)(iter.current_position());
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::Find(
    _In_ foundation::IInspectable *value, 
    _In_ pmod::library::CompareItemValueFunc compare, 
    _In_ void *Context, 
    _Outptr_ UINT32 *index)
{
    if (compare == nullptr)
    {
        compare = &_CompareInspectablesValuesAdapter;
    }
    UINT32 index_iter = 0;
    const_iterator iter = std::find_if(
        begin(),
        end(),
        [this, &index_iter, value, compare, Context](LPVOID item){

        foundation::InspectablePtr itemPtr;
        GetItemInternal(
            index_iter++,
            item,
            itemPtr.GetAddressOf());

        return (*compare)(value, itemPtr, Context) == 0;
    });
    if (iter.has_current())
    {
        *index = (UINT32)(iter.current_position());
    }
    else
    {
        *index = foundation::NINDEX;
    }
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::Sort(_In_ pmod::library::CompareItemValueFunc compare, _In_ void *Context)
{
    _Inspectable_ptr_vector_type vector_items;
    ToVectorItems(vector_items);

    std::sort(
        vector_items.begin(),
        vector_items.end(),
        [compare, Context](const foundation::InspectablePtr& item1, const foundation::InspectablePtr& item2){

        int cmp = (*compare)(item1.Get(), item2.Get(), Context);
        return cmp < 0;
    });

    for (_Inspectable_ptr_vector_type::iterator iter = vector_items.begin();
        iter != vector_items.end();
        ++iter)
    {
        UINT32 index = (UINT32)(iter - vector_items.begin());
        CFastContainerUtil::SetPropertyValue(
            _itemPropertyType,
            GetItemSlotValuePtr(index),
            *iter);
    }
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::First(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **first)
{
    return CFastItemsContainerIterator::CreateInstance(this, first);
}

STDMETHODIMP CFastItemsContainer::Clone(_COM_Outptr_ IItemsContainer **ppItemsContainer)
{
    FastItemsContainerParameters parameters;
    parameters._propertyType = this->_itemPropertyType;
    parameters._slotPrefixSize = this->_itemSlotPrefixSize;
    parameters._itemCapacitySlotSize = this->_itemCapacitySlotSize;
    parameters._initialSize = this->_itemsSize;

    IFR_ASSERT(CFastItemsContainer::CreateInstance(
        &parameters,
        reinterpret_cast<pmod::library::IFastItemsContainer **>(ppItemsContainer)));

    for (UINT32 index = 0; index < this->_itemsSize;++index)
    {
        foundation::InspectablePtr valuePtr;
        this->Get(index, valuePtr.GetAddressOf());
        (*ppItemsContainer)->Set(index, valuePtr);
    }
    return S_OK;
}

void CFastItemsContainer::ToVectorItems(_Inspectable_ptr_vector_type& vector_items)
{
    vector_items.resize(_itemsSize);
    for (_Inspectable_ptr_vector_type::iterator iter = vector_items.begin();
        iter != vector_items.end();
        ++iter)
    {
        UINT32 index = (UINT32)(iter - vector_items.begin());
        GetItemInternal(
            index,
            GetItemSlotValuePtr(index), 
            (*iter).GetAddressOf());
    }
}

STDMETHODIMP CFastItemsContainer::ReadCustomData(_In_ UINT32 index, LPVOID pCustomData)
{
    ReadCustomData(pCustomData, GetItemSlotPtr(index));
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::WriteCustomData(_In_ UINT32 index, LPVOID pCustomData)
{
    WriteCustomData(pCustomData, GetItemSlotPtr(index));
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::ResizeSlotItems(UINT32 size)
{
    DisposeInternal();
    IFR_ASSERT(AllocateSlotItems(size));
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::InsertSlotItems(_In_ UINT32 index, UINT32 size)
{
    if (index == NINDEX)
    {
        index = _itemsSize;
    }
    else if (index > _itemsSize)
    {
        return E_BOUNDS_;
    }

    if ((_itemsSize + size) <= _itemsCapacitySize)
    {
        // we still have capacity available
        if (index < _itemsSize)
        {
            memmove(
                reinterpret_cast<UINT8 *>(_itemsSlotsAlloc)+((index + size) * _itemSlotSize),
                GetItemSlotPtr(index),
                (_itemsSize - index) * _itemSlotSize);
            // zero the slots being inserted in this pos
            memset(
                reinterpret_cast<UINT8 *>(_itemsSlotsAlloc)+(index * _itemSlotSize),
                0,
                size * _itemSlotSize);
        }
        _itemsSize += size;
        return S_OK;
    }

    // allocate our items slots
    LPVOID newItemsSlotsAlloc = AllocateSlots(_itemsSize + size + _itemCapacitySlotSize,true);
    if (newItemsSlotsAlloc == nullptr)
    {
        return E_POINTER_;
    }

    // copy items before the insertion index
    if (index > 0)
    {
        memcpy(newItemsSlotsAlloc,
            _itemsSlotsAlloc,
            _itemSlotSize * index);
    }
    // copy items after the insertion index
    if (index < _itemsSize)
    {
        memcpy(
            reinterpret_cast<UINT8 *>(newItemsSlotsAlloc) + ((index + size) * _itemSlotSize),
            GetItemSlotPtr(index),
            (_itemsSize - index) * _itemSlotSize);
    }

    // adjust new size
    _itemsSize += size;
    _itemsCapacitySize = _itemsSize + _itemCapacitySlotSize;

    // swap slot allocators
    if (_itemsSlotsAlloc != nullptr)
    {
        _pal_MemFree(_itemsSlotsAlloc);
    }
    _itemsSlotsAlloc = newItemsSlotsAlloc;
    return S_OK;
}

STDMETHODIMP CFastItemsContainer::RemoveSlotItems(_In_ UINT32 index, UINT32 size)
{
    if (!CheckBounds(index) || (index + size) > _itemsSize)
    {
        return E_BOUNDS_;
    }

    // clear removed slots
    for (UINT32 indexSlot = 0; indexSlot < size; ++indexSlot)
    {
        LPVOID pItemSlotPtr = GetItemSlotValuePtr(index + indexSlot);
        OnClearSlotItem(index, pItemSlotPtr);
        CFastContainerUtil::ClearPropertySlot(
            _itemPropertyType,
            pItemSlotPtr);
    }

    if ((_itemsSize - size + _itemCapacitySlotSize) >=_itemsCapacitySize)
    {
        // preserve the capacity
        if ((index + size) < _itemsSize)
        {
            UINT32  numberOfItems = _itemsSize - (index + size);
            memmove(
                GetItemSlotPtr(index),
                GetItemSlotPtr(index + size),
                numberOfItems * _itemSlotSize);
            // zero the remaining slots at the end
            memset(
                GetItemSlotPtr(index + numberOfItems),
                0,
                size * _itemSlotSize);
        }
        _itemsSize -= size;
        return S_OK;
    }

    // allocate our items slots
    LPVOID newItemsSlotsAlloc = AllocateSlots(_itemsSize - size + _itemCapacitySlotSize, true);
    if (newItemsSlotsAlloc == nullptr)
    {
        return E_POINTER_;
    }

    // copy items before the deletion index
    if (index > 0)
    {
        memcpy(newItemsSlotsAlloc,
            _itemsSlotsAlloc,
            _itemSlotSize * index);
    }
    // copy items after the deletion index
    if ((index + size) < _itemsSize)
    {
        memcpy(
            reinterpret_cast<UINT8 *>(newItemsSlotsAlloc)+(index * _itemSlotSize),
            GetItemSlotPtr(index + size),
            (_itemsSize - (index + size)) * _itemSlotSize);
    }

    // adjust new size
    _itemsSize -= size;
    _itemsCapacitySize = _itemsSize + _itemCapacitySlotSize;
    // swap slot allocators
    _pal_MemFree(_itemsSlotsAlloc);
    _itemsSlotsAlloc = newItemsSlotsAlloc;
    return S_OK;
}

