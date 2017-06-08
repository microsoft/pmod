/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableVector.h
****/

#pragma once


#include "winrt/MoObservableCollectionAdapterBase.h"
#include "MoEventHandler.h"
#include "VectorChangedEventArgs.h"

//------------------------------------------------------------------------------
// Class:   CObservableVector
//
// Purpose: Define a base class for a 'IVector' implementation for WinRT
//
//------------------------------------------------------------------------------

template <class T>
class CObservableVectorEventHandler :
    public MoEventHandler
        <
            ABI::Windows::Foundation::Collections::VectorChangedEventHandler<T>,
            ABI::Windows::Foundation::Collections::IObservableVector<T>,
            ABI::Windows::Foundation::Collections::IVectorChangedEventArgs
        >
{
};

template <class T>
class CObservableVector :
    public TMoObservableCollectionAdapterBase<T>,
    public CObservableVectorEventHandler<T>,
    public ABI::Windows::Foundation::Collections::IObservableVector<T>
{
public:

    // Interface IObservableVector<T>
    HRESULT STDMETHODCALLTYPE add_VectorChanged (_In_opt_ ABI::Windows::Foundation::Collections::VectorChangedEventHandler<T>* handler, _Out_ EventRegistrationToken*  token) override
    {
        IFR(this->AddHandlerInternal(handler,token));
        IFR(this->_OnAdapterAddEventHandler(handler));
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE remove_VectorChanged(_In_ EventRegistrationToken  token) override
    {
        foundation::ComPtr<ABI::Windows::Foundation::Collections::VectorChangedEventHandler<T>> spEventHandler;
        IFR(this->RemoveHandlerInternal(token,spEventHandler.GetAddressOf()));
        IFR(this->_OnAdapterRemoveEventHandler(spEventHandler));
        return S_OK;
    }

    STDMETHOD (QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface)  override
    {
        if (iid == __uuidof(ABI::Windows::Foundation::Collections::IObservableVector<T>))
        {
            *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IObservableVector<T> *>(this);
        }
        else
        {
            return __super::QueryAdapterInterface(iid,ppInterface);
        }
        return S_OK;
    }   
    
    static HRESULT ToVectorChangedArgs(
            _In_ pmod::INotifyCollectionChangedEventArgs* pArgs,
            ABI::Windows::Foundation::Collections::CollectionChange& wfAction,
            UINT32& index)
    {
        foundation::NotifyCollectionModelChangedAction action;
        IFR(pArgs->GetAction(&action));

        index = (UINT32)-1;

        if(action == foundation::NotifyCollectionModelChangedAction::ItemAdded)
        {
            wfAction = ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_ItemInserted;
            IFR(pArgs->GetNewStartingIndex(&index));
        }
        else if (action == foundation::NotifyCollectionModelChangedAction::ItemReplaced)
        {
            wfAction = ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_ItemChanged;
            IFR(pArgs->GetNewStartingIndex(&index));
        }
        else if (action == foundation::NotifyCollectionModelChangedAction::ItemRemoved)
        {
            wfAction = ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_ItemRemoved;
            IFR(pArgs->GetOldStartingIndex(&index));
        }
        else // action == Reset
        {
            wfAction = ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_Reset;
            index = -1;
        }
        return S_OK;
    }
protected:
    // Interface IInterfaceAdapter
    STDMETHOD (HasEventHandlers)(_Outptr_ bool *pValue) override
    {
		*pValue = this->GetEventHandlers().size() > 0;
		return S_OK;
    }

    STDMETHOD (FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		_In_ IUnknown* pEventArgs) override
    {
        foundation::ComPtr<pmod::INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        if(SUCCEEDED(foundation::QueryInterface(pEventArgs,spNotifyCollectionChangedEventArgs.GetAddressOf())))
        {
            ABI::Windows::Foundation::Collections::CollectionChange wfAction;
            UINT32 index = -1;
            IFR(ToVectorChangedArgs(spNotifyCollectionChangedEventArgs,wfAction,index));

            foundation::ComPtr<ABI::Windows::Foundation::Collections::IVectorChangedEventArgs> spVectorChangedEventArgs;
            IFR(CVectorChangedEventArgs::CreateInstance(
                    wfAction,
                    index,
                    spVectorChangedEventArgs.GetAddressOf()));

            this->FireEvent(
				pFireEventHandlerContainer,
                (ABI::Windows::Foundation::Collections::IObservableVector<T> *)(this),
                spVectorChangedEventArgs
                );
        }
        return S_OK;
    }
};

template <class T>
class CInspectableObservableVector :
    public CObservableVector<T>
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
