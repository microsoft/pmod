/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchInfo.h
****/

#pragma once

#include "ObjectDispatchInfoBase.h"
#include <memory>

//------------------------------------------------------------------------------
// Class:   CObjectDispatchInfoClass
//
// Purpose: A concrete implementation to support IObjectDispatchInfo
//
//------------------------------------------------------------------------------
typedef TObjectDispatchInfoBase
<
    foundation::IObjectDispatchInfo,
    _CObjectDispatchInfoBase,
    foundation::ObjectDispatchTypeInfo
> _CObjectDispatchInfoClass_BaseType;

class CObjectDispatchInfoClass : public _CObjectDispatchInfoClass_BaseType
{
public:
    static HRESULT CreateInstance(
        _In_ const foundation::ObjectDispatchTypeInfo *pTypeInfo,
        _Outptr_ CObjectDispatchInfoClass **ppInstance);

    HRESULT _Initialize(
        const foundation::ObjectDispatchTypeInfo *pTypeInfo);
    // support for Dictionary serialize/deserialize
    HRESULT _DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr);
    HRESULT _SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr);
};

extern LPCSTR_t _Id_Key;
extern LPCSTR_t _Name_Key;

HRESULT _DeserializePropertyTypeInfoHelper(
    const foundation::DictionaryPtr& type_info_ptr,
    LPCSTR_t key,
    foundation::PropertyType& propertyType,
    IID& typeInfoType);

// support for Dictionary serialize/deserialize
HRESULT DeserializeTypeInfoInternal(
    _CObjectDispatchInfoBase *pPropertyDataInfoBase,
    const foundation::DictionaryPtr& type_info_ptr);

HRESULT SerializeTypeInfoInternal(
    _CObjectDispatchInfoBase *pPropertyDataInfoBase,
    foundation::DictionaryPtr& type_info_ptr);
