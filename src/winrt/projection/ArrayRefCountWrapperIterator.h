/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ArrayRefCountWrapperIterator.h
****/

#pragma once


#include "windows.foundation.collections.h"
#include <foundation/ctl/com_library.h>

#include <foundation/array_wrapper.h>


namespace utils
{
    template <class T,class TInner>
    class TArrayRefCountWrapperIterator: 
        public ABI::Windows::Foundation::Collections::IIterator<T>,
        public foundation::ctl::ComInspectableBase
    {
    private:
        typedef typename ABI::Windows::Foundation::Internal::GetAbiType<T>::type     T_abi;

        BEGIN_IID_MAP
            UUIDOF_INTERFACE_ENTRY(ABI::Windows::Foundation::Collections::IIterator<T>)
        END_IID_MAP(foundation::ctl::ComInspectableBase)

    protected:
        HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
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
        TArrayRefCountWrapperIterator(): m_nCurrentIndex(0)
        {
        }

        void FinalRelease() override
        {
            m_ArrayWrapper.Clear();
            foundation::ctl::ComInspectableBase::FinalRelease();
        }

        IFACEMETHODIMP get_Current(__out T_abi *current)
        {
            HRESULT hr = S_OK;

            IFCPTR(current);
            IFCEXPECT(m_nCurrentIndex > 0);

            IFR(foundation::QueryInterface(m_ArrayWrapper[m_nCurrentIndex - 1],current));
            return S_OK;
        }

        IFACEMETHODIMP get_HasCurrent(__out BOOLEAN *hasCurrent)
        {
            HRESULT hr = S_OK;

            IFCPTR(hasCurrent);
            *hasCurrent = (m_nCurrentIndex > 0 && m_nCurrentIndex <= m_ArrayWrapper.GetSize());

            return S_OK;
        }

        IFACEMETHODIMP MoveNext(__out BOOLEAN *hasCurrent)
        {
            HRESULT hr = S_OK;
            UINT32 nSize = m_ArrayWrapper.GetSize();
            
            IFCPTR(hasCurrent);
            *hasCurrent = FALSE;

            if (m_nCurrentIndex >= 0 && m_nCurrentIndex < nSize)
            {
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

        foundation::ArrayRefCountWrapper<TInner> &GetArrayWrapper() { return m_ArrayWrapper; }

    private:
        foundation::ArrayRefCountWrapper<TInner> m_ArrayWrapper;
        UINT32 m_nCurrentIndex;
    };
}
