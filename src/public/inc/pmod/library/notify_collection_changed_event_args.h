/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:notify_collection_changed_event_args.h
****/
#pragma once

#include <foundation/library/event_args_base.h>
#include <pmod/interfaces/observable_object.h>
#include <pmod/logger_type_info.h>
#include "interfaces/value_converter.h"

namespace pmod {

    namespace library {


        class _NotifyCollectionChangedEventArgsBase :
            public foundation::library::_EventArgsBase<pmod::INotifyCollectionChangedEventArgs>
        {
        public:
            typedef std::vector<foundation::InspectablePtr> _Vector_inspectable_ptr_type;
        protected:
            HRESULT _Initialize(
                foundation::NotifyCollectionModelChangedAction action,
                UINT32 oldStartingIndex,
                UINT32 newStartingIndex)
            {
                m_Action = action;
                m_OldStartingIndex = oldStartingIndex;
                m_NewStartingIndex = newStartingIndex;
                return S_OK;
            }
        protected:
            typedef foundation::library::_EventArgsBase<pmod::INotifyCollectionChangedEventArgs> _TBASE;
            // Interface INotifyCollectionChangedEventArgs
            STDMETHOD(GetAction)(_Outptr_ foundation::NotifyCollectionModelChangedAction* action) override
            {
                *action = m_Action;
                return S_OK;
            }

            STDMETHOD(GetNewStartingIndex)(_Outptr_ UINT32* index) override
            {
                *index = m_NewStartingIndex;
                return S_OK;
            }

            STDMETHOD(GetOldStartingIndex)(_Outptr_ UINT32* index)  override
            {
                *index = m_OldStartingIndex;
                return S_OK;
            }

            HRESULT AppendLogParameters(foundation::logger_util::log_parameter_pair_ptr_vector_type& logParameters) override
            {
                IFR_ASSERT(_TBASE::AppendLogParameters(logParameters));

                // event id
                IFR_ASSERT(foundation::logger_util::AppendLogParameter(
                    pmod::NotifyCollectionChangedEventArgs_LogRecord_Action_Id,
                    foundation::pv_util::CreateValue((UINT32)m_Action),
                    logParameters
                    ));
                // Old Index
                IFR_ASSERT(foundation::logger_util::AppendLogParameter(
                    pmod::NotifyCollectionChangedEventArgs_LogRecord_OldStartingIndex_Id,
                    foundation::pv_util::CreateValue(m_OldStartingIndex),
                    logParameters
                    ));
                // New Index
                IFR_ASSERT(foundation::logger_util::AppendLogParameter(
                    pmod::NotifyCollectionChangedEventArgs_LogRecord_NewStartingIndex_Id,
                    foundation::pv_util::CreateValue(m_NewStartingIndex),
                    logParameters
                    ));
                return S_OK;
            }

        private:
            foundation::NotifyCollectionModelChangedAction m_Action;
            UINT32 m_OldStartingIndex;
            UINT32 m_NewStartingIndex;
        };

