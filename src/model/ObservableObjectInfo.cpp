/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectInfo.cpp
****/

#include "pch.h"

#include "ObservableObjectInfo.h"
#include "PropertyInfo.h"
#include "EventInfo.h"
#include "PropertyInfoProxy.h"
#include "ObservableObjectClassInfoCache.h"
#include "TypeInfoFactory.h"

#include <pmod/errors.h>
#include <foundation/library/dictionary_class.h>

#pragma warning(push)
#pragma warning(disable : 4512)

LPCSTR_t _PropertyModelTypeInfo_Events_Key = U("events");
LPCSTR_t _PropertyViewModelTypeInfo_Source_Key = U("source");
LPCSTR_t _PropertyViewModelTypeInfo_InheritProperties_Key = U("inheritProperties");

LPCSTR_t _EventInfo_EventArgsType_Key = U("eventArgsType");

// {977C7285-E226-4638-838D-50D923101FE2}
EXTERN_C PMOD_API const GUID IID_CObservableObjectInfo =
{ 0x977c7285, 0xe226, 0x4638,{ 0x83, 0x8d, 0x50, 0xd9, 0x23, 0x10, 0x1f, 0xe2 } };

// {31F96ED9-CD7B-487C-A988-BA5012EFF74E}
const GUID pmod::library::IID_IObservableObjectInfoClass =
{ 0x31f96ed9, 0xcd7b, 0x487c,{ 0xa9, 0x88, 0xba, 0x50, 0x12, 0xef, 0xf7, 0x4e } };

CObservableObjectInfo::CObservableObjectInfo():
    m_TypeId(0),
    m_CountProperties(0),
    m_StartPropertyId(0),
    m_IsNormalized(false),
    m_AllowChanges(false)
{
}

CObservableObjectInfo::~CObservableObjectInfo()
{
}

HRESULT CObservableObjectInfo::EnsurePropertyInfoContainer()
{
    return pal_CallOnce_t(
        m_PropertyInfoContainerOnce,
        [this]() {
        return CreatePropertyInfoContainerInternal();
    });
}

