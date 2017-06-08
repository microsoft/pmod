/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelSerializerAdapters.cpp
****/
#include "pch.h"


#include <foundation/type_info_statics_util.h>
#include <foundation/interfaces/object_serializer.h>
#include <foundation/interfaces/stream_provider.h>
#include <foundation/library/adapter_base.h>
#include <foundation/activation_factory.h>
#include <foundation/dictionary_ptr.h>
#include <foundation/library/dictionary_class.h>
#include <foundation/library/interface_adapter_util.h>

#include <pmod/interfaces/observable_object.h>

using namespace pmod;

//------------------------------------------------------------------------------
// Class:   CObservableObjectSerializeAdapter
//
// Purpose: Adapter base class to host a IObservableObject instance in an adapter
//
//------------------------------------------------------------------------------
class CObservableObjectSerializeAdapter : public 
    foundation::AdapterBase
    <
        IObservableObject,
        foundation::IObjectSerializer,
        &foundation::IID_IObjectSerializer
    >
{
protected:
    STDMETHOD(GetContent)(HSTRING serializerActivationId, IInspectable *pContentOptions, IInspectable **ppSerializedContent) override
    {
        foundation::IStreamProvider *p_stream_provider = static_cast<foundation::IStreamProvider *>(foundation::GetActivationFactoryInstance(
            _pal_GetStringRawBuffer(serializerActivationId, nullptr)));
        IFCPTR_ASSERT(p_stream_provider);
        foundation::InspectablePtr valuePtr;
        IFR_ASSERT(p_stream_provider->Deserialize(pContentOptions, valuePtr.GetAddressOf()));
        foundation::DictionaryPtr options_ptr;
        IFR_ASSERT(foundation::QueryInterface(valuePtr, options_ptr.GetAddressOf()));

        foundation::ComPtr<IObservableObjectInfo> type_info_ptr;
        IFR_ASSERT(foundation::GetObjectTypeInfo(this->GetInner(), type_info_ptr.GetAddressOf()));

        IFR_ASSERT(options_ptr.GetValuePtr(U("properties"), valuePtr));

        foundation::InspectablePtr get_properties_options;

        foundation::PropertyType propertyType;
        foundation::pv_util::GetType(valuePtr, &propertyType);

        if ((propertyType & 0x400))
        {
            // we expect an array of property names
            foundation::HStringArrayWrapper properties_ptr;
            IFR_ASSERT(foundation::GetValueAs(valuePtr,properties_ptr));


            std::vector<UINT32> properties_id;
            properties_id.resize(properties_ptr.GetSize());

            for (UINT32 index = 0; index < properties_ptr.GetSize(); ++index)
            {
                foundation::ComPtr<foundation::IPropertyInfo> property_info_ptr;
                IFR_ASSERT(type_info_ptr->GetPropertyByName(properties_ptr[index], property_info_ptr.GetAddressOf()));
                UINT32 propertyId;
                property_info_ptr->GetId(&propertyId);
                properties_id[index] = propertyId;
            }
            get_properties_options = foundation::CreateValueFromVector(properties_id);
        }
        else
        {
            // this will enforce the use of InstanceFlags enum
            get_properties_options = valuePtr;
        }

        // proceed with invoking our 'Inner' model to get the properties
        foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> properties_values;
        IFR_ASSERT(this->GetInner()->GetProperties(
            get_properties_options,
            properties_values.GetSizeAddressOf(),
            properties_values.GetBufferAddressOf()));

        // convert IPropertyIdPair arrays into a IDictionary
        foundation::DictionaryPtr dictionary_properties_values;
        IFR_ASSERT(foundation::library::CreateDictionaryClass(dictionary_properties_values.GetAddressOf()));

        for (UINT32 index = 0; index < properties_values.GetSize(); ++index)
        {
            UINT32 propertyId;
            properties_values[index]->GetKey(&propertyId);
            properties_values[index]->GetValue(valuePtr.ReleaseAndGetAddressOf());

            foundation::ComPtr<foundation::IPropertyInfo> property_info_ptr;
            IFR_ASSERT(type_info_ptr->GetPropertyById(propertyId, property_info_ptr.GetAddressOf()));
            foundation::HStringPtr name;
            property_info_ptr->GetName(name.GetAddressOf());
            dictionary_properties_values.SetValue(name.GetRawBuffer(), valuePtr);
        }
        // serialize the result
        IFR_ASSERT(p_stream_provider->Serialize(dictionary_properties_values,ppSerializedContent));
        return S_OK;
    }

    STDMETHOD(SetContent)(HSTRING serializerActivationId, IInspectable *pSerializedContent) override
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }

};

HRESULT RegisterFactoryModelSerializerAdapters(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    foundation_assert(pGenericAdapterFactory != nullptr);

    // IID_IEnumValue Adapter
    pGenericAdapterFactory->AddAdapterEntry(
        pmod::IID_IObservableObject,
        foundation::IID_IObjectSerializer,
        pmod::IID_IObservableObject,
        foundation::CreateAdapter<CObservableObjectSerializeAdapter>);

    return S_OK;
}
