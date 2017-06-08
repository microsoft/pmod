/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:VectorView.h
****/

#pragma once

#undef new // to avoid build error when including WRL & mix with new operator override

#include "windows.foundation.h"
#include "windows.foundation.collections.h"
#include <winrt/com_activation.h>
#include <winrt/Iterator.h>
#include <list>

#if defined(_WINDOWS_PHONE_80)
// we need Windows.Foundation.Collections.IIterable<> not present in 'windows.foundation.h'
#include "Microsoft.PropertyModel.h"
#endif

EXTERN_C const GUID  FAR VectorGuid;
EXTERN_C const GUID  FAR VectorViewGuid;
EXTERN_C const GUID  FAR IteratorGuid;
EXTERN_C const GUID  FAR IterableGuid;
EXTERN_C const GUID  FAR ObservableVectorGuid;

namespace utils
{
    template<class T>
    class IsIInspectable
    {
    public: 
        enum { Inspectable = 0 };
    };

    template<>
    class IsIInspectable<IInspectable *>
    {
    public:
        enum { Inspectable = 1 };
    };

    template <typename T>
    class ValueTypeViewBase: 
        public ABI::Windows::Foundation::Collections::IVectorView<T>,
        public ABI::Windows::Foundation::Collections::IIterable<T>,
        public ctlwinrt::SupportErrorInfo
    {
        BEGIN_IID_MAP
            UUIDOF_INTERFACE_ENTRY(ABI::Windows::Foundation::Collections::IVectorView<T>)
            UUIDOF_INTERFACE_ENTRY(ABI::Windows::Foundation::Collections::IIterable<T>)
        END_IID_MAP(ctlwinrt::SupportErrorInfo)

    protected:
        __checkReturn HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
        {
            if (iid == __uuidof(ABI::Windows::Foundation::Collections::IVectorView<T>))
            {
                *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IVectorView<T> *>(this);
                RRETURN(S_OK);
            }
            else if (iid == __uuidof(ABI::Windows::Foundation::Collections::IIterable<T>))
            {
                *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IIterable<T> *>(this);
                RRETURN(S_OK);
            }

            RRETURN(ctlwinrt::SupportErrorInfo::QueryInterfaceImpl(iid, ppInterface));
        }
        
    public:
        ~ValueTypeViewBase()
        {
            ClearView();
        }

        virtual void ClearView()
        {
            m_list.clear();
        }

        __checkReturn HRESULT SetView(_In_ ABI::Windows::Foundation::Collections::IIterator<T>* view)
        {
            HRESULT hr = S_OK;
            IFCPTR(view);

            ClearView();
            BOOLEAN hasCurrent = FALSE;
            IFHR(view->get_HasCurrent(&hasCurrent));
            while (hasCurrent)
            {
                T item;
                IFHR(view->get_Current(&item));
                m_list.push_back(item);
                IFHR(view->MoveNext(&hasCurrent));
            }

            RRETURN(hr);
        }

        __checkReturn HRESULT SetView(__in_ecount_opt(cItems) T* pItems, _In_ UINT cItems)
        {
            ClearView();
            if (pItems)
            {
                for (UINT iItem = 0; iItem < cItems; iItem++)
                {
                    m_list.push_back(pItems[iItem]);
                }
            }
            RRETURN(S_OK);
        }

        IFACEMETHODIMP get_Size(__out UINT *size)
        {
            *size = m_list.size();
            RRETURN(S_OK);
        }

        IFACEMETHODIMP GetAt(_In_opt_ UINT index, __out T *item)
        {
            std::list<T>::iterator it;
            UINT nPosition = 0;

            for (it = m_list.begin(); it != m_list.end(); ++nPosition, ++it)
            {
                if (nPosition == index)
                {
                    *item = *it;
                    RRETURN(S_OK);
                }
            }

            RRETURN(E_FAIL);
        }

        // IIterable<T> implementation
        IFACEMETHODIMP First(__deref_out_opt ABI::Windows::Foundation::Collections::IIterator<T> **first)
        { 
            HRESULT hr = S_OK;
            Iterator<T>* pIterator = nullptr;

            IFHR(foundation::ctl::ComInspectableObject<Iterator<T>>::CreateInstance(&pIterator));
            IFHR(pIterator->SetView(this));
            *first = pIterator;
            pIterator = nullptr;

        Cleanup:
            foundation::ctl::inspectable::ReleaseInterface(pIterator);
            RRETURN(hr);
        }

    protected:
        std::list<T> m_list;
    };

