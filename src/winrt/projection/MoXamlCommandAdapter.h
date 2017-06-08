/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoXamlCommandAdapter.h
****/

#pragma once

// Note: classic Windows API define GetCurrentTime macro which trigger a warning 
#undef GetCurrentTime

#include <Windows.UI.Xaml.h>
#include <foundation/ctl/com_library.h>
#include "MoBaseAdapter.h"
#include <pmod\interfaces.h>

//------------------------------------------------------------------------------
// Class:   CMoXamlCommandAdapter
//
// Purpose: Define a base class for a 'ICommand' implementation for Direct UI
//
//------------------------------------------------------------------------------

class CMoXamlCommandAdapter :
    public CMoBaseAdapter
        <
        pmod::ICommand,
        ABI::Windows::UI::Xaml::Input::ICommand,
        ABI::Windows::Foundation::IEventHandler<IInspectable*>,
        IInspectable
        >
{
public: 

    // Interface ABI::Windows::UI::Xaml::Input::ICommand
    __override HRESULT STDMETHODCALLTYPE add_CanExecuteChanged (
        _In_opt_ ABI::Windows::Foundation::IEventHandler<IInspectable*> *value,
        _Out_ EventRegistrationToken*  token)
    {
        return this->add_Handler(value,token);
    }

    HRESULT STDMETHODCALLTYPE remove_CanExecuteChanged(_In_ EventRegistrationToken  token)
    {
        return this->remove_Handler(token);
        return S_OK;
    }

    // Interface ABI::Windows::UI::Xaml::Input::ICommand
    HRESULT STDMETHODCALLTYPE CanExecute(_In_opt_ IInspectable *parameter,_Out_ boolean *returnValue) override;
    HRESULT STDMETHODCALLTYPE Execute(_In_opt_ IInspectable *parameter) override;

protected:
    // Interface IModelAdapter
    STDMETHOD(FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		_In_ IUnknown* pEventArgs) override;
};




