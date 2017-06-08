/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection_impl.h
****/
#pragma once
#include <foundation/library/base_object_impl.h>
#include "notify_collection_changed_event_args.h"

#include <foundation/library/event_handler_source.h>
#include <foundation/library/boxing_value_adapters.h>
#include <pmod/interfaces/observable_collection.h>

#include <foundation/library/adapter_base.h>
#include "observable_object_impl.h"
#include "observable_object_type_info.h"

#include <vector>
#include <set>

namespace pmod {

    namespace library {

        template <
            class TInterface, 
            class TComBase = foundation::ctl::ComInspectableBase,
            class TFireEventHandlerContainer = foundation::library::_DefaultFireEventHandlerContainer
            >
        class _Observable_collection_impl_base :
            public foundation::library::_ObjectImplBase<TInterface, pmod::IObservableCollection, TComBase, TFireEventHandlerContainer>
        {
        public:
            // Interface IObservableCollection

            // Event methods.
            STDMETHOD(GetNotifyCollectionChangedEventSource)(_COM_Outptr_ INotifyCollectionChangedEventSource** ppEventSource)
            {
                if (_notifyCollectionChangedEventSourcePtr == nullptr)
                {
                    IFR_ASSERT(foundation::ctl::CreateInstance(_notifyCollectionChangedEventSourcePtr.GetAddressOf()));
                }
                *ppEventSource = _notifyCollectionChangedEventSourcePtr.Get();
                (*ppEventSource)->AddRef();
                return S_OK;
            }

            HRESULT _FireCollectionChanged(
                foundation::NotifyCollectionModelChangedAction action,
                UINT32 oldStartingIndex,
                UINT32 newStartingIndex,
                UINT32 sizeOldItems, _In_opt_ foundation::IInspectable **ppOldItems,
                UINT32 sizeNewItems, _In_opt_ foundation::IInspectable **ppNewItems
                )
            {
                foundation::ComPtr<INotifyCollectionChangedEventArgs> event_args_ptr;

                _NotifyCollectionChangedEventArgsBase::_Vector_inspectable_ptr_type oldItems, newItems;
                if (sizeOldItems)
                {
                    oldItems.assign(ppOldItems, ppOldItems + sizeOldItems);
                }
                if (sizeNewItems)
                {
                    newItems.assign(ppNewItems, ppNewItems + sizeNewItems);
                }

                IFR_ASSERT(NotifyCollectionChangedEventArgsImpl::CreateInstance(
                    action,
                    oldStartingIndex,
                    newStartingIndex,
                    oldItems,
                    newItems,
                    event_args_ptr.GetAddressOf()));

                return this->FireWithCallback(
                    this,
                    &_Observable_collection_impl_base::FireNotifyCollectionChanged,
                    event_args_ptr.Get());
            }

            IValueConverter *_GetItemValueConverter()
            {
                return nullptr;
            }

        protected:
            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == IObservableCollection::GetIID())
                {
                    *ppInterface = static_cast<IObservableCollection *>(this);
                }
                else if (iid == foundation::IEnumerable::GetIID())
                {
                    *ppInterface = static_cast<foundation::IEnumerable *>(this);
                }
                else if (iid == IObservableObject::GetIID())
                {
                    return this->template GetOrCreateAdapter<_ObservableObjectAdapter>(
                        this->CastToInspectable(),
                        reinterpret_cast<IObservableObject **>(ppInterface));
                }
                else
                {
                    return _base_type::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            HRESULT FireNotifyCollectionChanged(
                foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
                INotifyCollectionChangedEventArgs *pEventArgs)
            {
                if (_notifyCollectionChangedEventSourcePtr != nullptr)
                {
                    _notifyCollectionChangedEventSourcePtr->FireEvent(
                        pFireEventHandlerContainer,
                        this->CastToInspectable(),
                        pEventArgs);
                }
                return S_OK;
            }

            bool IsObservedInternal() override
            {
                return _notifyCollectionChangedEventSourcePtr != nullptr && _notifyCollectionChangedEventSourcePtr->GetEventHandlers().size() > 0;
            }

            typedef foundation::library::_ObjectImplBase<TInterface, pmod::IObservableCollection, TComBase, TFireEventHandlerContainer> _base_type;

            typedef foundation::library::_EventHandlerSource
            <
                INotifyCollectionChangedEventSource,
                foundation::library::_EventHandlerContainerWithCriticalSection<INotifyCollectionChangedEventHandler>
            > _NotifyCollectionChangedEventSource_Type;
        private:
            struct _IVoidAdapter : public foundation::IUnknown {};

            typedef foundation::AdapterBase<IObservableCollection, _IVoidAdapter, &foundation_GUID_NULL> _AdapterBase_type;
            class _ObservableObjectAdapter :
                public _ObservableObjectImplBase<IObservableObject, _AdapterBase_type>
            {
            public:
                HRESULT _Initialize(
                    _In_ foundation::IInspectable *pOuter,
                    _In_ foundation::IInspectable *pInner)
            {
                return _AdapterBase_type::_Initialize(pOuter,pInner);
            }

            protected:
                STDMETHOD(QueryAdapterInterface)(REFIID iid, foundation::IUnknown **ppInterface)
                {
                    if (iid == IObservableObject::GetIID() || iid == foundation::IObjectDispatch::GetIID())
                    {
                        *ppInterface = static_cast<IObservableObject *>(this);
                    }
                    else
                    {
                        return _AdapterBase_type::QueryAdapterInterface(iid, ppInterface);
                    }
                    return S_OK;
                }

                STDMETHOD(HasEventHandlers)(_Outptr_ bool *pValue)
                {
                    *pValue = this->IsObserved();
                    return S_OK;
                }
                STDMETHOD(FireAdapterEvent)(
                    _In_ foundation::library::IFireEventHandlerContainer *pIFireEventHandlerContainer,
                    _In_ foundation::IUnknown* pEventArgs) override
                {
                    foundation::ComPtr<INotifyCollectionChangedEventArgs> eventArgsPtr;
                    if (SUCCEEDED(foundation::QueryInterface(pEventArgs, eventArgsPtr.GetAddressOf())))
                    {
                        this->FirePropertyChangedWithCallback(ObservableCollection::Property_Count);
                        this->FirePropertyChangedWithCallback(ObservableCollection::Property_IsEmpty);
                    }
                    return S_OK;
                }

                STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue) override
                {
                    IFR_ASSERT(foundation::GetTypeInfoAs(ObservableCollection::IIDType, ppValue));
                    return S_OK;
                }

                STDMETHOD(GetProperty)(UINT32 propertyId, foundation::IInspectable** value)
                {
                    if (propertyId == ObservableCollection::Property_Count ||
                        propertyId == ObservableCollection::Property_IsEmpty)
                    {
                        UINT32 count;
                        this->GetInner()->GetCount(&count);
                        if (propertyId == ObservableCollection::Property_Count)
                        {
                            return foundation::pv_util::CreateUInt32Value(count, value);
                        }
                        else
                        {
                            return foundation::pv_util::CreateBooleanValue(count==0, value);
                        }
                    }
                    return E_NOTIMPL;
                }

            };

