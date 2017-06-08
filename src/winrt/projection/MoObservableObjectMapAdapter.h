/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectMapAdapter.h
****/

#pragma once


#include <foundation/library/adapter_base.h>
#include "MoBaseAdapter.h"
#include "windows.foundation.collections.h"

#include "ArrayRefCountWrapperIterator.h"
#include "MoObservableObjectAdapter.h"


//------------------------------------------------------------------------------
// Class:   CMoObservableObjectMapAdapter
//
// Purpose: Implementation for IMap<HSTRING,IInspectable *> for IDictionary
//
//------------------------------------------------------------------------------

class CMoObservableObjectMapAdapter :
    public foundation::AdapterBase
        <
        foundation::IDictionary,
        ABI::Windows::Foundation::Collections::IMap<HSTRING,IInspectable *>,
        &__uuidof(ABI::Windows::Foundation::Collections::IMap<HSTRING,IInspectable *>)
        >,
    public ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,IInspectable *> *>
{
private:
    typedef ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,IInspectable *> *_TypeKeyValuePair;
public: 
    // read methods
    HRESULT STDMETHODCALLTYPE Lookup(_In_opt_ HSTRING key, _Out_ IInspectable * *value)
    {
        return m_pInner->Lookup(key,value);
    }

    HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned int *size)
    {
        UINT32 _size;
        m_pInner->GetSize(&_size);
        *size = (unsigned int)_size;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE HasKey(_In_opt_ HSTRING key, _Out_ boolean *found)
    {
        bool _found;
        IFR(m_pInner->HasKey(key,&_found));
        *found = _found;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetView(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IMapView<HSTRING,IInspectable *> **view)
    {
        return E_NOTIMPL;
    }
    
    // write methods
    HRESULT STDMETHODCALLTYPE Insert(_In_opt_ HSTRING key, _In_opt_ IInspectable * value, _Out_ boolean *replaced)
    {
        bool _replaced;
        IFR(m_pInner->Insert(key,value,&_replaced));
        *replaced = _replaced;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Remove(_In_opt_ HSTRING key)
    {
        return m_pInner->Remove(key);
    }

    HRESULT STDMETHODCALLTYPE Clear()
    {
        return m_pInner->Clear();
    }
    // Interface IIterable<T>
    HRESULT STDMETHODCALLTYPE First(_Outptr_result_maybenull_ ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,IInspectable *> *> **first) override
    {
        HRESULT hr = S_OK;
        utils::TArrayRefCountWrapperIterator<_TypeKeyValuePair,foundation::IDictionaryPair> *pIterator;
        hr = foundation::ctl::ComInspectableObject<utils::TArrayRefCountWrapperIterator<_TypeKeyValuePair,foundation::IDictionaryPair>>::CreateInstance(&pIterator);
        IFHR(hr);
        IFHR(m_pInner->GetView(
            pIterator->GetArrayWrapper().GetSizeAddressOf(),
            pIterator->GetArrayWrapper().GetBufferAddressOf()
            ));
        // Move to the first position.
        BOOLEAN bHasCurrent;
        IFHR(pIterator->MoveNext(&bHasCurrent));

        *first = pIterator;
        return S_OK;
    }

protected:
    STDMETHOD (QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface) 
    {
        if (iid == __uuidof(ABI::Windows::Foundation::Collections::IIterable<_TypeKeyValuePair>))
        {
            *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IIterable<_TypeKeyValuePair> *>(this);
        }
        else
        {
            return __super::QueryAdapterInterface(iid,ppInterface);
        }
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CMoPropertyModelObservableMapAdapter
//
// Purpose: Implementation for IObservableMap<HSTRING,IInspectable *> for IDictionary
//
//------------------------------------------------------------------------------

class CMoPropertyModelObservableMapAdapter :
    public CMoBaseAdapter
    <
        pmod::IObservableObject,
        ABI::Windows::Foundation::Collections::IObservableMap<HSTRING,IInspectable *>,
        ABI::Windows::Foundation::Collections::MapChangedEventHandler<HSTRING, IInspectable *>,
        ABI::Windows::Foundation::Collections::IMapChangedEventArgs<HSTRING>,
        MoEventHandler
        <
            ABI::Windows::Foundation::Collections::MapChangedEventHandler<HSTRING, IInspectable *>,
            ABI::Windows::Foundation::Collections::IObservableMap<HSTRING,IInspectable *>,
            ABI::Windows::Foundation::Collections::IMapChangedEventArgs<HSTRING>
        >,
        TStaticCast
     >
{
private:
    class CMapChangedEventArgs :
        public foundation::ctl::ImplementsInspectable
        <
            ABI::Windows::Foundation::Collections::IMapChangedEventArgs<HSTRING>,
            &__uuidof(ABI::Windows::Foundation::Collections::IMapChangedEventArgs<HSTRING>)
        >
    {
    public:
        static HRESULT CreateInstance(
            ABI::Windows::Foundation::Collections::CollectionChange change,
            HSTRING key,
            ABI::Windows::Foundation::Collections::IMapChangedEventArgs<HSTRING> **ppEventArgs)
        {
            foundation::ctl::ComInspectableObject<CMapChangedEventArgs> *pMapChangedEventArgs = nullptr;
            IFR_ASSERT(foundation::ctl::ComInspectableObject<CMapChangedEventArgs>::CreateInstance(&pMapChangedEventArgs));
            pMapChangedEventArgs->_collectionChange = change;
            pMapChangedEventArgs->_key.CopyFrom(key);

            *ppEventArgs = pMapChangedEventArgs;
            return S_OK;
        }
    protected:
        HRESULT STDMETHODCALLTYPE get_CollectionChange (_Out_ ABI::Windows::Foundation::Collections::CollectionChange* value)
        {
            *value = _collectionChange;
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE get_Key (_Out_ HSTRING* value)
        {
            return WindowsDuplicateString(_key,value);
        }
    private:
        ABI::Windows::Foundation::Collections::CollectionChange _collectionChange;
        foundation::HStringPtr _key;
    };
public: 

    HRESULT STDMETHODCALLTYPE add_MapChanged (_In_opt_ ABI::Windows::Foundation::Collections::MapChangedEventHandler<HSTRING, IInspectable *>* handler, _Out_ EventRegistrationToken*  token)
    {
        return this->add_Handler(handler,token);
    }

    HRESULT STDMETHODCALLTYPE remove_MapChanged(_In_ EventRegistrationToken  token)
    {
        return this->remove_Handler(token);
    }
    
    STDMETHOD (FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		_In_ IUnknown* pEventArgs) override
    {
        foundation_assert(pEventArgs);

        foundation::ComPtr<pmod::IPropertyChangedEventArgs> spPropertyChangedEventArgs;
        foundation::ComPtr<pmod::IBatchPropertyChangedEventArgs> spBatchPropertyChangedEventArgs;
        if(SUCCEEDED(foundation::QueryInterface(pEventArgs,spPropertyChangedEventArgs.GetAddressOf())))
        {
            FirePropertyChangedEventArgs(pFireEventHandlerContainer,spPropertyChangedEventArgs);
        }
        else  if(SUCCEEDED(foundation::QueryInterface(pEventArgs,spBatchPropertyChangedEventArgs.GetAddressOf())))
        {
            foundation::ArrayRefCountWrapper<pmod::IPropertyChangedEventArgs> arrayPropertyChangedEventArgs;
            IFR_ASSERT(spBatchPropertyChangedEventArgs->GetPropertyChanges(
                arrayPropertyChangedEventArgs.GetSizeAddressOf(),
                arrayPropertyChangedEventArgs.GetBufferAddressOf()));
            for(UINT32 index = 0;index < arrayPropertyChangedEventArgs.GetSize();++index)
            {
                FirePropertyChangedEventArgs(pFireEventHandlerContainer, arrayPropertyChangedEventArgs[index]);
            }
        }

        return S_OK;
    }
private:
    HRESULT FirePropertyChangedEventArgs(
		_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		pmod::IPropertyChangedEventArgs *pPropertyChangedEventArgs)
    {
        UINT32 propertyId;
        IFR_ASSERT(pPropertyChangedEventArgs->GetProperty(&propertyId));

        foundation::HStringPtr propertyName;

        IFR(pPropertyChangedEventArgs->GetPropertyName(propertyName.GetAddressOf()));

        foundation::ComPtr<ABI::Windows::Foundation::Collections::IMapChangedEventArgs<HSTRING>> spMapChangedEventArgs;
        IFR_ASSERT(CMapChangedEventArgs::CreateInstance(
            ABI::Windows::Foundation::Collections::CollectionChange_ItemChanged,
            propertyName,
            spMapChangedEventArgs.GetAddressOf()));

        this->FireEvent(pFireEventHandlerContainer,this,spMapChangedEventArgs);
        return S_OK;
    }
};

