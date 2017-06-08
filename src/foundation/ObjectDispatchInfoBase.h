/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchInfoBase.h
****/

#pragma once

#include "TypeInfo.h"
#include "CriticalSection.h"
#include <foundation/interfaces/object_dispatch_info.h>
#include <foundation/library/object_dispatch_type_info.h>
#include <foundation/errors.h>
#include <foundation/com_ptr.h>
#include <vector>
#include <map>

typedef std::vector<foundation::ComPtr<foundation::IPropertyInfo> > PropertyInfoVector;
typedef std::vector<foundation::ComPtr<foundation::IMethodInfo> > MethodInfoVector;

class CPropertyInfo;

BEGIN_DECLARE_CLASS(class, _CObjectDispatchInfoBase, foundation::IUnknown, FOUNDATION_API)

    const PropertyInfoVector& GetProperties() { return m_properties; }
    const MethodInfoVector& GetMethods() { return m_methods; }

    void AddPropertyInfoInternal(foundation::IPropertyInfo *pPropertyInfo);

    // Add properties/methods trough a structure array
    HRESULT AddPropertyInfoArray(const foundation::PropertyInfo *pPropertyInfoArray);
    HRESULT AddMethodInfoArray(const foundation::MethodInfo *pMethodInfoArray);

    // Properties support
    HRESULT AddPropertyInfo(
        foundation::IPropertyInfo *pPropertyInfo,
        _In_ foundation::IObjectTypeInfo *pModelTypeInfo,
        _Out_opt_ CPropertyInfo **ppPropertyInfo = nullptr);

    HRESULT AddPropertyInfo(
        const foundation::PropertyInfo *pPropertyInfo,
        _In_ const GUID *pModelType = nullptr,
        _Out_opt_ CPropertyInfo **ppPropertyInfo = nullptr);

    // Methods support
    HRESULT AddMethodInfo(const foundation::MethodInfo *pMethodInfo);

    // QI for an Interface in my host
    template <class Q>
    Q *As()
    {
        Q *pQ = nullptr;
        HRESULT hr = foundation::QueryWeakReference(this, &pQ);
        foundation_assert(SUCCEEDED(hr));
        return pQ;
    }

    void AddMethodInfoInternal(foundation::IMethodInfo *pMethodInfo);

protected:
    friend class CObjectDispatch;

    _CObjectDispatchInfoBase()
    {
        // Initialize our critical section
        m_cs.Init();
    }

    HRESULT GetMethodsInternal(UINT32 methodsFlags, UINT32* size, foundation::IMethodInfo ***ppMethods);

    virtual HRESULT GetPropertiesInternal(UINT32 propertiesFlag, UINT32* size, foundation::IPropertyInfo ***ppProperties);
    template<class Q>
    Q *GetBaseTypeAs()
    {
        foundation::IUnknown *pUnk = GetBaseTypeUnknown();
        if (!pUnk)
        {
            return nullptr;
        }
        Q *pQ = nullptr;
        HRESULT hr = foundation::QueryWeakReference(pUnk, &pQ);
        foundation_assert(SUCCEEDED(hr));
        return pQ;
    }

    virtual HRESULT GetPropertyByIdInternal(UINT32 propertyId, foundation::IPropertyInfo **ppPropertyInfo);
    virtual HRESULT GetPropertyFlagsByIdInternal(UINT32 propertyId, UINT32 *flags);
    virtual HRESULT GetPropertyByNameInternal(LPCSTR_t propertyName, foundation::IPropertyInfo **ppPropertyInfo);

    HRESULT GetMethodByIdInternal(UINT32 methodId, foundation::IMethodInfo **ppMethodInfo);
    HRESULT GetMethodByNameInternal(LPCSTR_t methodName, foundation::IMethodInfo **ppMethodInfo);

    bool IsPropertyDefined(UINT32 propertyId)
    {
        return m_propertiesById.find(propertyId) != m_propertiesById.end();
    }
    const PropertyInfoVector& GetProperties() const { return m_properties; }

    // Pure Virtual Methods
    virtual foundation::IUnknown *GetBaseTypeUnknown() = 0;

private:
    HRESULT AddDeclaredProperties(
        PropertyInfoVector::iterator iter,
        PropertyInfoVector& properties,
        bool noReserved,
        bool noInspectable);

    HRESULT AddDeclaredMethods(MethodInfoVector::iterator iter, MethodInfoVector& methods);

