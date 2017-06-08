/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection_stl.h
****/
#pragma once

#include <vector>
#include <set>
#include <algorithm>

#include "mtl.h"
#include "observable_collection_impl.h"
#include "interfaces/observable_collection_class.h"

namespace pmod { namespace library
{

//------------------------------------------------------------------------------
// Class:   items_container_iterator
// foundation::IIterator_impl<IInspectable*> implementation using std::iterator
//------------------------------------------------------------------------------
template <class T>
class items_container_iterator :
    public foundation::ctl::ComInspectableBase,
    public foundation::IIterator_impl<foundation::IInspectable*>
{
public:
    static HRESULT CreateInstance(
        pmod::library::IItemsContainer *pItemsContainer,
        const T& items,
        _COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **ppIterator)
    {
        foundation_assert(pItemsContainer);
        items_container_iterator<T> *pIterator = nullptr;
        foundation::ctl::ComInspectableObject<items_container_iterator<T>>::CreateInstance(&pIterator, items);
        pIterator->_pItemsContainer = pItemsContainer;
        pIterator->_iterator = items.begin();
        // add ref
        pItemsContainer->AddRef();

        *ppIterator = pIterator;
        return S_OK;
    }
protected:
    items_container_iterator(const T& items) :
        _pItemsContainer(nullptr),
        _items(items)
    {}
    void FinalRelease()
    {
        // release our items container
        RELEASE_INTERFACE(_pItemsContainer);
    }
    STDMETHOD(GetCurrent)(_Outptr_ IInspectable* *current)
    {
        IFCPTR_ASSERT(current);
        *current = foundation::CreateValue(*_iterator).Detach();
        return S_OK;
    }
    STDMETHOD(GetHasCurrent)(_Out_ bool *hasCurrent)
    {
        IFCPTR_ASSERT(hasCurrent);
        *hasCurrent = _iterator != _items.end();
        return S_OK;
    }
    STDMETHOD(MoveNext)(_Out_ bool *hasCurrent)
    {
        IFCPTR_ASSERT(hasCurrent);
        ++_iterator;
        *hasCurrent = _iterator != _items.end();
        return S_OK;
    }
private:
    typedef typename T::const_iterator _TIterator_Type;
    pmod::library::IItemsContainer *_pItemsContainer;
    const T& _items;
    _TIterator_Type _iterator;
};
//------------------------------------------------------------------------------
// Class:   vector_items_container
// IItemsContainer implementation using std::vector
//------------------------------------------------------------------------------
template <
    class T,
    class _Unbox_adapter_t = foundation::library::_UnboxAdapter>
class vector_items_container :
    public foundation::ctl::ComInspectableBase,
    public IItemsContainer
{
public:
    typedef vector_items_container<T, _Unbox_adapter_t> _ThisType;

    template <class TContainer>
    static HRESULT create_instance_type(std::vector<T>& items, _COM_Outptr_ IItemsContainer ** ppItemsContainer)
    {
        TContainer *pT;
        foundation::ctl::ComInspectableObject<TContainer>::CreateInstance(&pT, items);
        * ppItemsContainer = static_cast<IItemsContainer *>(pT);
        return S_OK;
    }

    static HRESULT CreateInstance(std::vector<T>& items, _COM_Outptr_ IItemsContainer ** ppItemsContainer)
    {
        return create_instance_type<_ThisType>(items, ppItemsContainer);
    }
protected:
    vector_items_container(std::vector<T>& items):
        _delete_items(false)
    {
        _pitems = &items;
    }

    vector_items_container() :
        _pitems(nullptr),
        _delete_items(false)
    {
    }

    void FinalRelease()
    {
        if (_delete_items)
        {
            delete _pitems;
            _pitems = nullptr;
        }
    }

    STDMETHOD(GetCount)(_Out_ UINT32 *pSize)
    {
        IFCPTR_ASSERT(_pitems);

        *pSize = (UINT32)_pitems->size();
        return S_OK;
    }

    STDMETHOD(GetItems)(_Out_ UINT32 *pSize, foundation::IInspectable ***items)
    {
        IFCPTR_ASSERT(_pitems);
        foundation_assert(pSize);
        foundation_assert(items);

        *pSize = (UINT32)_pitems->size();
        *items = (foundation::IInspectable **)_pal_MemAlloc((UINT32)(_pitems->size() * sizeof(foundation::IInspectable *)));

        foundation::IInspectable **ppItems = *items;
        for(typename std::vector<T>::const_iterator iter = _pitems->begin();
            iter != _pitems->end();
            ++iter)
        {
            *ppItems = foundation::CreateValue(*iter).Detach();
            ++ppItems;
        }
        return S_OK;
    }

    STDMETHOD(Get)(_In_ UINT32 index, _COM_Outptr_ foundation::IInspectable **value)
    {
        IFCPTR_ASSERT(_pitems);
        *value = foundation::CreateValue(_pitems->operator[](index)).Detach();
        return S_OK;
    }

    STDMETHOD(Set)(_In_ UINT32 index, _In_ foundation::IInspectable *value)
    {
        IFCPTR_ASSERT(_pitems);
        T value_t;
        IFR_ASSERT(_Unbox_adapter_t::_From_inspectable_value(value, value_t));
        (*_pitems)[index] = value_t;
        return S_OK;
    }

    STDMETHOD(Append)(_In_ foundation::IInspectable *value)
    {
        IFCPTR_ASSERT(_pitems);
        T value_t;
        IFR_ASSERT(_Unbox_adapter_t::_From_inspectable_value(value, value_t));

        _pitems->push_back(value_t);
        return S_OK;
    }

    STDMETHOD(Insert)(_In_ UINT32 index,_In_ foundation::IInspectable *value)
    {
        IFCPTR_ASSERT(_pitems);
        T value_t;
        IFR_ASSERT(_Unbox_adapter_t::_From_inspectable_value(value, value_t));

        _pitems->insert(_pitems->begin() + index, value_t);
        return S_OK;
    }
    STDMETHOD(Remove)(_In_ foundation::IInspectable *value)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(RemoveAt)(_In_ UINT32 index)
    {
        IFCPTR_ASSERT(_pitems);
        _pitems->erase(_pitems->begin() + index);
        return S_OK;
    }

    STDMETHOD(InsertPositionOf)(
        _In_ foundation::IInspectable *pValue,
        _In_ CompareItemValueFunc compare,
        _In_opt_ void *Context,
        _Out_ UINT32 *pIndex)
    {
        IFCPTR_ASSERT(_pitems);
        T value;
        IFR_ASSERT(_Unbox_adapter_t::_From_inspectable_value(pValue, value));

        typename std::vector<T>::iterator iter = std::lower_bound(
            _pitems->begin(),
            _pitems->end(),
            value,
            [compare,Context](const T& item1,const T& item2){
            
            foundation::InspectablePtr itemPtr1 = foundation::CreateValue(item1);
            foundation::InspectablePtr itemPtr2 = foundation::CreateValue(item2);
              
            return (*compare)(itemPtr1,itemPtr2,Context) < 0;
        });

        *pIndex = (UINT32)(iter - _pitems->begin());
        return S_OK;
    }

    STDMETHOD(Find)(
        _In_ foundation::IInspectable *value,
        _In_ CompareItemValueFunc compare,
        _In_opt_ void *Context,
        _Out_ UINT32 *pIndex)
    {
        IFCPTR_ASSERT(_pitems);
        typename std::vector<T>::iterator iter = std::find_if(
            _pitems->begin(),
            _pitems->end(),
            [value,compare,Context](const T& item){
            
            foundation::InspectablePtr itemPtr;
            itemPtr = foundation::CreateValue(item);
            return (*compare)(value,itemPtr,Context)==0;
        });

        if(iter != _pitems->end())
        {
            *pIndex = (UINT32)(iter - _pitems->begin());
        }
        else
        {
            *pIndex = foundation::NINDEX;
        }

        return S_OK;
    }

    STDMETHOD(Sort)(_In_ CompareItemValueFunc compare, _In_opt_ void *Context)
    {
        std::sort(
            _pitems->begin(),
            _pitems->end(),
            [compare,Context](const T& item1,const T& item2){
            
            foundation::InspectablePtr itemPtr1 = foundation::CreateValue(item1);
            foundation::InspectablePtr itemPtr2 = foundation::CreateValue(item2);

            int cmp = (*compare)(itemPtr1, itemPtr2, Context);
            return cmp < 0;
        });

        return S_OK;
    }

    STDMETHOD(Clear)()
    {
        IFCPTR_ASSERT(_pitems);
        _pitems->clear();
        return S_OK;
    }

    STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **first)
    {
        IFCPTR_ASSERT(first);
        IFCPTR_ASSERT(_pitems);

        return items_container_iterator<std::vector<T>>::CreateInstance(this, *_pitems, first);
    }

