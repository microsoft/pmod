/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoBaseAdapter.h
****/

#pragma once

#include "MoEventHandler.h"
#include "FoundationUtilHelper.h"
#include "CastHelper.h"

// Public Headers
#include <foundation/library/adapter_base.h>

//------------------------------------------------------------------------------
// Class:   CMoBaseAdapter
//
// Purpose: Base Mo Adapter class
//
//------------------------------------------------------------------------------
template <
    class TCoreInterface,
    class TMoInterface,
    class TMoEventHandler,
    class TMoEventArgs,
    class TMoEventHandlerStorage=
        MoEventHandler
        <
            TMoEventHandler,
            IInspectable,
            TMoEventArgs
        >,
    class TCastToSender = TInspectableCast
>
class CMoBaseAdapter :
    public foundation::AdapterBase
        <
        TCoreInterface,
        TMoInterface,
        &__uuidof(TMoInterface)
        >,
    public TMoEventHandlerStorage
{
public:

    // Interface IInterfaceAdapter
    STDMETHOD (HasEventHandlers)(_Outptr_ bool *pValue) override
    {
        *pValue = this->GetEventHandlers().size() > 0;
        return S_OK;
    }
    STDMETHOD (FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		_In_ IUnknown* pEventArgs) override
    {
        foundation_assert(pEventArgs);

        foundation::ComPtr<TMoEventArgs> spMoEventArgs;
        if(SUCCEEDED(foundation::QueryInterface(pEventArgs,spMoEventArgs.GetAddressOf())))
        {
            TMoEventHandlerStorage::_TSenderType *pSender = nullptr;
            TCastToSender::CastTo(this,&pSender);
            this->FireEvent(pFireEventHandlerContainer,pSender,spMoEventArgs);
        }
        return S_OK;
    }
protected:
    HRESULT add_Handler( 
        TMoEventHandler *value,
        EventRegistrationToken *token)
    {
        IFR(this->AddHandlerInternal(value,token));
        IFR(this->_OnAdapterAddEventHandler(value));
        return S_OK;
    }
                    
    HRESULT remove_Handler( 
        EventRegistrationToken token)
    {
        foundation::ComPtr<TMoEventHandler> spMoEventHandler;
        IFR(this->RemoveHandlerInternal(token,spMoEventHandler.GetAddressOf()));
        IFR(this->_OnAdapterRemoveEventHandler(spMoEventHandler));
        return S_OK;
    }

};
