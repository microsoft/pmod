/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:foundation_adapter_base.h
****/
#pragma once

#include "adapter_base.h"
#include <foundation/boxing_util.h>
#include <foundation/array_wrapper.h>
#include <foundation/interfaces/enumerable.h>
#include <foundation/interfaces/object_dispatch.h>
#include <foundation/com_ptr.h>

namespace foundation
{

class _CastConverter
{
public:
    template <class Q, class T>
    static HRESULT ToValue(const Q&value, _COM_Outptr_result_maybenull_ T *pValue)
    {
        *pValue = value;
        return S_OK;
    }
    template <class Q, class T>
    static HRESULT FromValue(const T& value, Q *pValue)
    {
        *pValue = value;
    }
};

class _InspectableConverter
{
public:
    template <class T>
    static HRESULT ToValue(_In_opt_ foundation::IInspectable *pValue, _COM_Outptr_result_maybenull_ T **ppValue)
    {
        if (pValue)
        {
            return foundation::QueryInterface(pValue, ppValue);
        }

        *ppValue = nullptr;
        return S_OK;
    }

    template <class T>
    static HRESULT FromValue(_In_opt_ T *pValue, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppValue)
    {
        if(pValue)
        {
            (*ppValue) = pValue;
            (*ppValue)->AddRef();
        }
        else
        {
            *ppValue = nullptr;
        }

        return S_OK;
    }
};

class _PropertyValueConverter
{
public:
    template <class T>
    static HRESULT ToValue(_In_ foundation::IInspectable *pPropertyValue, _Out_ T *pValue)
    {
        foundation_assert(pPropertyValue);
        return foundation::pv_util::GetValue(pPropertyValue, pValue);
    }

    template <class T>
    static HRESULT FromValue(T value, _COM_Outptr_ foundation::IInspectable **ppPropertyValue)
    {
        InspectablePtr spPropertyValue = foundation::CreateValue(value);
        *ppPropertyValue = spPropertyValue.Detach();
        return S_OK;
    }
};

class _PropertyValueArrayConverter
{
public:
    template <class T>
    static HRESULT ToValue(_In_ foundation::IInspectable *pPropertyValue, _Out_ UINT32 *pSize, T **ppValues)
    {
        foundation_assert(pPropertyValue);
        return foundation::pv_util::GetValue(pPropertyValue, pSize, ppValues);
    }

