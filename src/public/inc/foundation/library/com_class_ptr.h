/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:com_class_ptr.h
****/

#pragma once

#include <foundation/com_ptr.h>

namespace foundation {



    template <class T, class Q, class TBASE = _ComPtrBase<T> >
    class ComClassPtr :
        public TBASE
    {
    public:
        typedef ComClassPtr<T, Q, TBASE> _This_Type;

        ComClassPtr() throw()
        {
        }

        ComClassPtr(_In_opt_ T* lp) throw():
            TBASE(lp)
        {
        }

        ComClassPtr(_In_opt_ Q* lp) throw()
        {
            SetClassPtr(lp);
        }

        ComClassPtr(_Inout_ const _This_Type& lp) throw() :
            TBASE(lp.Get())
        {
        }

        T* operator=(_Inout_ const _This_Type& lp) throw()
        {
            _classPtr.Release();
            return TBASE::operator=(lp);
        }

        T* operator=(_In_opt_ T* lp) throw()
        {
            _classPtr.Release();
            return TBASE::operator=(lp);
        }

        Q* operator=(_In_opt_ Q* lp) throw()
        {
            _classPtr.Release();
            TBASE::Release();
            if (lp != nullptr)
            {
                this->SetClassPtr(lp);
            }
            return _classPtr.Get();
        }

        T** ReleaseAndGetAddressOf() throw()
        {
            _classPtr.Release();
            return TBASE::ReleaseAndGetAddressOf();
        }

        ComPtr<Q>& GetClassPtr()
        { 
            EnsureClassPtr();
            return _classPtr; 
        }

        T *GetInterface() const { return this->Get(); }
        Q *GetClassInterface() const 
        { 
            EnsureClassPtr();
            return this->_classPtr.Get();
        }

        void Release() throw()
        {
            _classPtr.Release();
            this->_ComPtrBase<T>::Release();
        }
    protected:
        HRESULT SetClassPtr(_In_ Q *pQ)
        {
            foundation_assert(pQ != nullptr);
            _classPtr = pQ;
            IFR_ASSERT(foundation::QueryInterface(pQ, this->ReleaseAndGetAddressOf()));
            return S_OK;
        }
    private:
        void EnsureClassPtr() const
        {
            if (_classPtr == nullptr)
            {
                Q **ppQ = (const_cast<_This_Type *>(this))->_classPtr.GetAddressOf();
                foundation::QueryInterface(
                    this->Get(), 
                    ppQ);
            }
        }
    protected:
        ComPtr<Q>         _classPtr;
    };

    template <class Q>
    struct _IClassInterfaceHost
    {
        virtual Q *GetClassInterface() const = 0;

        void ReleaseHostWrapper() {}
    };

    template <class T, class THostWrapper, class TBASE = _ComPtrBase<T>, class Q = typename THostWrapper::_ClassInterfaceType >
    class _ComClassPtr :
        public ComClassPtr<T, Q, TBASE>,
        public THostWrapper
    {
    public:
        typedef ComClassPtr<T, Q, TBASE> _BaseType;

        _ComClassPtr() throw()
        {
        }

        _ComClassPtr(_In_opt_ T* lp) throw() :
            _BaseType(lp)
        {
        }

        _ComClassPtr(_In_opt_ Q* lp) throw()
        {
            if (lp != nullptr)
            {
                this->SetClassPtr(lp);
            }
        }

        void Release() throw()
        {
            this->ReleaseHostWrapper();
            this->_BaseType::Release();
        }

        // override _IClassInterfaceHost<Q>
        Q *GetClassInterface() const override
        {
            return _BaseType::GetClassInterface();
        }
    };
}