        private:
            foundation::ComPtr<_NotifyCollectionChangedEventSource_Type> _notifyCollectionChangedEventSourcePtr;

        };

        template <class T>
        class _ObservableCollectionBase
        {
        public:
            typedef std::vector<foundation::InspectablePtr> _InspectablePtr_VectorType;

            template <class Iterator>
            void _FireItemInserted(
                Iterator iter,
                UINT32 index)
            {
                if (!GetObservableCollectionImpl()->IsObserved())
                    return;

                _InspectablePtr_VectorType newItems;
                Iterator iterEnd = iter;
                _ToInspectableItems(iter, ++iterEnd, newItems);

                // notify the collection changed
                GetObservableCollectionImpl()->_FireCollectionChanged(
                    foundation::NotifyCollectionModelChangedAction::ItemAdded,
                    foundation::NINDEX,
                    index,
                    0,
                    nullptr,
                    1,
                    newItems[0].GetAddressOfPtr());
            }

            template <class Iterator>
            void _FireItemRemoved(
                Iterator iter,
                UINT32 index)
            {
                if (!GetObservableCollectionImpl()->IsObserved())
                    return;
                _InspectablePtr_VectorType oldItems;
                Iterator iterEnd = iter;
                _ToInspectableItems(iter, ++iterEnd, oldItems);

                // notify the collection changed
                GetObservableCollectionImpl()->_FireCollectionChanged(
                    foundation::NotifyCollectionModelChangedAction::ItemRemoved,
                    index,
                    foundation::NINDEX,
                    1,
                    oldItems[0].GetAddressOfPtr(),
                    0,
                    nullptr);
            }

