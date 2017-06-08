/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandAdapterBase.h
****/

#pragma once

#include <foundation/library/adapter_base.h>
#include <foundation/pv_util.h>

//------------------------------------------------------------------------------
// Class:   TMoCommandAdapterBase
//
// Purpose: Define a template base class for teh Command Model adapter generated classes
//
//------------------------------------------------------------------------------
template <class TInterface,const IID* piid = &__uuidof(TInterface)>
class TMoCommandAdapterBase :
    public foundation::AdapterBase
        <
        pmod::ICommand,
        TInterface,
        piid
        >
{
protected:
    template <class T>
    HRESULT ReturnValue(IInspectable *pParameter,T **ppValue)
    {
        if(pParameter)
        {
            IFR(pParameter->QueryInterface(__uuidof(T),(void **)ppValue));
        }

        return S_OK;
    }

    template <class T>
    HRESULT ReturnValue(IInspectable *pParameter, T *pValue)
    {
        return foundation::pv_util::GetValue(pParameter,pValue);
    }
    
    template<>
    HRESULT ReturnValue(IInspectable *pParameter, HSTRING *pValue)
    {
        return foundation::pv_util::GetStringValue(pParameter, pValue);
    }

    template <class T>
    HRESULT CreateValue(_In_ T value, _COM_Outptr_ IInspectable **ppParameter)
    {
        *ppParameter = foundation::pv_util::CreateValue(value).Detach();
        return S_OK;
    }

    template <class T>
    HRESULT CreateValue(_In_ UINT32 size, _In_reads_opt_(size) T* values, _COM_Outptr_ IInspectable **ppParameter)
    {
        *ppParameter = foundation::pv_util::CreateValue(size, values).Detach();
        return S_OK;
    }

    template <>
    HRESULT CreateValue(_In_ UINT32 size, _In_reads_opt_(size) HSTRING* values, _COM_Outptr_ IInspectable **ppParameter)
    {
        return foundation::pv_util::CreateStringArrayValue(size, values, ppParameter);
    }
};
