/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dictionary_impl.h
****/
#pragma once

#include <foundation/interfaces/dictionary.h>
#include "critical_section.h"
#include "critical_section_impl.h"
#include "base_adapter_host.h"
#include "key_value_pair_impl.h"
#include <foundation/array_wrapper.h>

#include <map>
#include <vector>

namespace foundation {

    namespace library {

        static HRESULT CreateDictionaryPairImpl(
            HSTRING key,
            _In_opt_ IInspectable *pValue,
            IDictionaryPair **ppDictionaryPair)
        {
            typedef _InspectableKeyValuePair
                <
                IDictionaryPair,
                _HStringValueAlloc
                > _Dictionary_pair;

            return CreateKeyValuePair<_Dictionary_pair>(key, pValue, ppDictionaryPair);
        }

        template <class T, class TBASE>
        class _DictionaryBase :
            public TBASE,
            public IDictionary
        {
        public:
            // IDictionary Interface
            STDMETHOD(GetView)(UINT32* size, foundation::IDictionaryPair ***ppProperties) override
            {
                FoundationCriticalSection lock(this->get_lock());
                // returned properties
                std::vector<foundation::ComPtr<foundation::IDictionaryPair> > properties;

                for (typename _Map_type::const_iterator iter = _container.begin();
                iter != _container.end();
                    ++iter)
                {
                    foundation::ComPtr<foundation::IDictionaryPair> spPropertyNamePair;
                    IFR_ASSERT(CreateDictionaryPairImpl(
                        (*iter).first,
                        (*iter).second,
                        spPropertyNamePair.GetAddressOf()));

                    properties.push_back(spPropertyNamePair);

                }
                // copy now to public API
                return RefCountCopyTo(properties, size, ppProperties);
            }

            STDMETHOD(GetSize)(_Out_ UINT32* size) override
            {
                FoundationCriticalSection lock(this->get_lock());
                *size = (UINT32)_container.size();
                return S_OK;
            }

            STDMETHOD(Lookup)(HSTRING propertyName, _COM_Outptr_result_maybenull_ foundation::IInspectable** value) override
            {
                FoundationCriticalSection lock(this->get_lock());

                typename _Map_type::const_iterator iter = _container.find(foundation::HStringPtr(propertyName));
                if (iter != _container.end())
                {
                    *value = (*iter).second;
                    if (*value)
                    {
                        (*value)->AddRef();
                    }
                    return S_OK;
                }
                return foundation::E_KEY_NOT_FOUND;
            }

            STDMETHOD(HasKey)(HSTRING propertyName, _Out_ bool* found) override
            {
                IFCPTR_ASSERT(found);
                FoundationCriticalSection lock(this->get_lock());
                typename _Map_type::const_iterator iter = _container.find(foundation::HStringPtr(propertyName));
                *found = iter != _container.end();
                return S_OK;
            }

            STDMETHOD(Insert)(HSTRING propertyName, _In_opt_ foundation::IInspectable *value, _Out_ bool* replaced) override
            {
                IFCPTR_ASSERT(replaced);

                FoundationCriticalSection lock(this->get_lock());
                typename _Map_type::iterator iter = _container.find(foundation::HStringPtr(propertyName));
                if (iter != _container.end())
                {
                    *replaced = true;
                    (*iter).second = value;
                    NotifyDictionaryChanged(propertyName, foundation::NotifyCollectionModelChangedAction::ItemReplaced);
                }
                else
                {
                    foundation::HStringPtr hkey(propertyName);
                    _container.insert(std::make_pair(hkey, value));
                    *replaced = false;
                    NotifyDictionaryChanged(propertyName, foundation::NotifyCollectionModelChangedAction::ItemAdded);
                }
                return S_OK;
            }

            STDMETHOD(Remove)(HSTRING propertyName) override
            {
                FoundationCriticalSection lock(this->get_lock());

                _container.erase(foundation::HStringPtr(propertyName));
                NotifyDictionaryChanged(propertyName, foundation::NotifyCollectionModelChangedAction::ItemRemoved);
                return S_OK;
            }
            STDMETHOD(Clear)() override
            {
                FoundationCriticalSection lock(this->get_lock());
                _container.clear();
                NotifyDictionaryChanged(nullptr, foundation::NotifyCollectionModelChangedAction::Reset);
                return S_OK;
            }
            STDMETHOD(Keys)(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first)
            {
                return _Dictionary_key_iterator::CreateInstance(this, first);
            }

        protected:
            virtual HRESULT NotifyDictionaryChanged(
                HSTRING propertyName,
                _In_ foundation::NotifyCollectionModelChangedAction action)
            {
                UNREFERENCED_PARAMETER(propertyName);
                UNREFERENCED_PARAMETER(action);
                return S_OK;
            }

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == foundation::IDictionary::GetIID())
                {
                    *ppInterface = static_cast<foundation::IDictionary *>(this);
                }
                else
                {
                    return TBASE::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }
        private:
            typedef _DictionaryBase<T, TBASE> _This_Type;
            typedef std::map<foundation::HStringPtr, foundation::InspectablePtr, foundation::library::HStringPtrLessThan> _Map_type;

