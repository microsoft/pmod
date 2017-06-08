/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObjectDispatchAdapter.h
****/

#pragma once
#include "Microsoft.PropertyModel.h"
#include <foundation/library/adapter_base.h>
#include <foundation/interfaces/object_dispatch.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoObjectDispatchAdapter
//
// Purpose: Implementation of IObjectDispatch Interface for WinRT
//
//------------------------------------------------------------------------------


class CMoObjectDispatchAdapter :
	public foundation::AdapterBase
	<
	foundation::IObjectDispatch,
	IObjectDispatch,
	&__uuidof(IObjectDispatch)
	>
{
protected:
    // Interface IObjectDispatch for WinRT
    HRESULT STDMETHODCALLTYPE GetProperty(
        unsigned int propertyId,
        IInspectable **value) override;

    HRESULT STDMETHODCALLTYPE SetProperty(
        unsigned int propertyId,
        IInspectable *value) override;

    HRESULT STDMETHODCALLTYPE GetProperties(
        foundation::IInspectable *pProperties,
        UINT32 *size,
        __FIKeyValuePair_2_UINT32_IInspectable ***kvpProperties) override;
    HRESULT STDMETHODCALLTYPE Invoke(unsigned int methodId, UINT32 length, IInspectable **pParameters, IInspectable **result) override;
};
