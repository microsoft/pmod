/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FastItemsContainer.h
****/
#pragma once

#include "FoundationTypes.h"

#include <foundation/ctl/com_library.h>
#include <pmod/library/model_library.h>
#include <pmod/library/interfaces/fast_items_container_parameters.h>

//------------------------------------------------------------------------------
// Class:   CFastItemsContainer
// IFastItemsContainer 'fast' implementation for a Static container
//------------------------------------------------------------------------------
class CFastItemsContainer :
    public foundation::ctl::ImplementsInspectable
    <
    pmod::library::IFastItemsContainer,
    &pmod::library::IID_IFastItemsContainer
    >
{
public:
    static HRESULT CreateInstance(
        const pmod::library::FastItemsContainerParameters *pParameters,
        pmod::library::IFastItemsContainer **ppItemsContainerBase);
protected:
    CFastItemsContainer() :
        _itemPropertyType(foundation::PropertyType_Empty),
        _itemsCapacitySize(0),
        _itemsSize(0),
        _itemCapacitySlotSize(0),
        _itemSlotSize(0),
        _itemSlotPrefixSize(0),
        _itemsSlotsAlloc(nullptr),
        _isDelegateWeakRef(false),
        _pDelegate(nullptr)
    {
    }

    // IItemsContainerBase Interface
    STDMETHOD(GetCount)(_Outptr_ UINT32 *pSize);
    STDMETHOD(Get)(_In_ UINT32 index, _COM_Outptr_ foundation::IInspectable **value);
    STDMETHOD(Set)(_In_ UINT32 index, _In_ foundation::IInspectable *value);
    STDMETHOD(GetItems)(_Outptr_ UINT32 *pSize, foundation::IInspectable ***items);
    STDMETHOD(Append)(_In_ foundation::IInspectable *value);
    STDMETHOD(Insert)(_In_ UINT32 index, _In_ foundation::IInspectable *value);
    STDMETHOD(Remove)(_In_ foundation::IInspectable *value);
    STDMETHOD(RemoveAt)(_In_ UINT32 index);
    STDMETHOD(InsertPositionOf)(
        _In_ foundation::IInspectable *value, 
        _In_ pmod::library::CompareItemValueFunc compare,
        _In_ void *Context, _Outptr_ UINT32 *index);
    STDMETHOD(Find)(_In_ foundation::IInspectable *value, _In_ pmod::library::CompareItemValueFunc compare, _In_ void *Context, _Outptr_ UINT32 *index);
    STDMETHOD(Sort)(_In_ pmod::library::CompareItemValueFunc compare, _In_ void *Context);
    STDMETHOD(Clear)();
    STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **first);
    STDMETHOD(Clone)(_COM_Outptr_ IItemsContainer **ppItemsContainer);

    // IDisposable Interface
    STDMETHOD(Dispose)()
    {
        return DisposeInternal();
    }

    // IFastItemsContainer Interface
    STDMETHOD(ReadCustomData)(_In_ UINT32 index, LPVOID pCustomData);
    STDMETHOD(WriteCustomData)(_In_ UINT32 index, LPVOID pCustomData);
    STDMETHOD(GetSlotItem)(_In_ UINT32 index, _COM_Outptr_ foundation::IInspectable **value);
    STDMETHOD(SetSlotItem)(_In_ UINT32 index, _In_ foundation::IInspectable *value);

    STDMETHOD(ResizeSlotItems)(_In_ UINT32 size);
    STDMETHOD(InsertSlotItems)(_In_ UINT32 index, UINT32 size);
    STDMETHOD(RemoveSlotItems)(_In_ UINT32 index, UINT32 size);

    // ComBase Interface
    void FinalRelease() override
    {
        DisposeInternal();
        if (_pDelegate && !_isDelegateWeakRef)
        {
            RELEASE_INTERFACE(_pDelegate);
        }
    }

    // internal methods
    HRESULT GetItemInternal(_In_ UINT32 index,LPVOID pItemSlotPtr, foundation::IInspectable **value);
    HRESULT SetItemInternal(_In_ UINT32 index, LPVOID pItemSlotPtr, foundation::IInspectable *value);

    foundation::PropertyType GetItemPropertyType()
    {
        return _itemPropertyType;
    }
    HRESULT _Initialize(
        const pmod::library::FastItemsContainerParameters* pParameters);

