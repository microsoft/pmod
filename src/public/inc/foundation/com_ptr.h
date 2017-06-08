/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:com_ptr.h
****/
#ifndef COMPTR_DEFINED
#define COMPTR_DEFINED

#include "assert.h"
#include "foundation/com_interfaces.h"

#if _HAS_CPP0X
#include <type_traits>
#endif

namespace foundation
{

///
/// @brief An abstract class that masks AddRef and Release methods of the associated interface.
///
template <class T>
class _NoAddRefReleaseOnCComPtr : 
    public T
{
    private:
        STDMETHOD_(_pal_AtomicIntegerType, AddRef)()=0;
        STDMETHOD_(_pal_AtomicIntegerType, Release)()=0;
};

///
/// @brief A tag struct to mark a class that will implement the '_ComPtrImpl' protocol to AddRef/Release/QueryInterface from a class.
///
struct _ComPtrImplBase {};

///
/// @brief Default class to implement the needed abstraction for _ComPtrBase class.
///
/// Implements AddRef, Release, QueryInterface
///
struct _ComPtrImpl
{
    // IUnknown implementations (not based on '_ComPtrImplBase')
    template <class T>
    static inline
#if _HAS_CPP0X
    typename std::enable_if<!std::is_base_of < _ComPtrImplBase, T >::value, _pal_AtomicIntegerType>::type
#else
    _pal_AtomicIntegerType
#endif
    _AddRef(T *pT)
    {
        return pT->AddRef();
    }

    template <class T>
    static inline
#if _HAS_CPP0X
    typename std::enable_if<!std::is_base_of < _ComPtrImplBase, T >::value, _pal_AtomicIntegerType>::type
#else
    _pal_AtomicIntegerType
#endif
    _Release(T *pT)
    {
        return pT->Release();
    }

    template <class T>
    static inline
#if _HAS_CPP0X
    typename std::enable_if<!std::is_base_of < _ComPtrImplBase, T >::value, HRESULT>::type
#else
    HRESULT
#endif
    _QueryInterface(T *pT, REFIID riid, _COM_Outptr_ void **ppvObject)
    {
        return pT->QueryInterface(riid, ppvObject);
    }

#if _HAS_CPP0X
    // _ComPtrImplBase implementations
    template <class T>
    typename std::enable_if<std::is_base_of < _ComPtrImplBase, T >::value, _pal_AtomicIntegerType>::type
        static inline _AddRef(T *pT)
    {
        return T::_AddRef(pT);
    }

    template <class T>
    typename std::enable_if<std::is_base_of < _ComPtrImplBase, T >::value, _pal_AtomicIntegerType>::type
        static inline _Release(T *pT)
    {
        return T::_Release(pT);
    }

    template <class T>
    typename std::enable_if<std::is_base_of < _ComPtrImplBase, T >::value, HRESULT>::type
        static inline _QueryInterface(T *pT, REFIID riid, _COM_Outptr_ void **ppvObject)
    {
        return T::_QueryInterface(pT, riid, ppvObject);
    }
#endif
};

///
/// @brief No Ref Count Ptr impl
///
struct _NoRefCountPtrImpl
{
    template <class T>
    static inline
    _pal_AtomicIntegerType _AddRef(T *)
    {
        return 1;
    }

    template <class T>
    static inline
        _pal_AtomicIntegerType _Release(T *)
    {
        return 1;
    }
};

///
/// @brief A common base for all smart pointer classes.
///
/// This class is empty.
///
struct _ComPtrBaseType {};

///
/// @brief _ComPtrBase provides the basis for all other smart pointers.
/// @param T An interface specifying the type of pointer to be stored.
///
/// The other smart pointers add their own constructors and operators.
///
template <class T, class _com_ptr_impl = _ComPtrImpl>
class _ComPtrBase : public _ComPtrBaseType
{
protected:
    ///
    /// @brief The constructor.
    ///
    _ComPtrBase() throw()
    {
        p = nullptr;
    }

