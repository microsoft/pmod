/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyTypeInfo.h
****/

#pragma once

#include "TypeInfo.h"
#include <foundation/library/base_adapter_host.h>
#include <foundation/weak_reference.h>

//------------------------------------------------------------------------------
// Class:   _TPropertyTypeInfoBase
//
// Purpose: Define a Base class to implement a IPropertyTypeInfo interface
//
//------------------------------------------------------------------------------
template <class T,class _TypeInfoProperty = foundation::PropertyTypeInfo>
class _TPropertyTypeInfoBase :
    public foundation::library::_DefaultAdapterHost<T, foundation::ctl::ComInspectableBase>,
    public T
{
public:
    void SetDeleteVarTypeInfo(bool newValue)
    {
        m_bDeletePropertyTypeInfo = newValue;
    }

    // IPropertyTypeInfo Interface
    STDMETHOD(GetPropertyType)(_Out_ foundation::PropertyType* type) override
    {
        *type = m_pPropertyTypeInfo->m_property_type;
        return S_OK;
    }

    STDMETHOD(GetTypeInfo)(_COM_Outptr_result_maybenull_ foundation::IObjectTypeInfo** ppTypeInfo) override
    {
        *ppTypeInfo = nullptr;
        if (m_TypeInfoWeakRef != nullptr)
        {
            foundation::ResolveWeakRef(m_TypeInfoWeakRef,*ppTypeInfo);
            foundation_assert(*ppTypeInfo != nullptr);
        }
        return S_OK;
    }
    
    // For internal use
    UINT32 GetTypeInfoTypeInternal()
    {
        foundation_assert(m_pPropertyTypeInfo);
        return m_pPropertyTypeInfo->m_p_type_info ? 
            m_pPropertyTypeInfo->m_p_type_info->m_type : 
            foundation::TypeInfo_None;
    }

protected:
    _TPropertyTypeInfoBase():
        m_pPropertyTypeInfo(nullptr),
        m_bDeletePropertyTypeInfo(false)
    {
    }

    HRESULT InitializePropertyTypeInfo(const _TypeInfoProperty* pVarTypeInfo)
    {
        m_pPropertyTypeInfo = pVarTypeInfo; 
        return S_OK;
    }

protected:
    // Override ComInspectableBase
    void FinalRelease() override
    {
        if(m_bDeletePropertyTypeInfo)
        {
            delete m_pPropertyTypeInfo;
            m_pPropertyTypeInfo = nullptr;
        }
        foundation::ctl::ComInspectableBase::FinalRelease();
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::IPropertyTypeInfo::GetIID())
        {
            *ppInterface = static_cast<foundation::IPropertyTypeInfo *>(this);
        }
        else if (iid == T::GetIID())
        {
            *ppInterface = static_cast<T *>(this);
        }
        else
        {
            return foundation::library::_DefaultAdapterHost<T, foundation::ctl::ComInspectableBase>::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    HRESULT SetTypeInfo(foundation::IObjectTypeInfo *pTypeInfo)
    {
        if (pTypeInfo != nullptr)
        {
            IFR_ASSERT(foundation::AsWeakRef(m_TypeInfoWeakRef, pTypeInfo));
        }
        return S_OK;
    }
protected:
    foundation::WeakReferencePtr     m_TypeInfoWeakRef;
    const _TypeInfoProperty             *m_pPropertyTypeInfo;   // Weak reference to the  Var Type Info structure
    bool                                m_bDeletePropertyTypeInfo;  // if delete Type Info on Final Release
};


//------------------------------------------------------------------------------
// Class:   _TPropertyTypeInfo
//
// Purpose: Define a Base class to implement a IPropertyTypeInfo interface
//
//------------------------------------------------------------------------------
template <class T, class _TypeInfoProperty = foundation::PropertyTypeInfo>
class _TPropertyTypeInfo :
    public _TPropertyTypeInfoBase<T,_TypeInfoProperty>
{
public:
    _TPropertyTypeInfo():
            m_iidType(foundation_GUID_NULL)
    {
    }

protected:
    HRESULT Initialize(
        _In_ const _TypeInfoProperty* pPropertyTypeInfo,
        _In_opt_ const GUID *pModelType)
    {
        IFR_ASSERT(this->InitializePropertyTypeInfo(pPropertyTypeInfo));

        if(pModelType)
        {
            m_iidType = *pModelType;
        }
        else if(pPropertyTypeInfo->m_p_type_info)
        {
            foundation::IObjectTypeInfo *pTypeInfo = nullptr;
            IFR_ASSERT(CTypeInfoCache::CreateOrGetTypeInfo(
                pPropertyTypeInfo->m_p_type_info,
                &pTypeInfo));
            // store in our weak reference
            this->SetTypeInfo(pTypeInfo);
        }
        return S_OK;
    }

    STDMETHOD(GetTypeInfo)(_COM_Outptr_result_maybenull_ foundation::IObjectTypeInfo** ppTypeInfo)
    {
        *ppTypeInfo = nullptr;
        IFR_ASSERT(EnsureTypeInfo());
        return _TPropertyTypeInfoBase<T, _TypeInfoProperty>::GetTypeInfo(ppTypeInfo);
    }

    HRESULT EnsureTypeInfo()
    {
        if (this->m_TypeInfoWeakRef == nullptr && !IsEqualIID(m_iidType, foundation_GUID_NULL))
        {
            foundation::IObjectTypeInfo *pTypeInfo;
            IFR_ASSERT(CTypeInfoCache::LookupTypeInfo(m_iidType, &pTypeInfo));
            IFR_ASSERT(this->SetTypeInfo(pTypeInfo));
        }
        return S_OK;
    }
protected:
    GUID    m_iidType;
};


//------------------------------------------------------------------------------
// Class:   CPropertyTypeInfo
//
// Purpose: Define a Base class to implement a IVarTypeInfo interface
//
//------------------------------------------------------------------------------
class CPropertyTypeInfo :
    public _TPropertyTypeInfo<foundation::IPropertyTypeInfo>
{
public:
	static HRESULT CreateInstance(
		_In_ const foundation::PropertyTypeInfo *pPropertyTypeInfo,
		_In_opt_ const GUID *pModelType,
        _COM_Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfoInstance);
};

IID GetTypeInfoTypeInternal(foundation::IPropertyTypeInfo *pPropertyTypeInfo);

template <class TypeInfo>
TypeInfo *CastTypeInfoTypeInternal(foundation::IPropertyTypeInfo *pPropertyTypeInfo)
{
    foundation::ComPtr<foundation::IObjectTypeInfo> spObjectTypeInfo;
    pPropertyTypeInfo->GetTypeInfo(spObjectTypeInfo.GetAddressOf());
    foundation_assert(spObjectTypeInfo != nullptr);
    return foundation::CastTo<TypeInfo>(spObjectTypeInfo);
}

template <class T>
T *CastTypeInfoTypeClassInternal(foundation::IPropertyTypeInfo *pPropertyTypeInfo)
{
    typename T::_ITypeInfo_Type *pTypeInfo = CastTypeInfoTypeInternal<typename T::_ITypeInfo_Type>(pPropertyTypeInfo);
    return static_cast<T*>(pTypeInfo);
}
