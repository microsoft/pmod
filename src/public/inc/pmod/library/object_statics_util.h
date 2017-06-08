/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_statics_util.h
****/
#pragma once

#include <foundation/macros.h>
#include <foundation/com_ptr.h>
#include <foundation/interfaces/stream_provider.h>
#include <foundation/activation_factory.h>

#include "interfaces/object_statics.h"

namespace pmod {

    namespace library
    {
        static IObjectStatics *get_object_statics()
        {
            return foundation::GetActivationFactoryWithCast<IObjectStatics>(U("pmod.ObjectStatics"));
        }

        inline static bool is_object_equals(
            _In_opt_ foundation::IObject *pObject1,
            _In_opt_ foundation::IObject *pObject2)
        {
            bool result;
            IFRF_ASSERT(get_object_statics()->IsObjectEquals(pObject1, pObject2, &result));
            return result;
        }

        inline static HRESULT serialize_object(
            _In_ foundation::IStreamProvider *p_stream_provider,
            _In_ foundation::IInspectable *pInspectable,
            _In_opt_ IObjectSerializerCallback *pObjectSerializerCallback,
            _COM_Outptr_ foundation::IInspectable **ppSerializedValue
            )
        {
            IFCPTR_ASSERT(ppSerializedValue);
            *ppSerializedValue = nullptr;

            foundation::InspectablePtr object_ptr;
            IFR_ASSERT(get_object_statics()->ToObject(pInspectable, pObjectSerializerCallback, object_ptr.GetAddressOf()));

            IFCPTR_ASSERT(p_stream_provider);
            return p_stream_provider->Serialize(object_ptr, ppSerializedValue);
        }

        inline static HRESULT deserialize_object(
            _In_ foundation::IStreamProvider *p_stream_provider,
            _In_ foundation::IInspectable *pSerializedValue,
            _In_ IObjectDeserializerCallback *pObjectDeserializerCallback,
            _COM_Outptr_ foundation::IInspectable **ppInspectable)
        {
            IFCPTR_ASSERT(ppInspectable);
            *ppInspectable = nullptr;

            IFCPTR_ASSERT(p_stream_provider);
            foundation::InspectablePtr spObject;
            IFR_ASSERT(p_stream_provider->Deserialize(pSerializedValue, spObject.GetAddressOf()));

            return get_object_statics()->FromObject(spObject, pObjectDeserializerCallback, ppInspectable);
        }

        inline static HRESULT STDMETHODCALLTYPE serialize_hierarchy_object(
            _In_ foundation::IStreamProvider *p_stream_provider,
            _In_ foundation::IInspectable *pInspectable,
            _In_ IObjectSerializerCallback *pObjectSerializerCallback,
            _In_opt_ ISerializeReferenceCallback *pSerializeReferenceCallback,
            _COM_Outptr_ foundation::IInspectable **ppSerializedValue
            )
        {
            IFCPTR_ASSERT(ppSerializedValue);
            *ppSerializedValue = nullptr;

            foundation::InspectablePtr object_ptr;
            IFR_ASSERT(get_object_statics()->HierarchyToObject(
                pInspectable,
                pObjectSerializerCallback,
                pSerializeReferenceCallback,
                object_ptr.GetAddressOf()));

            IFCPTR_ASSERT(p_stream_provider);
            return p_stream_provider->Serialize(object_ptr, ppSerializedValue);
        }

        inline static HRESULT deserialize_hierarchy_object(
            _In_ foundation::IStreamProvider *p_stream_provider,
            _In_ foundation::IInspectable *pSerializedValue,
            _In_ IObjectDeserializerCallback *pObjectDeserializerCallback,
            _Out_ foundation::IInspectable **ppInspectable)
        {
            IFCPTR_ASSERT(p_stream_provider);

            foundation::InspectablePtr spObject;
            IFR_ASSERT(p_stream_provider->Deserialize(pSerializedValue, spObject.GetAddressOf()));
            return get_object_statics()->HierarchyFromObject(spObject, pObjectDeserializerCallback, ppInspectable);
        }

    };

}