            class _Dictionary_key_iterator :
                public foundation::ctl::ComInspectableBase,
                public foundation::IIterator_impl<HSTRING>
            {
            public:
                static HRESULT CreateInstance(
                    _In_ _This_Type *pThis,
                    _COM_Outptr_ foundation::IIterator_impl<HSTRING> **ppIterator)
                {
                    foundation_assert(pThis);

                    _Dictionary_key_iterator *pIterator = nullptr;
                    IFR_ASSERT(foundation::ctl::ComInspectableObject<_Dictionary_key_iterator>::CreateInstance(&pIterator));
                    pIterator->_pThis = pThis;
                    pIterator->_iterator = pThis->_container.begin();
                    // add ref
                    foundation::ctl::inspectable::AddRefInterface(pThis);

                    *ppIterator = pIterator;
                    return S_OK;
                }
            protected:
                _Dictionary_key_iterator() :
                    _pThis(nullptr)
                {}

                void FinalRelease()
                {
                    // release our items container
                    foundation::ctl::inspectable::ReleaseInterface(_pThis);
                }
                STDMETHOD(GetCurrent)(_Outptr_ HSTRING *current)
                {
                    IFCPTR_ASSERT(current);
                    _pal_DuplicateString((*_iterator).first, current);
                    return S_OK;
                }
                STDMETHOD(GetHasCurrent)(_Out_ bool *hasCurrent)
                {
                    IFCPTR_ASSERT(hasCurrent);
                    *hasCurrent = _iterator != _pThis->_container.end();
                    return S_OK;
                }
                STDMETHOD(MoveNext)(_Out_ bool *hasCurrent)
                {
                    IFCPTR_ASSERT(hasCurrent);
                    ++_iterator;
                    *hasCurrent = _iterator != _pThis->_container.end();
                    return S_OK;
                }
            private:
                _This_Type *_pThis;
                typename _This_Type::_Map_type::const_iterator _iterator;
            };

            ICriticalSection* get_lock()
            {
                return (static_cast<T *>(this))->_Get_lock();
            }


        private:
            _Map_type _container;
        };

        class DictionaryImpl :
            public _DictionaryBase <DictionaryImpl, _DefaultAdapterHost<IDictionary, ctl::ComInspectableBase>>
        {
        public:
            HRESULT _Initialize(bool isFreeThreaded)
            {
                if (isFreeThreaded)
                {
                    IFR_ASSERT(CreateCriticalSectionImpl(_cs.GetAddressOf()));
                }
                return S_OK;
            }

            ICriticalSection* _Get_lock()
            {
                return _cs;
            }
        private:
            ComPtr<ICriticalSection> _cs;
        };
    }
}

