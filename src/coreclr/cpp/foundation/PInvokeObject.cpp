/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeObject.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"

#include <foundation/interfaces/object.h>
#include <foundation/type_info_statics_util.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Object_GetUniqueId(foundation::IObject *pObject, UINT32 *pUniqueId)
{
	return pObject->GetUniqueId(pUniqueId);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Object_GetProcessId(foundation::IObject *pObject, UINT32 *pProcessId)
{
	return pObject->GetProcessId(pProcessId);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Object_GetTypeInfo(foundation::IObject *pObject, foundation::IObjectTypeInfo **ppObjectTypeInfo)
{
	return pObject->GetTypeInfo(ppObjectTypeInfo);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Object_GetType(foundation::IObject *pObject, IID *pType)
{
	IID iidType;
	_IFR_(GetObjectType(pObject, iidType));
    *pType = iidType;
    return S_OK;
}