    ///
    /// @brief The constructor.
    /// @param lp Used to initialize the interface pointer.
    ///
    _ComPtrBase(_Inout_opt_ T* lp) throw()
    {
        p = lp;
        if (p != nullptr)
            _com_ptr_impl::_AddRef(p);
    }

    ///
    /// @brief Exchanges the stored pointers of two smart pointers.
    /// @param other A smart pointer providing a raw pointer to exchanges with that of this smart pointer.
    ///
    void swap(_ComPtrBase& other)
    {
        T* pTemp = p;
        p = other.p;
        other.p = pTemp;
    }

public:
    ///
    /// @typedef The interface of the attached pointer.
    ///
    typedef T _PtrClass;

    ///
    /// @brief The destructor.
    ///
    ~_ComPtrBase() throw()
    {
        if (p)
        {
            _com_ptr_impl::_Release(p);
        }
    }

    ///
    /// @brief The cast operator.
    /// @return Returns a pointer to the object data type defined in the class template.
    ///
    operator T*() const throw()
    {
        return p;
    }

    ///
    /// @brief The dereferencing operator.
    /// @return Returns the value of _ComPtrBase::p; that is, a pointer to the object referenced by the _ComPtrBase object.
    ///
    /// If debug builds, an assertion error will occur if _ComPtrBase::p is not equal to nullptr.
    ///
    T& operator*() const
    {
        foundation_assert(p!=nullptr);
        return *p;
    }
    
    ///
    /// @brief Get a copy of the underlying pointer.
    /// @return A copy of _ComPtrBase::p.
    ///
    T *Get() const
    {
        return p;
    }

    ///
    /// @brief Get the address of the underlying pointer.
    /// @return The address of _ComPtrBase::p.
    ///
    /// If debug builds, an assertion error will occur if _ComPtrBase::p is not equal to nullptr.
    ///
    T* const* GetAddressOf() const throw()
    {
        foundation_assert(p == nullptr);
        return &p;
    }

    ///
    /// @brief Get the address of the underlying pointer.
    /// @return The address of _ComPtrBase::p.
    ///
    /// If debug builds, an assertion error will occur if _ComPtrBase::p is not equal to nullptr.
    ///
    T** GetAddressOf() throw()
    {
        foundation_assert(p==nullptr);
        return &p;
    }

    ///
    /// @brief Release the managed pointer (if attached) and get the address of the underlying pointer.
    /// @return The address of _ComPtrBase::p.
    ///
    T** ReleaseAndGetAddressOf() throw()
    {
        Release();
        return &p;
    }

    ///
    /// @brief Get the address of the underlying pointer.
    ///
    /// If debug builds, NO assertion error will occur if _ComPtrBase::p is not equal to nullptr.
    /// @return The address of _ComPtrBase::p.
    ///
    T* const* GetAddressOfPtr() const throw()
    {
        return &p;
    }

    ///
    /// @brief Get the address of the underlying pointer.
    ///
    /// If debug builds, NO assertion error will occur if _ComPtrBase::p is not equal to nullptr.
    /// @return The address of _ComPtrBase::p.
    ///
    T** GetAddressOfPtr() throw()
    {
        return &p;
    }

    ///
    /// @brief The pointer-to-member operator.
    /// @return Returns the value of the _ComPtrBase::p data member variable.
    ///
    /// Use this operator to call a method in a class pointed to by the _ComPtrBase::p.
    /// In debug builds, an assertion failure will occur if the _ComPtrBase::p data member points to nullptr.
    /// 
    _NoAddRefReleaseOnCComPtr<T>* operator->() const throw()
    {
        foundation_assert(p != nullptr);
        return (_NoAddRefReleaseOnCComPtr<T>*)p;
    }

    ///
    /// @brief The negation operator.
    /// @return Returns true if the _ComPtrBase::p pointer is equal to nullptr, false otherwise.
    ///
    bool operator!() const throw()
    {	
        return (p == nullptr);
    }