HRESULT CObservableObjectInfo::CreatePropertyInfoContainerInternal()
{
    foundation_assert(!IsPropertyInfoContainerCreated());

    // Ensure m_base_type also created their Info Containers
    CObservableObjectInfo *baseType = GetBaseType();
    if(baseType)
    {
        IFR_ASSERT(baseType->EnsurePropertyInfoContainer());
    }

    // Class to iterate trough total info properties
    class CCountInfoProperties  : public _FuncProperty
    {
    public:
        CCountInfoProperties(
            size_t& countProperties,
            bool& isNormalized):
            _countProperties(countProperties),
            _isNormalized(isNormalized)
       {
            _isNormalized = true;
            _propertyId = 0;
            _countProperties = 0;
        }           
        bool NextProperty (CObservableObjectInfo *pThis,CPropertyInfo *pPropertyInfo) override
        {
            // If the Model Info support dynamic property addition we can't normalize
            if(pThis->m_AllowChanges)
            {
                _isNormalized = false;
            }

            NextPropertyInternal(pPropertyInfo->GetId());
            return true;
        }
    private:
        void NextPropertyInternal(UINT32 nextPropertyId)
        {
            if(
                // check if first property we iterate
                _propertyId != 0 && 
                // check if the proeprties id are normalized
                (nextPropertyId != (_propertyId+1)))
            {
                _isNormalized = false;
            }
            _propertyId = nextPropertyId;
            ++_countProperties;
        }
    protected:
        UINT32      _propertyId;
        size_t&     _countProperties;
        bool&       _isNormalized;
    };

    class CCountStorageProperties : public CCountInfoProperties
    {
    public:
        CCountStorageProperties(
            size_t& countProperties,
            bool& isNormalized):CCountInfoProperties(countProperties,isNormalized)
        {
        }
        bool NextProperty (CObservableObjectInfo *pThis,CPropertyInfo *pPropertyInfo) override
        {

            // we need to support the scenario where the PropertyInfo Array
            // will contain both Model & ViewModel properties
            // The container will only hold the ViewModel Property
            // which normally are defined at the begining.
            // So we will stop the iteration when we transition from 
            // ViewModel->Model properties
            
            if(_propertyId >= pmod::PropertyModel_ViewModel_Start &&
                pPropertyInfo->GetId() < pmod::PropertyModel_ViewModel_Start)
            {
                // stop here
                return false;
            }
            return CCountInfoProperties::NextProperty(pThis,pPropertyInfo);
        }
    };

    // Note: after this recursive method is call we will know
    // the total count of properties and if the property ids are normalized
    size_t totalInfoProperties;
    bool isInfoPropertiesNormalized;
    CCountInfoProperties countInfoProperties(totalInfoProperties,isInfoPropertiesNormalized);

    NextPropertyModelInfo(countInfoProperties);
    // now iterate to find storage only properties
    CCountStorageProperties countStorageProperties(m_CountProperties,m_IsNormalized);

    NextPropertyModelInfo(countStorageProperties);

    // Create the Property Info Container
    if (isInfoPropertiesNormalized && !this->m_AllowChanges)
    {
        // calculate the number of local info properties needed
        size_t infoContainerCount = totalInfoProperties;
        if(baseType)
        {
            infoContainerCount -= baseType->GetCountProperties();
        }
        m_spPropertyInfoContainer.reset(new TStaticPropertyIdContainer<foundation::IPropertyInfo *>(infoContainerCount));
    }
    else
    {
        m_spPropertyInfoContainer.reset(new TDynamicPropertyIdContainer<foundation::IPropertyInfo *>());
    }

    IFR_ASSERT(m_spPropertyInfoContainer.get() != nullptr ? S_OK : E_OUTOFMEMORY);

    // Populate the Container
    for(PropertyInfoVector::const_iterator iter = GetProperties().begin();
        iter != GetProperties().end();
        ++iter)
    {
        UINT32 propertyId;
        IFR_ASSERT((*iter)->GetId(&propertyId));
        if(propertyId  >= foundation::Property_ResevervedMask)
        {
            // skip reserved properties
            continue;
        }
        // assign once the first valid property id
        if(m_StartPropertyId == 0)
        {
            m_StartPropertyId = propertyId;
        }

        propertyId = NormalizePropertyId(propertyId);
        foundation::IPropertyInfo *pPropertyInfo = (*iter);
        foundation_assert(pPropertyInfo);
        bool bRet = m_spPropertyInfoContainer->Set(propertyId,pPropertyInfo);
        foundation_assert(bRet);
    }
    return S_OK;
}

#pragma warning(pop)

bool CObservableObjectInfo::HasStaticContainer()
{ 
    foundation_assert(IsPropertyInfoContainerCreated());
    return m_IsNormalized && !this->m_AllowChanges;
}

size_t CObservableObjectInfo::GetCountProperties()
{
    foundation_assert(IsPropertyInfoContainerCreated());
    return m_CountProperties;
}

bool CObservableObjectInfo::TryGetPropertyInfoById(UINT32 propertyId,foundation::IPropertyInfo **ppPropertyInfo)
{
    foundation_assert(ppPropertyInfo);
    foundation_assert(IsPropertyInfoContainerCreated());

    int propertyIdNormalized = NormalizePropertyId(propertyId);

    if(propertyIdNormalized >= 0 &&
        m_spPropertyInfoContainer->Get(propertyIdNormalized,*ppPropertyInfo))
    {
        // TODO: We need to foundation_assert once we fix the Core property Model class
        foundation_assert(*ppPropertyInfo);
        return true;
    }
    // we didn't find in our info containers so attempt the m_base_type
    CObservableObjectInfo *baseType = GetBaseType();

    if(baseType)
    {
        return baseType->TryGetPropertyInfoById(propertyId,ppPropertyInfo);
    }

    return false;
}

