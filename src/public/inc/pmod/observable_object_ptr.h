/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_object_ptr.h
****/
#pragma once

#include "object_node_ptr.h"
#include <foundation/pv_util.h>
#include <foundation/exception.h>
#include <pmod/interfaces.h>

namespace pmod
{

template <class T>
class _Observable_object_ptr_wrapper : public _ObjectNodePtrWrapper<T, IObservableObject>
{
public:
    template <class TValue>
    HRESULT GetProperty(UINT32 propertyId, TValue *value)
    {
        foundation::InspectablePtr spValue;
        IFR_ASSERT(this->GetClassInterface()->GetProperty(propertyId, spValue.GetAddressOf()));
        IFR(foundation::pv_util::GetValue(spValue, value));
        return S_OK;
    }

    template <class TValue>
    HRESULT GetProperty(UINT32 propertyId, TValue& value)
    {
        foundation::InspectablePtr spValue;
        IFR_ASSERT(this->GetClassInterface()->GetProperty(propertyId, spValue.GetAddressOf()));
        IFR(foundation::GetValue(spValue, value));
        return S_OK;
    }

    template <class TValue>
    HRESULT SetProperty(UINT32 propertyId, TValue value)
    {
        foundation::InspectablePtr spValue = foundation::pv_util::CreateValue(value);
        IFR(this->GetClassInterface()->SetProperty(propertyId, spValue));
        return S_OK;
    }
};

template <class T = IObservableObject>
class ObservableObjectPtr :
    public foundation::_ObjectPtr
    <
        _Observable_object_ptr_wrapper<ObservableObjectPtr<T>>,
        T
    >
{
public:
    typedef foundation::_ObjectPtr
        <
        _Observable_object_ptr_wrapper<ObservableObjectPtr<T>>,
        T
        > _Base_Type;

    ObservableObjectPtr() throw()
    {
    }

    ObservableObjectPtr(_Inout_ const ObservableObjectPtr<T>& lp) throw() :
        _Base_Type(lp)
    {
    }

    ObservableObjectPtr(_Inout_ const foundation::_ComPtrBase<T>& lp) throw() :
        _Base_Type(lp.get())
    {
    }

    ObservableObjectPtr(_In_ T* lp) throw() :
        _Base_Type(lp)
    {
    }
protected:
    template <class value_t>
    HRESULT GetValue(
        _In_ HRESULT(T::*pGetPropertyValue)(value_t* pT),
        _Out_ value_t *pValue
        )
    {
        _IFR_(this->hresult());
        if (this->get() == nullptr)
        {
            return E_POINTER;
        }
        return (this->get()->*pGetPropertyValue)(pValue);
    }

    template <class value_t>
    HRESULT SetValue(
        _In_ value_t value,
        _In_ HRESULT(T::*pSetPropertyValue)(value_t value)
        )
    {
        _IFR_(this->hresult());
        if (this->get() == nullptr)
        {
            return E_POINTER;
        }
        return (this->get()->*pSetPropertyValue)(value);
    }

    HRESULT SetValue(
        _In_ LPCSTR_t value,
        _In_ HRESULT(T::*pSetPropertyValue)(HSTRING value)
        )
    {
        return SetValue((HSTRING)foundation::HStringPtr(value), pSetPropertyValue);
    }

    HRESULT SetValue(
        _In_ foundation::SystemTime value,
        _In_ HRESULT(T::*pSetPropertyValue)(foundation::DateTime value)
        )
    {
        foundation::DateTime dateTime;
        _pal_SystemTimeToDateTime(value, &dateTime);
        return SetValue(dateTime, pSetPropertyValue);
    }

    template <class ComPtr>
    HRESULT GetComPtr(
        _In_ HRESULT(T::*pGetPropertyValue)(typename ComPtr::_PtrClass** ppT),
        _Out_ ComPtr& ptr
        )
    {
        HRESULT hr = this->get() == nullptr ? E_POINTER : this->hresult();
        if (SUCCEEDED(hr))
        {
            hr = (this->get()->*pGetPropertyValue)(ptr.ReleaseAndGetAddressOf());
        }
        ptr._Sethresult(hr);
        return hr;
    }

    template <class ComPtr>
    ComPtr GetComPtr(
        _In_ HRESULT(T::*pGetPropertyValue)(typename ComPtr::_PtrClass** ppT)
        )
    {
        ComPtr ptr;
        IGNOREHR(GetComPtr(pGetPropertyValue, ptr));
        return ptr;
    }

    template <class value_t>
    value_t GetValueWithException(
        _In_ HRESULT(T::*pGetPropertyValue)(value_t* pT)
        )
    {
        value_t value;
        THROW_IFR(GetValue(pGetPropertyValue, &value));
        return value;
    }

    foundation::HStringPtr GetValueWithException(
        _In_ HRESULT(T::*pGetPropertyValue)(HSTRING* pT)
        )
    {
        foundation::HStringPtr value;
        THROW_IFR(GetValue(pGetPropertyValue, value.GetAddressOf()));
        return value;
    }

    foundation::SystemTime GetValueWithException(
        _In_ HRESULT(T::*pGetPropertyValue)(foundation::DateTime* pT)
        )
    {
        foundation::DateTime value;
        THROW_IFR(GetValue(pGetPropertyValue, &value));
        foundation::SystemTime dateTime;
        _pal_DateTimeToSystemTime(value, &dateTime);
        return dateTime;
    }

    template <class value_t>
    void SetValueWithException(
        _In_ value_t value,
        _In_ HRESULT(T::*pSetPropertyValue)(value_t value)
        )
    {
        THROW_IFR(SetValue(value,pSetPropertyValue));
    }

    void SetValueWithException(
        _In_ LPCSTR_t value,
        _In_ HRESULT(T::*pSetPropertyValue)(HSTRING value)
        )
    {
        THROW_IFR(SetValue(value, pSetPropertyValue));
    }

    void SetValueWithException(
        _In_ foundation::SystemTime value,
        _In_ HRESULT(T::*pSetPropertyValue)(foundation::DateTime value)
        )
    {
        THROW_IFR(SetValue(value, pSetPropertyValue));
    }

    template <class ComPtr>
    ComPtr GetComPtrWithException(
        _In_ HRESULT(T::*pGetPropertyValue)(typename ComPtr::_PtrClass** ppT)
        )
    {
        ComPtr ptr;
        THROW_IFR(GetComPtr(pGetPropertyValue, ptr));
        return ptr;
    }

};


}