    template <class T>
    class ViewBase: 
        public ABI::Windows::Foundation::Collections::IVectorView<T>,
        public ABI::Windows::Foundation::Collections::IIterable<T>,
        public ctlwinrt::SupportErrorInfo
    {
        typedef typename ::ABI::Windows::Foundation::Internal::GetAbiType<typename ABI::Windows::Foundation::Collections::IVector<T>::T_complex>::type T_abi;

        BEGIN_IID_MAP
            UUIDOF_INTERFACE_ENTRY(ABI::Windows::Foundation::Collections::IVectorView<T>)
            UUIDOF_INTERFACE_ENTRY(ABI::Windows::Foundation::Collections::IIterable<T>)
        END_IID_MAP(ctlwinrt::SupportErrorInfo)

    protected:
        __checkReturn HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
        {
            if (iid == __uuidof(ABI::Windows::Foundation::Collections::IVectorView<T>) || (iid == VectorViewGuid && IsIInspectable<T>::Inspectable))
            {
                *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IVectorView<T> *>(this);
                RRETURN(S_OK);
            }
            else if (iid == __uuidof(ABI::Windows::Foundation::Collections::IIterable<T>) || (iid == IterableGuid && IsIInspectable<T>::Inspectable))
            {
                *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IIterable<T> *>(this);
                RRETURN(S_OK);
            }

            RRETURN(ctlwinrt::SupportErrorInfo::QueryInterfaceImpl(iid, ppInterface));
        }
        
    public:
        ~ViewBase()
        {
            ClearView();
        }

        virtual void ClearView()
        {
            std::list<T_abi>::iterator it;
            for (it = m_list.begin(); it != m_list.end(); ++it)
            {
                (*it)->Release();
            }

            m_list.clear();
        }

        __checkReturn HRESULT SetView(_In_ ABI::Windows::Foundation::Collections::IIterator<T>* view)
        {
            HRESULT hr = S_OK;
            IFCPTR(view);

            ClearView();
            BOOLEAN hasCurrent = FALSE;
            IFHR(view->get_HasCurrent(&hasCurrent));
            while (hasCurrent)
            {
                T_abi item;
                IFHR(view->get_Current(&item));
                m_list.push_back(item);
                ADDREF_INTERFACE(item);
                IFHR(view->MoveNext(&hasCurrent));
            }

        Cleanup:
            RRETURN(hr);
        }

        __checkReturn HRESULT SetView(__in_ecount_opt(cItems) T* pItems, _In_ UINT cItems)
        {
            ClearView();
            if (pItems)
            {
                for (UINT iItem = 0; iItem < cItems; iItem++)
                {
                    m_list.push_back(pItems[iItem]);
                    ADDREF_INTERFACE(pItems[iItem]);
                }
            }
            RRETURN(S_OK);
        }

        IFACEMETHODIMP get_Size(__out UINT *size)
        {
            *size = (UINT)m_list.size();
            RRETURN(S_OK);
        }

        IFACEMETHODIMP GetAt(_In_opt_ UINT index, __out T_abi *item)
        {
            std::list<T_abi>::iterator it;
            UINT nPosition = 0;

            for (it = m_list.begin(); it != m_list.end(); ++nPosition, ++it)
            {
                if (nPosition == index)
                {
                    *item = *it;
                    ADDREF_INTERFACE(*it);
                    RRETURN(S_OK);
                }
            }

            RRETURN(E_FAIL);
        }

        // IIterable<T> implementation
        IFACEMETHODIMP First(__deref_out_opt ABI::Windows::Foundation::Collections::IIterator<T> **first)
        { 
            HRESULT hr = S_OK;
            Iterator<T>* pIterator = nullptr;

            IFCL(foundation::ctl::ComInspectableObject<Iterator<T>>::CreateInstance(&pIterator));
            IFCL(pIterator->SetView(this));
            *first = pIterator;
            pIterator = nullptr;

        Cleanup:
            foundation::ctl::inspectable::ReleaseInterface(pIterator);
            RRETURN(hr);
        }

    protected:
        std::list<T_abi> m_list;
    };

    template <typename T>
    class ValueTypeView:
        public ValueTypeViewBase<T>
    {
    public:
        IFACEMETHODIMP IndexOf(_In_opt_ T value, __out UINT *index, __out BOOLEAN *found)
        {
            std::list<T>::iterator it;
            UINT nPosition = 0;

            for (it = m_list.begin(); it != m_list.end(); ++nPosition, ++it)
            {
                if (*it == value)
                {
                    *index = nPosition;
                    *found = TRUE;
                    RRETURN(S_OK);
                }
            }

            *index = 0;
            *found = FALSE;
            RRETURN(S_OK);
        }
    };

