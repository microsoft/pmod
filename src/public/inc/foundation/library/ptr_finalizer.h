/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ptr_finalizer.h
****/
#pragma once


namespace foundation
{
    namespace library
    {
        template <class T, class TFinalizer>
        class PtrFinalizer
        {
        public:
            PtrFinalizer(T *ptr) :
                _ptr(ptr)
            {
            }

            PtrFinalizer() :
                _ptr(nullptr)
            {
            }

            ~PtrFinalizer()
            {
                _Release();
            }

            T **GetAddressOf()
            {
                foundation_assert(_ptr == nullptr);
                return &_ptr;
            }

            T **ReleaseAndGetAddressOf()
            {
                _Release();
                return &_ptr;
            }

            operator T*() const throw()
            {
                return _ptr;
            }

            T *GetComPtr() const
            {
                return _ptr;
            }
            
            void Attach(T *ptr)
            {
                _Release();
                this->_ptr = ptr;
            }

            T *Detach()
            {
                T *result = this->_ptr;
                this->_ptr = nullptr;
                return result;
            }

        private:
            void _Release()
            {
                if (_ptr != nullptr)
                {
                    TFinalizer::release(_ptr);
                    _ptr = nullptr;
                }
            }
        private:
            T *_ptr;
        };
    }
}
