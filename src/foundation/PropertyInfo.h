/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyInfo.h
****/

#pragma once

#include "PropertyTypeInfo.h"
#include <foundation/interfaces/object_dispatch_info.h>
#include <foundation/library/object_dispatch_type_info.h>

class _CObjectDispatchInfoBase;

BEGIN_DECLARE_INTERFACE( IPropertyInfoClass,foundation::IUnknown,_VOID_MACRO)
    virtual LPCSTR_t GetNameInternal() const = 0;
END_DECLARE_INTERFACE()

//------------------------------------------------------------------------------
// Class:   CPropertyInfoBase
//
// Purpose: Define a Base class to implement a IPropertyInfo interface
//
//------------------------------------------------------------------------------
typedef _TPropertyTypeInfo
<
	foundation::IPropertyInfo,
	foundation::PropertyInfo
> CPropertyInfoBase_BaseType;

class CPropertyInfoBase :
    public CPropertyInfoBase_BaseType,
    public IPropertyInfoClass
{
public:

    // IPropertyInfo Interface
    STDMETHOD(GetId)(_Out_ UINT32* propertyId)
    {
        IFCPTR_ASSERT(m_pPropertyTypeInfo);
        *propertyId = m_pPropertyTypeInfo->m_id;
        return S_OK;
    }

    STDMETHOD(GetName)(HSTRING* propertyName)
    {
        IFCPTR_ASSERT(m_pPropertyTypeInfo);
        IFR_ASSERT(CreateStringHelper(m_pPropertyTypeInfo->m_name,propertyName));
        return S_OK;
    }

    STDMETHOD(GetFlags)(_Out_ UINT32* flags)
    {
        IFCPTR_ASSERT(m_pPropertyTypeInfo);
        *flags = m_pPropertyTypeInfo->m_flags;
        return S_OK;
    }

	UINT32 GetId() const
	{
        IFCPTR_ASSERT(m_pPropertyTypeInfo);
		return m_pPropertyTypeInfo->m_id;
	}

	// friendly accessor functions.
	UINT32 GetFlags() const
	{
        IFCPTR_ASSERT(m_pPropertyTypeInfo);
		return m_pPropertyTypeInfo->m_flags;
	}
	// friendly accessor functions.
	foundation::PropertyType GetPropertyType() const
	{
		return m_pPropertyTypeInfo->m_property_type;
	}

    friend class _CObjectDispatchInfoBase;
protected:
    // Override ComInspectableBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == IPropertyInfoClass::GetIID())
        {
            *ppInterface = static_cast<IPropertyInfoClass *>(this);
        }
        else
        {
            return CPropertyInfoBase_BaseType::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    void FinalRelease() override
    {
        if(m_bDeletePropertyTypeInfo && m_pPropertyTypeInfo)
        {
            _pal_MemFree((LPVOID)m_pPropertyTypeInfo->m_name);
        }
        CPropertyInfoBase_BaseType::FinalRelease();
    }

    // Overrides from IPropertyInfoClass
    LPCSTR_t GetNameInternal() const override
    {
        return m_pPropertyTypeInfo->m_name;
    }

};

//------------------------------------------------------------------------------
// Class:   CPropertyInfo
//
// Purpose: A PropertyInfo implementation based on const struct PropertyInfo
//
//------------------------------------------------------------------------------
class CPropertyInfo :
    public CPropertyInfoBase
{
 public:
     static HRESULT CreateInstance(
         _In_ const foundation::PropertyInfo *pPropertyInfo,
         _In_opt_ const GUID *pModelType,
         _In_ bool isDeleteVarTypeInfo,
         _COM_Outptr_ foundation::IPropertyInfo **ppPropertyInfo);

     static HRESULT CreateInstance(
         _In_ const foundation::PropertyInfo *pPropertyInfo,
         _In_opt_ const GUID *pModelType,
         _COM_Outptr_ foundation::IPropertyInfo **ppPropertyInfo)
     {
         return CreateInstance(pPropertyInfo,pModelType,false,ppPropertyInfo);
     }
protected:
    CPropertyInfo()
    {
    }

private:
    HRESULT Initialize(const foundation::PropertyInfo* pPropertyInfo,const GUID *pModelType);
};

