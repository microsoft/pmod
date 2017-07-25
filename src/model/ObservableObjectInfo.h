/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectInfo.h
****/

#pragma once

#include <pmod/library/model_library.h>

#include "ObservableObjectInfoBase.h"
#include "ObjectDispatchInfo.h"
#include "PropertyIdContainer.h"

typedef IBasePropertyIdContainer<foundation::IPropertyInfo *> IPropertyInfoContainer;

//class CPropertyInfo;
class CMethodInfo;

//------------------------------------------------------------------------------
// Class:   CObservableObjectInfo
//
// Purpose: A concrete implementation to support IObservableObjectInfo
//
//------------------------------------------------------------------------------
BEGIN_DECLARE_CLASS(class, CObservableObjectInfo, _CObservableObjectInfoBase, PMOD_API)

    // If Changes on the Model Info are allowed
    bool IsAllowChanges() { return this->m_AllowChanges; }
    void SetAllowChanges() { this->m_AllowChanges = true; }

    // Add events trough a structure array
    HRESULT AddEventInfoArray(const pmod::EventInfo *pEventInfoArray);

    // Event support
    HRESULT AddEventInfo(const pmod::EventInfo *pEventInfo);

protected:
    CObservableObjectInfo();
    virtual ~CObservableObjectInfo();
   
protected:
    CObservableObjectInfo *GetBaseType()
    {
        return GetBaseTypeAs<CObservableObjectInfo>();
    }
protected:
    HRESULT AddPropertyInternal(foundation::IPropertyInfo *pPropertyInfo);
    HRESULT AddMethodInternal(foundation::IMethodInfo *pMethodInfo);
    HRESULT AddEventInternal(pmod::IEventInfo *pEventInfo);

    // Add a property Info from a PropertyInfo structure & Model Type
    virtual HRESULT AddPropertyInternal(
        _In_ foundation::PropertyInfo *pPropertyInfo,
        _In_ GUID modelType);

    // support for Dictionary serialize/deserialize
    HRESULT DeserializeTypeInfoInternal(const foundation::DictionaryPtr& type_info_ptr);
    HRESULT SerializeTypeInfoInternal(foundation::DictionaryPtr& type_info_ptr);

protected:

private:
    // Ensure the Info Container is being created
    // Note: this call is now multi thread safe
    virtual HRESULT EnsurePropertyInfoContainer();

    // Unprotected Create of Info Container
    virtual HRESULT CreatePropertyInfoContainerInternal();

    // Return True if this Info Container is normalized
    virtual bool HasStaticContainer();

    // Count the properties including the base type
    virtual size_t GetCountProperties();

    // Attempt to retrieve a Property Info
    virtual bool TryGetPropertyInfoById(UINT32 propertyId,foundation::IPropertyInfo **ppPropertyInfo);

    // Attempt to retrieve an Event Info
    virtual bool TryGetEventInfoById(UINT32 eventId,pmod::IEventInfo **ppEventInfo);

    // Attempt to retrieve a Method Info
    virtual bool TryGetMethodInfoById(UINT32 methodId, foundation::IMethodInfo **ppMethodInfo);

    struct _FuncProperty
    {
        virtual bool NextProperty(CObservableObjectInfo *pThis,CPropertyInfo *pPropertyInfo) = 0;
    };
    void NextPropertyModelInfo(_FuncProperty&);

    UINT32 NormalizePropertyId(UINT32 properyId);

    bool IsPropertyInfoContainerCreated() { return m_spPropertyInfoContainer.get() != nullptr; }

    // Allow only this class to access the Info Container
	friend class CObservableObject;
protected:
    UINT32     m_TypeId;
    size_t     m_CountProperties;
    UINT32     m_StartPropertyId;
    std::unique_ptr<IPropertyInfoContainer> m_spPropertyInfoContainer;
