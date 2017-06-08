/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeObjectTypeInfo.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"

#include <foundation/interfaces/object.h>
#include <foundation/macros.h>
#include <foundation/hstring_wrapper.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObjectTypeInfo_GetTypeId(foundation::IObjectTypeInfo *pObjectTypeInfo, UINT32 *pTypeId)
{
	return pObjectTypeInfo->GetTypeId(pTypeId);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObjectTypeInfo_GetType(
	foundation::IObjectTypeInfo *pObjectTypeInfo, 
	IID *pType)
{
    return pObjectTypeInfo->GetType(pType);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObjectTypeInfo_GetName(foundation::IObjectTypeInfo *pObjectTypeInfo, CHAR_t **values)
{
    HStringPtr hstr;
    _IFR_(pObjectTypeInfo->GetName(hstr.GetAddressOf()));
    return ReturnString(hstr, values);
}