    ///
    /// @brief The less-than operator.
    /// @param pT A pointer to an object.
    /// @return Returns true if the pointer managed by current object is less than the pointer to which it is being compared.
    ///
    bool operator<(_In_opt_ T* pT) const throw()
    {
        return p < pT;
    }

    ///
    /// @brief Test for non-equality.
    /// @param pT A pointer to an object.
    /// @return Returns true if the pointer managed by this _ComPtrBase and pT do not point to the same object, false otherwise.
    ///
    bool operator!=(_In_opt_ T* pT) const
    {
        return !operator==(pT);
    }

    ///
    /// @brief The equality operator.
    /// @param pT A pointer to an object.
    /// @return Returns true if the pointer managed by this _ComPtrBase and pT point to the same object, false otherwise.
    ///
    bool operator==(_In_opt_ T* pT) const throw()
    {
        return p == pT;
    }

    ///
    /// @brief Call this method to release the interface.
    ///
    /// Releases the interface and sets _ComPtrBase::p to nullptr
    ///
    void Release() throw()
    {
        T* pTemp = p;
        if (pTemp)
        {
            p = nullptr;
            _com_ptr_impl::_Release(pTemp);
        }
    }

    ///
    /// @brief Call this method to check if the specified IUnknown points to the same object managed by this _ComPtrBase object.
    /// @param pOther The foundation::IUnknown* to compare to.
    /// @result Returns true if the objects are identical, false otherwise.
    ///
    bool IsEqualObject(_Inout_opt_ foundation::IUnknown* pOther) throw()
    {
        if (p == nullptr && pOther == nullptr)
        {
            // They are both nullptr objects
            return true;
        }

        if (p == nullptr || pOther == nullptr)
        {
            // One is nullptr the other is not
            return false;
        }

        foundation::IUnknown* pUnknownOther =
            foundation::CastTo<foundation::IUnknown>(pOther, foundation_IID_IUnknown);

        foundation::IUnknown* pUnknown = nullptr;
        _com_ptr_impl::_QueryInterface(p, foundation_IID_IUnknown, (void**)&pUnknown);
        if (pUnknown != nullptr) _com_ptr_impl::_Release(pUnknown);

        return (pUnknown == pUnknownOther);
    }


    ///
    /// @brief Call this method to take ownership of an existing pointer.
    /// @param p2 The _ComPtrBase object will take ownership of this pointer.
    ///
    /// Attach calls _com_ptr_impl::release on the existing _ComPtrBase::p member variable and then assigns p2 to
    /// _ComPtrBase::p. When a _ComPtrBase object takes ownership of a pointer, it will automatically call release on the
    /// pointer which will delete the pointer and any allocated data if the reference count on the object goes to 0.
    ///
    void Attach(_In_opt_ T* p2) throw()
    {
        if (p != nullptr)
        {
            ULONG ref = _com_ptr_impl::_Release(p);
            //(ref);
            // Attaching to the same object only works if duplicate references are being coalesced.  Otherwise
            // re-attaching will cause the pointer to be released and may cause a crash on a subsequent dereference.
            foundation_assert(ref != 0 || p2 != p);
        }

        p = p2;
    }

    ///
    /// @brief Call this method to release ownership of a pointer.
    /// @return Returns a copy of the pointer.
    ///
    /// Releases ownership of a pointer, sets the _ComPtrBase::p data member variable to nullptr, and returns a copy of the pointer;
    /// does not release the interface.
    ///
    T* Detach() throw()
    {
        T* pt = p;
        p = nullptr;
        return pt;
    }

    ///
    /// @brief Call this method to copy the _ComPtrBase pointer to another pointer variable.
    /// @param ppT Address of the variable which will receive the _ComPtrBase pointer.
    /// @return Returns S_OK on success, E_POINTER on failure.
    ///
    HRESULT CopyTo(_COM_Outptr_result_maybenull_ T** ppT) throw()
    {
    	foundation_assert(ppT != nullptr);
        if (ppT == nullptr)
        {
            return E_POINTER;
        }

        *ppT = p;
        
        if (p)
        {
            _com_ptr_impl::_AddRef(p);
        }

        return S_OK;
    }

