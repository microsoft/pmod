/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BinaryStreamProvider.cpp
****/
#include "pch.h"

#include "BinaryStreamProvider.h"
#include <sstream>

#include <foundation/type_info_statics_util.h>
#include <foundation/library/logger_util.h>
#include <foundation/library/logger_macros.h>
#include <foundation/library/dictionary_class.h>

#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;
const UINT16 _propertyType_Null = (UINT16)0x10000;

EXTERN_C FOUNDATION_API HRESULT  STDAPICALLTYPE PropertyValue_Deserialize(
    std::istream& is,
    foundation::PropertyType type,
    foundation::IPropertyValue **ppPropertyValue);
EXTERN_C FOUNDATION_API HRESULT  STDAPICALLTYPE PropertyValue_Serialize(
    std::ostream& os,
    foundation::PropertyType type,
    foundation::IPropertyValue *pPropertyValue);

IStreamProvider *CBinaryStreamProvider::GetInstance()
{
    return library::_GetFactoryInstance<CBinaryStreamProvider,IStreamProvider>();
}

STDMETHODIMP CBinaryStreamProvider::Serialize(IInspectable *pValue, IInspectable **ppValue)
{
    std::stringstream os;
    binary_serializer bos(os);
    _IFR_(SerializeInspectable(bos, pValue));

    std::string buffer = os.str();
    return pv_util::CreateUInt8ArrayValue((UINT32)buffer.size(), (UINT8 *)buffer.data(), ppValue);
}

STDMETHODIMP CBinaryStreamProvider::Deserialize(IInspectable *pValue, IInspectable **ppValue)
{
    foundation::UInt8ArrayWrapper arrayWrapper;
    IFR_ASSERT(pv_util::GetUInt8Array(pValue, arrayWrapper.GetSizeAddressOf(), arrayWrapper.GetBufferAddressOf()));

    std::string buffer((const char *)arrayWrapper.GetBuffer(), arrayWrapper.GetSize());
    std::stringstream is(buffer);
    binary_deserializer bis(is);
    _IFR_(DeserializeInspectable(bis, ppValue));
    return S_OK;
}


// Helper to convert an Enum to a String
HRESULT CBinaryStreamProvider::SerializeEnum(binary_serializer& bos, IEnumValue *pEnumValue)
{
    foundation_assert(pEnumValue);

    foundation::ComPtr<IObjectTypeInfo> spEnumTypeInfo;
    IFR(pEnumValue->GetTypeInfo(spEnumTypeInfo.GetAddressOf()));
    UINT32 typeId;
    spEnumTypeInfo->GetTypeId(&typeId);

    bos.serializeUInt16((UINT16)typeId);

    UINT32 enumValue;
    IFR(pEnumValue->GetValue(&enumValue));
    bos.serializeUInt16((UINT16)enumValue);
    return S_OK;
}

HRESULT CBinaryStreamProvider::DeserializeEnum(binary_deserializer& bis, IEnumValue **ppEnumValue)
{
    foundation_assert(ppEnumValue);
    UINT32 enumTypeId = bis.deserializeUInt16();
    UINT32 enumValue = bis.deserializeUInt16();
    IFR(foundation::GetEnumValue(enumTypeId, enumValue, ppEnumValue));
    return S_OK;
}

HRESULT CBinaryStreamProvider::SerializeDictionary(binary_serializer& bos, IDictionary* pDictionary)
{
    foundation_assert(pDictionary);

    foundation::ArrayRefCountWrapper<foundation::IDictionaryPair> properties;
    IFR_ASSERT(pDictionary->GetView(properties.GetSizeAddressOf(),properties.GetBufferAddressOf()));

    // serialize Dictionary size
    bos.serializeUInt(properties.GetSize());

    for(UINT32 index = 0 ;index < properties.GetSize();++index)
    {
        foundation::HStringPtr key;
        IFR_ASSERT(properties[index]->GetKey(key.GetAddressOf()));

        foundation::InspectablePtr spValue;
        IFR_ASSERT(properties[index]->GetValue(spValue.GetAddressOf()));
        // now serialize key value pair
        bos.serializeString(key.GetRawBuffer(),key.GetLength());
        IFR(SerializeInspectable(bos, spValue));
    }
    return S_OK;
}

