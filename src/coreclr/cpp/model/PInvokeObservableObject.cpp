/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeObservableObject.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <pmod/interfaces/observable_object.h>
#include <pmod/eventhandler_util.h>

using namespace pmod;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableObject_AddNotifyPropertyChanged(
	IObservableObject *pObservableObject, 
	_CallbackHandlerType pCallback,
	IPropertyChangedEventHandler **ppEventHandlder)
{
	return AttachCallbackHandler<IPropertyChangedEventHandler, ObservableObjectListenerBase>(pObservableObject, pCallback, ppEventHandlder);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableObject_RemoveNotifyPropertyChanged(
	IObservableObject *pObservableObject,
	IPropertyChangedEventHandler *pEventHandlder)
{
	_IFR_(ObservableObjectListenerBase::DetachEventHandler(pObservableObject, pEventHandlder));
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableObject_AddEventModelEvent(
	IObservableObject *pObservableObject,
	_CallbackHandlerType pCallback,
	IEventModelEventHandler **ppEventHandlder)
{
	return AttachCallbackHandler<IEventModelEventHandler, EventModelListenerBase>(pObservableObject, pCallback, ppEventHandlder);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableObject_RemoveEventModelEvent(
	IObservableObject *pObservableObject,
	IPropertyChangedEventHandler *pEventHandlder)
{
	_IFR_(EventModelListenerBase::DetachEventHandler(pObservableObject, pEventHandlder));
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyChangedEventArgs_GetProperty(
	IPropertyChangedEventArgs *pPropertyChangedEventArgs,
	UINT32 *pValue)
{
	return pPropertyChangedEventArgs->GetProperty(pValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyChangedEventArgs_GetOldValue(
	IPropertyChangedEventArgs *pPropertyChangedEventArgs,
	foundation::IInspectable **ppValue)
{
	return pPropertyChangedEventArgs->GetOldValue(ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyChangedEventArgs_GetNewValue(
	IPropertyChangedEventArgs *pPropertyChangedEventArgs,
	foundation::IInspectable **ppValue)
{
	return pPropertyChangedEventArgs->GetNewValue(ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyChangedEventArgs_GetPropertyName(
	IPropertyChangedEventArgs *pPropertyChangedEventArgs,
	CHAR_t **values)
{
	foundation::HStringPtr propertyName;
	_IFR_(pPropertyChangedEventArgs->GetPropertyName(propertyName.GetAddressOf()));
	return ReturnString(propertyName, values);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE EventModelEventArgs_GetEvent(
	IEventModelEventArgs *pEventModelEventArgs,UINT32 *pValue)
{
	return pEventModelEventArgs->GetEvent(pValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE EventModelEventArgs_GetEventArgs(
	IEventModelEventArgs *pEventModelEventArgs, foundation::IInspectable **ppEventArgs)
{
	return pEventModelEventArgs->GetEventArgs(ppEventArgs);
}
