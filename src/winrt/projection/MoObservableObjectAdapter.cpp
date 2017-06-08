/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectAdapter.cpp
****/

#include "pch.h"

#include "MoObservableObjectAdapter.h"

#include "../moproxy/MoCoreProxyFactory.h"

HRESULT STDMETHODCALLTYPE CMoObservableObjectAdapter::add_EventModel(
    IEventModelEventHandler *value,
    EventRegistrationToken *token)
{
    IFR(this->_TEventModelEventHandler::AddHandlerInternal(value, token));
    IFR(this->_OnAdapterAddEventHandler(value));
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoObservableObjectAdapter::remove_EventModel(
    EventRegistrationToken token)
{
    foundation::ComPtr<IEventModelEventHandler> spMoEventModelEventHandler;
    IFR(this->_TEventModelEventHandler::RemoveHandlerInternal(token, spMoEventModelEventHandler.GetAddressOf()));
    IFR(this->_OnAdapterRemoveEventHandler(spMoEventModelEventHandler));
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoObservableObjectAdapter::FireAdapterEvent(
	_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
	_In_ IUnknown* pEventArgs)
{
    foundation_assert(pEventArgs);

    IInspectable *pSender = static_cast<foundation::ctl::ComInspectableBase *>(this);

    foundation::ComPtr<IEventModelEventArgs> spMoEventModelEventArgs;
    foundation::ComPtr<pmod::IBatchPropertyChangedEventArgs> spBatchPropertyChangedEventArgs;

    if (SUCCEEDED(foundation::QueryInterface(pEventArgs, spMoEventModelEventArgs.GetAddressOf())))
    {
        this->_TEventModelEventHandler::FireEvent(pFireEventHandlerContainer,pSender, spMoEventModelEventArgs);
        return S_OK;
    }
    else if (SUCCEEDED(foundation::QueryInterface(pEventArgs, spBatchPropertyChangedEventArgs.GetAddressOf())))
    {
        this->FireEventWithInvoke<MoPropertyChangedEventHandler>
            (pFireEventHandlerContainer,pSender, spBatchPropertyChangedEventArgs.Get());
        return S_OK;
    }
    return __super::FireAdapterEvent(pFireEventHandlerContainer,pEventArgs);
}

HRESULT STDMETHODCALLTYPE CMoPropertyChangedEventArgsAdapter::get_Property(
    unsigned int *propertyId)
{
    m_pInner->GetProperty(propertyId);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoPropertyChangedEventArgsAdapter::get_PropertyName(
    HSTRING *propertyName)
{
    m_pInner->GetPropertyName(propertyName);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoPropertyChangedEventArgsAdapter::get_OldValue(
    IInspectable **value)
{
    return m_pInner->GetOldValue(value);
}

HRESULT STDMETHODCALLTYPE CMoPropertyChangedEventArgsAdapter::get_NewValue(
    IInspectable **value)
{
    return m_pInner->GetNewValue(value);
}

HRESULT STDMETHODCALLTYPE CMoEventModelEventArgsAdapter::get_Event(
    unsigned int *eventId)
{
    UINT32 _eventId;
    m_pInner->GetEvent(&_eventId);
    *eventId = (int)_eventId;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoEventModelEventArgsAdapter::get_EventArgs(
    IInspectable **value)
{
    return m_pInner->GetEventArgs(value);
}


HRESULT STDMETHODCALLTYPE CMoBatchPropertyChangedEventArgsAdapter::get_PropertyChanges(
    UINT32 *size,
    ABI::Microsoft::PropertyModel::IPropertyChangedEventArgs ***propertyChanges)
{
    foundation::ArrayRefCountWrapper<pmod::IPropertyChangedEventArgs> propertyChangedEventArgsArray;
    IFR_ASSERT(m_pInner->GetPropertyChanges(
        propertyChangedEventArgsArray.GetSizeAddressOf(),
        propertyChangedEventArgsArray.GetBufferAddressOf()));

    std::vector<foundation::ComPtr<ABI::Microsoft::PropertyModel::IPropertyChangedEventArgs>> moPropertyChangedEventArgsVector;
    moPropertyChangedEventArgsVector.resize(propertyChangedEventArgsArray.GetSize());

    for (UINT32 index = 0; index < propertyChangedEventArgsArray.GetSize(); ++index)
    {
        IFR_ASSERT(foundation::QueryInterface(
            propertyChangedEventArgsArray[index],
            moPropertyChangedEventArgsVector[index].GetAddressOf()
            ));
    }
    ContainerRefCountToArray(moPropertyChangedEventArgsVector, size, propertyChanges);
    return S_OK;
}

