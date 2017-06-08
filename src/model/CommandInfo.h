/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandInfo.h
****/

#pragma once

#include <pmod/library/model_library.h>
#include "TypeInfo.h"

#include <foundation/com_ptr.h>

//------------------------------------------------------------------------------
// Class:   CCommandModelTypeInfo
//
// Purpose: A concrete implementation for ICommandInfo
//
//------------------------------------------------------------------------------
typedef CTypeInfo
<
pmod::ICommandInfo,
pmod::CommandTypeInfo
> CCommandModelTypeInfo_BaseType;

class CCommandModelTypeInfo :
    public CCommandModelTypeInfo_BaseType
{
 public:
    static HRESULT CreateInstance(
        _In_ const pmod::CommandTypeInfo *pCommandModelInfo,
        _Outptr_ CCommandModelTypeInfo **ppCCommandModelInfo);

    static HRESULT create_runtime_instance(
        _In_ const pmod::CommandTypeInfo *pCommandModelInfo,
        _In_ foundation::IPropertyTypeInfo *pParameterTypeInfo,
        _In_ foundation::IPropertyTypeInfo *pResultTypeInfo,
        _Outptr_ CCommandModelTypeInfo **ppCCommandModelInfo);

    HRESULT _Initialize(
        const pmod::CommandTypeInfo *pCommandModelInfo);

protected:
    CCommandModelTypeInfo():
        m_isAsync(false)
    {
    }

public:
    // support for Dictionary serialize/deserialize
    HRESULT _DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr);
    HRESULT _SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr);

    // ICommandInfo Interface
    STDMETHOD(GetParameterTypeInfo)(foundation::IPropertyTypeInfo **ppVarTypeInfo);
    STDMETHOD(GetResultTypeInfo)(foundation::IPropertyTypeInfo **ppVarTypeInfo);
    STDMETHOD(GetIsAsync)(_Out_ bool *value)
    {
        IFCPTR_ASSERT(value);
        *value = m_isAsync;
        return S_OK;
    }
    BEGIN_IID_MAP
        IID_INTERFACE_ENTRY(pmod::ICommandInfo)
    END_IID_MAP(CCommandModelTypeInfo_BaseType)
private:
    foundation::ComPtr<foundation::IPropertyTypeInfo>  m_spParameterTypeInfo;
    foundation::ComPtr<foundation::IPropertyTypeInfo>  m_spResultTypeInfo;
    bool                                                m_isAsync;

};

