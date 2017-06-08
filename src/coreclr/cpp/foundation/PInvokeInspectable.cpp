/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeInspectable.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"

#include <foundation/com_interfaces.h>
#include <foundation/macros.h>
#include <foundation/macros.h>


using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Inspectable_AddRef(foundation::IInspectable *pValue)
{
    if (pValue != nullptr)
    {
        (pValue)->AddRef();
    }
    return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Inspectable_Release(foundation::IInspectable *pValue)
{
	if (pValue != nullptr)
	{
		(pValue)->Release();
	}
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Inspectable_IsTypeOf(foundation::IInspectable *pValue, GUID *pIIdType, BYTE *result)
{
	*result = IsTypeOf(pValue, *pIIdType);
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Inspectable_CastTo(foundation::IInspectable *pValue, GUID *pIIdType, foundation::IInspectable **ppValue)
{
	return QueryInterface(pValue, *pIIdType, ppValue);
}
