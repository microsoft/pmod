/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObjectDispatchFactory.h
****/

#pragma once

#include <foundation/library/library_util.h>
#include <winrt/com_activation.h>
#include "MoModelFactory.h"

#include "Microsoft.PropertyModel.Library.h"

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   MoObjectDispatchFactory
//
// Purpose: Factory class for a PropertyModel
//
//------------------------------------------------------------------------------
class MoObjectDispatchFactory :
	public MoModelFactoryBase<IObjectDispatchFactory>
{
public:
	HRESULT STDMETHODCALLTYPE CreateInstance(
		GUID iidType,
		ABI::Microsoft::PropertyModel::Library::ObjectDispatchOptions options,
		ABI::Microsoft::PropertyModel::IObjectDispatch **instance)
	{
		return foundation::library::CreateObjectDispatch(
			iidType,
			(foundation::ObjectDispatchOptions)options,
			instance);
	}
};
