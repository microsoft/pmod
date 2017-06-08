/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dictionary_ptr.h
****/
#pragma once

#include <foundation/com_ptr.h>
#include "macros.h"
#include "interfaces/dictionary.h"
#include "assert.h"
#include "boxing_util.h"
#include <iterator>

namespace foundation
{

    template <class TClass>
    class _DictionaryPtrWrapper
    {
    public:
        bool HasKey(LPCSTR_t key) const
        {
            bool exists;
            if (FAILED(GetDictionary()->HasKey(foundation::HStringRef(key), &exists)))
            {
                exists = false;
            }
            return exists;
        }
#if _HAS_CPP0X
        template<class T>
        inline HRESULT GetValuePtr(_In_ LPCSTR_t key, T &value) const
        {
            return this->GetValue(key,value);
        }
#else
        template<class T>
        HRESULT GetValuePtr(_In_ LPCSTR_t key, T &value) const
        {
            InspectablePtr valuePtr;
            _IFR_(GetDictionary()->Lookup(
                                           foundation::HStringRef(key),
                                           valuePtr.GetAddressOf()));
            return foundation::QueryInterfaceIf(valuePtr, value.ReleaseAndGetAddressOf());
        }
#endif
        template<class T>
        HRESULT GetValue(_In_ HSTRING key, T &value) const
        {
            InspectablePtr valuePtr;
            _IFR_(GetDictionary()->Lookup(
                key,
                valuePtr.GetAddressOf()));
            return foundation::GetValue(valuePtr, value);
        }

        template<class T>
        inline HRESULT GetValue(_In_ LPCSTR_t key, T &value) const
        {
            return GetValue(foundation::HStringRef(key), value);
        }

        template<class T>
        HRESULT GetValue(_In_ LPCSTR_t key, T* pValue) const
        {
            InspectablePtr valuePtr;
            _IFR_(GetDictionary()->Lookup(
                foundation::HStringRef(key),
                valuePtr.GetAddressOf()));
            return foundation::pv_util::GetValue(valuePtr, pValue);
        }

        template<class T>
        HRESULT GetValueAs(_In_ LPCSTR_t key, T &value) const
        {
            InspectablePtr valuePtr;
            _IFR_(GetDictionary()->Lookup(
                foundation::HStringRef(key),
                valuePtr.GetAddressOf()));
            return foundation::GetValueAs(valuePtr, value);
        }

        template<class T>
        HRESULT GetValue(_In_ LPCSTR_t key, T defaultValue, T &value) const
        {
            if (!HasKey(key))
            {
                value = defaultValue;
                return S_FALSE;
            }
            return GetValue(key, value);
        }
        
        HRESULT GetValue(_In_ LPCSTR_t key, IID &value) const
        {
            InspectablePtr valuePtr;
            _IFR_(this->GetValue(key,valuePtr.GetAddressOf()));
            IFR_ASSERT(pv_util::ChangeType(
                PropertyType_Guid,
                valuePtr.GetAddressOfPtr()));
            return foundation::GetValue(valuePtr, value);
        }

        template<class T>
        HRESULT SetValue(_In_ HSTRING key, _In_ T value)
        {
            bool replaced;
            return GetDictionary()->Insert(
                key,
                foundation::CreateValue(value),
                &replaced);
        }

        template<class T>
        inline HRESULT SetValue(_In_ LPCSTR_t key, _In_ T value)
        {
            return SetValue(HStringPtr(key), value);
        }

        inline HRESULT Remove(_In_ LPCSTR_t key)
        {
            return GetDictionary()->Remove(HStringPtr(key));
        }

        class keys_iterator : public std::iterator < std::forward_iterator_tag, HSTRING >
        {
        public:
            inline keys_iterator(IDictionary *pDictionary) :
                _current_position(0)
            {
                pDictionary->Keys(_foundationIterator.GetAddressOf());
            }

            inline keys_iterator() : _current_position(0)
            {
            }

            HSTRING* operator->()
            {
                _foundationIterator->GetCurrent(_current_item.ReleaseAndGetAddressOf());
                return _current_item.GetAddressOfPtr();
            }

            inline const HSTRING& operator*()
            {
                return *this->operator->();
            }

            inline void operator++()
            {
                if (_foundationIterator != nullptr)
                {
                    bool hasCurrent;
                    _foundationIterator->MoveNext(&hasCurrent);
                }
                this->_current_position++;
            }

            bool operator==(const keys_iterator& rValue) const
            {
                bool lHasCurrent = false;

                if (_foundationIterator != nullptr)
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

            inline bool operator!=(const keys_iterator& rValue) const
            {
                return !this->operator==(rValue);
            }
        private:
            // The original collection and the current position data:
            ComPtr<foundation::IIterator_impl<HSTRING> > _foundationIterator;
            unsigned int    _current_position;
            HStringPtr     _current_item;
        };

        keys_iterator keys_begin() const
        {
            return keys_iterator(this->GetDictionary());
        }

        keys_iterator keys_end() const
        {
            return keys_iterator();
        }

    protected:
        inline IDictionary *GetDictionary() const
        {
            const TClass *pThis = static_cast<const TClass *>(this);
            return pThis->GetDictionaryInterface();
        }

        inline IDictionary *GetDictionary()
        {
            TClass *pThis = static_cast<TClass *>(this);
            return pThis->GetDictionaryInterface();
        }
    };


    class DictionaryPtr :
        public ComPtr<IDictionary>,
        public _DictionaryPtrWrapper<DictionaryPtr>
    {
    public:
        DictionaryPtr() throw()
        {
        }
        DictionaryPtr(_In_ IDictionary* lp) throw() :
            ComPtr<IDictionary>(lp)
        {
        }

        IDictionary *GetDictionaryInterface() const
        {
            return this->ComPtr<IDictionary>::Get();
        }
        IDictionary *GetDictionaryInterface()
        {
            return this->ComPtr<IDictionary>::Get();
        }
    };

}

