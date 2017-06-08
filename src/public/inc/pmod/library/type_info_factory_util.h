/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_info_factory_util.h
****/
#pragma once

#include "interfaces/type_info_factory.h"
#include <foundation/library/interfaces/type_info_factory.h>
#include <foundation/activation_factory.h>

namespace pmod {

    namespace library
    {
		inline IModelTypeInfoFactory *GetModelTypeInfoFactory()
		{
			return foundation::GetActivationFactoryWithCast<IModelTypeInfoFactory>(U("pmod.TypeInfoFactory"));
		}

		inline foundation::library::ITypeInfoFactory *GetTypeInfoFactory()
        {
			return foundation::CastTo<foundation::library::ITypeInfoFactory>(GetModelTypeInfoFactory());
        }

        inline HRESULT RegisterTypeInfo(_In_ const foundation::TypeInfo* pTypeInfoEntry)
        {
            return GetTypeInfoFactory()->RegisterTypeInfo(pTypeInfoEntry);
        }

        inline HRESULT RegisterMultipleTypeInfos(_In_ const foundation::TypeInfo* pTypeInfoEntries[])
        {
            return GetTypeInfoFactory()->RegisterMultipleTypeInfos(pTypeInfoEntries);
        }
    }

}

