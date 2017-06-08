/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionInfo.cpp
****/

#include "pch.h"

#include "ObservableCollectionInfo.h"
#include "ObservableCollectionClassInfoCache.h"
#include "TypeInfoFactory.h"

using namespace pmod;

HRESULT CCollectionModelTypeInfo::CreateInstance(
        _In_ const foundation::EnumerableTypeInfo *pCollectionModelInfo,
        _Outptr_ CCollectionModelTypeInfo **ppCollectionModelInfo)
{
    foundation_assert(pCollectionModelInfo);

    HRESULT hr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CCollectionModelTypeInfo>::CreateInstance(ppCollectionModelInfo));

    // Now Cache this new created Collection Model Info
    CCollectionModelClassInfoCache::AddToCache(pCollectionModelInfo->get_iid_type(), *ppCollectionModelInfo);

    // now we can initialize
    IFR_ASSERT(hr = (*ppCollectionModelInfo)->_Initialize(pCollectionModelInfo));
    return S_OK;
}
