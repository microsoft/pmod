/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectAdapterBase.h
****/

#pragma once

#include <foundation/library/adapter_base.h>
#include <foundation/library/logger_macros.h>
#include "Microsoft.PropertyModel.h"
#include <foundation/pv_util.h>
#include <foundation/array_wrapper.h>

//------------------------------------------------------------------------------
// Class:   TMoObservableObjectAdapterBase
//
// Purpose: Define a template base class for all the model adapter generated classes
//
//------------------------------------------------------------------------------
template <class TInterface,const IID* piid = &__uuidof(TInterface)>
class TMoObservableObjectAdapterBase :
    public foundation::AdapterBase
        <
        pmod::IObservableObject,
        TInterface,
        piid
        >
{
protected:
    HRESULT Invoke(int methodId,UINT32 length,IInspectable **pParameters,IInspectable **result)
    {
        return m_pInner->InvokeMethod((UINT32)methodId,length,pParameters,result);
    }

    template <class T>
    HRESULT GetProperty(int propertyId,T **ppValue)
    {
        foundation::InspectablePtr spValue;
        IFR(GetPropertyValue(propertyId,spValue.GetAddressOf()));
        if(spValue)
        {
            return  spValue->QueryInterface(__uuidof(T),(void **)ppValue);
        }
        else
        {
            *ppValue = nullptr;
            return S_OK;
        }
    }

    template <class T>
    HRESULT GetProperty(int propertyId,T *ppValue)
    {
        IFCPTR(ppValue);
        UINT32 enumValue;
        IFR(this->GetProperty(propertyId,&enumValue));
        *ppValue = (T)enumValue;
        return S_OK;
    }

    template <class T>
    HRESULT SetProperty(int propertyId,T *pValue)
    {
        return SetPropertyValue(propertyId,pValue);
    }

    // HSTRING Type Support
    HRESULT GetProperty(int propertyId,HSTRING *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,HSTRING value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // INT16 Type Support
    HRESULT GetProperty(int propertyId,INT16 *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,INT16 value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // UINT16 Type Support
    HRESULT GetProperty(int propertyId,UINT16 *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,UINT16 value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // INT32 Type Support
    HRESULT GetProperty(int propertyId,INT32 *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,INT32 value)
    {
        return SetPropertyValue(propertyId,value);
    }
    // UINT32 Type Support
    HRESULT GetProperty(int propertyId,UINT32 *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,UINT32 value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // INT64 Type Support
    HRESULT GetProperty(int propertyId,INT64 *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,INT64 value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // UINT64 Type Support
    HRESULT GetProperty(int propertyId,UINT64 *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,UINT64 value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // bool Type Support
    HRESULT GetProperty(int propertyId,bool *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,bool value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // boolean Type Support
    HRESULT GetProperty(int propertyId,boolean *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,boolean value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // DateTime Type Support
    HRESULT GetProperty(int propertyId,ABI::Windows::Foundation::DateTime *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,ABI::Windows::Foundation::DateTime value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // Double Type Support
    HRESULT GetProperty(int propertyId,DOUBLE *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,DOUBLE value)
    {
        return SetPropertyValue(propertyId,value);
    }

    // Float Type Support
    HRESULT GetProperty(int propertyId,FLOAT *pValue)
    {
        return GetPropertyValue(propertyId,pValue);
    }
    HRESULT SetProperty(int propertyId,FLOAT value)
    {
        return SetPropertyValue(propertyId,value);
    }  

    // GUID Type Support
    HRESULT GetProperty(int propertyId, GUID *pValue)
    {
        return GetPropertyValue(propertyId, pValue);
    }
    HRESULT SetProperty(int propertyId, GUID value)
    {
        return SetPropertyValue(propertyId, value);
    }

    // Array support
    template <class T>
    HRESULT SetProperty(_In_ UINT32 propertyId,_In_ UINT32 size,_In_ T *values)
    {
        foundation::InspectablePtr spValue = foundation::pv_util::CreateValue(size,values);
        return this->SetPropertyValue(propertyId,spValue);
    }

    template <class T>
    HRESULT GetProperty(_In_ UINT32 propertyId,_Out_ UINT32 *size,_Out_ T **values)
    {
        foundation::InspectablePtr spValue;
        IFR(this->GetPropertyValue(propertyId,spValue.GetAddressOf()));
        IFR(foundation::pv_util::GetValue(spValue,size,values));
        return S_OK;
    }

    template <class T>
    HRESULT GetProperty(UINT32 propertyId,UINT32 *size,T ***ppValues)
    {
        return GetProperty(propertyId,size,__uuidof(T),ppValues);
    }

    template <>
    HRESULT GetProperty(UINT32 propertyId, UINT32 *size, HSTRING **ppValues)
    {
        IFCPTR(size);
        IFCPTR(ppValues);

        foundation::InspectablePtr value;
        IFR(GetProperty(propertyId, value.GetAddressOf()));
        return foundation::pv_util::GetStringArray(value.Get(), size, ppValues);
    }

    template <class T>
    HRESULT GetProperty(UINT32 propertyId,UINT32 *size,const IID& iid,T ***ppValues)
    {
        IFCPTR(size);
        IFCPTR(ppValues);

        foundation::InspectablePtr value;
        IFR(GetProperty(propertyId,value.GetAddressOf()));
        return foundation::pv_util::GetValue(value,size,iid,ppValues);
    }
private:
    HRESULT GetPropertyValue(int propertyId,IInspectable **ppValue)
    {
        return m_pInner->GetProperty(propertyId,ppValue);
    }

    HRESULT SetPropertyValue(int propertyId,IInspectable *pValue)
    {
        return m_pInner->SetProperty(propertyId,pValue);
    }

    template <class T>
    HRESULT GetPropertyValue(int propertyId,T *pValue)
    {
        foundation::InspectablePtr spValue;
        IFR(GetPropertyValue(propertyId,spValue.GetAddressOf()));
        return foundation::pv_util::GetValue(spValue,pValue);
    }

    template <>
    HRESULT GetPropertyValue(int propertyId,boolean *pValue)
    {
        foundation::InspectablePtr spValue;
        IFR(GetPropertyValue(propertyId,spValue.GetAddressOf()));
        bool bVal;
        IFR(foundation::pv_util::GetValue(spValue,&bVal));
        *pValue = (boolean)bVal;
        return S_OK;
    }

    template <class T>
    HRESULT SetPropertyValue(int propertyId,T value)
    {
        return m_pInner->SetProperty(propertyId,foundation::pv_util::CreateValue(value));
    }

    template <>
    HRESULT SetPropertyValue(int propertyId,boolean value)
    {
        return m_pInner->SetProperty(propertyId,foundation::pv_util::CreateValue((bool)(value ? true:false)));
    }

};
