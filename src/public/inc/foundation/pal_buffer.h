/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_buffer.h
****/
#pragma once

#include "pal.h"

namespace foundation
{
    template <class T>
    class PalBuffer
    {
    public:
        PalBuffer():
            _buffer(nullptr)
        {
        }

        PalBuffer(const T *pT)
        {
            _buffer = pT;
        }

        PalBuffer(UINT32 size)
        {
            _buffer = (const T*)_pal_MemAlloc(size * sizeof(T));
        }

        ~PalBuffer()
        {
            freeBuffer();
        }

        void clear()
        {
            freeBuffer();
        }

        void attach(const T *pT)
        {
            freeBuffer();
            _buffer = pT;
        }

        const T* Detach()
        {
            const T* pT = _buffer;
            _buffer = nullptr;
            return pT;
        }

        T* get()
        {
            return (T *)this->_buffer;
        }

        operator const T*() const throw()
        {
            return this->_buffer;
        }

    protected:
        void freeBuffer()
        {
            if(_buffer)
            {
                _pal_MemFree((LPVOID)_buffer);
                _buffer = nullptr;
            }
        }
    private :
        const T *_buffer;
    };

}

