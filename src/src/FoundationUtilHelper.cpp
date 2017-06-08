/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationUtilHelper.cpp
****/
#include "FoundationUtilHelper.h"
#include <assert.h>

foundation::string_t FormatNumber(const CHAR_t * _Format, ...)
{
    CHAR_t buffer[256];

    va_list argList;
    va_start(argList, _Format);     /* Initialize variable arguments. */
    pal::library::_Sprintf_helper(buffer, 256, _Format, argList);
    return buffer;
}

IID GetType(foundation::IInspectable *pInspectable)
{
    foundation::IIdsArrayWrapper IIdsArrayWrapper;
    pInspectable->GetIids(
        IIdsArrayWrapper.GetSizeAddressOf(),
        IIdsArrayWrapper.GetBufferAddressOf());
    assert(IIdsArrayWrapper.GetSize() != 0);
    return IIdsArrayWrapper[0];
}

HRESULT IsObjectEqualsInternal(foundation::IObject *pItem1, foundation::IObject *pItem2, bool *pResult)
{
    IFCPTR_ASSERT(pItem1);
    IFCPTR_ASSERT(pItem2);
    IFCPTR_ASSERT(pResult);

    *pResult = FALSE;

    UINT32 processId1, uniqueId1;
    UINT32 processId2, uniqueId2;

    IFR_ASSERT(pItem1->GetProcessId(&processId1));
    IFR_ASSERT(pItem2->GetProcessId(&processId2));
    if (processId1 != processId2)
    {
        return S_FALSE;
    }
    IFR_ASSERT(pItem1->GetUniqueId(&uniqueId1));
    IFR_ASSERT(pItem2->GetUniqueId(&uniqueId2));
    if (uniqueId1 != uniqueId2)
    {
        return S_FALSE;
    }
    *pResult = TRUE;
    return S_OK;
}