HRESULT CBinaryStreamProvider::SerializeInspectable(binary_serializer& bos, foundation::IInspectable* pInspectable)
{
    if(pInspectable == nullptr)
    {
        // TODO: we should find a better way to serialize a null value
        bos.serializeUInt16(_propertyType_Null);
        return S_OK;
    }

    foundation::PropertyType type;
    IF_ASSERT(pv_util::GetType(pInspectable,&type));

    // serialize type
    bos.serializeUInt16((UINT16)type);

    if(type == foundation::PropertyType_Inspectable)
    {
        ILogParameterPair* pLogParameterPair = nullptr;
        IDictionary *pDictionary;
        if(SUCCEEDED(foundation::QueryWeakReference(pInspectable,&pLogParameterPair)))
        {
            bos.serializeIID(IID_ILogParameterPair);
            IFR(SerializeLogParameter(bos,pLogParameterPair));
        }
        else if (SUCCEEDED(foundation::QueryWeakReference(pInspectable, &pDictionary)))
        {
            bos.serializeIID(IID_IDictionary);
            IFR(SerializeDictionary(bos, pDictionary));
        }
        else
        {
            bos.serializeString(U("[unknown object]"));
        }
    }
    else if(type == foundation::PropertyType_InspectableArray)
    {
        foundation::InspectableArrayWrapper inspectableArrayWrapper;
        IFR_ASSERT(pv_util::GetInspectableArray(
            pInspectable,
            inspectableArrayWrapper.GetSizeAddressOf(),
            inspectableArrayWrapper.GetBufferAddressOf()));

        // serialize Array Size
        bos.serializeUInt(inspectableArrayWrapper.GetSize());

        for(UINT32 index = 0;index < inspectableArrayWrapper.GetSize();++index)
        {
            foundation::IInspectable *pItem = inspectableArrayWrapper[index];
            IFR(SerializeInspectable(bos,pItem));
        }
    }
    else
    {
        IFR_ASSERT(PropertyValue_Serialize(
            bos.get_ostream(),
            type,
            pv_util::ToPropertyValuePtr(pInspectable)));
    }
    return S_OK;
}

HRESULT CBinaryStreamProvider::SerializeLogParameter(binary_serializer& bos, ILogParameterPair* pLogParameterPair)
{
    foundation_assert(pLogParameterPair);

    // Get Enum Key
    foundation::ComPtr<IEnumValue> spKey;
    IFR_ASSERT(pLogParameterPair->GetKey(spKey.GetAddressOf()));
    IFR(SerializeEnum(bos,spKey));

    // Get Logging Value
    foundation::ComPtr<foundation::IInspectable> spValue;
    IF_ASSERT(pLogParameterPair->GetValue(spValue.GetAddressOf()));

    if(spValue == nullptr)
    {
        pv_util::CreateEmptyValue(spValue.GetAddressOf());
    }
    IFR(SerializeInspectable(bos, spValue));
    return S_OK;
}

HRESULT CBinaryStreamProvider::DeserializeDictionary(binary_deserializer& bis, IDictionary** ppDictionary)
{
    foundation_assert(ppDictionary);

    foundation::ComPtr<IDictionary>                  spCoreDictionary;
    IFR_ASSERT(foundation::library::CreateDictionaryClass(spCoreDictionary.GetAddressOf()));

    // deserialize Dictionary size
    UINT32 size = bis.deserializeUInt();

    for(UINT32 index = 0 ;index < size;++index)
    {
        string_t key = bis.deserializeString();

        foundation::InspectablePtr spValue;
        IFR(DeserializeInspectable(bis,spValue.GetAddressOf()));

        bool bReplaced;
        IFR_ASSERT(spCoreDictionary->Insert(
            foundation::HStringRef(key.c_str()),
            spValue,
            &bReplaced));
    }
    *ppDictionary = spCoreDictionary.Detach();
    return S_OK;
}

HRESULT CBinaryStreamProvider::DeserializeInspectable(binary_deserializer& bis, foundation::IInspectable** ppInspectable)
{
    foundation_assert(ppInspectable != nullptr);
    
    // Get Type of Value
    foundation::PropertyType type = (foundation::PropertyType)bis.deserializeUInt16();
    if(type == _propertyType_Null)
    {
        *ppInspectable = nullptr;
    }
    else if(type == foundation::PropertyType_Inspectable)
    {
        IID iidType = bis.deserializeIID();
        if(iidType == IID_ILogParameterPair)
        {
            IFR(DeserializeLogParameter(bis,(ILogParameterPair**)ppInspectable));
        }
        else if(iidType == IID_IDictionary)
        {
            IFR(DeserializeDictionary(bis,(IDictionary**)ppInspectable));
        }
        else
        {
            IFR_ASSERT(E_NOTIMPL);
        }
    }
    else if(type == foundation::PropertyType_InspectableArray)
    {
        UINT32 size = bis.deserializeUInt();
        _Inspectable_ptr_vector_type values;
        values.resize(size);

        for(UINT32 index = 0;index < size;++index)
        {
            IFR(DeserializeInspectable(bis,values[index].GetAddressOf()));
        }
        IFR_ASSERT(pv_util::CreateInspectableArrayValue(
            size,
            size ? values.front().GetAddressOfPtr():nullptr,
            ppInspectable));
    }
    else
    {
        IFR_ASSERT(PropertyValue_Deserialize(
            bis.get_istream(),
            type,
            (foundation::IPropertyValue **)ppInspectable));
    }
    return S_OK;
}

HRESULT CBinaryStreamProvider::DeserializeLogParameter(binary_deserializer& bis, ILogParameterPair** ppLogParameterPair)
{
    // Get Enum Key
    foundation::ComPtr<IEnumValue> spKey;
    IFR(DeserializeEnum(bis,spKey.GetAddressOf()));

    // Get Value
    foundation::InspectablePtr spValue;
    IFR(DeserializeInspectable(bis, spValue.GetAddressOf()));

    IFR_ASSERT(logger_util::GetLoggerFactory()->CreateLogParameter(spKey, spValue, ppLogParameterPair));
    return S_OK;
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.BinaryStreamProvider"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CBinaryStreamProvider::GetInstance)
    );