bool CObservableObjectInfo::TryGetEventInfoById(UINT32 eventId,pmod::IEventInfo **ppEventInfo)
{
    HRESULT hr = this->GetEventByIdInternal(eventId,ppEventInfo);
    if (FAILED(hr))
    {
        return false;
    }
    if(hr == S_OK)
    {
        return true;
    }
    CObservableObjectInfo *baseType = GetBaseType();

    if(baseType)
    {
        return baseType->TryGetEventInfoById(eventId,ppEventInfo);
    }
    return false;
}

void CObservableObjectInfo::NextPropertyModelInfo(_FuncProperty& _Func)
{
    CObservableObjectInfo *baseType = GetBaseType();
    if(baseType)
    {
        baseType->NextPropertyModelInfo(_Func);
    }

    // Iterate on my properties
    for(PropertyInfoVector::const_iterator iter = GetProperties().begin();
        iter != GetProperties().end();
        ++iter)
    {
        CPropertyInfo *pPropertyInfo = static_cast<CPropertyInfo *>((*iter).Get());
        if((pPropertyInfo->GetId() & foundation::Property_ResevervedMask) == 0 )
        {
            if(!_Func.NextProperty(this,pPropertyInfo))
                break;
        }
   }
}

HRESULT CObservableObjectInfo::AddEventInfoArray(const pmod::EventInfo *pEventInfoArray)
{
    foundation_assert(pEventInfoArray);
    // Build the Array for each defined Property
    while(pEventInfoArray->m_eventName)
    {
        IFR_ASSERT(AddEventInfo(pEventInfoArray));
        // Next event Info description
        ++pEventInfoArray;
    }
    return S_OK;
}

HRESULT CObservableObjectInfo::AddPropertyInternal(
    _In_ foundation::PropertyInfo *pPropertyInfo,
    _In_ GUID modelType)
{
    foundation_assert(pPropertyInfo);

    if(this->IsPropertyDefined(pPropertyInfo->m_id))
    {
        return foundation::E_PROPERTY_EXIST;
    }
    if(pPropertyInfo->m_id == 0)
    {
        // generate a unique property id
        // TODO: verify multi thread scenario and and duplicate id's
        if(this->GetProperties().size())
        {
            this->GetProperties().back()->GetId(&pPropertyInfo->m_id);
        }
        ++pPropertyInfo->m_id;
    }

    bool hasModelType = !IsEqualIID(modelType,foundation_GUID_NULL);
    foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
    IFR_ASSERT(CPropertyInfo::CreateInstance(
        pPropertyInfo,
        hasModelType ? &modelType:nullptr,
        true,
        spPropertyInfo.GetAddressOf()));

    // add to my internal list
    this->AddPropertyInternal(spPropertyInfo);
    return S_OK;
}

HRESULT CObservableObjectInfo::AddPropertyInternal(foundation::IPropertyInfo *pPropertyInfo)
{
    // force to create the container
    IFR_ASSERT(EnsurePropertyInfoContainer());

    foundation_assert(pPropertyInfo);
    if(this->HasStaticContainer())
    {
        // TODO: provide better error code to report 'freeze' Model Info
        return E_UNEXPECTED;
    }
    // add to my internal maps
    AddPropertyInfoInternal(pPropertyInfo);

    UINT32 propertyId;
    IFR_ASSERT(pPropertyInfo->GetId(&propertyId));
    // mantain synched the Info Container
    foundation_assert(m_spPropertyInfoContainer.get());
    m_spPropertyInfoContainer->Set(propertyId,pPropertyInfo);

    return S_OK;
}
HRESULT CObservableObjectInfo::AddMethodInternal(foundation::IMethodInfo *pMethodInfo)
{
    // TODO: check freeze state
    AddMethodInfoInternal(pMethodInfo);
    return S_OK;
}

HRESULT CObservableObjectInfo::AddEventInternal(pmod::IEventInfo *pEventInfo)
{
    // TODO: check freeze state
    AddEventInfoInternal(pEventInfo);
    return S_OK;
}