private:
    bool       m_IsNormalized;
    bool       m_AllowChanges;
    pal::library::PalOnceFlag m_PropertyInfoContainerOnce;
};
//------------------------------------------------------------------------------
// Class:   TPropertyModelInfo
//
// Purpose: Template class to implement IObservableObjectInfo
//
//------------------------------------------------------------------------------
template <class T,class _TypeInfo,class TBaseType>
class TPropertyModelInfo :
    public TObservableObjectInfoBase<T,CObservableObjectInfo,_TypeInfo,TBaseType>,
    public pmod::library::IObservableObjectInfoClass
{    
protected:
    typedef TObservableObjectInfoBase<T, CObservableObjectInfo, _TypeInfo, TBaseType> TPropertyModelInfo_BaseType;

    // Override ComInspectableBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == CObservableObjectInfo::GetIID())
        {
            *ppInterface = static_cast<CObservableObjectInfo *>(this);
        }
        else if (iid == pmod::library::IObservableObjectInfoClass::GetIID())
        {
            *ppInterface = static_cast<pmod::library::IObservableObjectInfoClass *>(this);
        }
        else
        {
            return TPropertyModelInfo_BaseType::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // IObservableObjectInfo Interface

    // IObservableObjectInfoClass Interface
    STDMETHOD (AddProperty)(_In_ foundation::IPropertyInfo *pPropertyInfo)
    {
        IFCPTR_ASSERT(pPropertyInfo);
        return this->AddPropertyInternal(pPropertyInfo);
    }
    STDMETHOD (AddMethod)(_In_ foundation::IMethodInfo *pMethodInfo)
    {
        IFCPTR_ASSERT(pMethodInfo);
        return this->AddMethodInternal(pMethodInfo);
    }

    STDMETHOD (AddEvent)(_In_ pmod::IEventInfo *pEventInfo)
    {
        IFCPTR_ASSERT(pEventInfo);
        return this->AddEventInternal(pEventInfo);
    }

    BEGIN_IID_MAP
        IID_INTERFACE_ENTRY(pmod::IObservableObjectInfo)
    END_IID_MAP(TPropertyModelInfo_BaseType)

public:
    HRESULT InitializePropertyModelInfo(
        const pmod::ObservableObjectTypeInfo *pPropertyModelInfo,
        _In_ CObservableObjectInfo *pBaseType)
    {
        foundation_assert(pPropertyModelInfo);
        this->m_TypeId = pPropertyModelInfo->m_type_id;
        if(pBaseType)
        {
            this->SetBaseType(pBaseType->As<TBaseType>());
        }

        if(pPropertyModelInfo->m_properties)
        {
            IFR_ASSERT(this->AddPropertyInfoArray(pPropertyModelInfo->m_properties));
        }
        if(pPropertyModelInfo->m_methods)
        {
            IFR_ASSERT(this->AddMethodInfoArray(pPropertyModelInfo->m_methods));
        }
        if(pPropertyModelInfo->m_events)
        {
            IFR_ASSERT(this->AddEventInfoArray(pPropertyModelInfo->m_events));
        }
        return S_OK;
    }

    HRESULT Initialize(
        const pmod::ObservableObjectTypeInfo *pPropertyModelInfo,
        _In_ CObservableObjectInfo *pBaseType)
    {
        foundation_assert(pPropertyModelInfo);

        IFR_ASSERT(this->_InitializeTypeInfo(pPropertyModelInfo));
        IFR_ASSERT(InitializePropertyModelInfo(pPropertyModelInfo, pBaseType));
        return S_OK;
    }
    // support for Dictionary serialize/deserialize
    HRESULT _DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr)
    {
        IFR_ASSERT(TPropertyModelInfo_BaseType::_DeserializeTypeInfo(type_info_ptr));
        IFR_ASSERT(TPropertyModelInfo_BaseType::DeserializeTypeInfoInternal(type_info_ptr));
        return S_OK;
    }

    HRESULT _SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr)
    {
        IFR_ASSERT(TPropertyModelInfo_BaseType::_SerializeTypeInfo(type_info_ptr));
        IFR_ASSERT(TPropertyModelInfo_BaseType::SerializeTypeInfoInternal(type_info_ptr));
        return S_OK;
    }

};

//------------------------------------------------------------------------------
// Class:   CPropertyModelInfoClass
//
// Purpose: A concrete implementation for IObservableObjectInfo
//
//------------------------------------------------------------------------------
typedef TPropertyModelInfo
<
pmod::IObservableObjectInfo,
pmod::ObservableObjectTypeInfo,
pmod::IObservableObjectInfo
> CPropertyModelInfoClass_BaseType;

class CPropertyModelInfoClass : public CPropertyModelInfoClass_BaseType
{
 public:
    static HRESULT CreateInstance(
        _In_ const pmod::ObservableObjectTypeInfo   *pPropertyModelInfo,
        _In_ CObservableObjectInfo         *pBaseType,
        _Outptr_ CPropertyModelInfoClass** ppPropertyModelInfo);

    static HRESULT create_runtime_instance(
        _In_ const pmod::ObservableObjectTypeInfo   *pPropertyModelInfo,
        _In_ CObservableObjectInfo         *pBaseType,
        _Outptr_ CPropertyModelInfoClass** ppPropertyModelInfo);
protected:
    void FinalRelease() override;
};

//------------------------------------------------------------------------------
// Class:   CPropertyViewModelInfoClass
//
// Purpose: A concrete implementation for IViewModelInfo
//
//------------------------------------------------------------------------------

typedef TViewModelInfoBase
<
    TPropertyModelInfo
    <
        pmod::IViewModelInfo,
        pmod::ViewModelTypeInfo,
        pmod::IObservableObjectInfo
    >
> CPropertyViewModelInfoClass_BaseType;

class CPropertyViewModelInfoClass : public CPropertyViewModelInfoClass_BaseType
{
public:
    static HRESULT CreateInstance(
        _In_ const pmod::ViewModelTypeInfo   *pPropertyViewModelInfo,
        _In_ CObservableObjectInfo             *pBaseType,
        _Outptr_ CPropertyViewModelInfoClass** ppPropertyModelInfoClass);

     HRESULT Initialize(
        const pmod::ViewModelTypeInfo *pPropertyViewModelInfo,
        _In_ CObservableObjectInfo *pBaseType);

    static HRESULT create_runtime_instance(
        _In_ const pmod::ViewModelTypeInfo   *pPropertyViewModelInfo,
        _In_ CObservableObjectInfo         *pBaseType,
        _Outptr_ CPropertyViewModelInfoClass** ppPropertyModelInfoClass);

    // support for Dictionary serialize/deserialize
    HRESULT _DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr);
    HRESULT _SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr);

protected:
    // Interface IViewModelInfo
    STDMETHOD (GetSourceType)( 
            pmod::IObservableObjectInfo **ppPropertyModelInfo) override;
};


