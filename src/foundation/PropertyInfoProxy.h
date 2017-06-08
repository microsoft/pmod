/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyInfoProxy.h
****/

#pragma once

#include "PropertyInfo.h"
#include <foundation/hstring_wrapper.h>

class CPropertyInfoProxyBase :
    public _TPropertyTypeInfoBase<foundation::IPropertyInfo>,
    public IPropertyInfoClass
{
public:
    // IVarTypeInfo interface
    STDMETHOD(GetPropertyType)(_Outptr_ foundation::PropertyType* type)
    {
        * type = m_PropertyType;
        return S_OK;
    }
    // IPropertyInfo Interface
    STDMETHOD(GetId)(_Outptr_ UINT32* propertyId)
    {
        * propertyId = m_Id;
        return S_OK;
    }
    STDMETHOD(GetName)(_Outptr_ HSTRING* propertyName)
    {
        return _pal_DuplicateString(m_Name,propertyName);
    }
    STDMETHOD(GetFlags)(_Outptr_ UINT32* flags)
    {
        * flags = m_Flags;
        return S_OK;
    }

protected:

    CPropertyInfoProxyBase():
        m_Id(0),
        m_Flags(0)
    {
    }
    // Override ComInspectableBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == IPropertyInfoClass::GetIID())
        {
            *ppInterface = static_cast<IPropertyInfoClass *>(this);
        }
        else
        {
            return _TPropertyTypeInfoBase<foundation::IPropertyInfo>::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
    // Overrides from IPropertyInfoClass
    LPCSTR_t GetNameInternal() const override
    {
        return m_Name.GetRawBuffer();
    }

protected:
    foundation::HStringPtr m_Name;
    UINT32         m_Id;
    UINT32         m_Flags;
	foundation::PropertyType m_PropertyType;
};


//------------------------------------------------------------------------------
// Class:   CPropertyInfoProxy
//
// Purpose: A PropertyInfo implementation based on another IPropertyInfo
//
//------------------------------------------------------------------------------
class CPropertyInfoProxy :
    public CPropertyInfoProxyBase
{
 public:

    static HRESULT CreateInstance(
    _In_ foundation::IPropertyInfo *pPropertyInfo,
    _In_ foundation::IObjectTypeInfo *pModelTypeInfo,
    _Outptr_ foundation::IPropertyInfo **ppPropertyInfo);
 protected:
    CPropertyInfoProxy()
    {
    }

private:
    HRESULT Initialize(
        _In_ foundation::IPropertyInfo* pPropertyInfo,
        _In_ foundation::IObjectTypeInfo *pModelTypeInfo);

private:
    
};
