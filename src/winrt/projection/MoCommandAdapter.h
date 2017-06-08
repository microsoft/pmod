/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"
#include <pmod\interfaces.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoCommandAdapter
//
// Purpose: Implementation of ICommand Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoCommandAdapter :
    public CMoBaseAdapter
        <
        pmod::ICommand,
        ICommand,
        ICanExecuteChangedEventHandler,
        ICanExecuteChangedEventArgs
        >,
    public ICommandBase,
    public ISimpleCommand,
    public ISimpleAsyncCommand,
    public IAsyncCommand

{
public:
    // ICommandModelBase
    HRESULT STDMETHODCALLTYPE get_State(unsigned int *value) override;
                                        
    HRESULT STDMETHODCALLTYPE add_CanExecuteChanged( 
        ICanExecuteChangedEventHandler *value,
        EventRegistrationToken *token) override
    {
        return this->add_Handler(value,token);
    }
                    
    HRESULT STDMETHODCALLTYPE remove_CanExecuteChanged( 
        EventRegistrationToken token) override
    {
        return this->remove_Handler(token);
    }

    // ICommand/IAsyncCommand
    HRESULT STDMETHODCALLTYPE CanExecute( 
        IInspectable *parameter,
        boolean *value) override;
                    
    // ICommand
    HRESULT STDMETHODCALLTYPE Execute(
        IInspectable *parameter,
        IInspectable **result) override;

    // IAsyncCommand
    HRESULT STDMETHODCALLTYPE Execute(IInspectable *parameter, ABI::Windows::Foundation::IAsyncOperation<IInspectable *> **ppAsyncAction) override;

    // ISimpleCommand/ISimpleAsyncCommand
    HRESULT STDMETHODCALLTYPE CanExecute(boolean *value) override;

    // ISimpleCommand
    HRESULT STDMETHODCALLTYPE Execute(void) override;

    // ISimpleAsyncCommand
    HRESULT STDMETHODCALLTYPE Execute(ABI::Windows::Foundation::IAsyncOperation<IInspectable *> **ppAsyncAction) override;

    // Interface IInterfaceAdapter
    STDMETHOD (QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface)  override
    {
        if (iid == __uuidof(ICommandBase))
        {
            *ppInterface = static_cast<ICommandBase *>(this);
        }
        else if (iid == __uuidof(ISimpleCommand))
        {
            *ppInterface = static_cast<ISimpleCommand *>(this);
        }
        else if (iid == __uuidof(ISimpleAsyncCommand))
        {
            *ppInterface = static_cast<ISimpleAsyncCommand *>(this);
        }
        else if (iid == __uuidof(IAsyncCommand))
        {
            *ppInterface = static_cast<IAsyncCommand *>(this);
        }
        else
        {
            return __super::QueryAdapterInterface(iid,ppInterface);
        }

        return S_OK;
    }   

};

//------------------------------------------------------------------------------
// Class:   CMoCanExecuteChangedEventArgsAdapter
//
// Purpose: Implementation of ICanExecuteChangedEventArgs Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoCanExecuteChangedEventArgsAdapter :
    public foundation::AdapterBase
    <
        pmod::ICanExecuteChangedEventArgs,
        ICanExecuteChangedEventArgs,
        &__uuidof(ICanExecuteChangedEventArgs)
    >
{
public:
    HRESULT STDMETHODCALLTYPE get_OldState(unsigned int *value) override
    {
        UINT32 oldState;
        IFR( m_pInner->GetOldState(&oldState));
        *value = oldState;
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE get_NewState(unsigned int *value) override
    {
        UINT32 newState;
        IFR( m_pInner->GetNewState(&newState));
        *value = newState;
        return S_OK;
    }

};
