/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:weak_reference.h
****/
#pragma once

#include <foundation/assert.h>

#include "com_ptr.h"
#include "macros.h"

namespace foundation
{   
    typedef ComPtr<IWeakReference> WeakReferencePtr;

    // cast to IInspectable
    inline IInspectable *AsInspectable(IWeakReference *pWeakReference)
    {
        return reinterpret_cast<IInspectable *>(pWeakReference);
    }

    // cast to IWeakReference
    inline IWeakReference *AsWeakReference(IUnknown *pUnk)
    {
        return reinterpret_cast<IWeakReference *>(pUnk);
    }

    // Resolve a weak reference
    template <class T>
    inline HRESULT ResolveWeakRef(_In_ IWeakReference *pWeakRef, const IID& iidType, _Out_ T*& pStrongRef)
    {
        pStrongRef = nullptr;
        
        if (pWeakRef)
        {
            IUnknown* pResult = nullptr;
            IFR_ASSERT(pWeakRef->Resolve(iidType, (IInspectable **)&pResult));
            pStrongRef = static_cast<T*>(pResult);
        }       
        return S_OK;
    }

    template <class T>
    inline HRESULT ResolveWeakRef(_In_ IWeakReference *pWeakRef, _Out_ T*& pStrongRef)
    {
        return ResolveWeakRef(pWeakRef,T::GetIID(),pStrongRef);
    }

    template <class com_ptr_t>
    inline HRESULT ResolveWeakRef(_In_ IWeakReference *pWeakRef, com_ptr_t& ptr)
    {
        typedef typename com_ptr_t::_PtrClass TInterface;
        return ResolveWeakRef(pWeakRef, TInterface::GetIID(), *ptr.GetAddressOf());
    }

    template <>
    inline HRESULT ResolveWeakRef(_In_ IWeakReference *pWeakRef, _Out_ IInspectable*& pStrongRef)
    {
        return ResolveWeakRef(pWeakRef, foundation_IID_IInspectable, pStrongRef);
    }

    template <class T>
    inline HRESULT ResolveWeakRefIf(_In_ IInspectable *pUnk, const IID& iidType, _Out_ T*& pStrongRef)
    {
        ComPtr<IWeakReference> spWeakReference;
        // will silently fail to be handled by caller
        _IFR_(QueryInterface(pUnk, foundation_IID_IWeakReference, spWeakReference.GetAddressOf()));
        return ResolveWeakRef(spWeakReference, iidType, pStrongRef);
    }

    template <class T>
    inline HRESULT ResolveWeakRefWeak(_In_ IWeakReference *pWeakRef, const IID& iidType, _Out_ T*& pStrongRef)
    {
        IFR_ASSERT(ResolveWeakRef(pWeakRef,iidType,pStrongRef));
        if(pStrongRef)
        {
            // compensate QI AddRef
            pStrongRef->Release();
        }
        return S_OK;
    }

    template <class T>
    inline HRESULT ResolveWeakRefWeak(_In_ IWeakReference *pWeakRef, _Out_ T*& pStrongRef)
    {
        return ResolveWeakRefWeak(pWeakRef,T::GetIID(),pStrongRef);
    }

    template <>
    inline HRESULT ResolveWeakRefWeak(_In_ IWeakReference *pWeakRef, _Out_ IInspectable*& pStrongRef)
    {
        return ResolveWeakRefWeak(pWeakRef, foundation_IID_IInspectable, pStrongRef);
    }

    template <class T, class TInterface>
    inline HRESULT ResolveAndCastWeakRef(_In_ IWeakReference *pWeakRef, _COM_Outptr_opt_result_maybenull_ T** ppT)
    {
        *ppT = nullptr;
        TInterface *pInterface = nullptr;
        IGNOREHR(ResolveWeakRef(pWeakRef, pInterface));
        if (pInterface != nullptr)
        {
            *ppT = static_cast<T *>(pInterface);
            return S_OK;
        }
        return S_FALSE;
    }

