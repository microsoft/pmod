/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationTypeInfoAdapter.h
****/

#pragma once

// Public Headers
#include <foundation/library/adapter_base.h>
#include <foundation/interfaces.h>


#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoAsyncOperationTypeInfo
//
// Purpose: Implementation of IAsyncOperationTypeInfo Interface
//
// History: virivera    02/12/2014    Created
//
//------------------------------------------------------------------------------
class CMoAsyncOperationTypeInfoAdapter :
	public foundation::AdapterBase
        <
		foundation::IAsyncOperationTypeInfo,
        IAsyncOperationTypeInfo,
        &__uuidof(IAsyncOperationTypeInfo)
        >
{
public:                    
  
    HRESULT STDMETHODCALLTYPE get_ResultTypeInfo(
        PropertyTypeInfo *value) override
    {
        IFCPTR_ASSERT(value);

        foundation::ComPtr<foundation::IPropertyTypeInfo> spItemTypeInfo;
        IFR_ASSERT(m_pInner->GetResultTypeInfo(spItemTypeInfo.GetAddressOf()));

        spItemTypeInfo->GetPropertyType(&value->PropertyType);

        foundation::ComPtr<foundation::IObjectTypeInfo> spObjectTypeInfo;
        spItemTypeInfo->GetTypeInfo(spObjectTypeInfo.GetAddressOf());
        if (spObjectTypeInfo != nullptr)
        {
            spObjectTypeInfo->GetType(&value->ObjectType);
        }
        else
        {
            value->ObjectType = GUID_NULL;
        }
        return S_OK;
    }
};
