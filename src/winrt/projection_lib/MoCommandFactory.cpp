/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandFactory.cpp
****/

#include "pch.h"

#include "MoCommandFactory.h"

#include "MoBaseDelegateAdapter.h"

#include <pmod/library/model_library.h>
#include <pmod/library/library_util.h>
#include <pmod/errors.h>


STDMETHODIMP CMoCommandDelegateAdapter::OnCanExecute(_In_ IInspectable *parameter,_Outptr_ bool *pValue)
{
    if(IsOverrideComposed())
    {
        ICommandOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));

        boolean result;
        IFR(pOverrides->OnCanExecute(parameter,&result));
        *pValue = result ? true:false;
        return S_OK;
    }
    else
    {
        *pValue = true;
        return S_OK;
    }
}

STDMETHODIMP CMoCommandDelegateAdapter::OnExecute(_In_ IInspectable *parameter,_Outptr_ IInspectable **ppResult) 
{
    if(IsOverrideComposed())
    {
        ICommandOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));

        if (*ppResult != nullptr)
        {
            // we assume is a IAsyncOperation
            foundation::InspectablePtr parameter_;
            IFR_ASSERT(CreateAsyncParameters(parameter, *ppResult, parameter_.GetAddressOf()));
            foundation::InspectablePtr ignore_result_ptr;
            IFR(pOverrides->OnExecute(parameter_, ignore_result_ptr.GetAddressOf()));
        }
        else
        {
            IFR(pOverrides->OnExecute(parameter, ppResult));
        }
        return S_OK;
    }
    else
    {
        return E_NOTIMPL;
    }
}

HRESULT CMoCommandDelegateAdapter::CreateAsyncParameters(
    _In_ IInspectable *parameter,
    _In_ IInspectable *pAsyncOperation,
    _Out_ IInspectable** ppParameter)
{
    IInspectable *parameters[2];
    parameters[0] = parameter;
    IFR_ASSERT(foundation::QueryWeakReference(
        pAsyncOperation,
        __uuidof(IAsyncOperation),
        &parameters[1]));
    IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
        2, 
        parameters, 
        ppParameter));
    return S_OK;
}

STDMETHODIMP MoCommandFactory::CreateInstance(
        CommandOptions options,
        IInspectable* pOuter,
        IInspectable** ppInner,
        ICommand** ppInstance)
{
    return CreateInstanceWithType(
        options,
        GUID_NULL,
        pOuter,
        ppInner,
        ppInstance);
}

STDMETHODIMP MoCommandFactory::CreateInstanceWithType(
        CommandOptions options,
        GUID iidType,
        IInspectable* pOuter,
        IInspectable** ppInner,
        ICommand** ppInstance)
{
    // Find the Type...
    if (!::IsEqualIID(iidType, GUID_NULL) && !foundation::IsTypeInfoRegistered(iidType))
    {
        return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
    }
    HRESULT hr;

    foundation::ComPtr<CMoCommandDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    pmod::library::CommandCreateParameters param;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_command_id = 0;
    param.m_options = (pmod::library::CommandOptions)options;

    foundation::InspectablePtr spInner;
    hr = pmod::library::GetCommandClassFactory()->
        CreateCommandInstance(&param,pOuter,spInner.GetAddressOf());
    IFHR(hr);

    IFHR(foundation::QueryInterface(spInner,ppInstance));
    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE MoCommandFactory::get_CommandType(GUID *value)
{
    *value = pmod::CommandModel::IIDType;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE MoCommandFactory::get_SimpleCommandType(GUID *value)
{
    *value = pmod::SimpleCommandTypeInfo::IIDType;
    return S_OK;
}