    template <class T,class TInterface>
    inline T *ResolveAndCastWeakRefWeak(_In_ IWeakReference *pWeakRef)
    {
        TInterface *pInterface = nullptr;
        IGNOREHR(ResolveWeakRefWeak(pWeakRef, pInterface));
        return pInterface != nullptr ? static_cast<T *>(pInterface) : nullptr;
    }

    // Obtain a weak reference
    inline HRESULT AsWeakRef(_Out_ IWeakReference *&pWeakRef, _In_ IUnknown *pInput)
    {
        ComPtr<IWeakReferenceSource> spWS;

        IFCPTR_ASSERT(pInput);
        // will silently fail to be handled by caller
        _IFR_(QueryInterface(pInput, foundation_IID_IWeakReferenceSource, spWS.GetAddressOf()));
        IFR_ASSERT(spWS->GetWeakReference(&pWeakRef));

        return S_OK;
    }

    inline HRESULT AsWeakRef(_Out_ WeakReferencePtr& spWeakReference, _In_ IUnknown *pInput)
    {
        return AsWeakRef(*spWeakReference.GetAddressOfPtr(),pInput);
    }


    // Test a weak reference
    inline bool TestWeakRef(_In_ IWeakReference *pWeakRef)
    {
        if (!pWeakRef)
        {
            return false;
        }
        
        ComPtr<IUnknown> spStrongRef;
        HRESULT hr = ResolveWeakRef(pWeakRef, foundation_IID_IUnknown, *spStrongRef.GetAddressOf());
        if (FAILED(hr))
        {
            // We don't expect a failure when resolving to IUnknown
            foundation_assert(false);
            return false;
        }
        
        return spStrongRef != nullptr;
    }


    //------------------------------------------------------------------------------
    // Class:   IReferenceHolder Template
    //
    // Purpose: Define a Reference Holder Interface Type
    //
    //------------------------------------------------------------------------------
    template <class T>
    class IReferenceHolder
    {
    public:
        // Pure Virtual methods to implement
        virtual HRESULT ResolveReference(_Out_ T*& pReference) const = 0;
        virtual HRESULT Attach(_In_ IUnknown *pUnknown) = 0;
        virtual bool IsReferenceAttached() const = 0;

        HRESULT ResolveReferencePtr(_Out_ ComPtr<T>& spReference) const
        {
            return ResolveReference(*spReference.GetAddressOf());
        }
    };

    // Resolve a weak reference holder
    template <class T>
    HRESULT ResolveReferenceHolder(
        _In_ const IReferenceHolder<T>& referenceHolder,
        _In_ bool ignoreNullReference, 
        _Out_ T*& pStrongRef)
    {
        IFR_ASSERT(referenceHolder.ResolveReference(pStrongRef));

        if(!ignoreNullReference)
        {
            // in DEBUG build we want to foundation_assert if this situation happen
            foundation_assert(pStrongRef != nullptr);
            // this method requires that the Resolved Reference is not nullptr
            IFCEXPECT_ASSERT(pStrongRef);
        }
        return pStrongRef ? S_OK:S_FALSE;
    }  

    template <class T>
    HRESULT ResolveReferenceHolderPtr(
        _In_ const IReferenceHolder<T>& referenceHolder,
        _In_ bool ignoreNullReference,
        _Out_ ComPtr<T>& spStrongRef)
    {
        return ResolveReferenceHolder(referenceHolder, ignoreNullReference, *spStrongRef.GetAddressOf());
    }

    //------------------------------------------------------------------------------
    // Class:   ReferenceHolderBase Template
    //
    // Purpose: Define a Reference Holder Base Type
    //
    //------------------------------------------------------------------------------
    template <class T>
    class ReferenceHolderBase :
       public IReferenceHolder<T>
    {
    public:
        virtual ~ReferenceHolderBase() {}
    };

