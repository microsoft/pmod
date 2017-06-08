/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEventSourceProxy.h
****/

#pragma once

#include <foundation/assert.h>
#include <map>

#include "Microsoft.PropertyModel.h"
#include "MoAdapterProxyInterface.h"
#include <pmod/library/model_library.h>

template 
<
    class TCoreEventSource,
    class TCoreEventHandler,
    class TMoEventHandler,
    class TMoInterface,
    HRESULT  (STDMETHODCALLTYPE TMoInterface::*pAddHandler)(TMoEventHandler*,EventRegistrationToken*),
    HRESULT  (STDMETHODCALLTYPE TMoInterface::*pRemoveHandler)(EventRegistrationToken),
    class TCoreProxyClass
>
class TMoEventSourceProxy :
    public TAdapterProxyInterface
    <
        TCoreEventSource,
        TMoInterface
    >
{
protected:
    void FinalRelease() override
    {
        _registrationTokenMap.clear();
        __super::FinalRelease();
    }

    STDMETHOD(AddHandler)(TCoreEventHandler* pHandler)
    {
        foundation::ComPtr<TMoEventHandler> spMoHandler;
        HRESULT hr = TCoreProxyClass::CreateInstance<TCoreProxyClass>
            (pHandler,spMoHandler.GetAddressOf());
        IFR_ASSERT(hr);

        // Validate the Handler doesn't exist.
        if (_registrationTokenMap.find(pHandler) != _registrationTokenMap.end())
        {
            LOG_WARNING(U("EVENT HANDLER ALREADY EXIST!"), foundation::Errors::E_EVENT_HANDLER_EXIST);
            return foundation::Errors::E_EVENT_HANDLER_EXIST;
        }

        EventRegistrationToken token;
        hr = (this->m_pSourceInterface->*pAddHandler)(spMoHandler,&token);
        IFR(hr);
        _registrationTokenMap[pHandler] = token;
        return S_OK;
    }

    STDMETHOD(RemoveHandler)(TCoreEventHandler* pHandler)
    {
        _EventRegistrationTokenMapType::iterator iter = _registrationTokenMap.find(pHandler);
        if(iter != _registrationTokenMap.end())
        {
            HRESULT hr = (this->m_pSourceInterface->*pRemoveHandler)((*iter).second);
           
            _registrationTokenMap.erase(iter);

            // this case is specific we dont want to assert this specific error
            // because is expected in a clean up scenario when the client was
            // abruptly disconnected.
            if (hr == foundation::RPC_E_SERVER_NOT_AVAILABLE)
            {
                LOG_WARNING(U("RPC UNAVAILABLE ERROR!"), hr);
                return S_FALSE;
            }

            // default check for all other scenarios.
            IFR_ASSERT(hr);

            return S_OK;
        }
        else 
        {
            LOG_WARNING(U("EVENT HANDLER NOT EXIST!"), foundation::Errors::E_EVENT_HANDLER_NOT_FOUND);
        }

        return foundation::Errors::E_EVENT_HANDLER_NOT_FOUND;
    }

private:
    typedef std::map<foundation::ComPtr<TCoreEventHandler>,EventRegistrationToken> _EventRegistrationTokenMapType;
    
    _EventRegistrationTokenMapType _registrationTokenMap;
};
