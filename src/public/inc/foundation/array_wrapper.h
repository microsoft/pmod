/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:array_wrapper.h
****/
#ifndef _ARRAY_WRAPPER_DEFINED
#define _ARRAY_WRAPPER_DEFINED

#include <foundation/pal.h>
#include <iterator>
#include <memory>
#include "macros.h"
#include "com_ptr.h"

#include "type_traits_util.h"
#include <vector>
#include "hstring_wrapper.h"
#include "errors.h"

namespace foundation
{

template <class vector_t>
inline static typename vector_t::value_type::_PtrClass ** GetVectorBufferPtr(
    const vector_t& items)
{
    return items.size() ?
        const_cast<typename vector_t::value_type::_PtrClass **>(items.front().GetAddressOfPtr()) :
        nullptr;
}

template <class T, class TSize = UINT32>
class _ArrayWrapperAlloc
{
public:
    static HRESULT CopyTo(
        TSize size_in,
        _In_ T *pIn,
        TSize& size_out,
        T *&ppT)
    {
        size_out = size_in;
        ULONG allocate_size = (ULONG)(size_in * sizeof(T));
        ppT = (T*)_pal_MemAlloc(allocate_size);
        if (ppT != nullptr)
        {
            memcpy(ppT, (void *)pIn, allocate_size);
            return S_OK;
        }
        else
        {
            return Errors::E_OUTOFMEMORY_;
        }
    }

    static void release(
        _Inout_ TSize& size,
        _Inout_ T* &pBuffer)
    {
        size = 0;
        if (pBuffer)
        {
            _pal_MemFree((LPVOID)pBuffer);
            pBuffer = nullptr;
        }
    }
};

struct _ArrayWrapperBase {};
template <class T, class TSize = UINT32, class _Array_wrapper_alloc = _ArrayWrapperAlloc<T, TSize>>
class ArrayWrapper : public _ArrayWrapperBase
{
public:
    typedef T _Item_Type;
    typedef ArrayWrapper<T, TSize, _Array_wrapper_alloc> _This_Type;

    ArrayWrapper() :
        m_pBuffer(nullptr),
        m_size(0)
    {
    }

    ArrayWrapper(TSize size, _In_ T *pT) throw()
    {
        _Array_wrapper_alloc::CopyTo(
            size,
            pT,
            m_size,
            m_pBuffer);
    }

    ArrayWrapper(_In_ const _This_Type& other) throw()
    {
        other.CopyTo(&this->m_size, &this->m_pBuffer);
    }

    ~ArrayWrapper()
    {
        Clear();
    }

    class const_iterator : public std::iterator<std::forward_iterator_tag, T>
    {
    public:
        inline const_iterator(
            const ArrayWrapper<T, TSize, _Array_wrapper_alloc>& arr) :
            _arrayWrapper(arr),
            _current_position(0)
        {
        }

        inline const_iterator() : _current_position(0)
        {
        }

        T* operator->()
        {
            return &_arrayWrapper.m_pBuffer[_current_position];
        }

        inline const T& operator*()
        {
            return *this->operator->();
        }

        inline void operator++()
        {
            this->_current_position++;
        }

        bool operator==(const const_iterator& rValue) const
        {
            bool lIsEnd = this->_current_position >= this->_arrayWrapper.GetSize();
            bool rIsEnd = rValue._current_position >= rValue._arrayWrapper.GetSize();

            if (lIsEnd && rIsEnd) return true;
            if (lIsEnd != rIsEnd) return false;

            return this->_current_position == rValue._current_position;
        }

        inline bool operator!=(const const_iterator& rValue) const
        {
            return !this->operator==(rValue);
        }


    private:
        // The original array and the current position data:
        ArrayWrapper<T, TSize,_Array_wrapper_alloc> _arrayWrapper;
        unsigned int _current_position;
    };

public:
    static inline HRESULT CopyTo(
        TSize size, T *pT,
        TSize *pSize, T **pBuffer)
    {
        IFCPTR_ASSERT(pSize);
        IFCPTR_ASSERT(pBuffer);

        _Array_wrapper_alloc::CopyTo(
            size,
            pT,
            *pSize,
            *pBuffer);
        return S_OK;
    }

    _This_Type & operator= (const _This_Type & other)
    {
        if (this != &other) // protect against invalid self-assignment
        {
            Clear();
            other.CopyTo(&this->m_size, &this->m_pBuffer);
        }
        return *this;
    }

    void Clear() throw()
    {
        _Array_wrapper_alloc::release(m_size, m_pBuffer);
    }


    void CopyFrom(TSize size, T *pT) throw()
    {
        Clear();

        _Array_wrapper_alloc::CopyTo(
            size,
            pT,
            m_size,
            m_pBuffer);
    }

    void CopyTo(TSize *pSize, T **ppT) const throw()
    {
        foundation_assert(pSize);
        foundation_assert(ppT);

        if (ppT != nullptr && pSize != nullptr)
        {
            _Array_wrapper_alloc::CopyTo(
                m_size,
                m_pBuffer,
                *pSize,
                *ppT);
        }
    }

