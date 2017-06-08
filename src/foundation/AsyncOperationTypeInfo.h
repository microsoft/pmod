/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationTypeInfo.h
****/
#pragma once

#include <foundation/library/foundation_library.h>
#include "TypeInfo.h"

#include <foundation/com_ptr.h>

//------------------------------------------------------------------------------
// Class:   CAsyncOperationTypeInfo
//
// Purpose: A concrete implementation for IAsyncOperationTypeInfo
//
//------------------------------------------------------------------------------
typedef CTypeInfo
<
foundation::IAsyncOperationTypeInfo,
foundation::AsyncOperationTypeInfo
> CAsyncOperationTypeInfo_BaseType;

class CAsyncOperationTypeInfo :
    public CAsyncOperationTypeInfo_BaseType
{
 public:
    static HRESULT CreateInstance(
        _In_ const foundation::AsyncOperationTypeInfo *pAsyncOperationTypeInfo,
        _Outptr_ CAsyncOperationTypeInfo **ppAsyncOperationTypeInfo);

    static HRESULT create_runtime_instance(
        _In_ const foundation::AsyncOperationTypeInfo *pAsyncOperationTypeInfo,
        _In_ foundation::IPropertyTypeInfo *pResultTypeInfo,
        _Outptr_ CAsyncOperationTypeInfo **ppAsyncOperationTypeInfo);

    HRESULT _Initialize(
        const foundation::AsyncOperationTypeInfo *pAsyncOperationTypeInfo);

protected:
    CAsyncOperationTypeInfo()
    {
    }

public:
    // support for Dictionary serialize/deserialize
    HRESULT _DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr);
    HRESULT _SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr);

    // IAsyncOperationModelInfo Interface
    STDMETHOD(GetResultTypeInfo)(_COM_Outptr_ foundation::IPropertyTypeInfo **ppVarTypeInfo);

    BEGIN_IID_MAP
        IID_INTERFACE_ENTRY(foundation::IAsyncOperationTypeInfo)
    END_IID_MAP(CAsyncOperationTypeInfo_BaseType)

private:
    foundation::ComPtr<foundation::IPropertyTypeInfo> m_spResultTypeInfo;

};