    template <class T>
    class View:
        public ViewBase<T>
    {
        typedef typename ::ABI::Windows::Foundation::Internal::GetAbiType<typename ABI::Windows::Foundation::Collections::IVector<T>::T_complex>::type T_abi;

    public:
        IFACEMETHODIMP IndexOf(_In_opt_ T_abi value, __out UINT *index, __out BOOLEAN *found)
        {
            std::list<T_abi>::iterator it;
            UINT nPosition = 0;

            for (it = m_list.begin(); it != m_list.end(); ++nPosition, ++it)
            {
                if (*it == value)
                {
                    *index = nPosition;
                    *found = TRUE;
                    RRETURN(S_OK);
                }
            }

            *index = 0;
            *found = FALSE;
            RRETURN(S_OK);
        }
    };

    //
    // Template Specialization of View for IInspectable
    //
    template<>
    class View<IInspectable*>: 
        public ViewBase<IInspectable*>
    {
    public:
        IFACEMETHOD(IndexOf)(
            _In_opt_ IInspectable* value, 
            __out UINT *index, 
            __out BOOLEAN *found);
    };

    template <typename T>
    class ValueTypeCollection: 
        public ABI::Windows::Foundation::Collections::IVector<T>, 
        public ValueTypeView<T>
    {
        BEGIN_IID_MAP
            UUIDOF_INTERFACE_ENTRY(ABI::Windows::Foundation::Collections::IVector<T>)
        END_IID_MAP(ValueTypeView<T>)

    protected:
        __checkReturn HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
        {
            if (iid == __uuidof(ABI::Windows::Foundation::Collections::IVector<T>))
            {
                *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IVector<T> *>(this);
                RRETURN(S_OK);
            }

            RRETURN(ValueTypeView::QueryInterfaceImpl(iid, ppInterface));
        }
        
    public:    

        IFACEMETHODIMP GetAt(_In_opt_ UINT index, __out T *item)
        {
            RRETURN(ValueTypeView::GetAt(index, item));
        }

        IFACEMETHODIMP get_Size(__out UINT *size)
        {
            RRETURN(ValueTypeView::get_Size(size));
        }

        IFACEMETHODIMP GetView(__deref_out_opt ABI::Windows::Foundation::Collections::IVectorView<T>** view)
        {
            if (view)
            {
                RRETURN(foundation::do_query_interface(*view, this));
            }
            RRETURN(E_NOTIMPL);
        }  

        IFACEMETHODIMP IndexOf(_In_opt_ T value, __out UINT *index, __out BOOLEAN *found)
        {
            RRETURN(ValueTypeView::IndexOf(value, index, found));
        }

        IFACEMETHODIMP SetAt(_In_ UINT index, _In_opt_ T item)
        {
            HRESULT hr = S_OK;
            std::list<T>::iterator it;
            UINT nPosition = 0;

            for (it = m_list.begin(); nPosition < index && it != m_list.end(); ++it, ++nPosition){}
            *it = item;

            IFHR(RaiseVectorChanged(ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_ItemChanged, index));
        Cleanup:
            RRETURN(S_OK);
        }

        IFACEMETHODIMP InsertAt(_In_ UINT index, _In_opt_ T item)
        {
            HRESULT hr = S_OK;
            std::list<T>::iterator it;
            UINT nPosition = 0;
            for (it = m_list.begin(); nPosition < index && it != m_list.end(); ++it, ++nPosition){}

            m_list.insert(it, item);
            IFHR(RaiseVectorChanged(ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_ItemInserted, index));

        Cleanup:
            RRETURN(S_OK);
        }
        
        IFACEMETHODIMP RemoveAt(_In_ UINT index)
        {
            HRESULT hr = S_OK;
            std::list<T>::iterator it;
            UINT nPosition = 0;

            for (it = m_list.begin(); nPosition < index && it != m_list.end(); ++it, ++nPosition){}

            m_list.erase(it);
            IFHR(RaiseVectorChanged(ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_ItemRemoved, index));

        Cleanup:
            RRETURN(S_OK);
        }
        
        IFACEMETHODIMP Append(_In_opt_ T item)
        {
            HRESULT hr = S_OK;
            m_list.push_back(item);
            IFHR(RaiseVectorChanged(ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_ItemInserted, m_list.size() - 1));

        Cleanup:
            RRETURN(S_OK);
        }
        
        IFACEMETHODIMP RemoveAtEnd()
        {
            RRETURN(RemoveAt(m_list.size() - 1));
        }
        
        IFACEMETHODIMP Clear()
        {
            HRESULT hr = S_OK;
            ClearView();
            IFHR(RaiseVectorChanged(ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_Reset, 0));

        Cleanup:
            RRETURN(S_OK);
        }

    protected:
        virtual __checkReturn HRESULT RaiseVectorChanged(_In_ ABI::Windows::Foundation::Collections::CollectionChange action, UINT index)
        {
            RRETURN(S_OK);
        }
    };
}
