/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_dispatch_factory.h
****/
#pragma once

#include <foundation/interfaces/object_dispatch.h>
#include "object_dispatch_parameters.h"
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        //------------------------------------------------------------------------------
        // Interface:   IObjectDispatchFactory
        //
        // Purpose: An Interface to create IObjectDispatch instances
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IObjectDispatchFactory, foundation::IUnknown, FOUNDATION_LIB_API)
			STDMETHOD(CreateObjectDispatch)(
				_In_ const ObjectDispatchCreateParameters *pCreateParams,
				_In_opt_ foundation::IInspectable *pOuter,
				_COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
			STDMETHOD(CreatePropertyIdPair)(
				UINT32 propertyId,
				_In_opt_ foundation::IInspectable *pValue,
				_COM_Outptr_ IPropertyIdPair **ppPropertyIdPair) = 0;
		END_DECLARE_INTERFACE()
    }
}

