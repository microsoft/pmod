/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:collection_viewmodel_class_wrapper.h
****/
#pragma once

#include "interfaces/collection_viewmodel_class.h"


namespace pmod
{
    namespace library {

        // Filter Item Callback
        typedef HRESULT(STDMETHODCALLTYPE *FilterItemCallback)(_In_ foundation::IInspectable * item, _Out_ bool *pValue);

        // Compare Item Callback
        typedef HRESULT(STDMETHODCALLTYPE *CompareItemCallback)(_In_ foundation::IInspectable * item1, _In_ foundation::IInspectable * item2, _Out_ int *pResult);

        // Item Creator Adapter Callback
        typedef HRESULT(STDMETHODCALLTYPE *CreateItemAdapterCallback)(
            _In_ foundation::IInspectable *item,
            _COM_Outptr_ ISourceModel **ppNewInstance);

        // compare_item_callback_adapter Class
        class compare_item_callback_adapter :
            public foundation::_CallbackDelegateAdapterBase
            <
            compare_item_callback_adapter,
            CompareItemCallback,
            library::ICompareItemDelegate,
            &library::IID_ICompareItemDelegate
            >
        {
        protected:
            STDMETHOD(Invoke)(_In_ foundation::IInspectable * item1, _In_ foundation::IInspectable * item2, int *pResult)
            {
                return (*this->GetCallback())(item1, item2, pResult);
            }
        };

        // FilterItemCallbackAdapter Class
        class FilterItemCallbackAdapter :
            public foundation::_CallbackDelegateAdapterBase
            <
            FilterItemCallbackAdapter,
            FilterItemCallback,
            library::IFilterItemDelegate,
            &library::IID_IFilterItemDelegate
            >
        {
        protected:
            STDMETHOD(Invoke)(_In_ foundation::IInspectable * item, bool *pValue)
            {
                return (*this->GetCallback())(item, pValue);
            }
        };

        // CreateItemCallbackAdapter Class
        class CreateItemCallbackAdapter :
            public foundation::_CallbackDelegateAdapterBase
            <
            CreateItemCallbackAdapter,
            CreateItemAdapterCallback,
            library::ICreateItemAdapterDelegate,
            &library::IID_ICreateItemAdapterDelegate
            >
        {
        protected:
            STDMETHOD(Invoke)(_In_ foundation::IInspectable* pItem, ISourceModel **ppSourceModel)
            {
                return (*this->GetCallback())(pItem, ppSourceModel);
            }
        };

        template <class T>
        class _Collection_ViewModelClassWrapper
        {
        public:
            HRESULT SetFilterInfo(_In_ foundation::IExpression *pFilterInfoExpression)
            {
                return this->GetCollectionViewModelClassInternal()->SetFilterInfo(pFilterInfoExpression);
            }

            HRESULT SetFilterInfo(_In_ LPCSTR_t expression)
            {
                foundation::ComPtr<foundation::IExpression> spFilterInfoExpression;
                IFR_ASSERT(foundation::library::CreateExpression(expression, spFilterInfoExpression.GetAddressOf()));
                return SetFilterInfo(spFilterInfoExpression);
            }

            HRESULT AddSortInfo(_In_ foundation::IInspectable *pSortInfoPath, bool isAscending)
            {
                return this->GetCollectionViewModelClassInternal()->AddSortInfo(pSortInfoPath, isAscending);
            }

            HRESULT AddSortInfo(_In_ LPCSTR_t sort_info_str, bool isAscending)
            {
                return AddSortInfo(
                    foundation::pv_util::CreateValue(sort_info_str),
                    isAscending);
            }
            HRESULT GetFilterItemDelegate(_COM_Outptr_ library::IFilterItemDelegate **ppFilterItemDelegate)
            {
                return this->GetCollectionViewModelClassInternal()->GetFilterItemDelegate(ppFilterItemDelegate);
            }

            HRESULT SetFilterItemDelegate(_In_ library::IFilterItemDelegate *pFilterItemDelegate)
            {
                return this->GetCollectionViewModelClassInternal()->SetFilterItemDelegate(pFilterItemDelegate);
            }

            HRESULT SetFilterItemDelegate(_In_ FilterItemCallback callback)
            {
                foundation::ComPtr<library::IFilterItemDelegate> spFilterItemDelegate;
                IFR_ASSERT(FilterItemCallbackAdapter::CreateInstance(callback, spFilterItemDelegate.GetAddressOf()));

                return SetFilterItemDelegate(spFilterItemDelegate);
            }

            HRESULT GetCompareItemDelegate(_COM_Outptr_ library::ICompareItemDelegate **ppCompareItemDelegate)
            {
                return this->GetCollectionViewModelClassInternal()->GetCompareItemDelegate(ppCompareItemDelegate);
            }

            HRESULT SetCompareItemDelegate(_In_ library::ICompareItemDelegate *pCompareItemDelegate)
            {
                return this->GetCollectionViewModelClassInternal()->SetCompareItemDelegate(pCompareItemDelegate);
            }

            HRESULT SetCompareItemDelegate(_In_ CompareItemCallback callback)
            {
                foundation::ComPtr<library::ICompareItemDelegate> spComparerItemDelegate;
                compare_item_callback_adapter::CreateInstance(callback, spComparerItemDelegate.GetAddressOf());
                return SetCompareItemDelegate(spComparerItemDelegate);
            }

            HRESULT GetCeateItemAdapterDelegate(_COM_Outptr_ library::ICreateItemAdapterDelegate **ppCreateItemAdapterDelegate)
            {
                return this->GetCollectionViewModelClassInternal()->GetCreateItemAdapterDelegate(ppCreateItemAdapterDelegate);
            }

            HRESULT SetCeateItemAdapterDelegate(_In_ library::ICreateItemAdapterDelegate *pCreateItemAdapterDelegate)
            {
                return this->GetCollectionViewModelClassInternal()->SetCreateItemAdapterDelegate(pCreateItemAdapterDelegate);
            }

            HRESULT SetCeateItemAdapterDelegate(_In_ CreateItemAdapterCallback callback)
            {
                foundation::ComPtr<library::ICreateItemAdapterDelegate> spCreateItemAdapterDelegate;
                CreateItemCallbackAdapter::CreateInstance(callback, spCreateItemAdapterDelegate.GetAddressOf());

                return SetCeateItemAdapterDelegate(spCreateItemAdapterDelegate);
            }

            HRESULT RefreshSourceItem(_In_ foundation::IInspectable *item)
            {
                return this->GetCollectionViewModelClassInternal()->RefreshSourceItem(item);
            }

        protected:
            inline library::ICollectionViewModelClass *GetCollectionViewModelClassInternal()
            {
                T *pThis = static_cast<T *>(this);
                library::ICollectionViewModelClass *pClassInterface = pThis->GetCollectionViewModelClass();
                foundation_assert(pClassInterface != nullptr);
                return pClassInterface;
            }
        };
    }
}