    STDMETHOD(Clone)(_COM_Outptr_ IItemsContainer **ppItemsContainer)
    {
        IFCPTR_ASSERT(ppItemsContainer);
        *ppItemsContainer = nullptr;

        _ThisType *pT;
        foundation::ctl::ComInspectableObject<_ThisType>::CreateInstance(&pT);
        pT->_pitems = new std::vector<T>();
        pT->_delete_items = true;
        // copy items
        *pT->_pitems = *this->_pitems;

        *ppItemsContainer = static_cast<IItemsContainer*>(pT);
        return S_OK;
    }

    STDMETHOD(Dispose)()
    {
        if(_pitems)
        {
            _pitems->clear();
            _pitems = nullptr;
        }
        return S_OK;
    }

private:
    std::vector<T>* _pitems;
    bool            _delete_items;
};

//------------------------------------------------------------------------------
// Class:   set_items_container
// IItemsContainer implementation using std::set
//------------------------------------------------------------------------------
template <
    class T,
    class _Unbox_adapter_t = foundation::library::_UnboxAdapter>
class set_items_container :
    public foundation::ctl::ComInspectableBase,
    public pmod::library::IItemsContainer
{
public:
    typedef set_items_container<T, _Unbox_adapter_t> _ThisType;

    template <class TContainer>
    static HRESULT create_instance_type(std::set<T>& items,IItemsContainer ** ppItemsContainer)
    {
        TContainer *pT;
        foundation::ctl::ComInspectableObject<TContainer>::CreateInstance(&pT, items);
        * ppItemsContainer = static_cast<IItemsContainer *>(pT);
        return S_OK;
    }

    static HRESULT CreateInstance(std::set<T>& items,IItemsContainer ** ppItemsContainer)
    {
        return create_instance_type<_ThisType>(items,ppItemsContainer);
    }

protected:
    set_items_container(std::set<T>& items):
        _delete_items(false)
    {
        _pitems = &items;
        _Reset_items_iterator();
    }

    set_items_container() :
        _pitems(nullptr),
        _delete_items(false)
    {
    }

    void FinalRelease()
    {
        if (_delete_items)
        {
            delete _pitems;
            _pitems = nullptr;
        }
    }

    STDMETHOD(GetCount)(_Out_ UINT32 *pSize)
    {
        IFCPTR_ASSERT(_pitems);

        *pSize = (UINT32)_pitems->size();
        return S_OK;
    }

    STDMETHOD(GetItems)(_Out_ UINT32 *pSize, foundation::IInspectable ***items)
    {
        IFCPTR_ASSERT(_pitems);
        foundation_assert(pSize);
        foundation_assert(items);

        *pSize = (UINT32)_pitems->size();
        *items = (foundation::IInspectable **)_pal_MemAlloc((UINT32)(_pitems->size() * sizeof(foundation::IInspectable *)));

        foundation::IInspectable **ppItems = *items;
        for(typename std::set<T>::const_iterator iter = _pitems->begin();
            iter != _pitems->end();
            ++iter)
        {
            *ppItems = foundation::CreateValue(*iter).Detach();
            ++ppItems;
        }
        return S_OK;
    }

    STDMETHOD(Get)(_In_ UINT32 index,_COM_Outptr_ foundation::IInspectable **value)
    {
        IFCPTR_ASSERT(_pitems);
        UINT32 iter_index = (UINT32)std::distance(_pitems->begin(), _items_iterator);
        if (iter_index < index)
        {
            advance_items_iterator(index - iter_index);
        }
        else if (iter_index > index)
        {
            backward_items_iterator(iter_index - index);
        }
        *value = foundation::CreateValue(*_items_iterator).Detach();
        return S_OK;
    }

    STDMETHOD(Set)(_In_ UINT32 index, _In_ foundation::IInspectable *value)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(Append)(_In_ foundation::IInspectable *value)
    {
        IFCPTR_ASSERT(_pitems);
        T value_t;
        IFR_ASSERT(_Unbox_adapter_t::_From_inspectable_value(value, value_t));

        auto result =_pitems->insert(value_t);
        HRESULT hr = result.second ? S_OK : S_FALSE;
        _Reset_items_iterator();
        return hr;
    }

    STDMETHOD(Insert)(_In_ UINT32 index,_In_ foundation::IInspectable *value)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(Remove)(_In_ foundation::IInspectable *value) 
    {
        IFCPTR_ASSERT(_pitems);
        T value_t;
        IFR_ASSERT(_Unbox_adapter_t::_From_inspectable_value(value, value_t));

        return _pitems->erase(value_t) ? S_OK:S_FALSE;
    }

    STDMETHOD(RemoveAt)(_In_ UINT32 index)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(InsertPositionOf)(
        _In_ foundation::IInspectable *value,
        _In_ CompareItemValueFunc compare,
        _In_opt_ void *Context,
        _Out_ UINT32 *pIndex)
    {
        foundation_assert(pIndex);
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(Find)(
        _In_ foundation::IInspectable *value,
        _In_ CompareItemValueFunc compare,
        _In_opt_ void *Context,
        _Out_ UINT32 *pIndex)
    {
        UNREFERENCED_PARAMETER(compare);
        UNREFERENCED_PARAMETER(Context);

        foundation_assert(pIndex);

        T value_t;
        IFR_ASSERT(_Unbox_adapter_t::_From_inspectable_value(value, value_t));


        typename std::set<T>::const_iterator iter = _pitems->find(value_t);
        if(iter != _pitems->end())
        {
            *pIndex = (UINT32)std::distance(_pitems->begin(), iter);
        }
        return S_OK;
    }

    STDMETHOD(Sort)(_In_ CompareItemValueFunc compare, _In_opt_ void *Context)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(Clear)()
    {
        _pitems->clear();
        _Reset_items_iterator();
        return S_OK;
    }

    STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **first)
    {
        IFCPTR_ASSERT(first);
        IFCPTR_ASSERT(_pitems);
        return items_container_iterator<std::set<T>>::CreateInstance(this, *_pitems, first);
    }

    STDMETHOD(Clone)(_COM_Outptr_ IItemsContainer **ppItemsContainer)
    {
        IFCPTR_ASSERT(ppItemsContainer);

        _ThisType *pT;
        foundation::ctl::ComInspectableObject<_ThisType>::CreateInstance(&pT);
        pT->_pitems = new std::set <T>();
        pT->_delete_items = true;
        // copy items
        *pT->_pitems = *this->_pitems;

        *ppItemsContainer = static_cast<IItemsContainer *>(pT);
        return S_OK;
    }

    STDMETHOD(Dispose)()
    {
        if(_pitems != nullptr)
        {
            _pitems->clear();
            _pitems = nullptr;
        }
        return S_OK;
    }
public:
    inline void _Reset_items_iterator()
    {
        _items_iterator = _pitems->begin();
    }
private:
    inline void advance_items_iterator(UINT32 size)
    {
        while (size--)
        {
            ++_items_iterator;
        }
    }
    inline void backward_items_iterator(UINT32 size)
    {
        while (size--)
        {
            --_items_iterator;
        }
    }
private:
    std::set<T> *_pitems;
    bool _delete_items;
    typename std::set<T>::const_iterator _items_iterator;
};

class _Observable_stl_base :
    public _ObservableCollectionClass,
    public _ObservableCollectionBase<_Observable_stl_base>
{
public:
    IValueConverter *_GetItemValueConverter()
    {
        foundation::ComPtr<pmod::library::IValueConverter> spItemConverter;
        this->GetInterfaceProtected()->GetItemConverter(spItemConverter.GetAddressOf());
        return spItemConverter;
    }

    foundation::library::ICriticalSection *_GetCriticalSection() 
    { 
        return this->GetCriticalSection(); 
    }

    inline HRESULT _FireCollectionChanged(
        foundation::NotifyCollectionModelChangedAction action,
        UINT32 oldStartingIndex,
        UINT32 newStartingIndex,
        UINT32 sizeOldItems, _In_opt_ foundation::IInspectable **ppOldItems,
        UINT32 sizeNewItems, _In_opt_ foundation::IInspectable **ppNewItems
        )
    {
        return this->GetInterfaceProtected()->FireCollectionChanged(
            action,
            oldStartingIndex,
            newStartingIndex,
            sizeOldItems, ppOldItems,
            sizeNewItems, ppNewItems);
    }
protected:
    _Observable_stl_base()
    {
    }
};

template<
    class T,
    class _Unbox_adapter_t = foundation::library::_UnboxAdapter
    >
class _Observable_vector_stl : 
    public _ObservableVectorType<T, _Observable_vector_stl<T, _Unbox_adapter_t>>,
    public _Observable_stl_base
{
public:
    typedef _Observable_vector_stl<T, _Unbox_adapter_t> _Myt;
    typedef _ObservableVectorType<T, _Myt> _Mybase;

    _Myt& operator=(const std::vector<T>& _Right)
    {
        this->_Mybase::operator=(_Right);
        return (*this);
    }
protected:
    _Observable_vector_stl()
    {}
protected:
    // IObservableCollectionDelegate Implementation
    STDMETHOD(OnCreateItemsContainer)(IItemsContainer **ppValue)
    {
        return vector_items_container<T, _Unbox_adapter_t>::CreateInstance(*this,ppValue);
    }
};

template<
    class T,
    class _Unbox_adapter_t = foundation::library::_UnboxAdapter
>
class _Observable_set_stl :
    public _ObservableSetType<T, _Observable_set_stl<T, _Unbox_adapter_t>>,
    public _Observable_stl_base
{
public:
    typedef _Observable_set_stl<T, _Unbox_adapter_t> _Myt;
    typedef _ObservableSetType<T, _Myt> _Mybase;

    _Myt& operator=(const std::set<T>& _Right)
    {
        this->_Mybase::operator=(_Right);
        return (*this);
    }
    void _Reset_items_iterator()
    {
        if (_p_set_items_container != nullptr)
        {
            _p_set_items_container->_Reset_items_iterator();
        }
    }

protected:
    _Observable_set_stl():
        _p_set_items_container(nullptr)
    {}
protected:
    typedef set_items_container<T, _Unbox_adapter_t> _Set_items_container_type;

    // IObservableCollectionDelegate Implementation
    STDMETHOD(OnCreateItemsContainer)(IItemsContainer **ppValue)
    {
        HRESULT hr = _Set_items_container_type::CreateInstance(*this, ppValue);
        if (SUCCEEDED(hr))
        {
            _p_set_items_container = static_cast<_Set_items_container_type *>(*ppValue);
        }
        return hr;
    }
private:
    _Set_items_container_type *_p_set_items_container;
};

template <class T>
static HRESULT create_observable_collection_stl(T **pp, const IID& iidType, ObservableCollectionOptions options = ObservableCollectionOptions::IsReadOnly)
{
    return foundation::library::CreateObjectClass(iidType, options, pp);
}

}}

