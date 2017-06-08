/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_cache.h
****/
#pragma once

#include <foundation/weak_reference.h>
#include <foundation/interfaces/object.h>

#include <map>

namespace foundation {
    namespace library {
        template <typename _Kty, class TInterface, typename _Pr = std::less<_Kty>>
        class object_cache
        {
        public:
            inline size_t size()
            {
                return _cache.size();
            }
            HRESULT add_to_cache(_Kty key, TInterface *pT, bool useWeakRef)
            {
                IFCPTR_ASSERT(pT);

                ComPtr<foundation::IUnknown> unk_ptr;
                if (useWeakRef)
                {
                    AsWeakRef(*(reinterpret_cast<foundation::IWeakReference **>(unk_ptr.GetAddressOf())), pT);
                }
                else
                {
                    unk_ptr = pT;
                }

                typename _Map_cache_type::iterator iter = _cache.find(key);
                if (iter == _cache.end())
                {
                    _cache[key] = std::make_pair(useWeakRef, unk_ptr);
                }
                else
                {
                    (*iter).second.first = useWeakRef;
                    (*iter).second.second = unk_ptr;
                }
                return S_OK;
            }

            HRESULT remove_from_cache(_Kty key)
            {
                typename _Map_cache_type::iterator iter = _cache.find(key);
                if (iter != _cache.end())
                {
                    _cache.erase(iter);
                    return S_OK;
                }
                return S_FALSE;
            }

            bool is_cached(_Kty key) const
            {
                return _cache.find(key) != _cache.end();
            }

            HRESULT lookup(_Kty key, TInterface **ppT)
            {
                IFCPTR_ASSERT(ppT);

                typename _Map_cache_type::iterator iter = _cache.find(key);
                if (iter != _cache.end())
                {
                    if ((*iter).second.first)
                    {
                        // storage is weak ref
                        IFR_ASSERT(ResolveWeakRef(
                            reinterpret_cast<foundation::IWeakReference *>((*iter).second.second.Get()),
                            *ppT));
                        if (*ppT == nullptr)
                        {
                            // strong ref was gone
                            _cache.erase(iter);
                            return S_FALSE;
                        }
                    }
                    else
                    {
                        // object was strong ref so no need to QI
                        *ppT = reinterpret_cast<TInterface *>((*iter).second.second.Get());
                        (*ppT)->AddRef();
                    }
                    return S_OK;
                }
                // object not found in our map
                *ppT = nullptr;
                return S_FALSE;
            }

            template <class _Func>
            HRESULT for_each_cached_object(const _Func& func)
            {
                return for_each_cached_object(_cache, func);
            }

            HRESULT remove_all_empty_weak_ref()
            {
                typename _Map_cache_type::iterator iter = _cache.begin();
                while (iter != _cache.end())
                {
                    if ((*iter).second.first &&
                        !TestWeakRef(
                        reinterpret_cast<foundation::IWeakReference *>((*iter).second.second.Get())))
                    {
                        _cache.erase(iter++);
                    }
                    else
                    {
                        ++iter;
                    }
                }
                return S_OK;
            }

            void clear_cache()
            {
                _cache.clear();
            }

        protected:
            typedef std::pair<bool, ComPtr<foundation::IUnknown>> _CacheValue_type;
            typedef std::map<_Kty, _CacheValue_type, _Pr> _Map_cache_type;

            template <class _Func>
            static HRESULT for_each_cached_object(const _Map_cache_type& cache, const _Func& func)
            {
                typename _Map_cache_type::const_iterator iter = cache.begin();
                while (iter != cache.end())
                {
                    ComPtr<TInterface> object_ptr;
                    if ((*iter).second.first)
                    {
                        IFR_ASSERT(ResolveWeakRef(
                            reinterpret_cast<foundation::IWeakReference *>((*iter).second.second.Get()),
                            *object_ptr.GetAddressOf()));
                    }
                    else
                    {
                        object_ptr = reinterpret_cast<TInterface *>((*iter).second.second.Get());
                    }
                    if (object_ptr != nullptr)
                    {
                        _IFR_(func(object_ptr.Get()));
                    }
                    ++iter;
                }
                return S_OK;
            }

            _Map_cache_type& GetCache() { return _cache; }
        private:
            _Map_cache_type _cache;
        };

        typedef std::pair<UINT32, UINT32> proxy_cache_key;

        class proxy_cache_key_less_than :
            public std::binary_function<proxy_cache_key, proxy_cache_key, bool>
        {
        public:
            bool operator () (
                const proxy_cache_key& item1, 
                const proxy_cache_key& item2) const
            {
                if (item1.first == item2.first)
                {
                    return item1.second < item2.second;
                }
                return item1.first < item2.first;
            }
        };

        template <class TInterface>
        class object_proxy_cache :
            public object_cache < proxy_cache_key, TInterface, proxy_cache_key_less_than >
        {
        private:
            typedef object_cache<proxy_cache_key, TInterface, proxy_cache_key_less_than> _Base_type;
        public:
            HRESULT add_to_cache(TInterface *pT, bool useWeakRef)
            {
                IFCPTR_ASSERT(pT);

                ComPtr<foundation::IObject> object_ptr;
                IFR_ASSERT(foundation::QueryInterface(pT, object_ptr.GetAddressOf()));

                proxy_cache_key key;
                _IFR_(object_ptr->GetProcessId(&key.first));
                _IFR_(object_ptr->GetUniqueId(&key.second));

                return _Base_type::add_to_cache(key, pT, useWeakRef);
            }

            HRESULT lookup_object(foundation::IObject *pObject, TInterface **ppT)
            {
                IFCPTR_ASSERT(pObject);
                IFCPTR_ASSERT(ppT);

                proxy_cache_key key;
                _IFR_(pObject->GetProcessId(&key.first));
                _IFR_(pObject->GetUniqueId(&key.second));

                return _Base_type::lookup(key, ppT);
            }

        };
    }
}


