/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandProxy.h
****/

#pragma once

#include <foundation/assert.h>

#include "Microsoft.PropertyModel.h"
#include "MoAdapterProxyInterface.h"
#include "MoModelProxy.h"
#include <pmod/library/model_library.h>
#include "MoEventSourceProxy.h"
#include "MoCoreProxyFactory.h"
#include "MoObservableObjectProxy.h"

//------------------------------------------------------------------------------
// Class:   CMoCanExecuteChangedEventArgsProxy
//
// Purpose: Implementation of pmod::ICanExecuteChangedEventArgs
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoCanExecuteChangedEventArgsProxy :
    public TAdapterProxyInterface
    <
        pmod::ICanExecuteChangedEventArgs,
        ABI::Microsoft::PropertyModel::ICanExecuteChangedEventArgs
    >
{
protected:
    STDMETHOD(GetOldState)(UINT32* pState)
    {
        return this->m_pSourceInterface->get_OldState(pState);
    }
    STDMETHOD(GetNewState)(UINT32* pState) 
    {
        return this->m_pSourceInterface->get_NewState(pState);
    }
};

//------------------------------------------------------------------------------
// Class:   CCanExecuteChangedEventHandlerProxy
//
// Purpose: Implementation of Modern ICanExecuteChangedEventHandler
//          Interface using the Core Interface callback
//------------------------------------------------------------------------------
class CCanExecuteChangedEventHandlerProxy :
    public TFreeThreadedAdapterProxyInterface
    <
        ABI::Microsoft::PropertyModel::ICanExecuteChangedEventHandler,
        pmod::ICanExecuteChangedEventHandler
    >
{
protected:
    HRESULT STDMETHODCALLTYPE Invoke(
        IInspectable *sender,
        ABI::Microsoft::PropertyModel::ICanExecuteChangedEventArgs *e) override
    {
        foundation::ComPtr<pmod::ICanExecuteChangedEventArgs> spCanExecuteChangedEventArgs;
        HRESULT hr = CMoCanExecuteChangedEventArgsProxy::CreateInstance<CMoCanExecuteChangedEventArgsProxy>
           (e,spCanExecuteChangedEventArgs.GetAddressOf());
        IFR_ASSERT(hr);
        return this->m_pSourceInterface->Invoke(sender,spCanExecuteChangedEventArgs);
    }
};

typedef 
    TMoEventSourceProxy<
        pmod::ICanExecuteChangedEventSource,
        pmod::ICanExecuteChangedEventHandler,
        ABI::Microsoft::PropertyModel::ICanExecuteChangedEventHandler,
        ABI::Microsoft::PropertyModel::ICommandBase,
        &ABI::Microsoft::PropertyModel::ICommandBase::add_CanExecuteChanged,
        &ABI::Microsoft::PropertyModel::ICommandBase::remove_CanExecuteChanged,
        CCanExecuteChangedEventHandlerProxy>
    CMoCanExecuteChangedEventSourceProxy;

//------------------------------------------------------------------------------
// Class:   CMoCommandModelProxy
//
// Purpose: Implementation of pmod::ICommand
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoCommandModelProxy :
    public TBaseObservableObjectAggregationProxy
    <
		TMoObjectProxy
        <
            pmod::ICommand,
            ABI::Microsoft::PropertyModel::ICommand
        >
    >
{
public:
    HRESULT SetSourceInterface(ABI::Microsoft::PropertyModel::ICommand *pMoInterface)
    {
        IFR_ASSERT(__super::SetSourceInterface(pMoInterface));
        IFR_ASSERT(foundation::QueryInterface(pMoInterface,&this->_pCommandModelBase));
        return S_OK;
    }
protected:
    CMoCommandModelProxy():
        _pCommandModelBase(nullptr)
    {
    }

    void FinalRelease() override
    {
        RELEASE_INTERFACE(_pCommandModelBase);
        __super::FinalRelease();
    }

    // Properties
    STDMETHOD(GetState)(UINT32* pVal)
    {
        foundation_assert(_pCommandModelBase);
        return this->_pCommandModelBase->get_State(pVal);
    }
    // Methods.
    STDMETHOD(CanExecute)(IInspectable *pParameter,bool *pCanExecute)
    {
        PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_CommandModel_CanExecute_Id)

        foundation::InspectablePtr spCoreParameter;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxy(pParameter,spCoreParameter.GetAddressOf()));
        boolean bCanExecute;
        IFR(this->m_pSourceInterface->CanExecute(spCoreParameter,&bCanExecute));
        *pCanExecute = bCanExecute ? true:false;
        return S_OK;
    }

    STDMETHOD(Execute)(IInspectable *pParameter,IInspectable **ppResult)
    {
        PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_CommandModel_Execute_Id)

        foundation::InspectablePtr spCoreParameter;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxy(pParameter,spCoreParameter.GetAddressOf()));

        foundation::InspectablePtr spResultValue;
        IFR(this->m_pSourceInterface->Execute(spCoreParameter,spResultValue.GetAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spResultValue,ppResult));
        return S_OK;
    }

    // Event methods.
    STDMETHOD(GetCanExecuteChangedEventSource)(pmod::ICanExecuteChangedEventSource** ppEventSource)
    {
        if(_spCanExecuteChangedEventSource == nullptr)
        {
            HRESULT hr =  CMoCanExecuteChangedEventSourceProxy::CreateInstance<CMoCanExecuteChangedEventSourceProxy>
            (this->_pCommandModelBase,_spCanExecuteChangedEventSource.GetAddressOf());
        }
        return _spCanExecuteChangedEventSource.CopyTo(ppEventSource);

    }
private:
    ABI::Microsoft::PropertyModel::ICommandBase *    _pCommandModelBase;
    foundation::ComPtr<pmod::ICanExecuteChangedEventSource>      _spCanExecuteChangedEventSource;
};