    template <class TVector>
    void CopyToVector(TVector& vector) throw()
    {
        vector.clear();
        vector.insert(
            vector.end(),
            &this->m_pBuffer[0],
            &this->m_pBuffer[this->m_size]);
    }

    TSize GetSize() const
    {
        return m_size;
    }

    const T *GetBuffer() const
    {
        return this->m_pBuffer;
    }

    T *GetRawBuffer()
    {
        return this->m_pBuffer;
    }

    T *GetRawBuffer() const
    {
        return this->m_pBuffer;
    }

    T **GetBufferAddressOf() throw()
    {
        foundation_assert(m_pBuffer == nullptr);
        return &this->m_pBuffer;
    }

    TSize *GetSizeAddressOf() throw()
    {
        foundation_assert(m_size == 0);
        return &this->m_size;
    }

    T operator[](TSize  _Pos) const
    {
        foundation_assert(_Pos < m_size);
        return m_pBuffer[_Pos];
    }

    const_iterator begin() const
    {
        return const_iterator(*this);
    }

    const_iterator end() const
    {
        return const_iterator();
    }
private:
    T       *m_pBuffer;
    TSize  m_size;
};

class _HstringWrapperAlloc : public _ArrayWrapperAlloc<HSTRING>
{
public:
    static void release(
        UINT32& size,
        HSTRING *&pBuffer)
    {
        for (UINT32 index = 0; index < size; ++index)
        {
            _pal_DeleteString(pBuffer[index]);
        }
        _ArrayWrapperAlloc<HSTRING>::release(size, pBuffer);
    }
    static HRESULT CopyTo(
        UINT32 size_in,
        HSTRING *pIn,
        UINT32& size_out,
        HSTRING *&ppT)
    {
        _IFR_(_ArrayWrapperAlloc<HSTRING>::CopyTo(size_in, pIn, size_out, ppT));
        for (UINT32 index = 0; index < size_in; ++index)
        {
            HSTRING hString;
            _pal_DuplicateString(pIn[index], &hString);
            ppT[index] = hString;
        }
        return S_OK;
    }
};

template <class T >
class _ArrayRefCountAlloc : public _ArrayWrapperAlloc<T>
{
public:
    static void release(
        UINT32& size,
        T *&pBuffer)
    {
        for (UINT32 index = 0; index < size; ++index)
        {
            T pItem = pBuffer[index];
            if (pItem != nullptr)
            {
                pItem->Release();
            }
        }
        _ArrayWrapperAlloc<T>::release(size, pBuffer);
    }

    static HRESULT CopyTo(
        UINT32 size_in,
        T *pIn,
        UINT32& size_out,
        T *&ppT)
    {
        _IFR_(_ArrayWrapperAlloc<T>::CopyTo(size_in, pIn, size_out, ppT));
        for (UINT32 index = 0; index < size_in; ++index)
        {
            T pItem = pIn[index];
            if (pItem != nullptr)
            {
                pItem->AddRef();
            }
        }
        return S_OK;
    }
};

template <class T >
class ArrayRefCountWrapper : public ArrayWrapper<T*, UINT32, _ArrayRefCountAlloc<T*>>
{
private:
    typedef ArrayWrapper<T*, UINT32, _ArrayRefCountAlloc<T*>> _Base_Type;
public:

    ArrayRefCountWrapper()
    {
    }

    ArrayRefCountWrapper(_In_ const ArrayRefCountWrapper<T>& other) throw():
        _Base_Type(other)
    {
    }

    ArrayRefCountWrapper(UINT32 size,T** pT) throw() :
        _Base_Type(size, pT)
    {
    }

