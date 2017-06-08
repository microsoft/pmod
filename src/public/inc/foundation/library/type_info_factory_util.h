/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_info_factory_util.h
****/
#pragma once

#include <foundation/macros.h>
#include "interfaces/type_info_factory.h"
#include <foundation/foundation_factory.h>
#include "property_type_info.h"

#if defined(PMOD_LIBRARY_STATIC_LIB)
EXTERN_C void _ModelTypeInfoInitializerStub();
#endif

namespace foundation {

    namespace library
    {
        inline library::ITypeInfoFactory *GetTypeInfoFactory()
        {
            // TODO: move this pmod dependency 
#if defined(PMOD_LIBRARY_STATIC_LIB)
            _ModelTypeInfoInitializerStub();
#endif
            return GetFoundationFactory<library::ITypeInfoFactory>(U("foundation.TypeInfo"));
        }

        inline HRESULT CreatePropertyTypeInfo(
            _In_ foundation::PropertyType type,
            _In_ GUID modelType,
            _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo)
        {
            IFCPTR_ASSERT(ppPropertyTypeInfo);
            return GetTypeInfoFactory()->CreatePropertyTypeInfo(type, modelType, ppPropertyTypeInfo);
        }

        inline HRESULT CreatePropertyTypeInfo(
            _In_ const PropertyTypeInfo *p_property_type_info,
            _COM_Outptr_ IPropertyTypeInfo **ppPropertyTypeInfo)
        {
            IFCPTR_ASSERT(p_property_type_info);

            return CreatePropertyTypeInfo(
                p_property_type_info->m_property_type,
                p_property_type_info->m_p_type_info != nullptr ?
                p_property_type_info->m_p_type_info->get_iid_type() :
                    foundation_GUID_NULL,
                ppPropertyTypeInfo);
        }

        inline HRESULT RegisterTypeInfo(_In_ const TypeInfo* pTypeInfoEntry)
        {
            return GetTypeInfoFactory()->RegisterTypeInfo(pTypeInfoEntry);
        }

        inline HRESULT RegisterMultipleTypeInfos(_In_ const TypeInfo* pTypeInfoEntries[])
        {
            return GetTypeInfoFactory()->RegisterMultipleTypeInfos(pTypeInfoEntries);
        }
    }

}

