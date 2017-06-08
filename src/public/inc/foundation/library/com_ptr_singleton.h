/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:com_ptr_singleton.h
****/
#pragma once


#include <foundation/com_ptr.h>
#include <foundation/ctl/com_library.h>
#include <pal/library/pal_once_util.h>

namespace foundation {
    namespace library {

        // A thread safe singleton instance
        template <class T, class _com_ptr_impl = _ComPtrImpl>
        class _ComPtrSingleton
        {
        public:
            _ComPtrSingleton() :
                _isAvailable(true)
            {}

            ~_ComPtrSingleton()
            {
                _isAvailable = false;
            }

            bool IsAvailable()
            {
                return _isAvailable;
            }

            template<class _Func>
            T *_GetInstance(const _Func& factoryCreatorCallback)
            {
                pal::library::pal_CallOnce_t(
                    this->_initOnce,
                    [&]()
                {
                    _spInstance.Attach(factoryCreatorCallback());
                    return S_OK;
                });
                return this->_spInstance;
            }


            T *_Get()
            {
                return _spInstance.get();
            }
        private:
            pal::library::PalOnceFlag             _initOnce;
            ComPtr<T, _com_ptr_impl>   _spInstance;
            bool _isAvailable : 1;
        };

        template <class _TFactoryInterface, class _FactoryCreatorCallback>
        _TFactoryInterface *_GetFactoryInstance(const _FactoryCreatorCallback& creatorCallback)
        {
            static _ComPtrSingleton<_TFactoryInterface> _factory_instance_ptr;

            return _factory_instance_ptr._GetInstance(creatorCallback);
        }

        template <class _TFactoryClass, class _TFactoryInterface>
        _TFactoryInterface *_GetFactoryInstance()
        {
            return _GetFactoryInstance<_TFactoryInterface>(
                []() {
                _TFactoryClass *p_factory_class = nullptr;
                IF_ASSERT(ctl::ComInspectableObject<_TFactoryClass>::CreateInstance(&p_factory_class));
                return p_factory_class;
            });
        }
    }
}