HRESULT CObservableObjectInfo::AddEventInfo(
    const pmod::EventInfo *pEventInfo)
{
    foundation_assert(pEventInfo);
    foundation_assert(pEventInfo->m_eventName);

    foundation::ComPtr<pmod::IEventInfo> spEventInfo;
    IFR_ASSERT(CEventInfo::CreateInstance(pEventInfo, spEventInfo.GetAddressOf()));
    AddEventInfoInternal(spEventInfo);

    return S_OK;
}

UINT32 CObservableObjectInfo::NormalizePropertyId(UINT32 properyId)
{
    return m_IsNormalized ? 
        properyId - m_StartPropertyId:
        properyId;
}

HRESULT CObservableObjectInfo::DeserializeTypeInfoInternal(const foundation::DictionaryPtr& type_info_ptr)
{
    // Properties/Methods
	IFR_ASSERT(::DeserializeTypeInfoInternal(this,type_info_ptr));

    // Events
    foundation::ArrayRefCountWrapper<foundation::IDictionary> events;
    IFR_ASSERT(type_info_ptr.GetValue(_PropertyModelTypeInfo_Events_Key, events));
    for (UINT32 index = 0; index < events.GetSize(); ++index)
    {
        foundation::DictionaryPtr event_ptr = events[index];
        UINT32 id;
        IFR_ASSERT(event_ptr.GetValue(_Id_Key, id));
        foundation::HStringPtr name;
        IFR_ASSERT(event_ptr.GetValue(_Name_Key, name));

        foundation::PropertyType eventArgsPropertyType;
        IID eventArgsTypeInfoType;

        IFR_ASSERT(_DeserializePropertyTypeInfoHelper(
            event_ptr,
            _EventInfo_EventArgsType_Key,
            eventArgsPropertyType,
            eventArgsTypeInfoType));

        foundation::PropertyTypeInfo eventArgsTypeInfo(eventArgsPropertyType, nullptr);
        pmod::EventInfo eventInfo(
            id,
            name.GetRawBuffer(),
            &eventArgsTypeInfo
            );

        foundation::ComPtr<pmod::IEventInfo> event_info_ptr;
        IFR_ASSERT(CEventInfo::CreateInstance(
            &eventInfo,
            !IsEqualIID(eventArgsTypeInfoType, foundation_GUID_NULL) ? &eventArgsTypeInfoType : nullptr,
            event_info_ptr.GetAddressOf()));

        this->AddEventInfoInternal(event_info_ptr);
    }

    return S_OK;
}

HRESULT CObservableObjectInfo::SerializeTypeInfoInternal(foundation::DictionaryPtr& type_info_ptr)
{
    // Properties/Methods
	IFR_ASSERT(::SerializeTypeInfoInternal(this,type_info_ptr));

    // Events
    std::vector<foundation::ComPtr<foundation::IDictionary>> events;
    for (EventInfoVector::const_iterator iter = this->GetEvents().begin();
        iter != this->GetEvents().end();
        ++iter
        )
    {
        foundation::DictionaryPtr event_ptr;
        IFR_ASSERT(foundation::library::CreateDictionaryClass(event_ptr.GetAddressOf()));

        UINT32 id;
        (*iter)->GetId(&id);
        foundation::HStringPtr name;
        (*iter)->GetEventName(name.GetAddressOf());
        foundation::ComPtr<foundation::IPropertyTypeInfo> event_args_type_info_ptr;
        (*iter)->GetEventArgsTypeInfo(event_args_type_info_ptr.GetAddressOf());

        event_ptr.SetValue(_Id_Key, id);
        event_ptr.SetValue(_Name_Key, name);

        CTypeInfoFactory::_SerializePropertyTypeInfo(event_ptr, _EventInfo_EventArgsType_Key, event_args_type_info_ptr);

        events.push_back(event_ptr);
    }
    type_info_ptr.SetValue(_PropertyModelTypeInfo_Events_Key, foundation::CreateValueFromVectorPtr(events));

    return S_OK;

}

