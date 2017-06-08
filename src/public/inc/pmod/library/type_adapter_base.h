/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_adapter_base.h
****/

#pragma once

#include <foundation/pv_util.h>
#include <foundation/array_wrapper.h>
#include <foundation/library/adapter_base.h>
#include <foundation//library/foundation_adapter_base.h>
#include <foundation/com_ptr.h>

#include <pmod/interfaces/command.h>
#include <pmod/interfaces/observable_collection.h>
#include <pmod/interfaces/observable_object.h>

namespace pmod
{
    namespace library
    {
        //------------------------------------------------------------------------------
        // Class:   _AdapterBase
        //
        // Purpose: Adapter base template class for IObject interface
        //
        //------------------------------------------------------------------------------
        template
            <
                class TInnerModel,
                class TInterface,
                    const IID* piid,
                class TBASE = foundation::_ObjectAdapterBase<foundation::AdapterBase<TInnerModel, TInterface, piid> >
            >
        class _AdapterBase :
            public TBASE
        {
        protected:
            // override ComInspectableBase
            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (is_model_interface(iid))
                {
                    *ppInterface = static_cast<TInterface *>(this);
                }
                else
                {
                    return TBASE::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            // Return true if the iid is a Model Interface
            // Note: when an Interface is derived multiple times we want to add
            // all the chain of iid's
            virtual int is_model_interface(REFIID iid)
            {
                return (iid == *piid);
            }

        };
        //------------------------------------------------------------------------------
        // Class:   _ObservableObjectAdapterBaseImpl
        //
        // Purpose: Implements IObservableObject interface from an existing reference
        //
        //------------------------------------------------------------------------------
        template <class TBASE>
        class _ObservableObjectAdapterBaseImpl :
            public foundation::library::_ObjectDispatchAdapterBaseImpl<TBASE>
        {
        public:
            // IObservableObject Interface
            STDMETHOD(GetPropertyChangedEventSource)(_COM_Outptr_ IPropertyChangedEventSource** ppEventSource) override
            {
                return this->GetInner()->GetPropertyChangedEventSource(ppEventSource);
            }

            STDMETHOD(GetEventModelEventSource)(_COM_Outptr_ IEventModelEventSource** ppEventSource) override
            {
                return this->GetInner()->GetEventModelEventSource(ppEventSource);
            }

            STDMETHOD(InvokeMethod) (_In_ UINT32 methodId, _In_ UINT32 size, _In_opt_ foundation::IInspectable **parameters, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppResult) override
            {
                return this->GetInner()->InvokeMethod(methodId, size, parameters, ppResult);
            }
        };

        //------------------------------------------------------------------------------
        // Class:   _ObservableObjectAdapterBase
        //
        // Purpose: Define a template base class for an Property Model Adapter class
        //
        //------------------------------------------------------------------------------
        template <class TInterface, const IID* piid>
        class _ObservableObjectAdapterBase :
            public _ObservableObjectAdapterBaseImpl<_AdapterBase<IObservableObject, TInterface, piid>>
        {
        };

        //------------------------------------------------------------------------------
        // Class:   _CommandAdapterBase
        //
        // Purpose: Define a template base class for an Command Model Adapter class
        //
        //------------------------------------------------------------------------------
        template <
            class TParameter,
            class TResult,
            class TInterface,
                const IID* piid,
            class TParameterConverter,
            class TResultConverter>
        class _CommandAdapterBase :
            public _AdapterBase<ICommand, TInterface, piid>
        {
        protected:
            // Properties
            STDMETHOD(GetState)(UINT32* pVal)
            {
                return this->GetInner()->GetState(pVal);
            }

            // Methods.
            STDMETHOD(CanExecute)(TParameter parameter, bool *pCanExecute)
            {
                foundation::InspectablePtr spParameter;
                IFR_ASSERT(TParameterConverter::FromValue(parameter, spParameter.GetAddressOf()));
                return this->GetInner()->CanExecute(
                    spParameter,
                    pCanExecute);
            }

            STDMETHOD(Execute)(TParameter parameter, TResult *pResult)
            {
                foundation::InspectablePtr spParameter, spResult;
                IFR_ASSERT(TParameterConverter::FromValue(parameter, spParameter.GetAddressOf()));

                HRESULT hr = S_OK;
                _IFHR_(this->GetInner()->Execute(
                    spParameter,
                    spResult.GetAddressOf()));
                IFR_ASSERT(TResultConverter::ToValue(spResult, pResult));
                return hr;
            }

            // Event methods.
            STDMETHOD(GetCanExecuteChangedEventSource)(ICanExecuteChangedEventSource** ppEventSource)
            {
                return this->GetInner()->GetCanExecuteChangedEventSource(ppEventSource);
            }
        };
        //------------------------------------------------------------------------------
        // Class:   _CommandParameterAdapterBase
        //
        // Purpose: Define a template base class for an Command Model Adapter with only Parameter class
        //
        //------------------------------------------------------------------------------
        template <
            class TParameter,
            class TInterface,
                const IID* piid,
            class TParameterConverter>
        class _CommandParameterAdapterBase :
            public _AdapterBase<ICommand, TInterface, piid>
        {
        protected:
            // Properties
            STDMETHOD(GetState)(UINT32* pVal)
            {
                return this->GetInner()->GetState(pVal);
            }

            // Methods.
            STDMETHOD(CanExecute)(TParameter parameter, bool *pCanExecute)
            {
                foundation::InspectablePtr spParameter;
                IFR_ASSERT(TParameterConverter::FromValue(parameter, spParameter.GetAddressOf()));
                return this->GetInner()->CanExecute(
                    spParameter,
                    pCanExecute);
            }

            STDMETHOD(Execute)(TParameter parameter)
            {
                foundation::InspectablePtr spParameter, spResult;
                IFR_ASSERT(TParameterConverter::FromValue(parameter, spParameter.GetAddressOf()));

                HRESULT hr = S_OK;
                _IFHR_(this->GetInner()->Execute(
                    spParameter,
                    spResult.GetAddressOf()));
                return hr;
            }

            // Event methods.
            STDMETHOD(GetCanExecuteChangedEventSource)(ICanExecuteChangedEventSource** ppEventSource)
            {
                return this->GetInner()->GetCanExecuteChangedEventSource(ppEventSource);
            }
        };
        //------------------------------------------------------------------------------
        // Class:   _CommandResultAdapterBase
        //
        // Purpose: Define a template base class for an Command Model Adapter with only Result
        //
        //------------------------------------------------------------------------------
        template <
            class TResult,
            class TInterface,
                const IID* piid,
            class TResultConverter>
        class _CommandResultAdapterBase :
            public _AdapterBase<ICommand, TInterface, piid>
        {
        protected:
            // Properties
            STDMETHOD(GetState)(UINT32* pVal)
            {
                return this->GetInner()->GetState(pVal);
            }

            // Methods.
            STDMETHOD(CanExecute)(bool *pCanExecute)
            {
                foundation::InspectablePtr spParameter;
                return this->GetInner()->CanExecute(
                    spParameter,
                    pCanExecute);
            }

            STDMETHOD(Execute)(TResult *pResult)
            {
                foundation::InspectablePtr spParameter, spResult;

                HRESULT hr = S_OK;
                _IFHR_(this->GetInner()->Execute(
                    spParameter,
                    spResult.GetAddressOf()));
                IFR_ASSERT(TResultConverter::ToValue(spResult, pResult));
                return hr;
            }

            // Event methods.
            STDMETHOD(GetCanExecuteChangedEventSource)(ICanExecuteChangedEventSource** ppEventSource)
            {
                return this->GetInner()->GetCanExecuteChangedEventSource(ppEventSource);
            }
        };

        //------------------------------------------------------------------------------
        // Class:   _ObservableCollectionAdapterBase
        //
        // Purpose: Define a template base class for an Collection Model Adapter class
        //
        //------------------------------------------------------------------------------
        template <class T, class TInterface, const IID* piid, class TConverter>
        class _ObservableCollectionAdapterBase :
            public _AdapterBase<IObservableCollection, TInterface, piid>
        {
        protected:
            STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<T> **first)
            {
                IFCPTR_ASSERT(first);
                *first = nullptr;

                foundation::ComPtr<foundation::IIterator_impl<foundation::IInspectable *>> spIterator;
                IFR_ASSERT(this->m_pInner->First(spIterator.GetAddressOf()));

                return foundation::library::_IteratorAdapterBase<T, TConverter>::CreateInstance(spIterator, first);
            }

            STDMETHOD(GetCount)(_Out_ UINT32* pCount)
            {
                return this->m_pInner->GetCount(pCount);
            }

            STDMETHOD(GetItem)(UINT32 index, T *pItemValue)
            {
                foundation::InspectablePtr spItem;
                _IFR_(this->m_pInner->GetItem(index, spItem.GetAddressOf()));
                IFR_ASSERT(TConverter::ToValue(spItem, pItemValue));
                return S_OK;
            }

            STDMETHOD(GetItems)(_Out_ UINT32* size, _COM_Outptr_ T** items)
            {
                IFCPTR_ASSERT(size);
                IFCPTR_ASSERT(items);

                foundation::InspectableArrayWrapper itemsWrapper;
                IFR_ASSERT(this->m_pInner->GetItems(
                    itemsWrapper.GetSizeAddressOf(),
                    itemsWrapper.GetBufferAddressOf()));

                ULONG allocSize = (ULONG)(itemsWrapper.GetSize() * sizeof(T));
                *items = (T*)_pal_MemAlloc(allocSize);
                if (*items == nullptr)
                {
                    return E_OUTOFMEMORY;
                }

                T *pItems = *items;
                for (UINT32 index = 0; index < itemsWrapper.GetSize(); ++index, ++pItems)
                {
                    IFR_ASSERT(TConverter::ToValue(itemsWrapper[index], pItems));
                }
                // copy array size result
                *size = itemsWrapper.GetSize();
                return S_OK;
            }

            STDMETHOD(IndexOf)(_In_ T item, _Out_ UINT32* pnIndex)
            {
                foundation::InspectablePtr spItem;
                IFR_ASSERT(TConverter::FromValue(item, spItem.GetAddressOf()));
                return this->m_pInner->IndexOf(
                    spItem,
                    pnIndex);
            }

            // Event methods.
            STDMETHOD(GetNotifyCollectionChangedEventSource)(INotifyCollectionChangedEventSource** ppEventSource)
            {
                return this->m_pInner->GetNotifyCollectionChangedEventSource(ppEventSource);
            }
        };
    }

}

