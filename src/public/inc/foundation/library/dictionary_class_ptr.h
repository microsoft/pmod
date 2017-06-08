/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dictionary_class_ptr.h
****/
#pragma once

#include <foundation/dictionary_ptr.h>
#include <foundation/pv_util_stl.h>
#include "library_util.h"

namespace foundation
{
    namespace library {
        class DictionaryClassPtr : public ComPtr < IDictionary >,
            public _DictionaryPtrWrapper<DictionaryClassPtr>
        {
        public:
            typedef std::pair<foundation::HStringPtr, InspectablePtr> pair;
            typedef std::vector<pair> view;
            typedef ComPtr<IDictionary> _Mybase;

            /// <summary>
            ///     Constructs a <c>DictionaryClassPtr</c> object.
            /// </summary>
            /// <param name="pCoreDictionary">
            ///     A Core Dictionary reference
            /// </param>
            /**/


            explicit DictionaryClassPtr(_In_  IDictionary *pCoreDictionary) throw()
                : _Mybase(pCoreDictionary)
            {
            }

            /// <summary>
            ///     Constructs a <c>DictionaryClassPtr</c> object.
            /// </summary>
            /**/
            DictionaryClassPtr(_In_  foundation::IInspectable *pInspectableObject)
            {
                if (pInspectableObject != nullptr)
                {
                    THROW_IFR(foundation::QueryInterface(pInspectableObject, this->GetAddressOf()));
                }
            }

            /// <summary>
            ///     Constructs a <c>DictionaryClassPtr</c> object.
            /// </summary>
            /**/
            DictionaryClassPtr(_In_ bool CreateDictionaryClass = true)
            {
                THROW_IFR(library::CreateDictionaryClass(this->GetAddressOf()));
            }

            ~DictionaryClassPtr() throw () {}

			const view getview() const
			{
				foundation::ArrayRefCountWrapper<IDictionaryPair>   view;
				THROW_IFR(GetDictionaryInterface()->GetView(view.GetSizeAddressOf(), view.GetBufferAddressOf()));
				std::vector<pair> vec_view;
				vec_view.resize((size_t)view.GetSize());
				for (UINT32 index = 0; index < view.GetSize(); ++index)
				{
					foundation::HStringPtr key;
					foundation::InspectablePtr value;
					THROW_IFR(view[index]->GetKey(key.GetAddressOf()));
					THROW_IFR(view[index]->GetValue(value.GetAddressOf()));

					vec_view[index] = std::make_pair(key, value);
				}
				return vec_view;
			}

            template<class T>
            T Get(_In_ LPCSTR_t key) const
            {
                InspectablePtr spValue;
                THROW_IFR(GetDictionaryInterface()->Lookup(foundation::HStringRef(key), spValue.GetAddressOf()));
                return getvalue<T>(spValue);
            }


            template<class T>
            T Get(_In_ LPCSTR_t key, T defaultValue) const
            {
                if (!HasKey(key))
                {
                    return defaultValue;
                }
                return Get<T>(key);
            }

            foundation::string_t Get(_In_ LPCSTR_t key, LPCSTR_t defaultValue) const
            {
                if (!HasKey(key))
                {
                    return defaultValue;
                }
                return Get<foundation::string_t>(key);
            }

            template <class T>
            ComPtr<T> GetComPtr(_In_ LPCSTR_t key) const
            {
                InspectablePtr spValue;
                THROW_IFR(GetDictionaryInterface()->Lookup(foundation::HStringRef(key), spValue.GetAddressOf()));
                return getvalue_ptr<T>(spValue);
            }


            template<class T>
            void Set(_In_ LPCSTR_t key, _In_ T value)
            {
                THROW_IFR(SetValue(key, value));
            }

            template<class T>
            void Set(_In_ LPCSTR_t key, _In_ UINT32 size, _In_ T* pValues)
            {

                bool replaced;
                THROW_IFR(GetDictionaryInterface()->Insert(
                    foundation::HStringRef(key), 
                    foundation::CreateValue(size, pValues),
                    &replaced));
            }

            void Remove(_In_ LPCSTR_t key)
            {
                THROW_IFR(GetDictionaryInterface()->Remove(foundation::HStringRef(key)));
            }

            void Clear()
            {
                THROW_IFR(GetDictionaryInterface()->Clear());
            }

            IDictionary *GetDictionaryInterface() const
            {
                foundation_assert(this->_Mybase::Get());
                return this->_Mybase::Get();
            }
        };

        template <>
        inline DictionaryClassPtr DictionaryClassPtr::Get(LPCSTR_t key) const
        {
            ComPtr<IDictionary> spValue = this->GetComPtr<IDictionary>(key);
            return DictionaryClassPtr(spValue);
        }

        template <>
        inline void DictionaryClassPtr::Set(_In_ LPCSTR_t key, _In_ const DictionaryClassPtr& value)
        {
            bool replaced;
            THROW_IFR(GetDictionaryInterface()->Insert(foundation::HStringRef(key), value._Mybase::Get(), &replaced));
        }

#ifndef __UTF16_STRINGS
        template<>
        inline void DictionaryClassPtr::Set(_In_ LPCSTR_t key,_In_ const wchar_t * value)
        {
            bool replaced;
            THROW_IFR(GetDictionaryInterface()->Insert(foundation::HStringRef(key),CreateValue(foundation::to_platform_string(value).c_str()),&replaced));
        }
#endif
    }
}

