/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionAdapterBase.h
****/

#pragma once


#include <pmod/interfaces.h>
#include "Microsoft.PropertyModel.h"
#include "windows.foundation.h"
#include "windows.foundation.collections.h"
#include "Iterator.h"

#include <foundation/library/adapter_base.h>
#include <foundation/pv_util.h>

//------------------------------------------------------------------------------
// Class:   TMoObservableCollectionIterator
//
// Purpose: Implements winrt IIterator<IInspectable*> from ICollection
//
//------------------------------------------------------------------------------
template <class T>
class TMoObservableCollectionAdapterBase;

template <class T>
class TMoObservableCollectionIterator :
    public foundation::ctl::ComInspectableBase,
    public ABI::Windows::Foundation::Collections::IIterator<T>
{
public:
    static HRESULT CreateInstance(
        TMoObservableCollectionAdapterBase<T> *pMoCollectionModelAdapterBase,
        ABI::Windows::Foundation::Collections::IIterator<T> **ppIterator)
    {
        foundation_assert(pMoCollectionModelAdapterBase);
        foundation_assert(ppIterator);

        TMoObservableCollectionIterator<T> *pIterator = nullptr;
        foundation::ctl::ComInspectableObject<TMoObservableCollectionIterator<T>>::CreateInstance(&pIterator);
        pIterator->_Initialize(pMoCollectionModelAdapterBase);

        *ppIterator = static_cast<ABI::Windows::Foundation::Collections::IIterator<T> *>(pIterator);
        return S_OK;
    }
protected:
    TMoObservableCollectionIterator() :
        _pMoCollectionModelAdapterBase(nullptr),
        _pCoreIterator(nullptr)
    {
    }
    void FinalRelease();
    
    // override ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
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

    HRESULT STDMETHODCALLTYPE get_Current(T * current);

    HRESULT STDMETHODCALLTYPE get_HasCurrent(_Out_ boolean *hasCurrent)
    {
        bool bHasCurrent;
        _pCoreIterator->GetHasCurrent(&bHasCurrent);
        *hasCurrent = bHasCurrent ? TRUE : FALSE;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE MoveNext(_Out_ boolean *hasCurrent)
    {
        bool bHasCurrent;
        _pCoreIterator->MoveNext(&bHasCurrent);
        *hasCurrent = bHasCurrent ? TRUE : FALSE;
        return S_OK;
    }
private:
    HRESULT _Initialize(TMoObservableCollectionAdapterBase<T> *pMoCollectionModelAdapterBase);
private:
    foundation::IIterator_impl<IInspectable*> *_pCoreIterator;
    TMoObservableCollectionAdapterBase<T> *_pMoCollectionModelAdapterBase;
};

//------------------------------------------------------------------------------
// Class:   TMoObservableCollectionAdapterBase
//
// Purpose: Define a base class for a 'IVector' implementation for WinRT
// that is based on an IObservableCollection
//
//------------------------------------------------------------------------------
template <class T>
class TMoObservableCollectionAdapterBase :
    public foundation::AdapterBase
        <
        pmod::IObservableCollection,
        ABI::Windows::Foundation::Collections::IVector<T>,
        &GUID_NULL
        >,
    public ABI::Windows::Foundation::Collections::IIterable<T>,
    public ABI::Windows::Foundation::Collections::IVectorView<T>
{
public:
    // Interface IVectorView<T>
    HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned *size)
    {
        UINT32 _size;
        IFR_ASSERT(m_pInner->GetCount(&_size));
        *size = (unsigned)_size;
        return S_OK;
    }

    __override HRESULT STDMETHODCALLTYPE GetAt(_In_ unsigned index, _Out_ T *item)
    {
        foundation::InspectablePtr spValue;
        IFR(m_pInner->GetItem(index,spValue.GetAddressOf()));

        IFR(ToItem(spValue,item));
        return S_OK;
    }

    __override HRESULT STDMETHODCALLTYPE IndexOf(_In_opt_ T value, _Out_ unsigned *index, _Out_ boolean *found)
    {
        IFCPTR(index);
        IFCPTR(found);

        IFR_ASSERT(m_pInner->IndexOf(foundation::pv_util::CreateValue(value), index));
        *found = *index != foundation::NINDEX;
        return S_OK;
    }

    // Interface IVector<T>
    // read methods 
    __override HRESULT STDMETHODCALLTYPE GetView(ABI::Windows::Foundation::Collections::IVectorView<T> **view)
    {
        HRESULT hr = S_OK;
        IFCPTR(view);

        *view = static_cast<ABI::Windows::Foundation::Collections::IVectorView<T> *>(this);
        (*view)->AddRef();
        return S_OK;
    }

    // write methods
    IFACEMETHODIMP SetAt(_In_ unsigned index, _In_opt_ T item)
    {
        foundation::InspectablePtr spValue;
        IFR(FromItem(item,spValue.GetAddressOf()));

        foundation::ComPtr<pmod::IObservableList> spListModel;
        IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));

        return spListModel->SetItem(index,spValue);
    }

    IFACEMETHODIMP InsertAt(_In_ unsigned index, _In_opt_ T item)
    {
        foundation::InspectablePtr spValue;
        IFR(FromItem(item,spValue.GetAddressOf()));

        foundation::ComPtr<pmod::IObservableList> spListModel;
        IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));

        return spListModel->InsertItem(index,spValue);
    }

    IFACEMETHODIMP RemoveAt(_In_ unsigned index)
    {
        foundation::ComPtr<pmod::IObservableList> spListModel;
        IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));
        return spListModel->RemoveAt((UINT32)index);
    }

    IFACEMETHODIMP Append(_In_opt_ T item)
    {
        foundation::InspectablePtr spValue;
        IFR(FromItem(item,spValue.GetAddressOf()));

        foundation::ComPtr<pmod::IObservableList> spListModel;
        IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));
        IFR(spListModel->AppendItem(spValue));
        return S_OK;
    }

    IFACEMETHODIMP RemoveAtEnd()
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }

    IFACEMETHODIMP Clear()
    {
        foundation::ComPtr<pmod::IObservableList> spListModel;
        IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));

        return spListModel->RemoveAll();
    }
            
    // Interface IIterable<T>
    __override HRESULT STDMETHODCALLTYPE First(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IIterator<T> **first)
    {
        HRESULT hr = S_OK;
        foundation::ComPtr<ABI::Windows::Foundation::Collections::IIterator<T>> spIterator;
        IFHR(TMoObservableCollectionIterator<T>::CreateInstance(this,spIterator.GetAddressOf()));
        spIterator.CopyTo(first);
        return hr;
    }
    
    friend class TMoObservableCollectionIterator<T>;
