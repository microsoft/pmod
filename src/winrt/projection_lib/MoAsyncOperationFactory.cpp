/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationFactory.cpp
****/

#include "pch.h"

#include "MoAsyncOperationFactory.h"
#include "MoBaseDelegateAdapter.h"

#include <pmod/library/library_util.h>

//------------------------------------------------------------------------------
// Class:   CMoAsyncOperationDelegateAdapter
//
// Purpose: Implementation of IAsyncOperationModelDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoAsyncOperationDelegateAdapter :
    public CMoDelegeteBaseAdapter
    <
    foundation::library::IAsyncOperationDelegate,
        foundation::ICompletedEventHandler
    >
{
protected:
     // IAsyncOperationModelDelegate Implementation
    STDMETHOD( OnCancel )()
    {
        if(IsOverrideComposed())
        {
            IAsyncOperationOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));
            IFR(pOverrides->OnCancel());
        }
        return S_OK;
    }
};

STDMETHODIMP MoAsyncOperationFactory::CreateInstance(
        AsyncOperationOptions modelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        IAsyncOperation **ppInstance)
{
    return CreateInstanceWithType(GUID_NULL,modelOptions,pOuter,ppInner,ppInstance);
}

STDMETHODIMP MoAsyncOperationFactory::CreateInstanceWithType(
        GUID iidType,
        AsyncOperationOptions modelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        IAsyncOperation **ppInstance)
{
    // Find the Type...
    //if(!::IsEqualIID(iidType,GUID_NULL) &&  !foundation::IsTypeInfoRegistered(iidType))
    //{
    //    return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
    //}
    HRESULT hr;

    foundation::ComPtr<CMoAsyncOperationDelegateAdapter,foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    foundation::library::async_operation_create_parameters param;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_options = (foundation::AsyncOperationOptions)modelOptions;

    foundation::InspectablePtr spInner;
    hr = foundation::library::GetAsyncOperationFactory()->
        CreateAsyncOperation(&param,pOuter,spInner.GetAddressOf());

    IFHR(hr);
    IFHR(foundation::QueryInterface(spInner,ppInstance));

    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}
