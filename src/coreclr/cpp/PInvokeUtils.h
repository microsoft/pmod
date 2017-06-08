/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeUtils.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/ctl/com_library.h>

HRESULT EnsureLibraryPackage();
HRESULT ReturnString(HSTRING hstr, CHAR_t **values);

typedef HRESULT(STDAPICALLTYPE *_CallbackHandlerType)(foundation::IUnknown *pEventArgs);

template <class TEventHandler>
class EventHandlerAdapter :
    public foundation::ctl::ComBase,
    public TEventHandler
{
public:
    HRESULT _Initialize(_CallbackHandlerType pCallback)
    {
        this->_pCallback = pCallback;
        return S_OK;
    }
protected:
    EventHandlerAdapter():
        _pCallback(nullptr)
    {}
    void FinalRelease()
    {}
    STDMETHOD(Invoke)(foundation::IUnknown *pSender, typename TEventHandler::_EventArgs_Type *pEventArgs)
    {
        foundation_assert(_pCallback);
        pEventArgs->AddRef();
        return (*_pCallback)(pEventArgs);
    }
private:
    _CallbackHandlerType _pCallback;
};

template <class TEventHandler, class TAttachImpl, class TSource>
HRESULT AttachCallbackHandler(
    TSource *pSource,
    _CallbackHandlerType pCallback,
    TEventHandler **ppEventHandlder)
{
    EventHandlerAdapter<TEventHandler> *pT;
    foundation::ctl::CreateInstanceWithInitialize(&pT, pCallback);
    _IFR_(TAttachImpl::AttachEventHandler(pSource, pT));
    *ppEventHandlder = pT;
    return S_OK;
}