protected:
    STDMETHOD (QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface) 
    {
        if (iid == __uuidof(ABI::Windows::Foundation::Collections::IVector<T>))
        {
            *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IVector<T> *>(this);
        }
        else if (iid == __uuidof(ABI::Windows::Foundation::Collections::IVectorView<T>))
        {
            *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IVectorView<T> *>(this);
        }
        else if (iid == __uuidof(ABI::Windows::Foundation::Collections::IIterable<T>))
        {
            *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IIterable<T> *>(this);
        }
        else
        {
            return __super::QueryAdapterInterface(iid,ppInterface);
        }
        return S_OK;
    }

    // Overridable Methods
    virtual HRESULT ToItem(IInspectable *value,T *item) = 0;
    virtual HRESULT FromItem(T item,IInspectable **ppValue) = 0;

};

template <class T>
HRESULT TMoObservableCollectionIterator<T>::_Initialize(TMoObservableCollectionAdapterBase<T> *pMoCollectionModelAdapterBase)
{
    _pMoCollectionModelAdapterBase = pMoCollectionModelAdapterBase;
    IFR_ASSERT(pMoCollectionModelAdapterBase->m_pInner->First(&_pCoreIterator));
    // add ref
    foundation::ctl::inspectable::AddRefInterface(pMoCollectionModelAdapterBase);
    return S_OK;
}

template <class T>
void TMoObservableCollectionIterator<T>::FinalRelease()
{
    // release used interafces
    RELEASE_INTERFACE(_pCoreIterator);
    foundation::ctl::inspectable::ReleaseInterface(_pMoCollectionModelAdapterBase);
}

template <class T>
STDMETHODIMP TMoObservableCollectionIterator<T>::get_Current(_Out_ T *current)
{
    foundation::InspectablePtr spValue;
    IFR_ASSERT(_pCoreIterator->GetCurrent(spValue.GetAddressOf()));
    IFR(_pMoCollectionModelAdapterBase->ToItem(spValue, current));
    return S_OK;
}

//------------------------------------------------------------------------------
// Class:   TInspectableMoObservableCollectionAdapterBase
//
// Purpose: Inspectable specialization of the TMoObservableCollectionAdapterBase Class
//
//------------------------------------------------------------------------------
template <class T>
class TInspectableMoObservableCollectionAdapterBase :
    public TMoObservableCollectionAdapterBase<T>
{
protected:

    HRESULT ToItem(_In_ IInspectable *value,_COM_Outptr_ T *item) override
    {
        if(value)
        {
            return value->QueryInterface(__uuidof(T),(void **)item);
        }
        // the Value was null
        *item = nullptr;
        return S_OK;
    }

    HRESULT FromItem(_In_ T item,_COM_Outptr_ IInspectable** ppValue) override
    {
        *ppValue = item;
        if(*ppValue)
        {
            (*ppValue)->AddRef();
        }
        return S_OK;
    }

};

//------------------------------------------------------------------------------
// Class:   TPropertyValueMoObservableCollectionAdapterBase
//
// Purpose: IPropertyValue specialization of the TMoObservableCollectionAdapterBase Class
//
//------------------------------------------------------------------------------
template <class T>
class TPropertyValueMoObservableCollectionAdapterBase :
    public TMoObservableCollectionAdapterBase<T>
{
protected:

    HRESULT ToItem(_In_ IInspectable *value,_COM_Outptr_ T *item) override
    {
        return foundation::pv_util::GetValue(value,item);
    }

    HRESULT FromItem(_In_ T item,_COM_Outptr_ IInspectable** ppValue) override
    {
        foundation::InspectablePtr spValue = foundation::pv_util::CreateValue(item);
        return spValue.CopyTo(ppValue);
    }
};
