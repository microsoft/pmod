/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokePropertyTypeInfo.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"

#include <foundation/interfaces/property_type_info.h>
#include <foundation/macros.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyTypeInfo_GetPropertyType(IPropertyTypeInfo *pPropertyTypeInfo, UINT32 *value)
{
	PropertyType propertyType;
	pPropertyTypeInfo->GetPropertyType(&propertyType);
	*value = propertyType;
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyTypeInfo_GetTypeInfo(IPropertyTypeInfo *pPropertyTypeInfo, IObjectTypeInfo **value)
{
	return pPropertyTypeInfo->GetTypeInfo(value);
}