    ///
    /// @brief Retrieves pointers to the supported interfaces on the object _ComPtrBase::p points to.
    /// @param pp The address of a pointer variable that receives the interface pointer requested. Upon successful return, *pp contains the requested interface pointer to the object. If the object does not support the interface, *pp is set to nullptr.
    /// @return This method returns S_OK if the interface is supported, and E_NOINTERFACE otherwise. If pp is nullptr, this method returns E_POINTER.
    /// 
    template <class Q>
    HRESULT QueryInterface(_COM_Outptr_ Q** pp) const throw()
    {
        if (pp == nullptr)
        {
            foundation_assert(pp != nullptr);
            return E_POINTER;
        }

        return _com_ptr_impl::_QueryInterface(this->p,Q::GetIID(), (void**)pp);
    }

private:
    T* p;
};

///
/// @brief A smart pointer class for managing interface pointers.
/// @param An interface specifying the type of pointer to be stored.
///
template <class T, class _com_ptr_impl = _ComPtrImpl>
class ComPtr : 
    public _ComPtrBase<T, _com_ptr_impl>
{
public:
    ///
    /// @brief Use this factory method to create a smart pointer with an interface pointer attached.
    /// @param pT Used to initialize the interface pointer.
    /// @result A smart pointer with pT attached.
    ///
    static ComPtr<T> AttachPtr(T *pT)
    {
        ComPtr<T> ptr;
        ptr.Attach(pT);
        return ptr;
    }

    ///
    /// @brief The constructor.
    ///
    ComPtr() throw()
    {
    }

    ///
    /// @brief The constructor.
    /// @param lp Used to initialize the interface pointer.
    ///
    ComPtr(_Inout_opt_ T* lp) throw() :
        _ComPtrBase<T, _com_ptr_impl>(lp)
    {
    }

    ///
    /// @brief The copy constructor.
    /// @param lp Used to initialize the interface pointer.
    ///
    ComPtr(_Inout_ const ComPtr<T, _com_ptr_impl>& lp) throw() :
        _ComPtrBase<T, _com_ptr_impl>(lp.Get())
    {
    }

    ///
    /// @brief Assignment operator.
    /// @return Returns a pointer to the updated ComPtr object.
    /// @param lp An interface pointer ComPtr will take ownership of.
    ///
    /// Assigns a pointer to the member pointer.
    /// This operation will AddRef on the new object and release the underlying object, if one exists.
    ///
    T* operator=(_Inout_opt_ T* lp) throw()
    {
        if (*this != lp)
        {
            ComPtr(lp).swap(*this);
        }

        return *this;
    }

    ///
    /// @brief Assignment operator.
    /// @return Returns a pointer to the updated ComPtr object.
    /// @param lp A reference to another ComPtr object, whose underlying pointer this smart pointer will take ownership of.
    ///
    /// Assigns a pointer to the member pointer.
    /// This operation will AddRef on the new object and release the underlying object, if one exists.
    ///
    T* operator=(_Inout_ const ComPtr<T,_com_ptr_impl>& lp) throw()
    {
        if (*this != lp)
        {
            ComPtr(lp).swap(*this);
        }

        return *this;
    }
	
    /*
    ComPtr(_Inout_ ComPtr<T>& &lp) throw() :
        _ComPtrBase<T>()
    {	
        lp.Swap(*this);
    }	
    T* operator=(_Inout_ ComPtr<T>&& lp) throw()
    {			
        if (*this != lp)
        {
            ComPtr(static_cast<ComPtr&&>(lp)).Swap(*this);
        }
        return *this;
    }
    */
};

///
/// @typedef com_inspectable_ptr.
///
/// A specialized version of ComPtr holding an IInspectable pointer.
///
typedef ComPtr<foundation::IInspectable> InspectablePtr;

template <class T>
class ComQIPtr : 
	public ComPtr<T>
{
public:
    ///
    /// @brief The constructor.
    ///
    ComQIPtr() throw()
	{
	}
	
    ///
    /// @brief The constructor.
    /// @param lp Used to initialize the interface pointer.
    ///
    ComQIPtr(_Inout_opt_ foundation::IUnknown* lp) throw()
	{
		if (lp != NULL)
		{
            foundation::QueryInterface(lp, this->GetAddressOf());
		}
	}

    ///
    /// @brief The copy constructor.
    /// @param Q An interface.
    /// @param lp Used to initialize the interface pointer.
    ///
    template<class Q>
	ComQIPtr(_Inout_opt_ ComPtr<Q>& lp) throw()
	{
		if (lp != NULL)
		{
            foundation::QueryInterface(lp, this->GetAddressOf());
		}
	}
	
    ///
    /// @brief The assignment operator.
    /// @param Used to initialize the interface pointer.
    /// @return Returns a pointer to the updated ComQIPtr object. 
    ///
    T* operator=(_Inout_opt_ foundation::IUnknown* lp) throw()
	{
        if(*this!=lp)
        {
    		 ComQIPtr(lp).swap(*this);
        }

        return *this;
	}
	
    ///
    /// @brief The assignment operator.
    /// @param Q An interface of the RHS argument.
    /// @param Used to initialize the interface pointer.
    /// @return Returns a pointer to the updated ComQIPtr object. 
    ///
    template<class Q>
	T* operator=(_Inout_opt_ ComPtr<Q>& lp) throw()
	{
        if(*this!=lp)
        {
    		 ComQIPtr(lp).swap(*this);
        }

        return *this;
	}
};

#ifdef __cplusplus_winrt
#define _Nullptr __nullptr
#else
#define _Nullptr nullptr
#endif

// Comparation operators - don't compare COM object identity
template<class T, class U, class _com_ptr_impl>
bool operator==(const _ComPtrBase<T, _com_ptr_impl>& a, const _ComPtrBase<U, _com_ptr_impl>& b) throw()
{
    return a.Get() == b.Get();
}

template<class T, class _com_ptr_impl>
bool operator==(const _ComPtrBase<T, _com_ptr_impl>& a, decltype(_Nullptr)) throw()
{
    return a.Get() == nullptr;
}

template<class T, class _com_ptr_impl>
bool operator==(decltype(_Nullptr), const _ComPtrBase<T, _com_ptr_impl>& a) throw()
{
    return a.Get() == nullptr;
}

template<class T, class U, class _com_ptr_impl>
bool operator!=(const _ComPtrBase<T, _com_ptr_impl>& a, const _ComPtrBase<U, _com_ptr_impl>& b) throw()
{
    return a.Get() != b.Get();
}

template<class T, class _com_ptr_impl>
bool operator!=(const _ComPtrBase<T, _com_ptr_impl>& a, decltype(_Nullptr)) throw()
{
    return a.Get() != nullptr;
}

template<class T, class _com_ptr_impl>
bool operator!=(decltype(_Nullptr), const _ComPtrBase<T, _com_ptr_impl>& a) throw()
{
    return a.Get() != nullptr;
}

template<class T, class U, class _com_ptr_impl>
bool operator<(const _ComPtrBase<T, _com_ptr_impl>& a, const _ComPtrBase<U, _com_ptr_impl>& b) throw()
{
    return a.Get() < b.Get();
}

///
/// @brief Hash function utilized in STL structures. 
/// @return The hash for a specific ComPtr
/// @param ComPtr smart pointed.
///
/// NOTE: This method will not work with derived version of the comptr classes. 
/// the user needs to add such function in case they are needed.
///
// TEMPLATE STRUCT SPECIALIZATION hash
template<typename T>
struct ComPtrHash
{
    size_t operator()(const ComPtr<T>& ptr) const
    {
        return (size_t)ptr.Get();
    }
};

}


  
#endif