    template <class T>
    static HRESULT FromValue(UINT32 size, _In_opt_ T *values, _COM_Outptr_ foundation::IInspectable **ppPropertyValue)
    {
        InspectablePtr spPropertyValue = foundation::CreateValue(size, values);
        *ppPropertyValue = spPropertyValue.Detach();
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   _ObjectAdapterBase
//
// Purpose: Implements IObject interface from an existing reference
//
//------------------------------------------------------------------------------
template <class TBASE>
class _ObjectAdapterBase :
    public TBASE
{
public:
    // IObject Interface
    STDMETHOD(GetUniqueId)(UINT32 *uniqueId) override
    {
        return this->GetInner()->GetUniqueId(uniqueId);
    }

    STDMETHOD(GetProcessId)(UINT32 *processId) override
    {
        return this->GetInner()->GetProcessId(processId);
    }

    STDMETHOD(GetTypeInfo)(IObjectTypeInfo** ppValue) override
    {
        return this->GetInner()->GetTypeInfo(ppValue);
    }
};

//------------------------------------------------------------------------------
// Class:   _AsyncOperationBaseAdapter
//
// Purpose: Define a template base class for an Async Operation Adapter class
//
//------------------------------------------------------------------------------
template <class TInterface, const IID* piid>
class _AsyncOperationBaseAdapter :
    public _ObjectAdapterBase< AdapterBase<IAsyncOperation, TInterface, piid>>
{
protected:
    STDMETHOD(GetError)(HRESULT* hResult)
    {
        return this->m_pInner->GetError(hResult);
    }

    STDMETHOD(GetStatus)(ResultStatus* status)
    {
        return this->m_pInner->GetStatus(status);
    }

    STDMETHOD(GetIsCancelable)(bool* pVal)
    {
        return this->m_pInner->GetIsCancelable(pVal);
    }

    STDMETHOD(Cancel)()
    {
        return this->m_pInner->Cancel();
    }

    STDMETHOD(Wait)(UINT32 timeout,ResultStatus* status)
    {
        return this->m_pInner->Wait(timeout,status);
    }

    // Event methods.
    STDMETHOD(GetCompletedEventSource)(ICompletedEventSource** ppEventSource)
    {
        return this->m_pInner->GetCompletedEventSource(ppEventSource);
    }

    STDMETHOD(GetProgressEventSource)(IProgressEventSource** ppEventSource)
    {
        return this->m_pInner->GetProgressEventSource(ppEventSource);
    }
};

//------------------------------------------------------------------------------
// Class:   _AsyncOperationAdapterBase
//
// Purpose: Define a template base class for an Async Operation Adapter class
//
//------------------------------------------------------------------------------
template <class T,class TInterface,const IID* piid,class TConverter= _PropertyValueConverter >
class _AsyncOperationAdapterBase :
    public _AsyncOperationBaseAdapter<TInterface, piid>
{
protected:
    STDMETHOD(GetResult)(UINT32 timeout, T *pResult)
    {
        InspectablePtr spResult;
        IFR_ASSERT(this->m_pInner->GetResult(timeout, spResult.GetAddressOf()));
        IFR_ASSERT(TConverter::ToValue(spResult, pResult));
        return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   _AsyncOperationAdapterArrayBase
//
// Purpose: Define a template base class for an Async Operation Adapter class
//
//------------------------------------------------------------------------------
template <class T, class TInterface, const IID* piid, class TConverter = _PropertyValueArrayConverter >
class _AsyncOperationAdapterArrayBase :
    public _AsyncOperationBaseAdapter<TInterface, piid>
{
protected:
    STDMETHOD(GetResult)(UINT32 timeout,UINT32 *pSize, T **ppResultValues)
    {
        InspectablePtr spResult;
        IFR_ASSERT(this->m_pInner->GetResult(timeout,spResult.GetAddressOf()));
        IFR_ASSERT(TConverter::ToValue(spResult, pSize, ppResultValues));
        return S_OK;
    }
};

namespace library {
    //------------------------------------------------------------------------------
    // Class:   _ObjectDispatchAdapterBaseImpl
    //
    // Purpose: Implements IObjectDispatch interface from an existing reference
    //
    //------------------------------------------------------------------------------
    template <class TBASE>
    class _ObjectDispatchAdapterBaseImpl :
        public TBASE
    {
    public:
        // IObjectDispatch Interface
        STDMETHOD(GetProperty)(_In_ UINT32 propertyId, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppValue) override
        {
            return this->GetInner()->GetProperty(propertyId, ppValue);
        }

        STDMETHOD(SetProperty)(_In_ UINT32 propertyId, _In_opt_ foundation::IInspectable *pValue) override
        {
            return this->GetInner()->SetProperty(propertyId, pValue);
        }

        STDMETHOD(GetProperties)(foundation::IInspectable *pProperties, _Out_ UINT32* size, _Out_ foundation::IPropertyIdPair ***ppProperties) override
        {
            return this->GetInner()->GetProperties(pProperties, size, ppProperties);
        }

        STDMETHOD(InvokeMethod)(UINT32 methodId, _In_ UINT32 size, _In_opt_ foundation::IInspectable **parameters, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppResult)
        {
            return this->GetInner()->InvokeMethod(methodId, size, parameters, ppResult);
        }

    protected:
        template <class T>
        HRESULT _GetPropertyValue(
            UINT32 propertyId,
            _Out_ T* pValue)
        {
            foundation::InspectablePtr valuePtr;
            _IFR_(this->GetProperty(propertyId, valuePtr.GetAddressOf()));
            return foundation::pv_util::GetValue(valuePtr, pValue);
        }

        template <class T>
        HRESULT _GetPropertyValue(
            UINT32 propertyId,
            _Out_ UINT32 *size,
            _Out_ T** pValue)
        {
            foundation::InspectablePtr valuePtr;
            _IFR_(this->GetProperty(propertyId, valuePtr.GetAddressOf()));
            return foundation::pv_util::GetValue(valuePtr, size, pValue);
        }

        template <class T>
        HRESULT _SetPropertyValue(
            UINT32 propertyId,
            T value)
        {
            return this->SetProperty(propertyId, foundation::CreateValue(value));
        }

        template <class T>
        HRESULT _SetPropertyValue(
            UINT32 propertyId,
            UINT32 size,
            T *value)
        {
            return this->SetProperty(propertyId, foundation::CreateValue(size, value));
        }
    };
    //------------------------------------------------------------------------------
    // Class:   _ObjectDispatchAdapterBase
    //
    // Purpose: Define a template base class for an IObjectDispatch Adapter class
    //
    //------------------------------------------------------------------------------
    template <class TInterface, const IID* piid>
    class _ObjectDispatchAdapterBase :
        public _ObjectDispatchAdapterBaseImpl<_ObjectAdapterBase< AdapterBase<IObjectDispatch, TInterface, piid>>>
    {
    };

    //------------------------------------------------------------------------------
    // Class:   _IteratorAdapterBase
    // foundation::IIterator_impl<IInspectable*> implementation using std::iterator
    //------------------------------------------------------------------------------
    template <class T, class TConverter>
    class _IteratorAdapterBase :
        public ctl::ComInspectableBase,
        public IIterator_impl<T>
    {
    public:
        static HRESULT CreateInstance(
            _In_ foundation::IIterator_impl<foundation::IInspectable*> *pItemsContainerIterator,
            _COM_Outptr_ foundation::IIterator_impl<T> **ppIterator)
        {
            foundation_assert(pItemsContainerIterator);
            foundation_assert(ppIterator);
            *ppIterator = nullptr;

            _IteratorAdapterBase<T, TConverter> *pIterator = nullptr;
            const HRESULT hr = foundation::ctl::ComInspectableObject<_IteratorAdapterBase<T, TConverter>>::CreateInstance(&pIterator);
            IFR_ASSERT(hr);
            pIterator->_pItemsContainerIterator = pItemsContainerIterator;
            pItemsContainerIterator->AddRef();
            *ppIterator = pIterator;
            return S_OK;
        }
    protected:
        _IteratorAdapterBase() :
            _pItemsContainerIterator(nullptr)
        {}

        void FinalRelease()
        {
            RELEASE_INTERFACE(_pItemsContainerIterator);
        }

        STDMETHOD(GetCurrent)(T *current)
        {
            foundation::InspectablePtr spItem;
            IFR_ASSERT(_pItemsContainerIterator->GetCurrent(spItem.GetAddressOf()));
            IFR_ASSERT(TConverter::ToValue(spItem, current));
            return S_OK;
        }

        STDMETHOD(GetHasCurrent)(_Out_ bool *hasCurrent)
        {
            return _pItemsContainerIterator->GetHasCurrent(hasCurrent);
        }

        STDMETHOD(MoveNext)(_Out_ bool *hasCurrent)
        {
            return _pItemsContainerIterator->MoveNext(hasCurrent);
        }
    private:
        IIterator_impl<IInspectable*> *_pItemsContainerIterator;
    };
    //------------------------------------------------------------------------------
    // Class:   _EnumerableAdapterBase
    //
    // Purpose: Define a template base class for an Collection Model Adapter class
    //
    //------------------------------------------------------------------------------
    template <
        class T,
        class TInterface,
            const IID* piid,
        class TConverter,
        class TInner = IEnumerable,
        class TBASE = _ObjectAdapterBase<AdapterBase<TInner, TInterface, piid>
        >
    >
    class _EnumerableAdapterBase :
        public TBASE
    {
    protected:
        STDMETHOD(First)(_COM_Outptr_ IIterator_impl<T> **first)
        {
            IFCPTR_ASSERT(first);
            *first = nullptr;

            ComPtr<IIterator_impl<foundation::IInspectable *>> spIterator;
            IFR_ASSERT(this->m_pInner->First(spIterator.GetAddressOf()));

            return _IteratorAdapterBase<T, TConverter>::CreateInstance(spIterator, first);
        }
    };
}

}
