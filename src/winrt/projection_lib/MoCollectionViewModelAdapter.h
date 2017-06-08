/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCollectionViewModelAdapter.h
****/

#pragma once
#include "MoCallbackAdapterBase.h"

#include "Microsoft.PropertyModel.Library.h"

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoCollectionViewModelAdapter
//
// Purpose: Implementation of ICollectionViewModel Interface
//
//------------------------------------------------------------------------------
class CMoCollectionViewModelAdapter :
    public foundation::AdapterBase<
        pmod::library::ICollectionViewModelClass,
        ICollectionViewModel,
        &__uuidof(ICollectionViewModel)
        >
{
public:
    // Interface ICollectionViewModel
};

//------------------------------------------------------------------------------
// Class:   CMoCollectionViewModelClassAdapter
//
// Purpose: Implementation of ICollectionViewModelClass Interface
//
//------------------------------------------------------------------------------
class CMoCollectionViewModelClassAdapter :
    public foundation::AdapterBase<
        pmod::library::ICollectionViewModelClass,
        ICollectionViewModelClass,
        &__uuidof(ICollectionViewModelClass)
        >
{
private:
    class CCreateItemAdapterCallbackAdapter:
        public TMoCallbackAdapterBase
        <
        ICreateItemAdapterCallback,
        pmod::library::ICreateItemAdapterDelegate,
        &pmod::library::IID_ICreateItemAdapterDelegate
        >
    {
    protected:
        STDMETHOD (Invoke)(_In_ IInspectable* pItem,_COM_Outptr_ pmod::ISourceModel **ppSourceModel)
        {
            foundation::ComPtr<ISourceModel> spMoSourceModel;
            IFR(this->GetMoDelegate()->Invoke(pItem,spMoSourceModel.GetAddressOf()));
            if(spMoSourceModel != nullptr)
            {
                IFR(foundation::QueryInterface(spMoSourceModel,ppSourceModel));
            }
            else
            {
                *ppSourceModel = nullptr;
            }
            return S_OK;
        }
    };

    class compare_item_callback_adapter:
        public TMoCallbackAdapterBase
        <
        ICompareItemCallback,
        pmod::library::ICompareItemDelegate,
        &pmod::library::IID_ICompareItemDelegate
        >
    {
    protected:
        STDMETHOD (Invoke)(_In_ IInspectable * item1,_In_ IInspectable * item2,_Outptr_ int *pResult)
        {
            IFR(this->GetMoDelegate()->Invoke(item1,item2,pResult));
            return S_OK;
        }
    };

    class FilterItemCallbackAdapter:
        public TMoCallbackAdapterBase
        <
        IFilterItemCallback,
        pmod::library::IFilterItemDelegate,
        &pmod::library::IID_IFilterItemDelegate
        >
    {
    protected:
        STDMETHOD (Invoke)(_In_ IInspectable * item,_Outptr_ bool *pValue)
        {
            boolean _result;
            IFR(this->GetMoDelegate()->Invoke(item,&_result));
            *pValue = _result ? true:false;
            return S_OK;
        }
    };

public:
    // Interface ICollectionViewModelClass
    HRESULT STDMETHODCALLTYPE get_CreateItemAdapterCallback( 
       ICreateItemAdapterCallback **value)
    {
        foundation::ComPtr<pmod::library::ICreateItemAdapterDelegate> spCreateItemAdapterDelegate;
        IFR(m_pInner->GetCreateItemAdapterDelegate(spCreateItemAdapterDelegate.GetAddressOf()));
        IFR(CCreateItemAdapterCallbackAdapter::GetModernDelegateFromCore(spCreateItemAdapterDelegate,value));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE put_CreateItemAdapterCallback( 
        ICreateItemAdapterCallback *value)
    {
        foundation::ComPtr<pmod::library::ICreateItemAdapterDelegate> spCreateItemAdapterDelegate;
        if(value)
        {
            IFR(CCreateItemAdapterCallbackAdapter::CreateInstance<CCreateItemAdapterCallbackAdapter>(value,spCreateItemAdapterDelegate.GetAddressOf()));
        }
        IFR(m_pInner->SetCreateItemAdapterDelegate(spCreateItemAdapterDelegate));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_CompareItemCallback( 
        ICompareItemCallback **value)
    {
        foundation::ComPtr<pmod::library::ICompareItemDelegate> spCompareItemDelegate;
        IFR(m_pInner->GetCompareItemDelegate(spCompareItemDelegate.GetAddressOf()));
        IFR(compare_item_callback_adapter::GetModernDelegateFromCore(spCompareItemDelegate,value));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE put_CompareItemCallback( 
        ICompareItemCallback *value)
    {
        foundation::ComPtr<pmod::library::ICompareItemDelegate> spCompareItemDelegate;
        if(value)
        {
            IFR(compare_item_callback_adapter::CreateInstance<compare_item_callback_adapter>(value,spCompareItemDelegate.GetAddressOf()));
        }
        IFR(m_pInner->SetCompareItemDelegate(spCompareItemDelegate));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_FilterItemCallback( 
        IFilterItemCallback **value) 
    {
        foundation::ComPtr<pmod::library::IFilterItemDelegate> spFilterItemDelegate;
        IFR(m_pInner->GetFilterItemDelegate(spFilterItemDelegate.GetAddressOf()));
        IFR(FilterItemCallbackAdapter::GetModernDelegateFromCore(spFilterItemDelegate,value));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE put_FilterItemCallback( 
        IFilterItemCallback *value)
    {
        foundation::ComPtr<pmod::library::IFilterItemDelegate> spFilterItemDelegate;
        if(value)
        {
            IFR(FilterItemCallbackAdapter::CreateInstance<FilterItemCallbackAdapter>(value,spFilterItemDelegate.GetAddressOf()));
        }
        IFR(m_pInner->SetFilterItemDelegate(spFilterItemDelegate));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AddSortInfo( 
        IInspectable *sortInfoPath,
        boolean isAscending)
    {
        return m_pInner->AddSortInfo(sortInfoPath,isAscending ? true:false);
    }

    HRESULT STDMETHODCALLTYPE SetFilterInfo( 
        IExpression *filterExpression)
    {
        foundation::ComPtr<foundation::IExpression> spExpression;
        if(filterExpression)
        {
            IFR(foundation::QueryInterface(filterExpression,spExpression.GetAddressOf()));
        }
        return m_pInner->SetFilterInfo(spExpression);
    }

    HRESULT STDMETHODCALLTYPE RefreshSourceItem( IInspectable *item) override
    {
        return m_pInner->RefreshSourceItem(item);
    }
};
//------------------------------------------------------------------------------
// Class:   CMoCollectionViewModelProtectedAdapter
//
// Purpose: Implementation of ICollectionViewModelProtected Interface
//
//------------------------------------------------------------------------------
class CMoCollectionViewModelProtectedAdapter :
    public foundation::AdapterBase<
    pmod::library::ICollectionViewModelClass,
    ICollectionViewModelProtected,
    &__uuidof(ICollectionViewModelProtected)
    >
{
    // Interface ICollectionViewModelProtected
};
//------------------------------------------------------------------------------
// Class:   CMoCollectionViewModelOverridesAdapter
//
// Purpose: Implementation of ICollectionViewModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoCollectionViewModelOverridesAdapter :
    public foundation::AdapterBase<
        pmod::library::ICollectionViewModelClass,
        ICollectionViewModelBaseOverrides,
        &__uuidof(ICollectionViewModelBaseOverrides)
        >
{
public:
    HRESULT STDMETHODCALLTYPE OnSourceCollectionChanged( 
                            /* [in] */ INotifyCollectionChangedEventArgs *pEventArgs)
    {
        UNREFERENCED_PARAMETER(pEventArgs);
        
        return S_OK;    
    }

    HRESULT STDMETHODCALLTYPE OnSourceItemPropertyChanged( 
                            /* [in] */ IObservableObject *item,
                            /* [in] */ int propertyId,
                            /* [in] */ IInspectable *oldValue,
                            /* [in] */ IInspectable *newValue,
                            /* [out][retval] */ boolean *bNeedRefresh)
    {
        UNREFERENCED_PARAMETER(item);
        UNREFERENCED_PARAMETER(propertyId);
        UNREFERENCED_PARAMETER(oldValue);
        UNREFERENCED_PARAMETER(newValue);
        *bNeedRefresh = true;
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE FilterItem( 
        /* [in] */ IInspectable *item,
        /* [out][retval] */ boolean *bResult)
    {
        UNREFERENCED_PARAMETER(item);
        *bResult =  true;
        return S_FALSE;
    }
                        
    HRESULT STDMETHODCALLTYPE CompareItem( 
        /* [in] */ IInspectable *item1,
        /* [in] */ IInspectable *item2,
        /* [out][retval] */ int *result)
    {
        UNREFERENCED_PARAMETER(item1);
        UNREFERENCED_PARAMETER(item2);
        *result = -1;
        return S_FALSE;
    }
                        
    HRESULT STDMETHODCALLTYPE OnItemSourceAttached( 
        /* [in] */ IObservableObject *item)
    {
        UNREFERENCED_PARAMETER(item);
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE OnItemSourceDetached( 
        /* [in] */ IObservableObject *item)
    {
        UNREFERENCED_PARAMETER(item);
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE CreateModelAdapter( 
        /* [in] */ IInspectable *item,
        /* [out][retval] */ ISourceModel **ppSourceModel)
    {
        UNREFERENCED_PARAMETER(item);
        *ppSourceModel = nullptr;
        return S_FALSE;
    }
};


