/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:persistant_storage_service.h
****/
#pragma once

#include <foundation/interfaces/object_dispatch.h>
#include <foundation/interfaces/object_dispatch_info.h>
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        // IPersistantStorageService Interface
        BEGIN_DECLARE_INTERFACE(IPersistantStorageService, foundation::IInspectable, FOUNDATION_LIB_API)
        STDMETHOD(RegisterTypeProperties)(
            _In_ IObjectDispatchInfo *pPropertyDataInfo,
            UINT32 size,
            _In_ UINT32 *pProperties,
            foundation::PropertyType key_column_type) = 0;

        STDMETHOD(GetTypeProperties)(
            _In_ IObjectDispatchInfo *pPropertyDataInfo,
            UINT32 *pSize,
            _In_ UINT32 **ppProperties) = 0;

        STDMETHOD(LoadProperties)(
            _In_ IObjectDispatchInfo *pPropertyDataInfo,
            UINT32 size,
            UINT32 *pProperties,
            _In_ foundation::IInspectable *pKeyValue,
            UINT32 *pSizeProperties,
            foundation::IPropertyIdPair ***ppProperties
            ) = 0;

        STDMETHOD(UpdateProperties)(
            _In_ IObjectDispatchInfo *pPropertyDataInfo,
            UINT32 size,
            _In_ foundation::IPropertyIdPair **ppProperties,
            foundation::IInspectable *pKeyValue) = 0;

        END_DECLARE_INTERFACE()

    }
}

