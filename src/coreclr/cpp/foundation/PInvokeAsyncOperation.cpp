/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeAsyncOperation.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <foundation/type_info_statics_util.h>
#include <foundation/interfaces/async_operation.h>
#include <foundation/async_operation_connection.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE AsyncOperation_AddCompleted(
	IAsyncOperation *pAsyncOperation,
	_CallbackHandlerType pCallback,
	ICompletedEventHandler **ppEventHandlder)
{
	return AttachCallbackHandler<ICompletedEventHandler, AsyncOperationCompletedListenerBase>(pAsyncOperation, pCallback, ppEventHandlder);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE AsyncOperation_RemoveCompleted(
	IAsyncOperation *pAsyncOperation,
	ICompletedEventHandler *pEventHandlder)
{
	_IFR_(AsyncOperationCompletedListenerBase::DetachEventHandler(pAsyncOperation, pEventHandlder));
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE AsyncOperation_GetStatus(
	IAsyncOperation *pAsyncOperation,
	UINT32 *value)
{
	ResultStatus status;
	_IFR_(pAsyncOperation->GetStatus(&status));
	*value = (UINT32)status;
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE AsyncOperation_GetResults(
	IAsyncOperation *pAsyncOperation,
	foundation::IInspectable **ppResults)
{
	return pAsyncOperation->GetResult(foundation::_InfiniteTimeout, ppResults);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE AsyncOperation_GetResultTypeInfo(
	IAsyncOperation *pAsyncOperation,
	foundation::IPropertyTypeInfo **ppItemTypeInfo)
{
	foundation::ComPtr<IAsyncOperationTypeInfo> typeInfo;
	_IFR_(foundation::GetObjectTypeInfo(pAsyncOperation, typeInfo.GetAddressOf()));
	return typeInfo->GetResultTypeInfo(ppItemTypeInfo);
}
