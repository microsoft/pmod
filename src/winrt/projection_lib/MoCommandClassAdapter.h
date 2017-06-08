/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandClassAdapter.h
****/

#pragma once


#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoCommandClassAdapter
//
// Purpose: Implementation of ICommandClass Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoCommandClassAdapter :
    public foundation::AdapterBase
    <
    pmod::library::ICommandClass,
    ICommandClass,
    &__uuidof(ICommandClass)
    >
{
public:
    // Interface ICommandClass
    HRESULT STDMETHODCALLTYPE get_CanExecuteDelegate(
        ICanExecuteDelegate **value) override;

    HRESULT STDMETHODCALLTYPE put_CanExecuteDelegate(
        ICanExecuteDelegate *value) override;

    HRESULT STDMETHODCALLTYPE get_ExecuteDelegate(
        IExecuteDelegate **value) override;

    HRESULT STDMETHODCALLTYPE put_ExecuteDelegate(
        IExecuteDelegate *value) override;

    HRESULT STDMETHODCALLTYPE SetCommandState( int commandState,boolean fireCanExecute) override;
    HRESULT STDMETHODCALLTYPE FireCanExecuteChanged( void) override;

};
//------------------------------------------------------------------------------
// Class:   CMoCommandModelProtectedAdapter
//
// Purpose: Implementation of ICommandModelProtected Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoCommandModelProtectedAdapter :
    public foundation::AdapterBase
        <
        pmod::library::ICommandClass,
        ICommandProtected,
        &__uuidof(ICommandProtected)
        >
{
public:
    // Interface ICommandModelProtected

};

//------------------------------------------------------------------------------
// Class:   CMoCommandModelOverridesAdapter
//
// Purpose: Implementation of ICommandModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoCommandModelOverridesAdapter :
    public foundation::AdapterBase
        <
        pmod::library::ICommandClass,
        ICommandOverrides,
        &__uuidof(ICommandOverrides)
        >
{
public:
    // Interface ICommandModelOverrides
    HRESULT STDMETHODCALLTYPE OnCanExecute(IInspectable *parameter,boolean *value) override
    {
        UNREFERENCED_PARAMETER(parameter);
        *value = true;
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE OnExecute( IInspectable *parameter,IInspectable **result) override
    {
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(result);
        return E_NOTIMPL;
    }
};

