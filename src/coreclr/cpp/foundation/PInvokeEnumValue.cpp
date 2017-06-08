/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeEnumValue.cpp
****/
#include "../pinvoke_api.h"

#include <foundation/interfaces/enum_value.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE EnumValue_GetValue(IEnumValue *pEnumValue, UINT32 *value)
{
	return pEnumValue->GetValue(value);
}
