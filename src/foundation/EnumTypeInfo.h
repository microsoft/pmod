/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumTypeInfo.h
****/

#pragma once

#include <foundation/library/foundation_library.h>
#include "TypeInfo.h"
#include "CriticalSection.h"

#include <foundation/array_wrapper.h>
#include <foundation/com_ptr.h>

#include <map>

//------------------------------------------------------------------------------
// Class:   CEnumTypeInfo
//
// Purpose: A concrete implementation for IEnumTypeInfo
//
//------------------------------------------------------------------------------
typedef CTypeInfo
<
foundation::IEnumTypeInfo,
foundation::EnumTypeInfo
> CEnumTypeInfo_BaseType;

class CEnumTypeInfo :
    public CEnumTypeInfo_BaseType,
    public foundation::library::IDisposable
{
 public:
    static HRESULT CreateInstance(
        _In_ const foundation::EnumTypeInfo *pEnumTypeInfo,
        _Outptr_ CEnumTypeInfo **ppEnumTypeInfo);

    static HRESULT create_runtime_instance(
        _In_ const foundation::EnumTypeInfo *pEnumTypeInfo,
        _In_ UINT32     size,
        _In_ HSTRING    *pNames,
        _In_ UINT32     *pValues,
        _Outptr_ CEnumTypeInfo **ppEnumTypeInfo);

    HRESULT _Initialize(
        _In_ const foundation::EnumTypeInfo *pEnumTypeInfo);

    friend class CEnumTypeInfoCache;

protected:
    CEnumTypeInfo();
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if(iid == foundation::library::IDisposable::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IDisposable *>(this);
        }
        else
        {
            return CTypeInfo<foundation::IEnumTypeInfo, foundation::EnumTypeInfo>::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

public:
    // support for Dictionary serialize/deserialize
    HRESULT _DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr);
    HRESULT _SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr);

    // IDisposable Interface
    STDMETHOD (Dispose)();

    // IEnumTypeInfo Interface
    STDMETHOD (GetIsFlags)(bool *value);
    STDMETHOD (GetNames)(UINT32 *__valueSize,HSTRING **values);
    STDMETHOD (GetValues)(UINT32 *__valueSize,UINT32 **values);
    STDMETHOD (GetDefaultValue)(UINT32 *enumValue);
    STDMETHOD (GetStringValue)(UINT32 enumValue,HSTRING *value);
    STDMETHOD (IsEnumValid)(UINT32 enumValue,bool *value);
    STDMETHOD(GetEnumValue)(UINT32 enumValue, foundation::IEnumValue **ppEnumvalue);
    STDMETHOD(GetEnumValueByName)(HSTRING enumValueName, foundation::IEnumValue **ppEnumvalue);

    friend class CEnumTypeClassInfoCache;
    BEGIN_IID_MAP
        IID_INTERFACE_ENTRY(foundation::IEnumTypeInfo)
    END_IID_MAP(CEnumTypeInfo_BaseType)


private:
    CEnumTypeInfo *GetBaseType() { return (CEnumTypeInfo *)this->CEnumTypeInfo_BaseType::GetBaseType(); }

    HRESULT GetFlagsStringValue(UINT32 enumValue, foundation::string_t& enumValues);
    HRESULT InitializeArrays(UINT32 size, HSTRING **ppNames, UINT32 **ppValues);
    UINT32 IndexOfValue(UINT32 value);
    UINT32 IndexOfName(HSTRING name);
private:
    foundation_lib::CAutoDeleteCriticalSection m_cs;
    typedef std::map<UINT32, foundation::ComPtr<foundation::IEnumValue> > _EnumValueMapType;
    _EnumValueMapType           _enumValues;
    bool                        _isFlags;
    foundation::HStringArrayWrapper _namesWrapper;
    foundation::UInt32ArrayWrapper  _valuesWrapper;
};




