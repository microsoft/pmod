/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeObjectDispatch.cpp
****/
#include "../pinvoke_api.h"

#include <foundation/interfaces/object_dispatch.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObjectDispatch_GetProperty(IObjectDispatch *pObjectDispatch, UINT32 propertyId, foundation::IInspectable **ppValue)
{
	return pObjectDispatch->GetProperty(propertyId, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObjectDispatch_SetProperty(IObjectDispatch *pObjectDispatch, UINT32 propertyId, foundation::IInspectable *pValue)
{
	return pObjectDispatch->SetProperty(propertyId, pValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObjectDispatch_Invoke(
    IObjectDispatch *pObjectDispatch,
    UINT32 methodId,
    UINT32 size, foundation::IInspectable **pParameters,
    foundation::IInspectable **ppResult)
{
    return pObjectDispatch->InvokeMethod(methodId, size, pParameters, ppResult);
}
