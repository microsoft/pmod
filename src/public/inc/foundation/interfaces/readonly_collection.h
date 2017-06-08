/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:readonly_collection.h
****/
#pragma once

#include "enumerable.h"

namespace foundation {

    ////////////////////// IReadOnlyCollection declare ////////////////////////////
    template <class T,const IID& iidType, class TBASE = foundation::IObject>
    struct IReadOnlyCollection_impl : public IEnumerable_impl<T, iidType, TBASE>
    {
    public:
        STDMETHOD(GetCount)(_Out_ UINT32* pCount) = 0;
        STDMETHOD(GetItem)(UINT32 index, T *pItemValue) = 0;
    };
}

