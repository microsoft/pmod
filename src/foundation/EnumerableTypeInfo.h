/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumerableTypeInfo.h
****/

#pragma once

#include "TypeInfo.h"
#include <foundation/com_ptr.h>
#include <foundation/interfaces/enumerable.h>
#include <foundation/library/enumerable_type_info.h>

//------------------------------------------------------------------------------
// Class:   CEnumerableTypeInfo
//
// Purpose: A concrete implementation for IEnumerableTypeInfo
//
//------------------------------------------------------------------------------
typedef CTypeInfo
<
    foundation::IEnumerableTypeInfo,
    foundation::EnumerableTypeInfo
> CEnumerableTypeInfo_BaseType;

class CEnumerableTypeInfo :
    public CEnumerableTypeInfo_BaseType
{
 public:
    static HRESULT CreateInstance(
		_In_ const foundation::EnumerableTypeInfo *pEnumerableTypeInfo,
		_Outptr_ CEnumerableTypeInfo **ppCollectionModelInfo);

    static HRESULT create_runtime_instance(
        _In_ const foundation::EnumerableTypeInfo *pEnumerableTypeInfo,
        _In_ foundation::IPropertyTypeInfo *pItemTypeInfo,
        _Outptr_ CEnumerableTypeInfo **ppEnumerableTypeInfo);

    HRESULT _Initialize(
        _In_ const foundation::EnumerableTypeInfo *pEnumerableTypeInfo);

protected:
    CEnumerableTypeInfo()
    {
    }
public:
    // support for Dictionary serialize/deserialize
    HRESULT _DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr);
    HRESULT _SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr);

    // IEnumerableTypeInfo Interface
    STDMETHOD(GetItemTypeInfo)(foundation::IPropertyTypeInfo **ppVarTypeInfo);

    BEGIN_IID_MAP
        IID_INTERFACE_ENTRY(foundation::IEnumerableTypeInfo)
    END_IID_MAP(CEnumerableTypeInfo_BaseType)

private:
    HRESULT Initialize(
        _In_ const foundation::EnumerableTypeInfo *pEnumerableTypeInfo,
        _In_ foundation::IEnumerableTypeInfo *pBaseType);

    foundation::ComPtr<foundation::IPropertyTypeInfo> m_spItemTypeInfo;
};



