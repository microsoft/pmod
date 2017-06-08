/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:value_alloc.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation
{

    // Class::_ValueAlloc
    // Simple Value allocation that provide the follwing semantics:
    // initialize/CopyTo/release
    template <class T>
    class _ValueAlloc
    {
    public:
        static void initialize(T &pT)
        {
            UNREFERENCED_PARAMETER(pT);
        }
        static void CopyTo(T from, T &to)
        {
            to = from;
        }
        static void release(T &pT)
        {
            UNREFERENCED_PARAMETER(pT);
        }
    };

    // Class::_RefCountedValueAlloc
    // Ref counted value allocator
    class _HStringValueAlloc
    {
    public:
        static void initialize(HSTRING &pT)
        {
            pT = nullptr;
        }
        static void CopyTo(HSTRING from, HSTRING &to)
        {
            if (from != nullptr)
            {
                _pal_DuplicateString(from, &to);
            }
            else
            {
                to = nullptr;
            }
        }
        static void release(HSTRING &pT)
        {
            if (pT != nullptr)
            {
                _pal_DeleteString(pT);
            }
            pT = nullptr;
        }
    };

    // Class::_RefCountedValueAlloc
    // Ref counted value allocator
    template <class T>
    class _RefCountedValueAlloc
    {
    public:
        static void initialize(T &pT)
        {
            pT = nullptr;
        }
        static void CopyTo(T from, T &to)
        {
            to = from;
            if (from != nullptr)
            {
                from->AddRef();
            }
        }
        static void release(T &pT)
        {
            if (pT != nullptr)
            {
                pT->Release();
            }
            pT = nullptr;
        }
    };

    typedef _RefCountedValueAlloc<foundation::IInspectable *> inspectable_ref_counted_value_alloc;

    // Class::_PropertyValueWrapper
    // Value wrapper with initialize/CopyTo/release semantics
    template <class T, class _Alloc>
    class _PropertyValueWrapper
    {
    public:
        _PropertyValueWrapper()
        {
            _Alloc::initialize(_property_value);
        }

        _PropertyValueWrapper(const _PropertyValueWrapper &other)
        {
            _Alloc::CopyTo(other._property_value, _property_value);
        }

        ~_PropertyValueWrapper()
        {
            _Alloc::release(_property_value);
        }

        _PropertyValueWrapper & operator= (const _PropertyValueWrapper & other)
        {
            if (this != &other) // protect against invalid self-assignment
            {
                _Alloc::CopyTo(other._property_value, _property_value);
            }
            return *this;
        }

        T *relase_and_get_address_of() throw()
        {
            _Alloc::release(_property_value);
            return &this->_property_value;
        }

    private:
        T   _property_value;
    };

}