private:
    HRESULT AllocateSlotItems(UINT32 size);
    LPVOID AllocateSlots(UINT32 size, bool zeroFlag);
    friend class CFastItemsContainerIterator;
    class const_iterator : public std::iterator<std::forward_iterator_tag, LPVOID>
    {
    public:
        inline const_iterator(const CFastItemsContainer *pItemsContainer, unsigned int pos) :
            _pItemsContainer(pItemsContainer),
            _current_position(pos)
        {
        }
        inline const_iterator():
            _pItemsContainer(nullptr),
            _current_position((unsigned int)-1)
        {
        }

        inline LPVOID operator*() const
        {
            return _pItemsContainer ? _pItemsContainer->GetItemSlotValuePtr(_current_position):nullptr;
        }

        inline const_iterator& operator++()
        {
            this->_current_position++;
            return *this;
        }

        const_iterator& operator=(const const_iterator& other)
        {
            if (*this != other)
            {
                this->_current_position = other._current_position;
            }
            return *this;
        }

        bool operator==(const const_iterator& rValue) const
        {
            bool lHasCurrent = has_current();

            bool rHasCurrent = rValue.has_current();

            if (!lHasCurrent && !rHasCurrent) return true;
            if (lHasCurrent != rHasCurrent) return false;

            return this->_current_position == rValue._current_position;
        }

        inline bool has_current() const
        {
            return _pItemsContainer && _current_position != (unsigned int)-1 &&
                _current_position < _pItemsContainer->_itemsSize;
        }

        inline bool operator!=(const const_iterator& rValue) const
        {
            return !this->operator==(rValue);
        }
        inline unsigned int current_position() const { return _current_position; }
    private:
        const CFastItemsContainer *_pItemsContainer;
        unsigned int _current_position;
    };

    const_iterator begin() const
    {
        return const_iterator(this,0);
    }

    const_iterator end() const
    {
        return const_iterator();
    }

    void ToVectorItems(_Inspectable_ptr_vector_type& vector_items);

    HRESULT DisposeInternal();
    LPVOID GetItemSlotPtr(_In_ UINT32 index) const;
    LPVOID GetItemSlotValuePtr(_In_ UINT32 index) const;

    HRESULT OnClearSlotItem(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr);

    inline bool CheckBounds(UINT32 index)
    {
        return index < _itemsSize;
    }
    inline void ReadCustomData(void *_Dest, LPVOID pCustomItemSlotPtr)
    {
        memcpy(_Dest, pCustomItemSlotPtr, _itemSlotPrefixSize);
    }
    inline void WriteCustomData(void *_Src, LPVOID pCustomItemSlotPtr)
    {
        memcpy(pCustomItemSlotPtr, _Src, _itemSlotPrefixSize);
    }
    inline LPVOID ToCustomSlotPtr(LPVOID pItemSlotPtr)
    {
        return reinterpret_cast<UINT8 *>(pItemSlotPtr)-_itemSlotPrefixSize;
    }
private:
    foundation::PropertyType        _itemPropertyType;
    UINT32                          _itemsCapacitySize;
    UINT32                          _itemsSize;
    UINT32                          _itemCapacitySlotSize;
    size_t                          _itemSlotSize;
    UINT32                          _itemSlotPrefixSize;
    LPVOID                          _itemsSlotsAlloc;
    bool                            _isDelegateWeakRef;
    pmod::library::IFastItemsContainerDelegate *_pDelegate;
};
