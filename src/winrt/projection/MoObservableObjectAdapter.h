/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectAdapter.h
****/

#pragma once
#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"
#include "..\..\model_library\BatchPropertyChangedHelper.h"
#include <pmod/library/property_changed_event_args_impl.h>

using namespace ABI::Microsoft::PropertyModel;

typedef PropertyChangedEventHandlerBase
<
    MoEventHandler
    <
        IPropertyChangedEventHandler,
        IInspectable,
        IPropertyChangedEventArgs
    >,
    IPropertyChangedEventHandler,
    IInspectable,
    IPropertyChangedEventArgs,
    IBatchPropertyChangedEventHandler,
    IBatchPropertyChangedEventArgs
> MoPropertyChangedEventHandler;

//------------------------------------------------------------------------------
// Class:   CMoObservableObjectAdapter
//
// Purpose: Implementation of IObservableObject Interface for WinRT
//
//------------------------------------------------------------------------------
typedef MoEventHandler
<
IEventModelEventHandler,
IInspectable,
IEventModelEventArgs
> _TEventModelEventHandler;

class CMoObservableObjectAdapter :
    public CMoBaseAdapter
    <
        pmod::IObservableObject,
        IObservableObject,
        IPropertyChangedEventHandler,
        IPropertyChangedEventArgs,
        MoPropertyChangedEventHandler
    >,
    public _TEventModelEventHandler
{
protected:
    // Interface IObservableObject for WinRT

    HRESULT STDMETHODCALLTYPE add_PropertyChanged(
        IPropertyChangedEventHandler *value,
        EventRegistrationToken *token) override
    {
        return this->add_Handler(value, token);
    }

    HRESULT STDMETHODCALLTYPE remove_PropertyChanged(
        EventRegistrationToken token) override
    {
        return this->remove_Handler(token);
    }

    STDMETHOD(HasEventHandlers)(_Outptr_ bool *pValue) override
    {
        if (this->_TEventModelEventHandler::GetEventHandlers().size() > 0)
        {
            *pValue = true;
            return S_OK;
        }
        return __super::HasEventHandlers(pValue);
    }

    HRESULT STDMETHODCALLTYPE add_EventModel(
        IEventModelEventHandler *value,
        EventRegistrationToken *token) override;

    HRESULT STDMETHODCALLTYPE remove_EventModel(
        EventRegistrationToken token) override;

    // Interface IModelAdapter
    STDMETHOD(FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		_In_ IUnknown* pEventArgs) override;

};

//------------------------------------------------------------------------------
// Class:   CMoPropertyChangedEventArgsAdapter
//
// Purpose: Implementation of IPropertyChangedEventArgs Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoPropertyChangedEventArgsAdapter :
    public foundation::AdapterBase <
    pmod::IPropertyChangedEventArgs,
    IPropertyChangedEventArgs,
    &__uuidof(IPropertyChangedEventArgs)
    >
{
protected:
    // Interface IPropertyChangedEventArgs for WinRT
    HRESULT STDMETHODCALLTYPE get_Property(
        unsigned int *propertyId) override;

    HRESULT STDMETHODCALLTYPE get_PropertyName(
        HSTRING *propertyName) override;

    HRESULT STDMETHODCALLTYPE get_OldValue(
        IInspectable **value) override;

    HRESULT STDMETHODCALLTYPE get_NewValue(
        IInspectable **value) override;
};
//------------------------------------------------------------------------------
// Class:   CMoBatchPropertyChangedEventArgsAdapter
//
// Purpose: Implementation of IBatchPropertyChangedEventArgs Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoBatchPropertyChangedEventArgsAdapter :
    public foundation::AdapterBase <
    pmod::IBatchPropertyChangedEventArgs,
    IBatchPropertyChangedEventArgs,
    &__uuidof(IBatchPropertyChangedEventArgs)
    >
{
protected:
    // Interface IBatchPropertyChangedEventArgs for WinRT
    HRESULT STDMETHODCALLTYPE get_PropertyChanges(
        UINT32 *size,
        ABI::Microsoft::PropertyModel::IPropertyChangedEventArgs ***propertyChanges) override;
};
//------------------------------------------------------------------------------
// Class:   CMoPropertyChangedEventWithCallbackArgsAdapter
//
// Purpose: Implementation of IPropertyChangedEventWithCallbackArgs Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoPropertyChangedEventWithCallbackArgsAdapter :
    public foundation::AdapterBase <
    pmod::library::IPropertyChangedWithCallbackEventArgs,
    IPropertyChangedWithCallbackEventArgs,
    &__uuidof(IPropertyChangedWithCallbackEventArgs)
    >
{
protected:
};
//------------------------------------------------------------------------------
// Class:   CMoEventModelEventArgsAdapter
//
// Purpose: Implementation of IEventModelEventArgs Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoEventModelEventArgsAdapter :
    public foundation::AdapterBase <
        pmod::IEventModelEventArgs,
        IEventModelEventArgs,
        &__uuidof(IEventModelEventArgs)
    >
{
protected:
    // Interface IEventModelEventArgs for WinRT
    HRESULT STDMETHODCALLTYPE get_Event(
        unsigned int *eventId) override;

    HRESULT STDMETHODCALLTYPE get_EventArgs(
        IInspectable **value) override;
};