    ArrayRefCountWrapper<T> & operator= (const ArrayRefCountWrapper<T> & other)
    {
        if (this != &other) // protect against invalid self-assignment
        {
            _Base_Type::operator=(other);
        }
        return *this;
    }

};


typedef ArrayWrapper<BYTE>      UInt8ArrayWrapper;
typedef ArrayWrapper<INT16>     Int16ArrayWrapper;
typedef ArrayWrapper<UINT16>    UInt16ArrayWrapper;
typedef ArrayWrapper<INT32>     Int32ArrayWrapper;
typedef ArrayWrapper<UINT32>    UInt32ArrayWrapper;
typedef ArrayWrapper<INT64>     Int64ArrayWrapper;
typedef ArrayWrapper<UINT64>    UInt64ArrayWrapper;
typedef ArrayWrapper<FLOAT>     SingleArrayWrapper;
typedef ArrayWrapper<DOUBLE>    DoubleArrayWrapper;
typedef ArrayWrapper<boolean>   BooleanArrayWrapper;
typedef ArrayWrapper<WCHAR>     Char16ArrayWrapper;
typedef ArrayWrapper<bool>      BoolArrayWrapper;

typedef ArrayWrapper<HSTRING, UINT32, _HstringWrapperAlloc> HStringArrayWrapper;
typedef ArrayRefCountWrapper<IInspectable> InspectableArrayWrapper;

typedef ArrayWrapper<IID, ULONG> IIdsArrayWrapper;


template <class T >
inline HRESULT CopyTo(
    UINT32 size_in,
    _In_ T *pIn,
    _Out_ UINT32 *p_size_out,
    _Out_ T **ppT
#if _HAS_CPP0X
    , typename std::enable_if<!std::is_base_of<foundation::IInspectable, T>::value >::type* = nullptr
#endif
    )
{
    IFCPTR_ASSERT(pIn);
    IFCPTR_ASSERT(p_size_out);
    IFCPTR_ASSERT(ppT);

    return _ArrayWrapperAlloc<T>::CopyTo(size_in, pIn, *p_size_out, *ppT);
}

template <class T >
inline HRESULT RefCountCopyTo(
    UINT32 size_in,
    _In_opt_ T *pIn,
    _Out_ UINT32 *p_size_out,
    _Out_ T **ppT)
{
    IFCPTR_ASSERT(p_size_out);
    IFCPTR_ASSERT(ppT);

    return _ArrayRefCountAlloc<T>::CopyTo(size_in, pIn, *p_size_out, *ppT);
}

template <class Vector_type, class T >
HRESULT RefCountCopyTo(
    const Vector_type& items,
    _Out_ UINT32 *p_size_out,
    _Out_ T **ppT)
{
    IFCPTR_ASSERT(p_size_out);
    IFCPTR_ASSERT(ppT);

    *p_size_out = (UINT32)items.size();
    *ppT = (T *)_pal_MemAlloc((UINT32)(sizeof(T*)*items.size()));
    if (ppT == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    UINT32 index = 0;
    for (typename Vector_type::const_iterator iter = items.begin();
        iter != items.end();
        ++iter)
    {
        *(*ppT + index++) = *iter;

        if ((*iter) != nullptr)
        {
            ((foundation::IUnknown *)*iter)->AddRef();
        }
    }
    return S_OK;
}

#if _HAS_CPP0X
template <class T >
inline HRESULT CopyTo(
    UINT32 size_in,
    _In_ T *pIn,
    _Out_ UINT32 *p_size_out,
    _Out_ T **ppT,
    typename std::enable_if<std::is_base_of<foundation::IInspectable, T>::value >::type* = nullptr
    )
{
    return RefCountCopyTo(size_in, pIn, p_size_out, ppT);
}
#endif

inline HRESULT CopyTo(
    UINT32 size_in,
    _In_opt_ HSTRING *pIn,
    _Out_ UINT32 *p_size_out,
    HSTRING **ppT)
{
    IFCPTR_ASSERT(pIn);
    IFCPTR_ASSERT(p_size_out);
    IFCPTR_ASSERT(ppT);
    *ppT = nullptr;

    return _HstringWrapperAlloc::CopyTo(size_in, pIn, *p_size_out, *ppT);
}

template <class T,class Vector_t>
void CopyTo(UINT32 size,_In_ T *pArray, Vector_t& vector)
{
    foundation_assert(vector.size() == 0);
    vector.resize((size_t)size);
    for (UINT32 index = 0; index < size; ++index, ++pArray)
    {
        vector[index] = *pArray;
    }
}

#if _HAS_CPP0X
template <class ContainerT>
inline HRESULT CopyTo(
    const ContainerT& values,
    _Out_ UINT32 *p_size_out,
    _Out_ typename ContainerT::value_type **ppT,
    ENABLE_IF_VECTOR_VALUE)
{
    return CopyTo(
        (UINT32)values.size(),
        values.size() ? const_cast<typename ContainerT::value_type *>(&values.front()) : nullptr,
        p_size_out,
        ppT);
}

template <class ContainerT>
inline HRESULT CopyTo(
    const ContainerT& values,
    _Out_ UINT32 *p_size_out,
    _Out_ typename ContainerT::value_type::_PtrClass ***ppT,
    ENABLE_IF_VECTOR_PTR)
{
    return RefCountCopyTo(
        (UINT32)values.size(),
        GetVectorBufferPtr(values),
        p_size_out,
        ppT);
}
#endif

inline HRESULT CopyTo(
    const std::vector<bool>& values,
    _Out_ UINT32 *p_size_out,
    _Out_ bool **ppT)
{
    PalBuffer<bool> bool_buffer((UINT32)values.size());
    for (std::vector<bool>::const_iterator it = values.begin(); it != values.end(); ++it)
    {
        *(bool_buffer.get() + (it - values.begin())) = *it;
    }
    return CopyTo(
        (UINT32)values.size(),
        bool_buffer.get(),
        p_size_out,
        ppT);
}

inline HRESULT CopyTo(
    const std::vector<HStringPtr>& values,
    _Out_ UINT32 *p_size_out,
    HSTRING **ppT)
{
    foundation_assert(ppT);

    return CopyTo(
        (UINT32)values.size(),
        values.size() ? const_cast<HSTRING *>(values.front().GetAddressOfPtr()) : nullptr,
        p_size_out,
        ppT);
}

}

#endif
