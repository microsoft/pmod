/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BinaryStreamProvider.h
****/
#pragma once

#include <foundation/library/base_adapter_host.h>
#include "SerializeUtil.h"
#include <foundation/ctl/com_library.h>
#include <foundation/interfaces/stream_provider.h>
#include <foundation/interfaces/enum_value.h>
#include <foundation/interfaces/logger.h>

typedef foundation::library::_DefaultAdapterHost<foundation::IStreamProvider, foundation::ctl::ComInspectableBase> _CBinaryStreamProvider_base;

class CBinaryStreamProvider :
    public _CBinaryStreamProvider_base,
    public foundation::IStreamProvider
{
public:
    static foundation::IStreamProvider *GetInstance();

    static HRESULT SerializeEnum(binary_serializer& bos, foundation::IEnumValue *pEnumValue);
    static HRESULT SerializeInspectable(binary_serializer& bos, foundation::IInspectable* pInspectable);
    static HRESULT SerializeLogParameter(binary_serializer& bos, foundation::ILogParameterPair* pLogParameterPair);
    static HRESULT SerializeDictionary(binary_serializer& bos, foundation::IDictionary* pDictionary);

    static HRESULT DeserializeEnum(binary_deserializer& bis,foundation::IEnumValue **ppEnumValue);
    static HRESULT DeserializeInspectable(binary_deserializer& bis, foundation::IInspectable** ppInspectable);
    static HRESULT DeserializeLogParameter(binary_deserializer& bis, foundation::ILogParameterPair** ppLogParameterPair);
    static HRESULT DeserializeDictionary(binary_deserializer& bis, foundation::IDictionary** ppDictionary);
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::IStreamProvider::GetIID())
        {
            *ppInterface = static_cast<foundation::IStreamProvider *>(this);
        }
        else
        {
            return _CBinaryStreamProvider_base::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
    // Interface IStreamProvider
    STDMETHOD(Serialize)(IInspectable *pValue, IInspectable **ppValue) override;
    STDMETHOD(Deserialize)(IInspectable *pValue, IInspectable **ppValue) override;
};