        class NotifyCollectionChangedEventArgsImpl :
            public _NotifyCollectionChangedEventArgsBase
        {
        public:
            template <class T>
            static HRESULT CreateInstance(
                _In_ foundation::NotifyCollectionModelChangedAction action,
                _In_ const T& t,
                _In_ UINT32 index,
                _Outptr_ pmod::INotifyCollectionChangedEventArgs **ppNewInstance)
            {
                NotifyCollectionChangedEventArgsImpl *pT;
                IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
                pT->_Initialize(action, t, index);
                *ppNewInstance = pT;
                return S_OK;
            }

            static HRESULT CreateResetInstance(
                _In_ const _Vector_inspectable_ptr_type& oldItems,
                _In_ const _Vector_inspectable_ptr_type& newItems,
                _Outptr_ pmod::INotifyCollectionChangedEventArgs **ppNewInstance)
            {
                NotifyCollectionChangedEventArgsImpl *pT;
                IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
                pT->_Initialize(oldItems, newItems);
                *ppNewInstance = pT;
                return S_OK;
            }

            static HRESULT CreateReplaceInstance(
                _In_ UINT32 nIndex,
                _In_ const _Vector_inspectable_ptr_type& oldItems,
                _In_ const _Vector_inspectable_ptr_type& newItems,
                _Outptr_ pmod::INotifyCollectionChangedEventArgs **ppNewInstance)
            {
                NotifyCollectionChangedEventArgsImpl *pT;
                IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
                pT->_Initialize(foundation::NotifyCollectionModelChangedAction::ItemReplaced, nIndex, nIndex, oldItems, newItems);
                *ppNewInstance = pT;
                return S_OK;
            }

            template <class T>
            static HRESULT CreateMoveInstance(
                _In_ foundation::NotifyCollectionModelChangedAction action,
                _In_ const T& t,
                _In_ UINT32 nOldIndex,
                _In_ UINT32 nNewIndex,
                _Outptr_ pmod::INotifyCollectionChangedEventArgs **ppNewInstance)
            {
                NotifyCollectionChangedEventArgsImpl *pT;
                IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
                pT->_Initialize(action, t, nOldIndex, nNewIndex);
                *ppNewInstance = pT;
                return S_OK;
            }

            static HRESULT CreateInstance(
                _In_ foundation::NotifyCollectionModelChangedAction action,
                _In_ UINT32 oldStartingIndex,
                _In_ UINT32 newStartingIndex,
                _In_ const _Vector_inspectable_ptr_type& oldItems,
                _In_ const _Vector_inspectable_ptr_type& newItems,
                _Outptr_ pmod::INotifyCollectionChangedEventArgs **ppNewInstance)
            {
                NotifyCollectionChangedEventArgsImpl *pT;
                IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
                pT->_Initialize(
                    action,
                    oldStartingIndex,
                    newStartingIndex,
                    oldItems,
                    newItems);
                *ppNewInstance = pT;
                return S_OK;
            }

            inline IValueConverterCallback **GetValueConverterCallbackPtr()
            {
                return m_spValueConverterCallback.GetAddressOf();
            }
        private:
            HRESULT _Initialize
                (
                    foundation::NotifyCollectionModelChangedAction action,
                    UINT32 oldStartingIndex,
                    UINT32 newStartingIndex,
                    const _Vector_inspectable_ptr_type * olditems,
                    const _Vector_inspectable_ptr_type * newitems
                    )
            {
                _NotifyCollectionChangedEventArgsBase::_Initialize(action, oldStartingIndex, newStartingIndex);
                if (olditems)
                {
                    m_OldItems = *olditems;
                }
                if (newitems)
                {
                    m_NewItems = *newitems;
                }
                return S_OK;
            }
        protected:
            HRESULT _Initialize(
                foundation::NotifyCollectionModelChangedAction action,
                const _Vector_inspectable_ptr_type& items,
                UINT32 index)
            {
                foundation_assert(action == foundation::NotifyCollectionModelChangedAction::ItemAdded || action == foundation::NotifyCollectionModelChangedAction::ItemRemoved);

                UINT32 newStartingIndex = -1;
                UINT32 oldStartingIndex = -1;
                if (action == foundation::NotifyCollectionModelChangedAction::ItemAdded)
                {
                    m_NewItems = items;
                    newStartingIndex = index;
                }
                else if (action == foundation::NotifyCollectionModelChangedAction::ItemRemoved)
                {
                    m_OldItems = items;
                    oldStartingIndex = index;
                }
                _NotifyCollectionChangedEventArgsBase::_Initialize(action, oldStartingIndex, newStartingIndex);

                return S_OK;
            }

            HRESULT _Initialize(
                foundation::NotifyCollectionModelChangedAction action,
                foundation::IInspectable *item,
                int nOldIndex,
                int nNewIndex)
            {
                _Vector_inspectable_ptr_type items;
                items.push_back(item);
                return _Initialize(action, items, nOldIndex, nNewIndex);
            }

            HRESULT _Initialize(
                foundation::NotifyCollectionModelChangedAction action,
                const _Vector_inspectable_ptr_type& items,
                int nOldIndex,
                int nNewIndex)
            {
                foundation_assert(action == foundation::NotifyCollectionModelChangedAction::ItemAdded || action == foundation::NotifyCollectionModelChangedAction::ItemRemoved);

                _NotifyCollectionChangedEventArgsBase::_Initialize(action, nOldIndex, nNewIndex);
                if (action == foundation::NotifyCollectionModelChangedAction::ItemAdded)
                {
                    m_NewItems = items;
                }
                else if (action == foundation::NotifyCollectionModelChangedAction::ItemRemoved)
                {
                    m_OldItems = items;
                }
                return S_OK;
            }

            HRESULT _Initialize(
                foundation::NotifyCollectionModelChangedAction action,
                UINT32 oldStartingIndex,
                UINT32 newStartingIndex,
                const _Vector_inspectable_ptr_type& oldItems,
                const _Vector_inspectable_ptr_type& newItems)
            {
                _NotifyCollectionChangedEventArgsBase::_Initialize(action, oldStartingIndex, newStartingIndex);
                m_OldItems = oldItems;
                m_NewItems = newItems;
                return S_OK;
            }

            HRESULT _Initialize(
                const _Vector_inspectable_ptr_type& oldItems,
                const _Vector_inspectable_ptr_type& newItems)
            {
                return _Initialize(foundation::NotifyCollectionModelChangedAction::Reset, -1, -1, oldItems, newItems);
            }

            void FinalRelease() override
            {
                m_OldItems.clear();
                m_NewItems.clear();
                _NotifyCollectionChangedEventArgsBase::FinalRelease();
            }
        protected:
            // Interface INotifyCollectionChangedEventArgs
            STDMETHOD(GetOldItems)(_Out_ UINT32 *size, _Outptr_ foundation::IInspectable*** oldItems) override
            {
                return ReturnItems(m_OldItems, size, oldItems);
            }

            STDMETHOD(GetNewItems)(_Out_ UINT32 *size, _Outptr_ foundation::IInspectable*** newItems) override
            {
                return ReturnItems(m_NewItems, size, newItems);
            }

            HRESULT ReturnItems(
                const _Vector_inspectable_ptr_type& items,
                _Out_ UINT32 *size,
                _Outptr_ foundation::IInspectable*** ppItems)
            {
                if (m_spValueConverterCallback != nullptr)
                {
                    return RefCountCopyTo(items, m_spValueConverterCallback, size, ppItems);
                }
                foundation::RefCountCopyTo(items, size, ppItems);
                return S_OK;
            }
        private:
                HRESULT RefCountCopyTo(
                    const _Vector_inspectable_ptr_type& vector,
                    IValueConverterCallback *pValueConverterCallback,
                    UINT32 *size,
                    foundation::IInspectable ***ppArray)
                {
                    foundation_assert(pValueConverterCallback);
                    _Vector_inspectable_ptr_type convertedValues(vector);
                    for (_Vector_inspectable_ptr_type::iterator iter = convertedValues.begin();
                    iter != convertedValues.end();
                        ++iter)
                    {
                        foundation::InspectablePtr spItemValue;
                        _IFR_(pValueConverterCallback->Invoke(*iter, spItemValue.GetAddressOf()));
                        *iter = spItemValue;
                    }
                    foundation::RefCountCopyTo(convertedValues, size, ppArray);
                    return S_OK;
                }

        private:
            foundation::ComPtr<IValueConverterCallback>   m_spValueConverterCallback;

            _Vector_inspectable_ptr_type m_OldItems;
            _Vector_inspectable_ptr_type m_NewItems;
        };

