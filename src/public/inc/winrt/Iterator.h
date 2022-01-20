/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Iterator.h
****/

#pragma once

#include <foundation/macros.h>
#include <foundation/ctl/com_library.h>
#include "windows.foundation.h"
#include "windows.foundation.collections.h"
#include <winstring.h>

// include specialization of Iterators for BYTE,INT32,etc..
#include "Microsoft.PropertyModel.h"

namespace utils
{
    template <class T>
    class IteratorBase: 
        public ABI::Windows::Foundation::Collections::IIterator<T>,
        public foundation::ctl::ComInspectableBase
    {
        typedef typename ::ABI::Windows::Foundation::Internal::GetAbiType<typename ABI::Windows::Foundation::Collections::IIterator<T>::T_complex>::type T_abi;

        BEGIN_IID_MAP
            UUIDOF_INTERFACE_ENTRY(ABI::Windows::Foundation::Collections::IIterator<T>)
        END_IID_MAP(foundation::ctl::ComInspectableBase)

    protected:
        __checkReturn HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
        {
            if (iid == __uuidof(ABI::Windows::Foundation::Collections::IIterator<T>))
            {
                *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IIterator<T> *>(this);
            }
            else
            {
                return __super::QueryInterfaceImpl(iid, ppInterface);
            }
            return S_OK;
        }

    public:
        IteratorBase(): m_nCurrentIndex(0), m_pView(nullptr)
        {
        }

        ~IteratorBase()
        {
            RELEASE_INTERFACE(m_pView);
        }

        HRESULT SetView(_In_ ABI::Windows::Foundation::Collections::IVectorView<T>* pView)
        {
            HRESULT hr = S_OK;
            BOOLEAN bHasCurrent;

            IFCPTR_ASSERT(pView);

            m_pView = pView;
            ADDREF_INTERFACE(pView);

            // Move to the first position.
            IFHR_ASSERT(MoveNext(&bHasCurrent));

			return hr;
        }


        IFACEMETHODIMP get_Current(__out T_abi *current)
        {
            HRESULT hr = S_OK;

            IFCPTR_ASSERT(current);
            IFCEXPECT_ASSERT(m_nCurrentIndex > 0);
            IFHR_ASSERT(GetCurrent(current));

            return S_OK;
        }

        IFACEMETHODIMP get_HasCurrent(__out BOOLEAN *hasCurrent)
        {
            HRESULT hr = S_OK;
            unsigned nSize;

            IFCPTR_ASSERT(hasCurrent);
            IFHR_ASSERT(m_pView->get_Size(&nSize));
            *hasCurrent = (m_nCurrentIndex > 0 && m_nCurrentIndex <= nSize);

            return S_OK;
        }

        IFACEMETHODIMP MoveNext(__out BOOLEAN *hasCurrent)
        {
            HRESULT hr = S_OK;
            unsigned nSize;
            
            IFCPTR_ASSERT(hasCurrent);
            IFHR_ASSERT(m_pView->get_Size(&nSize));

            *hasCurrent = FALSE;
            ClearCurrent();

            if (m_nCurrentIndex >= 0 && m_nCurrentIndex < nSize)
            {
                IFHR_ASSERT(m_pView->GetAt(m_nCurrentIndex, &m_current));

                m_nCurrentIndex++;
                *hasCurrent = TRUE;
            }
            else 
            {
                // Use this as a marker that we're done, this will make it so 
                // HasCurrent will also return false
                m_nCurrentIndex = nSize + 1;
            }

            return S_OK;
        }

    protected:
        virtual __checkReturn HRESULT GetCurrent(__out T_abi *current) = 0;

        virtual void ClearCurrent() = 0;

        T_abi m_current;

    private:
        ABI::Windows::Foundation::Collections::IVectorView<T> *m_pView;
        unsigned m_nCurrentIndex;
    };

    template <class T>
    class Iterator:
        public IteratorBase<T>
    {
        typedef typename ::ABI::Windows::Foundation::Internal::GetAbiType<typename ABI::Windows::Foundation::Collections::IIterator<T>::T_complex>::type T_abi;

    public:
        Iterator()
        {
           m_current = nullptr;
        }

        ~Iterator()
        {
            ClearCurrent();
        }

    protected:
        __checkReturn HRESULT GetCurrent(__out T_abi *current)
        {
            HRESULT hr = S_OK;

            IFCPTR_ASSERT(current);
            *current = m_current;
            ADDREF_INTERFACE(m_current);
            return S_OK;
        }

        void ClearCurrent()
        {
            RELEASE_INTERFACE(m_current);
        }
    };

    template<>
    class Iterator<HSTRING>: 
        public IteratorBase<HSTRING>
    {
    public:
        Iterator()
        {
           m_current = nullptr;
        }

        ~Iterator()
        {
            ClearCurrent();
        }

    protected:
        __checkReturn HRESULT GetCurrent(__out HSTRING *current)
        {
            HRESULT hr = S_OK;

            IFCPTR_ASSERT(current);
            return ::WindowsDuplicateString(m_current,current);
        }

        void ClearCurrent()
        {
            if(m_current)
            {
                ::WindowsDeleteString(m_current);
                m_current = nullptr;
            }
        }
    };

    template<class T>
    class ValueTypeIterator: 
        public IteratorBase<T>
    {
    protected:
        __checkReturn HRESULT GetCurrent(__out T *current)
        {
            IFCPTR_ASSERT(current);
            *current = m_current;

            return S_OK;
        }

        void ClearCurrent()
        {
        }
    };

    template<>
    class Iterator<BYTE>: 
        public ValueTypeIterator<BYTE>
    {
    };

    template<>
    class Iterator<DOUBLE>: 
        public ValueTypeIterator<DOUBLE>
    {
    };

    template<>
    class Iterator<INT16>: 
        public ValueTypeIterator<INT16>
    {
    };

    template<>
    class Iterator<UINT16>: 
        public ValueTypeIterator<UINT16>
    {
    };
    template<>
    class Iterator<INT32>: 
        public ValueTypeIterator<INT32>
    {
    };

    template<>
    class Iterator<UINT32>: 
        public ValueTypeIterator<UINT32>
    {
    };
    template<>
    class Iterator<INT64>: 
        public ValueTypeIterator<INT64>
    {
    };

    template<>
    class Iterator<UINT64>: 
        public ValueTypeIterator<UINT64>
    {
    };

    template<>
    class Iterator<ABI::Windows::Foundation::DateTime>: 
        public ValueTypeIterator<struct ABI::Windows::Foundation::DateTime>
    {
    };
}
