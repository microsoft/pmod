/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandViewModelFactory.cpp
****/

#include "pch.h"

#include "MoCommandViewModelFactory.h"
#include "MoCommandFactory.h"

#include "MoSourceModelDelegateAdapter.h"

#include <pmod/library/library_util.h>
#include <pmod/errors.h>

//------------------------------------------------------------------------------
// Class:   CMoCommandViewModelDelegateAdapter
//
// Purpose: Implementation of ICommandViewModelDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoCommandViewModelDelegateAdapter :
    public CMoSourceModelDelegateAdapter
    <
        pmod::library::ICommandViewModelDelegate,
        pmod::ICanExecuteChangedEventHandler,
        pmod::ICommand
    >
{
protected:
    // ICommandViewModelDelegate Implementation
    STDMETHOD (OnSourceCommandStateChanged)(_In_ int oldState,_In_ int newState)
    {
        if(IsOverrideComposed())
        {
            ICommandViewModelOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));

            IFR(pOverrides->OnSourceCommandStateChanged(oldState,newState));
        }
        return S_OK;
    }
};

STDMETHODIMP MoCommandViewModelFactory::CreateInstanceWithSource( 
        ICommandBase *pSource,
        CommandOptions modelOptions,
        CommandViewModelOptions viewModelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        ICommandViewModel **ppInstance)
{
    return CreateInstanceWithSourceAndType( 
        GUID_NULL,
        pSource,
        modelOptions,
        viewModelOptions,
        pOuter,
        ppInner,
        ppInstance);
}

STDMETHODIMP MoCommandViewModelFactory::CreateInstanceWithSourceAndType( 
        GUID iidType,
        ICommandBase *pSource,
        CommandOptions modelOptions,
        CommandViewModelOptions viewModelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        ICommandViewModel **ppInstance)
{
    // Find the Type...
    if (!::IsEqualIID(iidType, GUID_NULL) && !foundation::IsTypeInfoRegistered(iidType))
    {
        return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
    }
    HRESULT hr;

    foundation::ComPtr<CMoCommandDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    foundation::ComPtr<CMoCommandViewModelDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spViewModelDelegate;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spViewModelDelegate.GetAddressOf()));

    foundation::ComPtr<pmod::ICommand> spSource;
    if(pSource != nullptr)
    {
        IFHR(foundation::QueryInterface(pSource,spSource.GetAddressOf()));
    }
    pmod::library::CommandViewModelCreateParameters param;
    param.m_options = (pmod::library::CommandOptions)modelOptions;
    param.m_view_model_options = (pmod::library::CommandViewModelOptions)viewModelOptions;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_p_viewmodel_delegate = spViewModelDelegate;
    param.m_p_source = spSource;
    param.m_command_id = 0;
    param.m_command_label = nullptr;

    foundation::ComPtr<IInspectable> spInner;
    hr = pmod::library::GetCommandViewModelClassFactory()->
        CreateCommandViewModelInstance(&param,pOuter,spInner.GetAddressOf());
    IFHR(hr);
    IFHR(foundation::QueryInterface(spInner,ppInstance));
    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}