            void _FireItemsRemoved(
                const _InspectablePtr_VectorType& oldItems,
                UINT32 index)
            {
                if (!GetObservableCollectionImpl()->IsObserved())
                    return;

                // notify the collection changed
                GetObservableCollectionImpl()->_FireCollectionChanged(
                    foundation::NotifyCollectionModelChangedAction::ItemRemoved,
                    index,
                    foundation::NINDEX,
                    (UINT32)oldItems.size(),
                    foundation::GetVectorBufferPtr(oldItems),
                    0,
                    nullptr);
            }

            template <class TValue>
            void _FireItemReplaced(
                const TValue& old_value,
                const TValue& new_value,
                UINT32 index)
            {
                if (!GetObservableCollectionImpl()->IsObserved())
                    return;

                IValueConverter *pValueConverter = GetObservableCollectionImpl()->_GetItemValueConverter();

                _InspectablePtr_VectorType oldItems, newItems;
                AppendItems(old_value, oldItems, pValueConverter);
                AppendItems(new_value, newItems, pValueConverter);

                // notify the collection changed
                GetObservableCollectionImpl()->_FireCollectionChanged(
                    foundation::NotifyCollectionModelChangedAction::ItemReplaced,
                    index,
                    index,
                    1,
                    oldItems[0].GetAddressOfPtr(),
                    1,
                    newItems[0].GetAddressOfPtr());
            }

            template <class Iterator>
            void _FireReset(
                Iterator oldIterFirst, Iterator oldIterEnd,
                Iterator newIterFirst, Iterator newIterEnd)
            {
                if (!GetObservableCollectionImpl()->IsObserved())
                    return;

                _InspectablePtr_VectorType oldItems;
                _ToInspectableItems(oldIterFirst, oldIterEnd, oldItems);
                _InspectablePtr_VectorType newItems;
                _ToInspectableItems(newIterFirst, newIterEnd, newItems);
                // notify the collection changed
                GetObservableCollectionImpl()->_FireCollectionChanged(
                    foundation::NotifyCollectionModelChangedAction::Reset,
                    foundation::NINDEX,
                    foundation::NINDEX,
                    (UINT32)oldItems.size(), foundation::GetVectorBufferPtr(oldItems),
                    (UINT32)newItems.size(), foundation::GetVectorBufferPtr(newItems));
            }


            template <class Iterator>
            void _ToInspectableItems(
                Iterator iterFirst,
                Iterator iterLast,
                _InspectablePtr_VectorType& items)
            {
                IValueConverter *pValueConverter = GetObservableCollectionImpl()->_GetItemValueConverter();

                while (iterFirst != iterLast) {

                    AppendItems(
                        *iterFirst,
                        items,
                        pValueConverter);
                    ++iterFirst;
                }
            }
private:
            template <class TValue>
            static void AppendItems(
                const TValue& value,
                _InspectablePtr_VectorType& items,
                IValueConverter *pValueConverter)
            {
                foundation::InspectablePtr item = foundation::CreateValue(value);

                if (pValueConverter != nullptr)
                {
                    foundation::InspectablePtr rawItem(item);
                    pValueConverter->Convert(rawItem, nullptr, nullptr, item.ReleaseAndGetAddressOf());
                }
                items.push_back(item);
            }
        private:
            inline T *GetObservableCollectionImpl()
            {
                return static_cast<T *>(this);
            }
        };