template <class TClass,class model_type_info>
HRESULT CreateObservableObjectRuntimeInstance(
    _In_ const model_type_info   *pModelTypeInfo,
    _In_ CObservableObjectInfo         *pBaseType,
    _Outptr_ TClass** ppPropertyModelInfoClass)
{
    foundation_assert(pModelTypeInfo);
    HRESULT hr;

    TClass *pClass = nullptr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<TClass>::CreateInstance(&pClass));
    IFR_ASSERT(hr = pClass->Initialize(pModelTypeInfo,pBaseType));
    // return created instance
    *ppPropertyModelInfoClass = pClass;
    return S_OK;
}

HRESULT CPropertyModelInfoClass::create_runtime_instance(
    _In_ const pmod::ObservableObjectTypeInfo   *pPropertyModelInfo,
    _In_ CObservableObjectInfo         *pBaseType,
    _Outptr_ CPropertyModelInfoClass** ppPropertyModelInfo)
{
    return CreateObservableObjectRuntimeInstance(
        pPropertyModelInfo,
        pBaseType,
        ppPropertyModelInfo);
}

HRESULT CPropertyModelInfoClass::CreateInstance(
        _In_ const pmod::ObservableObjectTypeInfo *pPropertyModelInfo,
        _In_ CObservableObjectInfo *pBaseType,
        _Outptr_ CPropertyModelInfoClass **ppPropertyModelInfo)
{
    foundation_assert(pPropertyModelInfo);

    bool hasType = !IsEqualIID(pPropertyModelInfo->get_iid_type(), foundation_GUID_NULL);
    if (hasType && CPropertyModelClassInfoCache::LookupType(pPropertyModelInfo->get_iid_type()))
    {
        return foundation::E_TYPE_INFO_ALREADY_REGISTERED;
    }

    HRESULT hr;

    CPropertyModelInfoClass *pPropertyModelInfoClass = nullptr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CPropertyModelInfoClass>::CreateInstance(&pPropertyModelInfoClass));

    if(hasType)
    {
        // Now Cache this new created Property Model Info
        CPropertyModelClassInfoCache::AddToCache(pPropertyModelInfo->get_iid_type(), pPropertyModelInfoClass);
    }

    // now we can initialize
    IFR_ASSERT(hr = pPropertyModelInfoClass->Initialize(pPropertyModelInfo, pBaseType));
    // return created instance
    *ppPropertyModelInfo = pPropertyModelInfoClass;

    return S_OK;
}

void CPropertyModelInfoClass::FinalRelease()
{
    CPropertyModelInfoClass_BaseType::FinalRelease();
}

HRESULT CPropertyViewModelInfoClass::create_runtime_instance(
        _In_ const pmod::ViewModelTypeInfo   *pPropertyViewModelInfo,
        _In_ CObservableObjectInfo         *pBaseType,
        _Outptr_ CPropertyViewModelInfoClass** ppPropertyModelInfoClass)
{
    return CreateObservableObjectRuntimeInstance(
        pPropertyViewModelInfo,
        pBaseType,
        ppPropertyModelInfoClass);
}

