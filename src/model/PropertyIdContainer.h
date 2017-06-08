/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyIdContainer.h
****/

#pragma once

#include <map>
#include <vector>

//------------------------------------------------------------------------------
// Class:   vector_properties_container
// Base class for a Static property container
//------------------------------------------------------------------------------
template <class T>
class vector_properties_container:
    protected std::vector<T>
{
protected:
    vector_properties_container()
    {
    }
    vector_properties_container(size_t size):
        std::vector<T>(size)
    {
    }

    bool get(_In_ UINT32 propertyId, _Outptr_ T& value)
    {
        // some of the propertyIds on propertyModel have starting index of 0x1000, offset to 0.
        int propertyIdIndex = toPropertyIdIndex(propertyId);
        if(checkBounds(propertyIdIndex))
        {
            value = this->operator[](propertyIdIndex);
            return true;
        }
        return false;
    }
    bool set(_In_ UINT32 propertyId, _In_ const T& value)
    {
        // some of the propertyIds on propertyModel have starting index of 0x1000, offset to 0.
        int propertyIdIndex = toPropertyIdIndex(propertyId);
        if(checkBounds(propertyIdIndex))
        {
            this->operator[](propertyIdIndex) = value;
            return true;
        }
        return false;
    }
    bool has_property(_In_ UINT32 propertyId)
    {
        return checkBounds(toPropertyIdIndex(propertyId));
    }
private:
    inline UINT32 toPropertyIdIndex(UINT32 propertyId)
    {
        return propertyId%pmod::PropertyModel_ViewModel_Start;
    }
    inline bool checkBounds(int propertyIdIndex)
    {
        foundation_assert(propertyIdIndex >= 0);
        return static_cast<int>(this->size()) > propertyIdIndex;
    }
};
//------------------------------------------------------------------------------
// Class:   map_properties_container
// Base class for a Dynamic property container
//------------------------------------------------------------------------------
template <class T>
class map_properties_container:
    protected std::map<UINT32, T>
{
public:
    bool has_property(_In_ UINT32 propertyId)
    {
        return this->find(propertyId) != this->end();
    }

    bool get(_In_ UINT32 propertyId, _Outptr_ T& value)
    {
        typename std::map<UINT32, T>::iterator iter = this->find(propertyId);
        if(iter != this->end())
        {
            value = iter->second;
            return true;
        }
        return false;
    }
    bool set(_In_ UINT32 propertyId, _In_ const T& value)
    {
        this->operator[](propertyId) = value;
        return true;
    }

    void remove(_In_ UINT32 propertyId)
    {
        typename std::map<UINT32, T>::iterator iter = this->find(propertyId);
        if(iter != this->end())
        {
            this->erase(propertyId);
        }
    }
};
//------------------------------------------------------------------------------
// Interface: IBasePropertyIdContainer
//
// Purpose: defines functions for setting and retriving properties by propertyId.
//
//------------------------------------------------------------------------------
template <class T>
class IBasePropertyIdContainer
{
public:
    virtual ~IBasePropertyIdContainer()
    {
    }
    virtual void Clear() = 0;
    virtual bool Get(_In_ UINT32 propertyId, _Outptr_ T& value) = 0;
    virtual bool Set(_In_ UINT32 propertyId, _In_ const T& value) = 0;
    virtual void Remove(_In_ UINT32 propertyId) = 0;
    virtual UINT32 GetSize() = 0;
};

//------------------------------------------------------------------------------
// Class:   TStaticPropertyIdContainer
//
// Purpose: Implements IBasePropertyContainer to set and retrieve properties by propertyId over std::vector<>.
//          The class is used when the propertyIds are defined as contiguous lists and the number of the properties 
//          are predefined.
//
//------------------------------------------------------------------------------
template <class T>
class TStaticPropertyIdContainer:
    public IBasePropertyIdContainer<T>,
    private vector_properties_container<T>
{
public:
    TStaticPropertyIdContainer(size_t propertySize):
        vector_properties_container<T>(propertySize)
    {
    }
    ~TStaticPropertyIdContainer()
    {
        Clear();
    }

    void Clear() override
    {
        this->clear();
    }

    bool Get(_In_ UINT32 propertyId, _Outptr_ T& value) override
    {
        return this->get(propertyId,value);
    }
    bool Set(_In_ UINT32 propertyId, _In_ const T& value) override
    {
        return this->set(propertyId,value);
    }
    void Remove(_In_ UINT32 propertyId) override
    {
        foundation_assert(false);
        UNREFERENCED_PARAMETER(propertyId);
    }

    UINT32 GetSize() override
    {
        return (UINT32)this->size();
    }
};

//------------------------------------------------------------------------------
// Class:   TDynamicPropertyIdContainer
//
// Purpose: Implements IBasePropertyContainer to set and retrieve properties by propertyId over std::map<>.
//          The class is used when the propertyIds are not contiguous lists or the number of the properties 
//          are predefined.
//
//------------------------------------------------------------------------------
template <class T>
class TDynamicPropertyIdContainer:
    public IBasePropertyIdContainer<T>,
    private map_properties_container<T>
{
public:
    ~TDynamicPropertyIdContainer()
    {
        Clear();
    }
    void Clear() override
    {
        this->clear();
    }
    bool Get(_In_ UINT32 propertyId, _Outptr_ T& value) override
    {
        return this->get(propertyId,value);
    }
    bool Set(_In_ UINT32 propertyId, _In_ const T& value) override
    {
        return this->set(propertyId,value);
    }
    void Remove(_In_ UINT32 propertyId) override
    {
        this->remove(propertyId);
    }
    UINT32 GetSize() override
    {
        return (UINT32)this->size();
    }
};