    //------------------------------------------------------------------------------
    // Class:   ReferenceHolderBase Template
    //
    // Purpose: Define a Reference Holder Base Type
    //
    //------------------------------------------------------------------------------
    template <class T, class TReference>
    class ReferenceHolder :
        public ReferenceHolderBase<T>
    {
    protected:
        ReferenceHolder()
        {
        }
        
        virtual ~ReferenceHolder()
        {
            Clear();
        }

        ReferenceHolder(_In_opt_ T* p) throw()
        {
            m_spReference = p;
        }

        ReferenceHolder(_Inout_ const ReferenceHolder<T,TReference>& other) throw()
        {
            m_spReference = other.m_spReference;
        }

    public:
        ReferenceHolder<T,TReference> & operator= (const ReferenceHolder<T,TReference> & other)
        {
            if (this != &other) // protect against invalid self-assignment
            {
                Clear();
                m_spReference = other.m_spReference;
            }
            return *this;
        }

        HRESULT Clear()
        {
            m_spReference = nullptr;
            return S_OK;
        }
    
    protected:
        mutable ComPtr<TReference> m_spReference;
    };

    //------------------------------------------------------------------------------
    // Class:   WeakReferenceHolder Template
    //
    // Purpose: Define a Weak reference Holder for a type
    //
    //------------------------------------------------------------------------------
    template <class T>
    class WeakReferenceHolder : public ReferenceHolder<T,IWeakReference>
    {
    public:
        WeakReferenceHolder()
        {
        }

        WeakReferenceHolder(_In_opt_ IWeakReference* p) throw() :
            ReferenceHolder<T,IWeakReference>(p)
        {
        }

        WeakReferenceHolder(_Inout_ const WeakReferenceHolder<T>& other) throw() :
            ReferenceHolder<T,IWeakReference>(other)
        {
        }

        // Implements ReferenceHolder<T>
        HRESULT ResolveReference(_Out_ T*& pReference) const override
        {
            HRESULT hr = ResolveWeakRef(this->m_spReference, pReference);
            if (SUCCEEDED(hr) && !pReference)
            {
                // If the weak reference successfully resolves to nullptr, then we can
                // safely release the weak reference itself, since it cannot change
                // back to being resolvable to non-nullptr.  This enables the system to
                // release the weak reference itself more quickly.
                this->m_spReference.Release();
            }
            return hr;
        }

        HRESULT Attach(_In_ IUnknown *pUnknown) override
        {
            this->Clear();
            if(pUnknown)
            {
                return AsWeakRef(*this->m_spReference.GetAddressOf(), pUnknown);
            }
            return S_OK;
        }

        bool IsReferenceAttached() const override
        {
            return TestWeakRef(this->m_spReference);
        }
    };

    //------------------------------------------------------------------------------
    // Class:   StrongReferenceHolder Template
    //
    // Purpose: Define a Strong reference Holder for a type
    //
    //------------------------------------------------------------------------------
    template <class T>
    class StrongReferenceHolder : public ReferenceHolder<T,T>
    {
    public:
        StrongReferenceHolder()
        {
        }

        StrongReferenceHolder(_In_opt_ T* pT) throw() :
        	ReferenceHolder<T,T>(pT)
        {
        }

        StrongReferenceHolder(_In_ const StrongReferenceHolder<T>& other) throw() :
        	ReferenceHolder<T,T>(other)
        {
        }

        HRESULT Attach(_In_ T *pT)
        {
            IFCPTR_ASSERT(pT);
            this->m_spReference = pT;
            return S_OK;
        }

        // Implements ReferenceHolder<T>
        HRESULT ResolveReference(_Out_ T*& pReference) const
        {
            pReference = this->m_spReference;
            ADDREF_INTERFACE(pReference);
            return S_OK;
        }

        HRESULT Attach(_In_ IUnknown *pUnknown) override
        {
            this->Clear();
            if(pUnknown)
            {
                return QueryInterface(pUnknown,this->m_spReference.GetAddressOf());
            }
            return S_OK;
        }

        bool IsReferenceAttached() const override
        {
            return this->m_spReference != nullptr;
        }
    };
}

