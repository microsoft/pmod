/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ItemsContainer.h
****/
#ifndef _ITEMS_CONTAINER_DEFINED
#define _ITEMS_CONTAINER_DEFINED

#include <vector>
#include <algorithm>

//------------------------------------------------------------------------------
// Class:   IItemsContainerBase
//
// Purpose: defines an interface to access a Container of Items
//
//------------------------------------------------------------------------------

template <class T>
class IItemsContainerBase
{
public:
    struct IContextComparer
    {
        virtual int compare(const T& item1,const T& item2) const  = 0;
    };

    virtual ~IItemsContainerBase()
    {
    }
    virtual UINT32 getCount() = 0;

    virtual const T& get(_In_ UINT32 index) = 0;
    virtual void set(_In_ UINT32 index, _In_ const T& value) = 0;
    virtual void append(_In_ const T& value) = 0;
    virtual void insert(_In_ UINT32 index,_In_ const T& value) = 0;
    virtual void removeAt(_In_ UINT32 index) = 0;
    virtual UINT32 insertPositionOf(_In_ const T& value,IContextComparer *pContextComparer) = 0;
    virtual UINT32 find(_In_ const T& value,IContextComparer *pContextComparer) = 0;
    virtual void sort(IContextComparer *pContextComparer) = 0;
    virtual void clear() = 0;
};

template <class T>
class binary_compare_helper : std::binary_function<T, T, bool>
{
public:
    binary_compare_helper(typename IItemsContainerBase<T>::IContextComparer *pContextComparer):
        m_pContextComparer(pContextComparer)
    {}

    bool operator()(const T& item1, const T& item2)const
    {
        return m_pContextComparer->compare(item1,item2) < 0;
    }
private:
    typename  IItemsContainerBase<T>::IContextComparer *m_pContextComparer;
};

template <class T>
class unary_compare_helper : std::unary_function<T,bool>
{
public:
    unary_compare_helper(
        const T& item,
        typename IItemsContainerBase<T>::IContextComparer *pContextComparer):
        _item(item),
        m_pContextComparer(pContextComparer)
    {}

    bool operator()(const T& item)const
    {
        return m_pContextComparer->compare(_item,item) == 0;
    }
private:
    const T& _item;
    typename  IItemsContainerBase<T>::IContextComparer *m_pContextComparer;
};

//------------------------------------------------------------------------------
// Class:   vector_items_container
//
// Purpose: defines an interface to access a Container of Items
//
//------------------------------------------------------------------------------
template <class T,class TBASE = IItemsContainerBase<T> >
class vector_items_container : public TBASE
{
public:
    std::vector<T>& getItems() { return m_items; }
    void copyItems(const std::vector<T>& items)
    {
        m_items = items;
    }
    // IItemsContainerBase<T> implementation
    UINT32 getCount()
    {
        return (UINT32)m_items.size();
    }
    const T& get(_In_ UINT32 index)
    {
        return m_items.at(index);
    }
    void set(_In_ UINT32 index, _In_ const T& value)
    {
        m_items.at(index) = value;
    }
    void append(_In_ const T& value)
    {
        m_items.push_back(value);
    }

    void insert(_In_ UINT32 index,_In_ const T& value)
    {       
        m_items.insert (m_items.begin() + index,value);
    }

    void removeAt(_In_ UINT32 index)
    {
        m_items.erase(m_items.begin() + index);
    }

    UINT32 insertPositionOf(
        _In_ const T& value,
        typename IItemsContainerBase<T>::IContextComparer *pContextComparer)
    {
        typename std::vector<T>::iterator iter = std::lower_bound(
            m_items.begin(),
            m_items.end(),
            value,
            binary_compare_helper<T>(pContextComparer));
        return (UINT32)(iter - m_items.begin());
    }

    UINT32 find(_In_ const T& value,
            typename IItemsContainerBase<T>::IContextComparer *pContextComparer)
    {
        typename std::vector<T>::iterator iter = std::find_if(
            m_items.begin(),
            m_items.end(),
            unary_compare_helper<T>(value,pContextComparer));
        if(iter != m_items.end())
        {
            return (UINT32)(iter - m_items.begin());
        }
        return foundation::NINDEX;
    }

    void sort(
        typename IItemsContainerBase<T>::IContextComparer *pContextComparer)
    {
        std::sort(
            m_items.begin(),
            m_items.end(),
            binary_compare_helper<T>(pContextComparer));
    }

    void clear()
    {
        m_items.clear();
    }
private:
    std::vector<T> m_items;
};

typedef IItemsContainerBase<foundation::InspectablePtr> CInspectablePtrItemsContainerBase;

#endif