        template<
            class T,
            class _Collection_model_impl>
        class _ObservableVectorType : public std::vector<T>
        {
        public:
            typedef _ObservableVectorType<T, _Collection_model_impl> _Myt;
            typedef std::vector<T> _Mybase;
            typedef typename _Mybase::value_type value_type;
            typedef typename _Mybase::iterator iterator;
            typedef typename _Mybase::const_iterator const_iterator;

            _Myt& operator=(const _Mybase& _Right)
            {
                _Mybase oldItems = *this;

                // assign by copying _Right
                _Mybase::operator=(_Right);

                GetObservableCollectionImpl()->_FireReset(
                    oldItems.begin(), 
                    oldItems.end(), 
                    _Mybase::begin(), 
                    _Mybase::end());
                return (*this);
            }

            void push_back(const value_type& _Val)
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();

                _Mybase::push_back(_Val);
                GetObservableCollectionImpl()->_FireItemInserted(
                    _Mybase::rbegin(), 
                    (UINT32)(this->size() - 1));
            }

            void replace(size_t _Pos, const value_type& _Val)
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();

                value_type old_value = _Mybase::at(_Pos);
                _Mybase::at(_Pos) = _Val;
                GetObservableCollectionImpl()->_FireItemReplaced(
                    old_value, 
                    _Val, 
                    (UINT32)_Pos);
            }

            iterator insert(
#if !defined(NO_OBSERVABLE_VECTOR_CONST_ITER)
                const_iterator
#else
                iterator
#endif
                _Where,
                const T& _Val)
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();
                // insert _Val at _Where
                iterator iter = _Mybase::insert(_Where, _Val);

                GetObservableCollectionImpl()->_FireItemInserted(
                    iter, 
                    (UINT32)(iter - _Mybase::begin()));
                return iter;
            }

            void erase(
#if !defined(NO_OBSERVABLE_VECTOR_CONST_ITER)
                const_iterator _From, const_iterator _To
#else
                iterator _From, const_iterator _To
#endif
                )
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();

                typename _Collection_model_impl::_InspectablePtr_VectorType oldItems;
                GetObservableCollectionImpl()->_ToInspectableItems(
                    _From, 
                    _To, 
                    oldItems);

                _Mybase::erase(_From, _To);
                GetObservableCollectionImpl()->_FireItemsRemoved(
                    oldItems, 
                    (UINT32)(_From - _Mybase::begin()));
            }

            iterator erase(
#if !defined(NO_OBSERVABLE_VECTOR_CONST_ITER)
                const_iterator
#else
                iterator
#endif
                _Where)
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();

                iterator iter = _Mybase::erase(_Where);
                GetObservableCollectionImpl()->_FireItemRemoved(
                    iter, 
                    (UINT32)(iter - _Mybase::begin()));
                return iter;
            }

            void swap(std::vector<T>& _Right)
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();

                // exchange contents with non-movable _Right
                _Mybase::swap(_Right);

                GetObservableCollectionImpl()->_FireReset(
                    _Right.begin(), 
                    _Right.end(), 
                    _Mybase::begin(), 
                    _Mybase::end());
            }

            void clear()
            {
                std::vector<T> empty;
                swap(empty);
            }

        private:
            inline _Collection_model_impl *GetObservableCollectionImpl()
            {
                return static_cast<_Collection_model_impl *>(this);
            }
        };

        template<
            class T,
            const IID& _iidType = foundation_GUID_NULL
        >
        class _ObservableVectorImpl :
            public _Observable_collection_impl_base<IObservableCollection>,
            public _ObservableVectorType<T,_ObservableVectorImpl<T, _iidType>>,
            public _ObservableCollectionBase<_ObservableVectorImpl<T, _iidType>>
        {
        private:
            typedef _ObservableVectorImpl<T,_iidType> _This_type;
            typedef _ObservableVectorType<T, _ObservableVectorImpl<T, _iidType>> _Vector_base_type;
        public:
            _This_type& operator=(const std::vector<T>& _Right)
            {
                this->_Vector_base_type::operator=(_Right);
                return (*this);
            }

            // IObject interface
            STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue) override
            {
                IFR_ASSERT(foundation::GetTypeInfoAs(_iidType, ppValue));
                return S_OK;
            }
            // IEnumerable interface
            STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **first)
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }

            // IObservableCollection interface
            STDMETHOD(GetCount)(_Out_ UINT32* pCount)
            {
                *pCount = (UINT32)this->size();
                return S_OK;
            }

            STDMETHOD(GetItem)(UINT32 index, foundation::IInspectable **pItemValue)
            {
                *pItemValue = foundation::CreateValue(this->operator[](index)).Detach();
                return S_OK;

            }
            // interface IObservableCollection_impl
            STDMETHOD(GetItems)(_Out_ UINT32* size, foundation::IInspectable*** items)
            {
                std::vector<foundation::InspectablePtr> itemsContainer;
                itemsContainer.resize(this->size());
                for(auto iter = this->begin();
                    iter != this->end();
                    ++iter)
                {
                    itemsContainer[iter - this->begin()] = foundation::CreateValue(*iter);
                }
                
                foundation::RefCountCopyTo(itemsContainer,size,items);
                return S_OK;
            }
            STDMETHOD(IndexOf)(_In_ foundation::IInspectable* item, _Out_ UINT32* pnIndex)
            {
                return S_OK;
            }

        };

        template<
            class T,
            class _Collection_model_impl>
        class _ObservableSetType : public std::set<T>
        {
        public:
            typedef _ObservableSetType<T, _Collection_model_impl> _Myt;
            typedef std::set<T> _Mybase;
            typedef typename _Mybase::iterator iterator;
            typedef typename _Mybase::value_type value_type;
            typedef typename _Mybase::size_type size_type;

            _Myt& operator=(const _Mybase& _Right)
            {
                _Mybase oldItems = *this;

                // assign by copying _Right
                _Mybase::operator=(_Right);

                GetObservableCollectionImpl()->_FireReset(oldItems.begin(), oldItems.end(), _Mybase::begin(), _Mybase::end());
                GetObservableCollectionImpl()->_Reset_items_iterator();
                return (*this);
            }

            std::pair<iterator, bool> insert(const value_type& _Val)
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();
                // insert a key value
                std::pair<iterator, bool> result = _Mybase::insert(_Val);

                if (result.second) // item was inserted
                {
                    GetObservableCollectionImpl()->_FireItemInserted(result.first, foundation::NINDEX);
                }

                GetObservableCollectionImpl()->_Reset_items_iterator();
                return result;
            }
            void swap(std::set<T>& _Right)
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();

                // exchange contents with non-movable _Right
                _Mybase::swap(_Right);

                GetObservableCollectionImpl()->_FireReset(_Right.begin(), _Right.end(), _Mybase::begin(), _Mybase::end());
                GetObservableCollectionImpl()->_Reset_items_iterator();
            }

            size_type erase(const value_type& _Keyval)
            {
                foundation::library::FoundationCriticalSection cs = GetObservableCollectionImpl()->_GetCriticalSection();

                size_type result = _Mybase::erase(_Keyval);

                if (result)
                {
                    std::set<T> removed;
                    GetObservableCollectionImpl()->_FireItemRemoved(removed.insert(_Keyval).first, foundation::NINDEX);
                }

                GetObservableCollectionImpl()->_Reset_items_iterator();
                return result;
            }

            void clear()
            {
                std::set<T> empty;
                swap(empty);
                GetObservableCollectionImpl()->_Reset_items_iterator();
            }
        private:
            inline _Collection_model_impl *GetObservableCollectionImpl()
            {
                return static_cast<_Collection_model_impl *>(this);
            }
        };

        typedef _ObservableVectorImpl<foundation::InspectablePtr, IID_IObservableCollection> ObservableVectorImplInspectable;
        typedef _ObservableVectorImpl<UINT32, IID_IObservableCollection_UInt32> ObservableVectorImplUInt32;
        typedef _ObservableVectorImpl<INT32, IID_IObservableCollection_Int32> ObservableVectorImplInt32;


    }
}