        class NotifyCollectionChangedEventArgsWithCallback :
            public _NotifyCollectionChangedEventArgsBase
        {
        public:
            static HRESULT CreateResetInstance(
                _In_ pmod::IObservableCollection *pSource,
                _Outptr_ pmod::INotifyCollectionChangedEventArgs **ppNewInstance)
            {
                foundation_assert(pSource != nullptr);

                NotifyCollectionChangedEventArgsWithCallback *pT;
                IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));

                pT->_spSource = pSource;
                pT->_Initialize(
                    foundation::NotifyCollectionModelChangedAction::Reset,
                    (UINT32)-1,
                    (UINT32)-1);

                *ppNewInstance = pT;
                return S_OK;
            }

            static HRESULT CreateItemInstance(
                foundation::NotifyCollectionModelChangedAction action,
                _In_ pmod::IObservableCollection *pSource,
                _In_ UINT32 index,
                _Outptr_ pmod::INotifyCollectionChangedEventArgs **ppNewInstance)
            {
                foundation_assert(pSource != nullptr);
                foundation_assert(action == foundation::NotifyCollectionModelChangedAction::ItemAdded ||
                    action == foundation::NotifyCollectionModelChangedAction::ItemReplaced);

                NotifyCollectionChangedEventArgsWithCallback *pT;
                IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));

                pT->_spSource = pSource;
                pT->_Initialize(
                    action,
                    (UINT32)-1,
                    index);

                *ppNewInstance = pT;
                return S_OK;
            }

        protected:
            // Interface INotifyCollectionChangedEventArgs
            STDMETHOD(GetOldItems)(_Outptr_ UINT32 *size, _Outptr_ foundation::IInspectable*** oldItems) override
            {
                *size = 0;
                *oldItems = nullptr;
                return S_OK;
            }

            STDMETHOD(GetNewItems)(_Outptr_ UINT32 *size, _Outptr_ foundation::IInspectable*** newItems) override
            {
                UINT32 index;
                this->GetNewStartingIndex(&index);
                if (index == (UINT32)-1)
                {
                    // Reset case
                    return _spSource->GetItems(size, newItems);
                }
                // ItemAdded/ItemReplaced case
                foundation::InspectablePtr item_ptr;
                IFR_ASSERT(_spSource->GetItem(index, item_ptr.GetAddressOf()));

                _Vector_inspectable_ptr_type items;
                items.push_back(item_ptr);
                foundation::RefCountCopyTo(items, size, newItems);

                return S_OK;
            }
        private:
            foundation::ComPtr<pmod::IObservableCollection> _spSource;
        };
    }
}