private:
    typedef std::map<UINT32, foundation::IPropertyInfo *> _TypePropertiesByIdMap;
    typedef std::map<LPCSTR_t, foundation::IPropertyInfo *, foundation::library::StringLessThan> _TypePropertiesNameMap;

    foundation_lib::CAutoDeleteCriticalSection m_cs;
    PropertyInfoVector              m_properties;
    std::map<UINT32, UINT32>         m_propertiesFlagsById;
    _TypePropertiesByIdMap          m_propertiesById;
    _TypePropertiesNameMap          m_propertiesByName;
    MethodInfoVector                m_methods;
};

BEGIN_DECLARE_INTERNAL_INTERFACE(IObjectDispatchInfoInternalClass, foundation::IUnknown)
virtual HRESULT GetPropertyFlagsById(UINT32 propertyId, UINT32 *flags) = 0;
END_DECLARE_INTERFACE()

template <class T, class TBASE, class _TypeInfo, class TBaseType = T>
class TObjectDispatchInfoBase :
    public CTypeInfo<T, _TypeInfo, TBaseType>,
    public TBASE,
    public IObjectDispatchInfoInternalClass
{
protected:
    TObjectDispatchInfoBase()
    {
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == _CObjectDispatchInfoBase::GetIID())
        {
            *ppInterface = static_cast<_CObjectDispatchInfoBase *>(this);
        }
        else if (iid == IObjectDispatchInfoInternalClass::GetIID())
        {
            *ppInterface = static_cast<IObjectDispatchInfoInternalClass *>(this);
        }
        else if (iid == foundation::IObjectDispatchInfo::GetIID())
        {
            *ppInterface = static_cast<foundation::IObjectDispatchInfo *>(this);
        }
        else
        {
            return CTypeInfo<T, _TypeInfo, TBaseType>::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // Override Pure Methods from TBASE
    foundation::IUnknown *GetBaseTypeUnknown() override
    {
        return this->CTypeInfo<T, _TypeInfo, TBaseType>::GetBaseType();
    }

    // IObjectDispatchInfo Interface
    STDMETHODIMP GetProperties(UINT32 propertiesFlag, UINT32* size, foundation::IPropertyInfo ***ppProperties)
    {
        return this->GetPropertiesInternal(propertiesFlag, size, ppProperties);
    }

    STDMETHOD(GetPropertyById)(UINT32 propertyId, foundation::IPropertyInfo **ppPropertyInfo)
    {
        HRESULT hr = this->GetPropertyByIdInternal(propertyId, ppPropertyInfo);
        _IFHR_(hr);
        return hr == S_FALSE ? (HRESULT)foundation::Errors::E_PROPERTY_NOT_FOUND : hr;
    }

    STDMETHOD(GetPropertyByName)(HSTRING propertyName, foundation::IPropertyInfo **ppPropertyInfo)
    {
        IFCPTR_ASSERT(propertyName);
        HRESULT hr = this->GetPropertyByNameInternal(_pal_GetStringRawBuffer(propertyName, nullptr), ppPropertyInfo);
        _IFHR_(hr);
        return hr == S_FALSE ? (HRESULT)foundation::Errors::E_PROPERTY_NOT_FOUND : hr;
    }

    STDMETHOD(GetMethods)(UINT32 methodsFlags, UINT32* size, foundation::IMethodInfo ***ppMethods)
    {
        return this->GetMethodsInternal(methodsFlags, size, ppMethods);
    }

    STDMETHOD(GetMethodyById)(UINT32 methodId, foundation::IMethodInfo **ppMethodInfo)
    {
        HRESULT hr = this->GetMethodByIdInternal(methodId, ppMethodInfo);
        _IFHR_(hr);
        ADDREF_INTERFACE(*ppMethodInfo);
        return hr == S_FALSE ? (HRESULT)foundation::Errors::E_METHOD_NOT_DEFINED : hr;
    }

    STDMETHOD(GetMethodByName)(HSTRING methodName, foundation::IMethodInfo **ppMethodInfo)
    {
        HRESULT hr = this->GetMethodByNameInternal(_pal_GetStringRawBuffer(methodName, nullptr), ppMethodInfo);
        _IFHR_(hr);
        return hr == S_FALSE ? (HRESULT)foundation::Errors::E_METHOD_NOT_DEFINED : hr;
    }


    // IObjectDispatchInfoInternalClass Interface

    HRESULT GetPropertyFlagsById(UINT32 propertyId, UINT32 *flags)  override
    {
        return this->GetPropertyFlagsByIdInternal(propertyId, flags);
    }
};
