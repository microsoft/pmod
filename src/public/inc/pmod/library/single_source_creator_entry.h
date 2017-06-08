/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:single_source_creator_entry.h
****/
#pragma once

#include <pmod/interfaces/source_model.h>

namespace pmod {

    // Single Source Model Creator Callback
    typedef HRESULT(STDMETHODCALLTYPE *CreateSingleSourceModelCallback)(
        _In_ foundation::IObject *pSource,                                // the 'Source' IObjectNode
        _COM_Outptr_ ISourceModel **ppNewInstance);     // the new Source Model instance

    struct SingleSourceCreatorEntry
    {
    public:
#ifdef _MSC_VER
        SingleSourceCreatorEntry(
            const IID& iidType,
            CreateSingleSourceModelCallback callback
            ) :
            SourceModelIIdType(iidType),
            CreatorCallback(callback)
        {}

        SingleSourceCreatorEntry() :
            SourceModelIIdType(foundation_GUID_NULL),
            CreatorCallback(nullptr)
        {}

        SingleSourceCreatorEntry & operator=(const SingleSourceCreatorEntry &) {}
#endif

        // IID of the Source Model IID Type
        const IID                               SourceModelIIdType;
        // the Creator Method
        CreateSingleSourceModelCallback         CreatorCallback;
    };

}

