/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeCommand.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <foundation/type_info_statics_util.h>
#include <pmod/interfaces/command.h>
#include <pmod/eventhandler_util.h>

using namespace pmod;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Command_AddCanExecuteChanged(
	ICommand *pCommandModel, 
	_CallbackHandlerType pCallback,
	ICanExecuteChangedEventHandler **ppEventHandlder)
{
	return AttachCallbackHandler<ICanExecuteChangedEventHandler, CommandListenerBase>(pCommandModel, pCallback, ppEventHandlder);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Command_RemoveCanExecuteChanged(
	ICommand *pCommandModel,
	ICanExecuteChangedEventHandler *pEventHandlder)
{
	_IFR_(CommandListenerBase::DetachEventHandler(pCommandModel, pEventHandlder));
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Command_Execute(
	ICommand *pCommandModel,
	foundation::IInspectable *pParameter,
	foundation::IInspectable **ppResult)
{
	return pCommandModel->Execute(pParameter, ppResult);
}
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Command_CanExecute(
	ICommand *pCommandModel,
	foundation::IInspectable *pParameter,
	bool *pResult)
{
	return pCommandModel->CanExecute(pParameter, pResult);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Command_GetState(
	ICommand *pCommandModel,
	UINT32 *pState)
{
	return pCommandModel->GetState(pState);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Command_GetParameterTypeInfo(
	ICommand *pCommandModel,
	foundation::IPropertyTypeInfo **ppItemTypeInfo)
{
	foundation::ComPtr<ICommandInfo> typeInfo;
	_IFR_(foundation::GetObjectTypeInfo(pCommandModel, typeInfo.GetAddressOf()));
	return typeInfo->GetParameterTypeInfo(ppItemTypeInfo);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Command_GetResultTypeInfo(
	ICommand *pCommandModel,
	foundation::IPropertyTypeInfo **ppItemTypeInfo)
{
	foundation::ComPtr<ICommandInfo> typeInfo;
	_IFR_(foundation::GetObjectTypeInfo(pCommandModel, typeInfo.GetAddressOf()));
	return typeInfo->GetResultTypeInfo(ppItemTypeInfo);
}
