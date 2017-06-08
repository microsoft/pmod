/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeEnumerable.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <foundation/type_info_statics_util.h>
#include <foundation/interfaces/enumerable.h>

using namespace foundation;


EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Enumerable_GetItemTypeInfo(
    IObject *pEnumerable,
    foundation::IPropertyTypeInfo **ppItemTypeInfo)
{
    foundation::ComPtr<IEnumerableTypeInfo> objectTypeInfo;
    _IFR_(foundation::GetObjectTypeInfo(pEnumerable, objectTypeInfo.GetAddressOf()));
    return objectTypeInfo->GetItemTypeInfo(ppItemTypeInfo);
}

