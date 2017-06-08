/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectInfoAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoMethodInfoAdapter
//
// Purpose: Implementation of IMethodInfo Interface
//
//------------------------------------------------------------------------------
class CMoMethodInfoAdapter :
    public foundation::AdapterBase
        <
        foundation::IMethodInfo,
        IMethodInfo,
        &__uuidof(IMethodInfo)
        >
{
protected:
    HRESULT STDMETHODCALLTYPE get_MethodId( 
        /* [out][retval] */ UINT32 *id) 

    {
        IFR_ASSERT(m_pInner->GetId(id));
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE get_Name( 
        /* [out][retval] */ HSTRING *value)
    {
        IFR_ASSERT(m_pInner->GetMethodName(value));
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE GetParameters( 
        UINT32 *length,
        ABI::Microsoft::PropertyModel::ParameterInfo **value)
    {
        UNREFERENCED_PARAMETER(length);
        UNREFERENCED_PARAMETER(value);
        return E_NOTIMPL;
    }
};
//------------------------------------------------------------------------------
// Class:   CMoObservableObjectInfoAdapter
//
// Purpose: Implementation of IObservableObjectInfo Interface
//
//------------------------------------------------------------------------------
class CMoObservableObjectInfoAdapter :
    public foundation::AdapterBase
        <
        pmod::IObservableObjectInfo,
        IObservableObjectInfo,
        &__uuidof(IObservableObjectInfo)
        >
{
public:
};
//------------------------------------------------------------------------------
// Class:   CMoViewModelInfoAdapter
//
// Purpose: Implementation of IViewModelInfo Interface
//
//------------------------------------------------------------------------------
class CMoViewModelInfoAdapter :
    public foundation::AdapterBase
        <
        pmod::IViewModelInfo,
        IViewModelInfo,
        &__uuidof(IViewModelInfo)
        >
{
public:
     HRESULT STDMETHODCALLTYPE get_IsInherited( 
                            boolean *value) override
     {
         IFCPTR(value);

         bool result;
         IFR(m_pInner->GetIsInherited(&result));
         *value = result ? true:false;
         return S_OK;
     }
                        
     HRESULT STDMETHODCALLTYPE get_SourceType( 
                            IObservableObjectInfo **value) override
     {
         IFCPTR(value);

         foundation::ComPtr<pmod::IObservableObjectInfo> spPropertyModelInfo;
         IFR(m_pInner->GetSourceType(spPropertyModelInfo.GetAddressOf()));
         if(spPropertyModelInfo)
         {
             IFR(foundation::QueryInterface(spPropertyModelInfo,value));
         }
         else
         {
             *value = nullptr;
         }
         return S_OK;
     }
};

