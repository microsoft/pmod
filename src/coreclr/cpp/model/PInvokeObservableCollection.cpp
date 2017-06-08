/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeObservableCollection.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <foundation/type_info_statics_util.h>
#include <pmod/interfaces/observable_collection.h>
#include <pmod/eventhandler_util.h>

using namespace pmod;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableCollection_AddNotifyCollectionChanged(
	IObservableCollection *pObservableCollection, 
	_CallbackHandlerType pCallback,
	INotifyCollectionChangedEventHandler **ppEventHandlder)
{
	return AttachCallbackHandler<INotifyCollectionChangedEventHandler, ObservableCollectionListenerBase>(pObservableCollection, pCallback, ppEventHandlder);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableCollection_RemoveNotifyCollectionChanged(
	IObservableCollection *pObservableCollection,
	INotifyCollectionChangedEventHandler *pEventHandlder)
{
	_IFR_(ObservableCollectionListenerBase::DetachEventHandler(pObservableCollection, pEventHandlder));
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableCollection_GetCount(
	IObservableCollection *pObservableCollection,
	UINT32 *pValue)
{
	return pObservableCollection->GetCount(pValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableCollection_GetItem(
	IObservableCollection *pObservableCollection,
	UINT32 index,
	foundation::IInspectable **ppValue)
{
	return pObservableCollection->GetItem(index, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableCollection_GetItems(
	IObservableCollection *pObservableCollection,
	UINT32 *size, foundation::IInspectable ***values)
{
	return pObservableCollection->GetItems(size, values);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableCollection_IndexOf(
	IObservableCollection *pObservableCollection,
	foundation::IInspectable *pValue,
	UINT32 *pIndex)
{
	return pObservableCollection->IndexOf(pValue, pIndex);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObservableCollection_GetItemTypeInfo(
	IObservableCollection *pObservableCollection,
	foundation::IPropertyTypeInfo **ppItemTypeInfo)
{
	foundation::ComPtr<foundation::IEnumerableTypeInfo> objectTypeInfo;
	_IFR_(foundation::GetObjectTypeInfo(pObservableCollection, objectTypeInfo.GetAddressOf()));
	return objectTypeInfo->GetItemTypeInfo(ppItemTypeInfo);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE NotifyCollectionChangedEventArgs_GetAction(
	INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs,
	UINT32 *value)
{
	foundation::NotifyCollectionModelChangedAction action;
	_IFR_(pNotifyCollectionChangedEventArgs->GetAction(&action));
	*value = (UINT32)action;
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE NotifyCollectionChangedEventArgs_GetOldStartingIndex(
	INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs,
	UINT32 *value)
{
	return pNotifyCollectionChangedEventArgs->GetOldStartingIndex(value);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE NotifyCollectionChangedEventArgs_GetNewStartingIndex(
	INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs,
	UINT32 *value)
{
	return pNotifyCollectionChangedEventArgs->GetNewStartingIndex(value);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE NotifyCollectionChangedEventArgs_GetNewItems(
	INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs,
	UINT32 *size, foundation::IInspectable ***values)
{
	return pNotifyCollectionChangedEventArgs->GetNewItems(size, values);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE NotifyCollectionChangedEventArgs_GetOldItems(
	INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs,
	UINT32 *size, foundation::IInspectable ***values)
{
	return pNotifyCollectionChangedEventArgs->GetOldItems(size, values);
}