HRESULT CPropertyViewModelInfoClass::Initialize(
        const pmod::ViewModelTypeInfo *pPropertyViewModelInfo,
        _In_ CObservableObjectInfo *pBaseType)
{
    IFR_ASSERT(_InitializeTypeInfo(pPropertyViewModelInfo));
    IFR_ASSERT(InitializePropertyModelInfo(pPropertyViewModelInfo, pBaseType));
    if(pPropertyViewModelInfo->m_p_source_model_type_info != nullptr)
    {
        // next block will ensure the cache will have this Type created
        CObservableObjectInfo *pSourcePropertyModelInfo = nullptr;
        IFR_ASSERT(CPropertyModelClassInfoCache::GetOrCreatePropertyModelInfo(
            pPropertyViewModelInfo->m_p_source_model_type_info,
            &pSourcePropertyModelInfo));
        // If 'Inherited' then inject 'Source' properties
        if(pPropertyViewModelInfo->m_inherit_properties)
        {
            const pmod::ObservableObjectTypeInfo *pSourceTypeModelInfo = pPropertyViewModelInfo->m_p_source_model_type_info;
            while(pSourceTypeModelInfo && pSourceTypeModelInfo->m_properties)
            {
                IFR_ASSERT(AddPropertyInfoArray(pSourceTypeModelInfo->m_properties));
                pSourceTypeModelInfo = static_cast<const pmod::ObservableObjectTypeInfo *>(pSourceTypeModelInfo->m_base_type);
            }
        }
        // Store our 'Source' IID Type
        m_IIDTypeSource = pPropertyViewModelInfo->m_p_source_model_type_info->get_iid_type();
    }
    return S_OK;
}

        
STDMETHODIMP CPropertyViewModelInfoClass::GetSourceType( 
        pmod::IObservableObjectInfo **ppPropertyModelInfo)
{
    if(!IsEqualGUID(m_IIDTypeSource,foundation_GUID_NULL))
    {
        CObservableObjectInfo *pPropertyModelInfo = CPropertyModelClassInfoCache::LookupType(m_IIDTypeSource);
        foundation_assert(pPropertyModelInfo);
        IFCEXPECT_ASSERT(pPropertyModelInfo);
        return foundation::QueryInterface(pPropertyModelInfo,ppPropertyModelInfo);
    }
    *ppPropertyModelInfo = nullptr;
    return S_OK;
}

HRESULT CPropertyViewModelInfoClass::CreateInstance(
        _In_ const pmod::ViewModelTypeInfo *pPropertyViewModelInfo,
        _In_ CObservableObjectInfo *pBaseType,
        _Outptr_ CPropertyViewModelInfoClass **ppPropertyModelInfo)
{
    foundation_assert(pPropertyViewModelInfo);

    HRESULT hr;

    foundation::ctl::ComInspectableObject<CPropertyViewModelInfoClass> *pComPropertyViewModelInfo = nullptr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CPropertyViewModelInfoClass>::CreateInstance(&pComPropertyViewModelInfo));

    // Now Cache this new created Property Model Info
    CPropertyModelClassInfoCache::AddToCache(pPropertyViewModelInfo->get_iid_type(), pComPropertyViewModelInfo);

    // now we can initialize
    IFR_ASSERT(hr = pComPropertyViewModelInfo->Initialize(pPropertyViewModelInfo, pBaseType));
    // return created instance
    *ppPropertyModelInfo = pComPropertyViewModelInfo;

    return S_OK;
}

HRESULT CPropertyViewModelInfoClass::_DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CPropertyViewModelInfoClass_BaseType::_DeserializeTypeInfo(type_info_ptr));
    IFR_ASSERT(type_info_ptr.GetValue(_PropertyViewModelTypeInfo_Source_Key, m_IIDTypeSource));
    bool inheritProperties;
    IFR_ASSERT(type_info_ptr.GetValue(_PropertyViewModelTypeInfo_InheritProperties_Key, inheritProperties));
    // change the dynamic type info create before serialization
    const_cast<pmod::ViewModelTypeInfo *>(this->GetTypeInfo())->m_inherit_properties = inheritProperties;
    return S_OK;
}

HRESULT CPropertyViewModelInfoClass::_SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr)
{
    IFR_ASSERT(CPropertyViewModelInfoClass_BaseType::_SerializeTypeInfo(type_info_ptr));
    type_info_ptr.SetValue(_PropertyViewModelTypeInfo_Source_Key, m_IIDTypeSource);
    type_info_ptr.SetValue(_PropertyViewModelTypeInfo_InheritProperties_Key, this->GetTypeInfo()->m_inherit_properties);
    
    return S_OK;
}
