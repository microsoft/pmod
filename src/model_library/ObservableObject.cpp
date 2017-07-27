/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObject.cpp
****/

#include "pch.h"

#include "ObservableObject.h"
#include "ObjectDispatch.h"
#include "CreateFoundationHelper.h"
#include <pmod/errors.h>
#include "PropertyInfo.h"
#include "ObservableObjectStatics.h"
#include "PropertyBinding.h"

#include "FastPropertiesContainer.h"

// from model API
#include "../model/ModelTypeInfoStatics.h"
#include "../model/ObservableObjectClassInfoCache.h"
#include "../model/PropertyIdContainer.h"
#include "../foundation/MethodInfo.h"

#include <foundation/library/com_ptr_singleton.h>

// from public API
#include <foundation/pv_util.h>
#include <foundation/library/async_operation_class_ptr.h>
#include <foundation/library/critical_section.h>
#include <foundation/library/dictionary_class.h>
#include <foundation/library/object_dispatch_impl.h>

#include <pmod/library/event_model_event_args_impl.h>
#include <pmod/library/library_util.h>
#include <pmod/library/property_changed_event_args_impl.h>
#include <pmod/library/type_info_factory_util.h>
#include <pmod/event_connection.h>

#include <foundation/library/perf_counter_util.h>
#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#endif

// {5175B68A-B974-4072-80E9-77E0C8C77630}
const GUID pmod::library::IID_IObservableObjectClassFactory =
{ 0x5175b68a, 0xb974, 0x4072,{ 0x80, 0xe9, 0x77, 0xe0, 0xc8, 0xc7, 0x76, 0x30 } };

using namespace pmod;
using namespace pmod::library;
using namespace foundation::library;

HRESULT RegisterFactoryObservableObjectClassAdapters(
    foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    foundation_assert(pGenericAdapterFactory != nullptr);
    pGenericAdapterFactory->AddAdapterEntry(
        IID_IObservableObject,
        foundation::IID_IDictionary,
        IID_IObservableObjectClass,
        foundation::CreateAdapter<CObservableObjectDictionaryAdapter>);
    return S_OK;
}
typedef TDynamicPropertyIdContainer<foundation::InspectablePtr> CDynamicInspectablePropertyIdContainer;

//------------------------------------------------------------------------------
// Class:   CStaticPropertiesContainer
// IPropertiesContainer implementation for a Static container
//------------------------------------------------------------------------------
class CStaticPropertiesContainer :
    public foundation::ctl::ImplementsInspectable<foundation::library::IPropertiesContainer,&foundation::library::IID_IPropertiesContainer>,
    private vector_properties_container<foundation::InspectablePtr>
{
public:
    static HRESULT CreateInstance(UINT32 size, foundation::library::IPropertiesContainer **ppPropertiesContainer)
    {
        foundation::ctl::ComObject<CStaticPropertiesContainer> *pStaticPropertiesContainer = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CStaticPropertiesContainer>::CreateInstance(&pStaticPropertiesContainer));
        pStaticPropertiesContainer->resize(size);
        *ppPropertiesContainer = pStaticPropertiesContainer;
        return S_OK;
    }
protected:
    STDMETHOD(HasProperty)(_In_ UINT32 propertyId, _Out_ bool *found)
    {
        IFCPTR_ASSERT(found);
        *found = this->has_property(propertyId);
        return S_OK;
    }

    STDMETHOD(GetValue)(_In_ UINT32 propertyId, _In_ foundation::IInspectable **value)
    {
        foundation::InspectablePtr spValue;
        if(this->get(propertyId,spValue))
        {
            *value = spValue.Detach();
            return S_OK;
        }
        return E_BOUNDS;
    }

    STDMETHOD(SetValue)(_In_ UINT32 propertyId, _In_ foundation::IInspectable *value)
    {
        return this->set(propertyId,value) ? S_OK:E_FAIL;
    }

    STDMETHOD(RemoveProperty)(_In_ UINT32 propertyId)
    {
        UNREFERENCED_PARAMETER(propertyId);
        foundation_assert(false);
        return E_NOTIMPL;
    }

    STDMETHOD(GetSize)(_Outptr_ UINT32* size)
    {
        * size = (UINT32)this->size();
        return S_OK;
    }
    STDMETHOD(Dispose)()
    {
        this->clear();
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CDynamicPropertiesContainer
// IPropertiesContainer implementation for a Dynamic container
//------------------------------------------------------------------------------
class CDynamicPropertiesContainer :
    public foundation::ctl::ImplementsInspectable<foundation::library::IPropertiesContainer,&foundation::library::IID_IPropertiesContainer>,
    private map_properties_container<foundation::InspectablePtr>
{
public:
    static HRESULT CreateInstance(foundation::library::IPropertiesContainer **ppPropertiesContainer)
    {
        foundation::ctl::ComObject<CDynamicPropertiesContainer> *pDynamicPropertiesContainer = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CDynamicPropertiesContainer>::CreateInstance(&pDynamicPropertiesContainer));
        *ppPropertiesContainer = pDynamicPropertiesContainer;
        return S_OK;
    }
protected:
    STDMETHOD(HasProperty)(_In_ UINT32 propertyId, _Out_ bool *found)
    {
        IFCPTR_ASSERT(found);
        *found = this->has_property(propertyId);
        return S_OK;
    }

    STDMETHOD(GetValue)(_In_ UINT32 propertyId, _In_ foundation::IInspectable **value)
    {
        foundation::InspectablePtr spValue;
        if(this->get(propertyId,spValue))
        {
            *value = spValue.Detach();
            return S_OK;
        }
        return foundation::Errors::E_PROPERTY_NOT_FOUND;
    }

    STDMETHOD(SetValue)(_In_ UINT32 propertyId, _In_ foundation::IInspectable *value)
    {
        return this->set(propertyId,value) ? S_OK:E_FAIL;
    }

    STDMETHOD(RemoveProperty)(_In_ UINT32 propertyId)
    {
        this->remove(propertyId);
        return S_OK;
    }

    STDMETHOD(GetSize)(_Outptr_ UINT32* size)
    {
        * size = (UINT32)this->size();
        return S_OK;
    }
    STDMETHOD(Dispose)()
    {
        this->clear();
        return S_OK;
    }
};

const CLogCategory* CPropertyModelLogFactory::GetLogCategory()
{
    static _ComPtrSingleton<const CLogCategory, foundation::_NoRefCountPtrImpl> _propertyModelLogCategoryInstance;

    return _propertyModelLogCategoryInstance._GetInstance([]() {
        return CLogger::GetCategory(
            foundation::Foundation_CategoryEnum::TypeId,
            foundation::Object_CategoryEnum::TypeId,
            ObjectNode_CategoryEnum::TypeId,
            foundation::ObjectDispatch_CategoryEnum ::TypeId,
            -1);
    });
}

CObservableObject::CObservableObject():
        m_SupportRootProperty(false),
        m_NoAutoParent(false),
        m_UseGetPropertyDispatcher(false),
        m_UseSetPropertyDispatcher(false),
        m_UseGetPropertiesDispatcher(false),
        m_UseModelTransaction(false),
        m_UseFastPropertiesContainer(false),
        m_ClassOnSetProperty(false),
        m_AllowReturnAsyncOnVoidResult(false),
        m_IsContainerWeakRef(false),
        m_pRoot(nullptr),
        m_pProperties(nullptr)
{
}

HRESULT CObservableObject::_Initialize(const pmod::library::ObservableObjectCreateParameters *modelImpl)
{
    IFR_ASSERT(_ObservableObjectBase_Type::_Initialize(modelImpl));
    m_SupportRootProperty = IsOptionEnabled(modelImpl->m_options,ObservableObjectOptions::SupportRootProperty);
    m_NoAutoParent = IsOptionEnabled(modelImpl->m_options,ObservableObjectOptions::NoAutoParent);
    m_UseGetPropertyDispatcher = IsOptionEnabled(modelImpl->m_options, ObservableObjectOptions::UseGetPropertyDispatcher);
    m_UseSetPropertyDispatcher = IsOptionEnabled(modelImpl->m_options, ObservableObjectOptions::UseSetPropertyDispatcher);
    m_UseGetPropertiesDispatcher = IsOptionEnabled(modelImpl->m_options, ObservableObjectOptions::UseGetPropertiesDispatcher);
    m_UseFastPropertiesContainer = IsOptionEnabled(modelImpl->m_options, ObservableObjectOptions::UseFastPropertiesContainer);
    m_ClassOnSetProperty = IsOptionEnabled(modelImpl->m_options, ObservableObjectOptions::ClassOnSetProperty);
    m_AllowReturnAsyncOnVoidResult = IsOptionEnabled(modelImpl->m_options, ObservableObjectOptions::AllowReturnAsyncOnVoidResult);
    m_IsContainerWeakRef = IsOptionEnabled(modelImpl->m_options, ObservableObjectOptions::IsContainerWeakRef);
    if (IsOptionEnabled(modelImpl->m_options,ObservableObjectOptions::InitializeProperties))
    {
        IFR_ASSERT(InitializeProperties());
    }

    return S_OK;
}

HRESULT CObservableObject::InitializeEventSourceModel()
{
    // Initialize our Event Source Model with our critical section
    IFR_ASSERT(this->_ObservableObjectBase_Type::_CreateEventSourceWithType<PropertyChangedEventHandler>(this,this->m_pCritsec));

    return S_OK;
}

void CObservableObject::OnFinalRelease()
{
    // Call Base
    _ObservableObjectBase_Type::OnFinalRelease();

    ForceDisconnectBindings();

    if (!m_IsContainerWeakRef)
    {
        RELEASE_INTERFACE(m_pProperties);
    }
}

void CObservableObject::ForceDisconnectBindings()
{
    // Note: we don't need locking since this is called from FinalRelease

    // disconnect my bindings if necessary
    if(m_spPropertyInfoSupport.get() &&
        m_spPropertyInfoSupport->m_pBindingValueChangedDelegate)
    {
        _MapPropertyInfoInternalType propertyInfoSupport;
        propertyInfoSupport.swap(*m_spPropertyInfoSupport);
        IGNOREHR(DetachBindings(propertyInfoSupport));
        m_spPropertyInfoSupport->m_pBindingValueChangedDelegate->SetCallback(nullptr);
        foundation::ctl::ReleaseInterface(m_spPropertyInfoSupport->m_pBindingValueChangedDelegate);
    }
}

HRESULT CObservableObject::DisposeInternal()
{
    IFR_ASSERT(_ObservableObjectBase_Type::DisposeInternal());
    // Use critical section to get rid of properties and other structures
    {
        _FoundationBaseLock csl(*this);
        // Dispose any pending Model Changes
        if(m_ModelChanges.get())
        {
            m_ModelChanges->clear();
        }

        if (m_pProperties != nullptr)
        {
            m_pProperties->Dispose();
        }
    }
    return S_OK;
}

bool CObservableObject::IsPropertyIdInContainer(UINT32 propertyId)
{
    return 
        // Is this property in our container ?
        IsPropertyIdContained(propertyId) &&
        // skip also intrinsic properties
        propertyId < foundation::Property_ResevervedMask;
}

HRESULT CObservableObject::InitializeProperties(CObservableObjectInfo *pPropertyModelInfo)
{
    // process base type properties
    if(pPropertyModelInfo->GetBaseType())
    {
        IFR_ASSERT(InitializeProperties(pPropertyModelInfo->GetBaseType()));
    }

    // process type properties
    for(PropertyInfoVector::const_iterator iter = pPropertyModelInfo->GetProperties().begin();
        iter != pPropertyModelInfo->GetProperties().end();
        ++iter)
    {
        CPropertyInfoBase *pPropertyInfoBase = static_cast<CPropertyInfoBase *>((*iter).Get());
        if(!IsPropertyIdInContainer(pPropertyInfoBase->GetId()))
        {
            continue;
        }

        foundation::InspectablePtr spPropertyValue;

        // Init Non-array Enum Value require type info inspection
        if(IsOptionEnabled(pPropertyInfoBase->GetFlags(),foundation::PropertyFlagType::IsEnum) &&
            (pPropertyInfoBase->GetPropertyType() & 0x400) == 0)
        {
            foundation::IEnumTypeInfo *pEnumTypeInfo =
                CastTypeInfoTypeInternal<foundation::IEnumTypeInfo>(pPropertyInfoBase);

            UINT32 enumValue;
            IFR_ASSERT(pEnumTypeInfo->GetDefaultValue(&enumValue));
            foundation::ComPtr<foundation::IEnumValue> spEnumValue;
            IFR_ASSERT(pEnumTypeInfo->GetEnumValue(enumValue, spEnumValue.GetAddressOf()));
            spPropertyValue = spEnumValue;
        }
        else
        {
            // use utils default
            IFR_ASSERT(foundation::pv_util::CreateDefaultValue(
                pPropertyInfoBase->GetPropertyType(),
                spPropertyValue.GetAddressOf()));
        }
        // Set the property in the container
        m_pProperties->SetValue(pPropertyInfoBase->GetId(), spPropertyValue);
    }
    return S_OK;
}

HRESULT CObservableObject::InitializeProperties()
{
    IFR_ASSERT(InitializeProperties(EnsurePropertyModelInfoClassAndContainer()));
    return S_OK;
}

CObservableObjectInfo *CObservableObject::GetPropertyModelInfoClass()
{
    foundation_assert(this->m_pTypeInfo);
    return static_cast<CObservableObjectInfo *>(this->m_pTypeInfo);
}

CObservableObjectInfo *CObservableObject::EnsurePropertyModelInfoClassAndContainer()
{
    HRESULT hr = EnsurePropertiesContainer();
    foundation_assert(SUCCEEDED(hr));
    return GetPropertyModelInfoClass();
}

HRESULT CObservableObject::GetAncestorInternal(
    _In_ foundation::IPropertyInfo *pPropertyInfo,
    _Outptr_ foundation::IObject **ppAncestor)
{
    foundation_assert(ppAncestor);
    *ppAncestor = nullptr;

    foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
    IFR_ASSERT(pPropertyInfo->GetTypeInfo(spModelTypeInfo.GetAddressOf()));
    if(spModelTypeInfo == nullptr)
    {
        return foundation::Errors::E_WRONG_TYPE;
    }
    IID iidType;
    IFR_ASSERT(spModelTypeInfo->GetType(&iidType));
    IFR_ASSERT(foundation::GetObjectAncestor(this, iidType, ppAncestor));
    return S_OK;
}

HRESULT CObservableObject::RecalcAncestorInternal(
    _In_ foundation::IPropertyInfo *pPropertyInfo,
    _Outptr_ foundation::IObject **ppAncestor)
{
    IFR_ASSERT(GetAncestorInternal(pPropertyInfo,ppAncestor));
    foundation::WeakReferencePtr spValue;

    if(*ppAncestor != nullptr)
    {
        IFR_ASSERT(AsWeakRef(spValue,*ppAncestor));
    }

    UINT32 propertyId;
    pPropertyInfo->GetId(&propertyId);
    m_pProperties->SetValue(propertyId, foundation::AsInspectable(spValue));
    return S_OK;
}

HRESULT CObservableObject::RecalcAncestorProperties(CObservableObjectInfo *pPropertyModelInfo)
{
    foundation::ComPtr<pmod::library::BatchPropertyChangedEventArgsImpl, foundation::ctl::_ComBasePtrImpl> spBatchPropertyChangedEventArgs;
    IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(spBatchPropertyChangedEventArgs.GetAddressOf()));

    _FoundationBaseLock csl(*this);
    IFR_ASSERT(RecalcAncestorProperties(pPropertyModelInfo, spBatchPropertyChangedEventArgs));
    if (spBatchPropertyChangedEventArgs->GetPropertyChanges().size() > 0)
    {
        IFR(FirePropertyChangedEvent(csl,spBatchPropertyChangedEventArgs))
    }
    return S_OK;
}

HRESULT CObservableObject::RecalcAncestorProperties(
    _In_ CObservableObjectInfo *pPropertyModelInfo,
    _In_ pmod::library::BatchPropertyChangedEventArgsImpl *pBatchPropertyChangedEventArgs)
{
    // process base type properties
    if(pPropertyModelInfo->GetBaseType())
    {
        IFR_ASSERT(RecalcAncestorProperties(
            pPropertyModelInfo->GetBaseType(),
            pBatchPropertyChangedEventArgs));
    }

    // process type properties
    for(PropertyInfoVector::const_iterator iter = pPropertyModelInfo->GetProperties().begin();
        iter != pPropertyModelInfo->GetProperties().end();
        ++iter)
    {
        CPropertyInfoBase *pPropertyInfoBase = static_cast<CPropertyInfoBase *>((*iter).Get());
        if(IsPropertyIdContained(pPropertyInfoBase->GetId()) &&
            IsOptionEnabled(pPropertyInfoBase->GetFlags(), foundation::PropertyFlagType::IsAncestor))
        {
            foundation::InspectablePtr spValue;
            if(SUCCEEDED(m_pProperties->GetValue(pPropertyInfoBase->GetId(),spValue.GetAddressOf())) && 
                spValue != nullptr)
            {
                foundation::ComPtr<foundation::IObject> spOldValue;
                IFR_ASSERT(foundation::ResolveWeakRef(AsWeakReference(spValue), *spOldValue.GetAddressOf()));

                // the ancestor property was created and we need to recalc
                foundation::ComPtr<foundation::IObject> spNewValue;
                IFR_ASSERT(RecalcAncestorInternal(pPropertyInfoBase, spNewValue.GetAddressOf()));

                if(spOldValue != spNewValue)
                {
                    foundation::ComPtr<IPropertyChangedEventArgs> spPropertyChangedEventArgs;
                    IFR_ASSERT(CreatePropertyChangedEventArgs(
                        this,
                        pPropertyInfoBase->GetId(),
                        nullptr,
                        spNewValue,
                        spPropertyChangedEventArgs.GetAddressOf()));
                    IFR_ASSERT(pBatchPropertyChangedEventArgs->add_property_changed_event_args(spPropertyChangedEventArgs));
                }
            }
        }
    }
    return S_OK;
}

HRESULT CObservableObject::RecalcRoot()
{
    _FoundationBaseLock csl(*this);

    foundation::ComPtr<foundation::IObject> spRoot;
    IFR_ASSERT(foundation::GetObjectRoot(this, spRoot.GetAddressOf()));
    IObjectNodeClass *pNewRoot = nullptr;
    if(spRoot != nullptr)
    {
        _IFR_(foundation::QueryWeakReference(spRoot, &pNewRoot));
    }
    if(m_pRoot != pNewRoot)
    {
        m_pRoot = pNewRoot;
        if(HasEventHandlers())
        {
            FirePropertyChangedEvent(csl,Property_Root, nullptr, pNewRoot);
        }
    }
    return S_OK;
}

void CObservableObject::RecalcAncestorPropertiesIf()
{
    if (!IsFinalRelease())
    {
        RecalcAncestorProperties(EnsurePropertyModelInfoClassAndContainer());
        if (m_SupportRootProperty)
        {
            RecalcRoot();
        }
    }
}

STDMETHODIMP CObservableObject::_OnAncestorChanged(
    _In_ foundation::library::_INode_impl *pChild,
    _In_opt_ foundation::library::_INode_impl *pParent)
{
    _ObservableObjectBase_Type::_OnAncestorChanged(pChild,pParent);
    RecalcAncestorPropertiesIf();
    return S_OK;
}

STDMETHODIMP CObservableObject::_OnParentChanged(_In_opt_ foundation::library::_INode_impl *pParent)
{
    _ObservableObjectBase_Type::_OnParentChanged(pParent);
    RecalcAncestorPropertiesIf();
    return S_OK;
}

HRESULT CObservableObject::ConvertPropertyValue(
    _In_ IValueConverter *pValueConverter,
    _In_ foundation::IPropertyInfo *pPropertyInfo,
    _In_ foundation::IInspectable *pValue,
    _In_ bool isConvertBack,
    _COM_Outptr_ foundation::IInspectable **ppConvertedValue)
{
    if(pValueConverter)
    {
        foundation::IInspectable *pParameter = foundation::ctl::inspectable::CastToInspectable(this);
        return isConvertBack ?
            pValueConverter->ConvertBack(pValue,pPropertyInfo,pParameter,ppConvertedValue):
            pValueConverter->Convert(pValue,pPropertyInfo,pParameter,ppConvertedValue);
    }
    *ppConvertedValue = pValue;
    if(pValue)
    {
        pValue->AddRef();
    }
    return S_OK;
}

HRESULT CObservableObject::ConvertPropertyValue(
    _In_ foundation::IPropertyInfo *pPropertyInfo,
    _In_ foundation::IInspectable *pValue,
    _In_ bool isConvertBack,
    _COM_Outptr_ foundation::IInspectable **ppConvertedValue)
{
    IValueConverter *pValueConverter = nullptr;
    {
        _FoundationBaseLock csl(*this);
        if (m_spPropertyInfoSupport.get() != nullptr)
        {
            UINT32 propertyId;
            pPropertyInfo->GetId(&propertyId);
            _MapPropertyInfoInternalType::const_iterator iter = m_spPropertyInfoSupport->find(propertyId);
            if (iter != m_spPropertyInfoSupport->end())
            {
                pValueConverter = (*iter).second._spValueConverter;
            }
        }
    }
    return ConvertPropertyValue(pValueConverter,
        pPropertyInfo,
        pValue,
        isConvertBack,
        ppConvertedValue);
}

STDMETHODIMP CObservableObject::GetProperty(UINT32 propertyId, foundation::IInspectable** ppValue)
{
    PERF_CONTEXT(Core_PerfId_PropertyModel_GetProperty_Id)
    
    if (ppValue == nullptr)
    {
        // Protect property model implementors from having to check the pointer.
        return E_INVALIDARG;
    }

    // Intrinsic Properties
    if(propertyId == Property_Parent)
    {
        foundation::IObject *pParent;
        IFR_ASSERT(GetParentNode(&pParent));
        return foundation::InspectablePtr(pParent).CopyTo(ppValue);
    }
    else if(propertyId == Property_Root)
    {
        if(m_pRoot)
        {
            // A not null Root will be returned
            return foundation::InspectablePtr(m_pRoot).CopyTo(ppValue);
        }
        else
        {
            // Evaluate the 'Root' now
            foundation::ComPtr<foundation::IObject> spRoot;
            IFR_ASSERT(foundation::GetObjectRoot(this, spRoot.GetAddressOf()));
            return foundation::InspectablePtr(spRoot).CopyTo(ppValue);
        }
    }
    else if(propertyId == Property_IsInModelChanges)
    {
        bool value;
        IFR_ASSERT(GetIsInModelChangesInternal(value));
        return foundation::pv_util::CreateBooleanValue(value, ppValue);
    }

    foundation::library::IDispatcher *pGetPropertyDispatcher = GetWorkingDispatcherForRuntimeClassOption(
        propertyId,
        this->m_UseGetPropertyDispatcher,
        PropertyRuntimeClassOptions::UseWorkingDispatcherOnGetProperty);

    if (pGetPropertyDispatcher != nullptr)
    {
        return foundation::RunSync(pGetPropertyDispatcher,
            [=] { return this->GetPropertyInternal(propertyId, true, ppValue); });
    }

    return this->GetPropertyInternal(propertyId, false, ppValue);
}

HRESULT CObservableObject::GetPropertyInternal(
    _In_ UINT32 propertyId,
    _In_ bool isWorkingDispatcher,
    _Outptr_ foundation::IInspectable** ppValue)
{
    auto traceParameterCallback = CreateLogTraceCallback(U("propertyId"), propertyId);

    foundation::IPropertyInfo *pPropertyInfo = nullptr;
    IFR(GetPropertyInfoById(propertyId, &pPropertyInfo));

    foundation::InspectablePtr spValue;
    foundation::ComPtr<IValueConverter> spValueConverter;

    // support for per property info
    _FoundationBaseLock csl(*this);
    if (m_spPropertyInfoSupport.get() != nullptr)
    {
        _MapPropertyInfoInternalType::const_iterator iter = m_spPropertyInfoSupport->find(propertyId);
        if (iter != m_spPropertyInfoSupport->end())
        {
            // if binding attached
            if (iter->second._spPropertyBinding != nullptr)
            {
                IFR(iter->second._spPropertyBinding->GetValue(spValue.GetAddressOf()));
                return spValue.CopyTo(ppValue);
            }
            spValueConverter = iter->second._spValueConverter;
        }
    }

    HRESULT hr = TryGetPropertyInternal(propertyId, /*initialLock*/false, spValue);
    bool propertyAvailable;
    IFHR(IsPropertyAvailable(hr, propertyAvailable));


    UINT32 flags = 0;
    IFR_ASSERT(pPropertyInfo->GetFlags(&flags));

    if (IsOptionEnabled(flags, foundation::PropertyFlagType::IsAncestor))
    {
        foundation::ComPtr<foundation::IObject> spAncestor;
        if (propertyAvailable && spValue != nullptr)
        {
            IFR_ASSERT(foundation::ResolveWeakRef(AsWeakReference(spValue), *spAncestor.GetAddressOf()));
        }
        else
        {
            IFR_ASSERT(RecalcAncestorInternal(pPropertyInfo, spAncestor.GetAddressOf()));
        }
        return foundation::InspectablePtr(spAncestor).CopyTo(ppValue);
    }

    // logging support for 'GetProperty Call'
    if (IsLevelEnabled(foundation::LoggingLevel::Trace,
        foundation::ObjectDispatch_Category_GetProperty_Id))
    {
        LogGetProperty((UINT32)this->m_UniqueId, propertyId);
    }

    if (m_pDelegate != nullptr)
    {
        IFHR_PARAM_CALLBACK(m_pDelegate->OnGetProperty(propertyId, spValue.GetAddressOfPtr()), traceParameterCallback);
    }

    if (!propertyAvailable && hr != S_OK)
    {
        // our Container does not have the property but also the delegate callback did not fully succeeded
        IFHR_PARAM_CALLBACK(foundation::Errors::E_PROPERTY_NOT_FOUND, traceParameterCallback);
    }

    if (IsOptionEnabled(flags, foundation::PropertyFlagType::IsWeakReference) && spValue != nullptr)
    {
        foundation::IInspectable *pStrongRef = nullptr;
        // convert from Weak Reference storage value
        IFR_ASSERT(foundation::ResolveWeakRefWeak(AsWeakReference(spValue), foundation_IID_IInspectable, pStrongRef));

        spValue = pStrongRef;
    }

    return ConvertPropertyValue(
        spValueConverter,
        pPropertyInfo,
        spValue,
        false,
        ppValue);
}

STDMETHODIMP CObservableObject::SetProperty(_In_ UINT32 propertyId,_In_ foundation::IInspectable *value)
{
    return InternalSetProperty(propertyId, value,false,true);
}

STDMETHODIMP CObservableObject::InvokeMethod(UINT32 methodId,_In_ UINT32 size,foundation::IInspectable **parameters,foundation::IInspectable **ppResult)
{
    PERF_CONTEXT(Core_PerfId_PropertyModel_Invoke_Id)
    
    if (m_pDelegate == nullptr && m_MethodOnInvokeDelegate == nullptr)
    {
        return E_NOTIMPL;
    }

    // copy parameters so we can do type safe checking conversion
    _Inspectable_ptr_vector_type typeParameters;
    CopyArrayToVector(size,parameters,typeParameters);

    foundation::IMethodInfo *pMethodInfo = nullptr;
    bool isAsync = false;
    GUID iidTypeInfo = this->_ObservableObjectBase_Type::GetTypeInfo();
    if(!
        (IsEqualIID(iidTypeInfo,IID_IObservableObject) // Object does not have Custom Type Info
        || 
        IsUseQuirksMode()                           // Quirks mode does not have method support checking
        )
     )
    {
        if(!GetPropertyModelInfoClass()->TryGetMethodInfoById(methodId,&pMethodInfo))
        {
            return foundation::Errors::E_METHOD_NOT_DEFINED;
        }

        // verify arguments
        foundation::ArrayRefCountWrapper<foundation::IParameterInfo> methodParameters;
        IFR_ASSERT(pMethodInfo->GetParameters(methodParameters.GetSizeAddressOf(),methodParameters.GetBufferAddressOf()));
        if(methodParameters.GetSize() != size)
        {
            return E_INVALIDARG;
        }

        for(UINT32 index = 0;index < size; ++index)
        {
            CParameterInfo *pParameterInfo = static_cast<CParameterInfo *>(methodParameters[index]);
            if (FAILED(CModelTypeInfoStatics::_VerifyPropertyTypeInfo(
                pParameterInfo,typeParameters[index].GetAddressOfPtr())))
            {
                //PropertyModelHelper::TracePropertyError(this,propertyId,__WFILE__,__LINE__,foundation::Errors::E_WRONG_TYPE,L"foundation::Errors::E_WRONG_TYPE");
                return foundation::Errors::E_WRONG_TYPE;
            }
        }

        foundation::ComPtr<foundation::IPropertyTypeInfo> spResultTypeInfo;
        IFR_ASSERT(pMethodInfo->GetResultTypeInfo(spResultTypeInfo.GetAddressOf()));

        foundation::library::IDispatcher *pWorkingDispatcher = this->GetWorkingDispatcherInternal();
        pMethodInfo->GetIsAsync(&isAsync);

        bool bReturnAsyncOperation = isAsync || 
            (
                m_AllowReturnAsyncOnVoidResult &&
                (pWorkingDispatcher && IsPropertyTypeInfoVoid(spResultTypeInfo))
            );

        if(bReturnAsyncOperation)
        {
            foundation::AsyncOperationClassPtr spAsyncOperationClass;

            // create the Async Operation Class
            IFR_ASSERT(foundation::library::CreateAsyncOperationClass(
                spResultTypeInfo,
                foundation::library::ToFreeThreadApartmentOptions(foundation::AsyncOperationOptions::None),
                spAsyncOperationClass.GetAddressOf()));

            // have the async operation to 'inherit' the current event dispatcher
            IFR_ASSERT(spAsyncOperationClass.SetEventDispatcher(this->_GetEventDispatcher()));

            // set the ppResult to IAsyncOperationModel
            *ppResult = spAsyncOperationClass.Detach();
        }

        if (pWorkingDispatcher != nullptr && !foundation::HasThreadAccess(pWorkingDispatcher))
        {
            IInspectable *pAsyncOperationClass = nullptr;
            if (bReturnAsyncOperation)
            {
                pAsyncOperationClass = (*ppResult);
                // keep alive Async Operation 
                pAsyncOperationClass->AddRef();
            }

            foundation::ComPtr<foundation::IAsyncOperation> spAsyncAction;
            // AddRef to keep us alive during the async invocation
            foundation::IInspectable *pThis = foundation::ctl::inspectable::CastToInspectable(this);
            pThis->AddRef();

            IFR_ASSERT(foundation::RunAsync(pWorkingDispatcher,
                [=]() {
                // attach to the AddRef'ed so we balance the Ref Count
                foundation::InspectablePtr context_ptr;
                context_ptr.Attach(pThis);

                return InvokeAsyncInternal(
                    bReturnAsyncOperation,
                    pMethodInfo,
                    methodId,
                    pAsyncOperationClass,
                    typeParameters,
                    bReturnAsyncOperation ? nullptr : ppResult
                    );
                },
                spAsyncAction.GetAddressOf()));

            // if not Async we will have to Wait for the Worker Dispatcher and copy results
            if (!bReturnAsyncOperation && spAsyncAction != nullptr)
            {
                // Wait sync until method is completed
                IFR_ASSERT(spAsyncAction->Wait(foundation::_InfiniteTimeout, nullptr));
                HRESULT hrInvoke = S_OK;
                IFR_ASSERT(spAsyncAction->GetError(&hrInvoke));
                return hrInvoke;
            }

            return S_OK;
        }
    }

    return InvokeInternal(false,
        isAsync,
        pMethodInfo,
        methodId,
        size,
        size ? typeParameters.front().GetAddressOfPtr() : nullptr,
        ppResult);
}

HRESULT CObservableObject::InvokeAsyncInternal(
    bool bReturnAsyncOperation, 
    _In_ foundation::IMethodInfo *pMethodInfo,
    _In_ UINT32 methodId,
    IInspectable *pAsyncOperationClass,
    const _Inspectable_ptr_vector_type& typeParameters,
    foundation::IInspectable **ppResult)
{
    foundation::InspectablePtr spResult;
    if (bReturnAsyncOperation)
    {
        spResult.Attach(pAsyncOperationClass);
    }

    HRESULT hr = InvokeInternal(
        true,
        true,
        pMethodInfo,
        methodId,
        (UINT32)typeParameters.size(),
        typeParameters.size() ? ((foundation::InspectablePtr&)typeParameters.front()).GetAddressOfPtr() : nullptr,
        spResult.GetAddressOfPtr());

    if (!bReturnAsyncOperation)
    {
        spResult.CopyTo(ppResult);
    }
    else if (FAILED(hr))
    {
        // return async operation but the callback implementation failed
        // Note: the callback could have return a failure HRESULT but miss
        // failing the async operation. We will ensure to fail the operation to 
        // unblock the calling code
        foundation::library::IAsyncOperationClass *pAsyncOperationClass_ = nullptr;
        foundation::QueryWeakReference(
            pAsyncOperationClass,
            &pAsyncOperationClass_);

        pAsyncOperationClass_->SetCompleted(hr, nullptr);
    }

    return hr;
}

HRESULT CObservableObject::InvokeInternal(
    _In_ bool isDispatched,
    _In_ bool isAsync,
    _In_ foundation::IMethodInfo *pMethodInfo,
    _In_ UINT32 methodId,
    _In_ UINT32 size,
    _In_ foundation::IInspectable **parameters,
    foundation::IInspectable **ppResult)
{    
    PERF_CONTEXT(Core_PerfId_PropertyModel_InvokeCallback_Id)
    
    foundation::IInspectable **ppLocalResult = ppResult;
    if(isAsync && *ppResult != nullptr)
    {
        foundation::IAsyncOperation *pAsyncOperation = reinterpret_cast<foundation::IAsyncOperation *>(*ppResult);

        foundation::QueryWeakReference(
            pAsyncOperation,
            (foundation::library::IAsyncOperationClass **)ppLocalResult);
        foundation_assert(*ppLocalResult);
        // return if operation was canceled
        foundation::ResultStatus status;
        IFR_ASSERT(pAsyncOperation->GetStatus(&status));
        if (status == foundation::ResultStatus::Canceled)
        {
            // no point in invoking any delegate at this point so return
            return S_FALSE;
        }
    }

    // logging support for 'OnInvoke Call' 
    if (IsLevelEnabled(foundation::LoggingLevel::Info,
            foundation::ObjectDispatch_Category_Invoke_Id))
    {
        LogInvokeMethod((UINT32)this->m_UniqueId, pMethodInfo, methodId, size, parameters);
    }

    HRESULT hr = S_FALSE;
    // if delegate is set we call if the call returns S_OK the main Execute is not called. 
    // if the call returns S_FALSE is used only to spy the calls.
    if (m_MethodOnInvokeDelegate != nullptr)
    {
        hr = m_MethodOnInvokeDelegate->Invoke(pMethodInfo, methodId, size, parameters, ppLocalResult);
        IFHR_OK(hr);
    }

    if (hr == S_FALSE && m_pDelegate !=nullptr)
    {
        hr = m_pDelegate->OnInvokeMethod(pMethodInfo, methodId, size, parameters, ppLocalResult);
        IFHR(hr);
    }

    return hr;
}

STDMETHODIMP CObservableObject::GetMethodOnInvokeDelegate(_COM_Outptr_ pmod::library::IMethodOnInvokeDelegate **ppMethodOnInvokeDelegate)
{
    m_MethodOnInvokeDelegate.CopyTo(ppMethodOnInvokeDelegate);
    return S_OK;
}

STDMETHODIMP CObservableObject::SetMethodOnInvokeDelegate(_In_ pmod::library::IMethodOnInvokeDelegate *pMethodOnInvokeDelegate)
{
    m_MethodOnInvokeDelegate = pMethodOnInvokeDelegate;
    return S_OK;
}

STDMETHODIMP CObservableObject::GetPropertyChangedEventSource(_Outptr_ IPropertyChangedEventSource** ppEventSource)
{
    IFR_ASSERT(EnsureEventSourceModel());

    * ppEventSource =  this->_ObservableObjectBase_Type::GetEventSourceImpl();
    (*ppEventSource)->AddRef();
    return S_OK;
}

STDMETHODIMP CObservableObject::GetEventModelEventSource(_Outptr_ IEventModelEventSource** ppEventSource)
{
    _FoundationBaseLock csl(*this);

    if (!this->_EventModelSourceType::IsEventHandlerSource())
    {
        // Initialize our EventModel Source
        IFR_ASSERT(this->_EventModelSourceType::_CreateEventSource(this, this->m_pCritsec));
    }

    * ppEventSource =  this->_EventModelSourceType::GetEventSourceImpl();
    (*ppEventSource)->AddRef();
    return S_OK;
}

HRESULT  CObservableObject::CreateInstance(
    _In_ const pmod::library::ObservableObjectCreateParameters *pParameters,
    _In_ foundation::IInspectable *pOuter,
    _Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CObservableObject>(pParameters, pOuter, ppNewInstance);
}

STDMETHODIMP CObservableObject::SetPropertyInternal(_In_ UINT32 propertyId, _In_ foundation::IInspectable *newValue,_In_ bool fireNotifyChanged)
{
    return InternalSetProperty(propertyId,newValue,true,fireNotifyChanged);
}

STDMETHODIMP CObservableObject::FirePropertyChanged(_In_ UINT32 propertyId, _In_ bool useCallback)
{
    foundation::InspectablePtr value;
    if (useCallback || (propertyId == Property_All))
    {
        // create empy value in case 'InModelChanges' state
        value = foundation::pv_util::CreateValue();
    }
    else
    {
        // retrieve current value
        IFR(GetProperty(propertyId,value.GetAddressOf()));
    }

    _FoundationBaseLock csl(*this);

    if(IsInModelChanges() && propertyId != pmod::Property_IsInModelChanges)
    {
        // store in 'Model Changes' dictionary
        m_ModelChanges->operator[](propertyId) = _TValueChanged(value,value);
    }
    else
    {
        if (useCallback)
        {
            if (this->IsObserved())
            {
                // notify with callback
                foundation::ComPtr<IPropertyChangedEventArgs> spPropertyChangedEventArgs;
                IFR_ASSERT(pmod::library::PropertyChangedEventArgsWIthCallbackImpl::CreateInstance(
                    static_cast<IObservableObject *>(this),
                    propertyId,
                    spPropertyChangedEventArgs.GetAddressOf()));

                return FirePropertyChangedEvent(csl,spPropertyChangedEventArgs);
            }
        }
        else
        {
            // notify with current property value
            return FirePropertyChangedEvent(csl,propertyId, value, value);
        }
    }
    return S_OK;
}

STDMETHODIMP CObservableObject::SetBinding(_In_ UINT32 propertyId,_In_ IBindingBase *pPropertyBinding)
{
    IFCPTR(pPropertyBinding);
    return SetBindingInternal(propertyId,pPropertyBinding);
}

STDMETHODIMP CObservableObject::ClearBinding(_In_ UINT32 propertyId)
{
    return ClearBindingInternal(propertyId);
}

STDMETHODIMP CObservableObject::SetPropertyConverter(_In_ UINT32 propertyId,_In_ pmod::library::IValueConverter *pValueConverter)
{
    _FoundationBaseLock csl(*this);

    EnsurePropertyInfoSupport_NoLock();
    // Add to our Map
    _PropertyClassInfoInternal propertyInfoInternal;
    _MapPropertyInfoInternalType::const_iterator iter = m_spPropertyInfoSupport->find(propertyId);

    if(iter != m_spPropertyInfoSupport->end())
    {
        propertyInfoInternal = (*iter).second;
    }
    propertyInfoInternal._spValueConverter = pValueConverter;
    m_spPropertyInfoSupport->operator[](propertyId) = propertyInfoInternal;
    return S_OK;
}

HRESULT CObservableObject::GetPropertyId(_In_ HSTRING propertyName,_Outptr_ UINT32 *propertyId)
{
    return CObjectDispatch::_GetPropertyId(
        GetPropertyModelInfoClass(),
        propertyName,
        propertyId);
}

foundation::library::IDispatcher *CObservableObject::GetWorkingDispatcherForRuntimeClassOption(
    _In_ UINT32 propertyId,
    _In_ bool localOption,
    _In_ PropertyRuntimeClassOptions propertyRuntimeOptions)
{
    foundation::library::IDispatcher *pWorkingDispatcher;

    if ((
        localOption ||
        IsPropertyRuntimeClassOptionEnabled(propertyId, propertyRuntimeOptions)
        ) &&
        (pWorkingDispatcher = this->GetWorkingDispatcherInternal()) != nullptr &&
        !foundation::HasThreadAccess(pWorkingDispatcher))
    {
        return pWorkingDispatcher;
    }
    return nullptr;
}


HRESULT CObservableObject::TryGetPropertyInternal(
    _In_ UINT32 propertyId,
    _In_ bool initialLock,
    _Outptr_ foundation::InspectablePtr& value)
{
    _FoundationBaseLock csl(*this, initialLock ? FoundationLockState::InitialLock: FoundationLockState::None);
    IFR_ASSERT(EnsurePropertiesContainer());
    return m_pProperties->GetValue(propertyId, value.GetAddressOf());
}

HRESULT CObservableObject::StorePropertyInternal(
    _In_ foundation::IPropertyInfo *pPropertyInfo,
    _In_ foundation::IInspectable * oldValue,
    _In_ foundation::IInspectable *newValue)
{
    foundation_assert(pPropertyInfo);

    foundation::PropertyType type;
    pPropertyInfo->GetPropertyType(&type);
    bool isInspectable = type == foundation::PropertyType_Inspectable;

    IFR_ASSERT(EnsurePropertiesContainer());
    if(isInspectable)
    {
        IFR_ASSERT(ClearParentNodeImpl(oldValue));
    }
    UINT32 propertyId;
    pPropertyInfo->GetId(&propertyId);

    IFR_ASSERT(m_pProperties->SetValue(propertyId, newValue));

    if(isInspectable)
    {
        IFR_ASSERT(SetAsParentIf(newValue));
    }
    return S_OK;
}

HRESULT CObservableObject::GetPropertyInfoById(_In_ UINT32 propertyId,_Outptr_ foundation::IPropertyInfo **ppPropertyInfo)
{
    if(!EnsurePropertyModelInfoClassAndContainer()->TryGetPropertyInfoById(propertyId,ppPropertyInfo))
    {
        return foundation::Errors::E_PROPERTY_NOT_DEFINED;
    }
    return S_OK;
}

HRESULT CObservableObject::SetAsParentIf(foundation::IInspectable *newValue)
{
    if(!m_NoAutoParent && !IsFinalRelease())
    {
        IFR_ASSERT(SetAsParentNodeImpl(newValue));	
    }
    return S_OK;
}

HRESULT CObservableObject::InternalSetProperty(
    _In_ UINT32 propertyId,
    _In_ foundation::IInspectable *newVal,
    _In_ bool isProtected,
    _In_ bool fireNotifyChanged)
{
    PERF_CONTEXT(Core_PerfId_PropertyModel_SetPropertyInternal_Id)

    foundation::library::IDispatcher *pSetPropertyDispatcher = GetWorkingDispatcherForRuntimeClassOption(
        propertyId,
        this->m_UseSetPropertyDispatcher,
        PropertyRuntimeClassOptions::UseWorkingDispatcherOnSetProperty);

    if (pSetPropertyDispatcher != nullptr)
    {
        return foundation::RunSync(pSetPropertyDispatcher,
            [=] { return this->InternalSetProperty(propertyId, true,newVal, isProtected, fireNotifyChanged); });
    }

    return this->InternalSetProperty(propertyId, false,newVal, isProtected, fireNotifyChanged);

}

HRESULT CObservableObject::IsPropertyAvailable(HRESULT hr, bool& propertyAvailable)
{
    if (hr == foundation::Errors::E_PROPERTY_NOT_FOUND || hr == E_BOUNDS)
    {
        // HRESULT match mapped to property not available
        propertyAvailable = false;
        return S_FALSE;
    }
    else
    {
        propertyAvailable = SUCCEEDED(hr);
        return hr;
    }
}

HRESULT CObservableObject::InternalSetProperty(
    _In_ UINT32 propertyId, 
    _In_ bool isWorkingDispatcher,
    _In_ foundation::IInspectable *newVal,
    _In_ bool isProtected,
    _In_ bool fireNotifyChanged)
{
    auto traceParameterCallback = CreateLogTraceCallback(U("propertyId"), propertyId);

    if(propertyId >= foundation::Property_ResevervedMask)
    {
        IFR_PARAM_CALLBACK(
            foundation::Errors::E_PROPERTY_READ_ONLY,
            traceParameterCallback);
    }

    foundation::IPropertyInfo *pPropertyInfo = nullptr;
    IFR(GetPropertyInfoById(propertyId,&pPropertyInfo));

    UINT32 flags = 0;
    IFR_ASSERT(pPropertyInfo->GetFlags(&flags));

    if((IsOptionEnabled(flags, foundation::PropertyFlagType::IsAncestor)) != 0 )
    {
        IFR_PARAM_CALLBACK(
            foundation::Errors::E_PROPERTY_READ_ONLY,
            traceParameterCallback);
    }

    // Check if we can write to a property from the public API
    if(!isProtected &&  !IsOptionEnabled(flags, foundation::PropertyFlagType::CanWrite))
    {
        if(!IsUseQuirksMode())
        {
            IFR_PARAM_CALLBACK(
                foundation::Errors::E_PROPERTY_READ_ONLY,
                traceParameterCallback);
        }
    }

    // Check if Weak reference Type
    bool isWeakReference = IsOptionEnabled(flags, foundation::PropertyFlagType::IsWeakReference);

    foundation::InspectablePtr newValue(newVal);

    if (FAILED(CModelTypeInfoStatics::_VerifyPropertyTypeInfo(
        pPropertyInfo,newValue.GetAddressOfPtr())))
    {
        IFR_PARAM_CALLBACK(foundation::Errors::E_WRONG_TYPE, traceParameterCallback);
    }

    // lock this object until the firing
    _FoundationBaseLock csl(*this);

    // in case a binding property was defined before
    ClearBindingInternal(propertyId);

    foundation::InspectablePtr oldValue;
    HRESULT hr = TryGetPropertyInternal(propertyId, /*initialLock*/false, oldValue);
    bool propertyAvailable;
    IFHR(IsPropertyAvailable(hr, propertyAvailable));

    if(isWeakReference)
    {
        // convert oldValue to Model
        if(oldValue != nullptr)
        {
            foundation::IInspectable *pStrongRef;
            // convert from Weak Reference storage value
            IFR_ASSERT(foundation::ResolveWeakRefWeak(AsWeakReference(oldValue), foundation_IID_IInspectable, pStrongRef));

            oldValue = pStrongRef;
        }
    }

    if (propertyAvailable && foundation::pv_util::CompareInspectableValues(oldValue, newValue) == 0)
    {
        if((static_cast<CPropertyInfoBase *>(pPropertyInfo))->GetPropertyType() == foundation::PropertyType_Inspectable)
        {
            // even if the property exists we will attempt to establish parent/child relationship
            // Note: this code typicall runs when custom container already report an existing property during initialize
            // trough code generated template
            this->SetAsParentIf(newValue);
        }
        return S_FALSE;
    }
    // For non protected access we want to invoke the Delegate to validate this update
    if ((!isProtected || m_ClassOnSetProperty) && m_pDelegate != nullptr)
    {
        HRESULT hr = m_pDelegate->OnSetProperty(propertyId, !isProtected,oldValue, newValue);
        IFR(hr);
        if (hr == S_FALSE)
        {
            // The delegate will fire an event change notification itself.
            // The underlying property bag should not be updated yet.
            return S_OK;
        }
        // The delegate allows this update -- set the property and fire the event
    }

    foundation::InspectablePtr _newValue(newValue); // actual value to store in our container

    if(isWeakReference && newValue != nullptr)
    {
        IFR_ASSERT(AsWeakRef(
           *(reinterpret_cast<foundation::IWeakReference **>(_newValue.ReleaseAndGetAddressOf())),
           newValue));
    }
    else if(!isProtected)
    {
        IFR(ConvertPropertyValue(pPropertyInfo,newValue,true,_newValue.ReleaseAndGetAddressOf()));
    }

    // logging support for 'SetProperty Call'
    if (IsLevelEnabled(foundation::LoggingLevel::Trace,
        foundation::ObjectDispatch_Category_SetProperty_Id))
    {
        LogSetProperty((UINT32)this->m_UniqueId, propertyId);
    }

    if(IsInModelChanges())
    {
        // Store in our Model Changes Map
        m_ModelChanges->operator[](propertyId) = _TValueChanged(oldValue,newValue);
        // Update the value now if not in transaction mode
        if(!m_UseModelTransaction)
        {
            IFR(StorePropertyInternal(pPropertyInfo, oldValue, _newValue));
        }
    }
    else
    {
        IFR(StorePropertyInternal(pPropertyInfo, oldValue, _newValue));
        if(fireNotifyChanged)
        {
            IFR(FirePropertyChangedEvent(csl,propertyId, oldValue, newValue));
        }
    }
    return S_OK;
}

HRESULT CObservableObject::CreatePropertyChangedEventArgs(
    _In_ IObservableObject *pSource,
    _In_ UINT32 propertyId,
    _In_ foundation::IInspectable *oldValue,
    _In_ foundation::IInspectable *newValue,
    _Outptr_ IPropertyChangedEventArgs **ppPropertyChangedEventArgs)
{
    return pmod::library::PropertyChangedEventArgsImpl::CreateInstance(
        pSource,
        propertyId,
        oldValue,
        newValue,
        ppPropertyChangedEventArgs);
}

HRESULT CObservableObject::FirePropertyChangedEvent(
    _FoundationBaseLock& csl,
    _In_ IObservableObject *pSource,
    _In_ UINT32 propertyId,
    _In_ foundation::IInspectable *oldValue,
    _In_ foundation::IInspectable *newValue)
{
    if(this->IsObserved())
    {
        foundation::ComPtr<IPropertyChangedEventArgs> spPropertyChangedEventArgs;
        IFR_ASSERT(CreatePropertyChangedEventArgs(
            pSource,
            propertyId,
            oldValue,
            newValue,
            spPropertyChangedEventArgs.GetAddressOf()));
        
        return FirePropertyChangedEvent(csl,spPropertyChangedEventArgs);
    }
    return S_OK;
}

HRESULT CObservableObject::FirePropertyChangedEvent(
    _FoundationBaseLock& csl,
    _In_ UINT32 propertyId,
    _In_ foundation::IInspectable *oldValue,
    _In_ foundation::IInspectable *newValue)
{
    return FirePropertyChangedEvent(
        csl,
        static_cast<IObservableObject *>(this),
        propertyId,
        oldValue,
        newValue
        );
}

HRESULT CObservableObject::FirePropertyChangedEvent(
    _FoundationBaseLock& csl,
    _In_ IPropertyChangedEventArgs* pEventArgs)
{
    PERF_CONTEXT(Core_PerfId_PropertyModel_FirePropertyChanged_Id)

    return FireWithCallback(csl,&_ObservableObjectBase_Type::FireEvent, pEventArgs);
}

HRESULT CObservableObject::PerformBatchFirePropertyChanged(
    _In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
    _In_ IBatchPropertyChangedEventArgs* pEventArgs)
{
    if (this->_ObservableObjectBase_Type::GetEventSourceImpl() != nullptr)
    {
        this->_ObservableObjectBase_Type::GetEventSourceImpl()->FireEventWithInvoke<PropertyChangedEventHandler>(
            pEventHandlerContainer,
            AsInterface(),
            pEventArgs);
    }
    return S_OK;
}

HRESULT CObservableObject::FirePropertyChangedEvent(
    _FoundationBaseLock& csl,
    _In_ IBatchPropertyChangedEventArgs* pEventArgs)
{
    PERF_CONTEXT(Core_PerfId_PropertyModel_FireBatchPropertyChanged_Id)
    
        return FireWithCallback(csl,&CObservableObject::PerformBatchFirePropertyChanged, pEventArgs);
}

HRESULT CObservableObject::PerformFireEventModel(
    _In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
    _In_ IEventModelEventArgs* pEventArgs)
{
    if(this->_EventModelSourceType::GetEventSourceImpl() != nullptr)
    {
        this->_EventModelSourceType::GetEventSourceImpl()->FireEvent(
            pEventHandlerContainer,
            AsInterface(),
            pEventArgs);
    }
    return S_OK;
}

STDMETHODIMP CObservableObject::FireEventModel(_In_ UINT32 eventId,_In_ foundation::IInspectable *pEventArgs)
{
    foundation::InspectablePtr spEventArgs(pEventArgs);
    GUID iidTypeInfo = this->_ObservableObjectBase_Type::GetTypeInfo();
    if(!IsEqualIID(iidTypeInfo,IID_IObservableObject))
    {
        IEventInfo *pEventInfo = nullptr;
        if(!GetPropertyModelInfoClass()->TryGetEventInfoById(eventId,&pEventInfo))
        {
            return pmod::Errors::E_EVENT_NOT_DEFINED;
        }
        // Verify Event Args Type
        foundation::ComPtr<foundation::IPropertyTypeInfo> spEventArgsTypeInfo;
        IFR(pEventInfo->GetEventArgsTypeInfo(spEventArgsTypeInfo.GetAddressOf()));
        IFR(CModelTypeInfoStatics::_VerifyPropertyTypeInfo(
            static_cast<foundation::IPropertyTypeInfo *>(spEventArgsTypeInfo),
            spEventArgs.GetAddressOfPtr()));
   }

    foundation::ComPtr<IEventModelEventArgs> spEventModelEventArgs;
    IFR_ASSERT(pmod::library::EventModelEventArgsImpl::CreateInstance(eventId, spEventArgs, spEventModelEventArgs.GetAddressOf()));
    
    PERF_CONTEXT(Core_PerfId_PropertyModel_FireEventModel_Id)
    return FireWithCallback(&CObservableObject::PerformFireEventModel, (IEventModelEventArgs *)spEventModelEventArgs);
}

STDMETHODIMP CObservableObject::EnsurePropertiesContainer()
{
    if (m_pProperties == nullptr)
    {
        _FoundationBaseLock csl(*this);
        if (m_pProperties == nullptr)
        {
            EnsurePropertiesContainerInternal();
        }
    }
    return S_OK;
}

STDMETHODIMP CObservableObject::GetPropertiesContainer(_COM_Outptr_ foundation::library::IPropertiesContainer **ppPropertiesContainer)
{
    IFR(EnsurePropertiesContainer());
    foundation_assert(m_pProperties != nullptr);
    *ppPropertiesContainer = m_pProperties;
    m_pProperties->AddRef();
    return S_OK;
}

HRESULT CObservableObject::EnsurePropertiesContainerInternal()
{
    foundation_assert(m_pProperties == nullptr);

    CObservableObjectInfo* pPropertyModelInfo = GetPropertyModelInfoClass();
    foundation_assert(pPropertyModelInfo);

    IFR_ASSERT(pPropertyModelInfo->EnsurePropertyInfoContainer());

    IObservableObjectInfo *pPropertyModelInfoInterface = pPropertyModelInfo->As<IObservableObjectInfo>();

    UINT32 containerSize = -1;
    if (pPropertyModelInfo->HasStaticContainer())
    {
         // +1, since most of the properties start with index 1.
        containerSize = (UINT32)pPropertyModelInfo->GetCountProperties() +1;
    }

    if(m_pDelegate)
    {
        IFR(m_pDelegate->OnCreatePropertiesContainer(
            pPropertyModelInfoInterface,
            containerSize,
            &m_pProperties));
    }

    if(m_pProperties == nullptr)
    {
        m_IsContainerWeakRef = false;
        if (!this->IsLocalObjectTypeInfo() && containerSize != (UINT32)-1)
        {
            if (m_UseFastPropertiesContainer)
            {
                foundation::library::FastPropertiesContainerParameters parameters;
                parameters._pPropertyDataInfo = pPropertyModelInfo->As<IObservableObjectInfo>();
                IFR_ASSERT(CFastPropertiesContainer::CreateInstance(
                    &parameters,
                    reinterpret_cast<foundation::library::IFastPropertiesContainer **>(&m_pProperties)));
            }
            else
            {
                IFR_ASSERT(CStaticPropertiesContainer::CreateInstance(
                    containerSize,
                    &m_pProperties));
            }
        }
        else
        {
            IFR_ASSERT(CDynamicPropertiesContainer::CreateInstance(&m_pProperties));
        }
    }
    return S_OK;
}

HRESULT CObservableObject::SetTypeInfo(_In_ const IID& iidType)
{
    // if Type is GUID_NULL we will create a dynamic info
    if (IsEqualIID(iidType, foundation_GUID_NULL))
    {
        foundation::HStringRef name(U("PropertyModel"));

        // Create a 'runtime' Map Model Info Type
        foundation::ComPtr<IObservableObjectInfo> spPropertyModelInfo;
        IFR_ASSERT(pmod::library::GetModelTypeInfoFactory()->CreateObservableObjectInfo
            (
            0,
            IID_IObservableObject,
            name,
            nullptr,
            spPropertyModelInfo.GetAddressOf()
            ));
        IFR_ASSERT(this->_ObservableObjectBase_Type::SetTypeInfo(spPropertyModelInfo));
        return S_OK;
    }
    return _ObservableObjectBase_Type::SetTypeInfo(iidType);
}

STDMETHODIMP CObservableObject::_SetParent(
    foundation::library::_INode_impl *pThisNode, 
    foundation::library::_INode_impl *pParent)
{
    // lock protection required 
    _FoundationBaseLock csl(*this);

    // Change the effective 'Parent'
    IFR_ASSERT(_ObservableObjectBase_Type::_SetParent(pThisNode, pParent));

    foundation::IObject *pNewParent = nullptr;
    if (pParent != nullptr)
    {
        IFR_ASSERT(foundation::QueryWeakReference(pParent,&pNewParent));
    }
    // Notify 'Parent' property
    FirePropertyChangedEvent(
        csl,
        Property_Parent,
        nullptr,
        pNewParent);

    return S_OK;
}

HRESULT CObservableObject::OnBindingValueChanged(foundation::IUnknown *pSender, IBindingValueChangedEventArgs *pArgs)
{
    UNREFERENCED_PARAMETER(pArgs);

    // lock protection required 
    _FoundationBaseLock csl(*this);

    foundation_assert(m_spPropertyInfoSupport.get());

    foundation::ComPtr<IBindingBase> spPropertyBinding;
    IFR_ASSERT(foundation::QueryInterface(pSender, spPropertyBinding.GetAddressOf()));

    for(_MapPropertyInfoInternalType::iterator iter = m_spPropertyInfoSupport->begin();
        iter != m_spPropertyInfoSupport->end();
        ++iter)
    {
        if(iter->second._spPropertyBinding == spPropertyBinding)
        {
            IFR(FireBindingValue(csl,spPropertyBinding, iter->first));
            return S_OK;
        }
    }
    // we should not arrive here
    foundation_assert(false);
    return S_FALSE;
}

HRESULT CObservableObject::FireBindingValue(
    _FoundationBaseLock& csl,
    _In_ IBindingBase *pPropertyBinding, 
    _In_ int propertyId)
{
    foundation_assert(pPropertyBinding);

    foundation::InspectablePtr value;
    IFR(pPropertyBinding->GetValue(value.GetAddressOf()));
    IFR(FirePropertyChangedEvent(
        csl,
        propertyId,
        nullptr,
        value));
    return S_OK;
}

HRESULT CObservableObject::AttachBinding(_In_ IBindingBase *pPropertyBinding)
{
    foundation_assert(m_spPropertyInfoSupport.get());
    foundation_assert(m_spPropertyInfoSupport->m_pBindingValueChangedDelegate);
    IFR_ASSERT(PropertyBindingListener::AttachEventHandler(
        pPropertyBinding,
        m_spPropertyInfoSupport->m_pBindingValueChangedDelegate));
    return S_OK;
}

HRESULT CObservableObject::DetachBinding(_In_ IBindingBase *pPropertyBinding)
{
    foundation_assert(m_spPropertyInfoSupport.get());
    foundation_assert(m_spPropertyInfoSupport->m_pBindingValueChangedDelegate);
    IFR_ASSERT(PropertyBindingListener::DetachEventHandler(
        pPropertyBinding,
        m_spPropertyInfoSupport->m_pBindingValueChangedDelegate));
    return S_OK;
}

HRESULT CObservableObject::AttachBindings()
{
    foundation_assert(m_spPropertyInfoSupport.get());
    for(_MapPropertyInfoInternalType::iterator iter = m_spPropertyInfoSupport->begin();
        iter != m_spPropertyInfoSupport->end();
        ++iter)
    {
        if(iter->second._spPropertyBinding != nullptr)
        {
            IFR_ASSERT(AttachBinding(iter->second._spPropertyBinding));
        }
    }
    return S_OK;
}

HRESULT CObservableObject::DetachBindings()
{
    foundation_assert(m_spPropertyInfoSupport.get());

    _MapPropertyInfoInternalType propertyInfosInternal;
    propertyInfosInternal.swap(*m_spPropertyInfoSupport);
    HRESULT hr = DetachBindings(propertyInfosInternal);
    m_spPropertyInfoSupport->swap(propertyInfosInternal);
    IFHR(hr);
    return S_OK;
}

HRESULT CObservableObject::DetachBindings(const _MapPropertyInfoInternalType& propertyInfosInternal)
{
    for(_MapPropertyInfoInternalType::const_iterator iter = propertyInfosInternal.begin();
        iter != propertyInfosInternal.end();
        ++iter)
    {
        if(iter->second._spPropertyBinding != nullptr)
        {
            IFR_ASSERT(DetachBinding(iter->second._spPropertyBinding));
        }
    }
    return S_OK;
}

void CObservableObject::EnsurePropertyInfoSupport_NoLock()
{
    // Note : this method does not any locking mechanism
    if (m_spPropertyInfoSupport.get() == nullptr)
    {
        m_spPropertyInfoSupport.reset(new _PropertyInfoSupport());
    }
}

bool CObservableObject::LookupPropertyInfoSupport_NoLock(
    _In_ UINT32 propertyId,
    _PropertyClassInfoInternal& propertyInfoInternal)
{
    foundation_assert(m_spPropertyInfoSupport.get() != nullptr);

    _MapPropertyInfoInternalType::iterator iter = m_spPropertyInfoSupport->find(propertyId);

    if (iter != m_spPropertyInfoSupport->end())
    {
        propertyInfoInternal = (*iter).second;
        return true;
    }
    return false;
}

STDMETHODIMP CObservableObject::GetPropertyRuntimeClassOptions(_In_ UINT32 propertyId, _Out_ PropertyRuntimeClassOptions* propertyOptions)
{
    IFCPTR_ASSERT(propertyOptions);

    // lock protection required 
    _FoundationBaseLock csl(*this);

    _PropertyClassInfoInternal propertyInfoInternal;
    if (m_spPropertyInfoSupport.get() != nullptr &&
        LookupPropertyInfoSupport_NoLock(propertyId, propertyInfoInternal))
    {
        *propertyOptions = (PropertyRuntimeClassOptions)propertyInfoInternal._propertyRuntimeClassOptions;
    }
    else
    {
        *propertyOptions = PropertyRuntimeClassOptions::None;
    }
    return S_OK;
}

HRESULT CObservableObject::SetPropertyRuntimeClassOptionsInternal(_In_ UINT32 propertyId, _In_ PropertyRuntimeClassOptions propertyOptions)
{
    // lock protection required 
    _FoundationBaseLock csl(*this);

    EnsurePropertyInfoSupport_NoLock();

    // Add to our Map
    _PropertyClassInfoInternal propertyInfoInternal;
    LookupPropertyInfoSupport_NoLock(propertyId, propertyInfoInternal);
    propertyInfoInternal._propertyRuntimeClassOptions = static_cast<UINT16>(propertyOptions);

    // store the options value only
    m_spPropertyInfoSupport->operator[](propertyId) = propertyInfoInternal;

    return S_OK;
}

bool CObservableObject::IsPropertyRuntimeClassOptionEnabled(_In_ UINT32 propertyId, _In_ PropertyRuntimeClassOptions propertyOption)
{
    // lock protection required 
    _FoundationBaseLock csl(*this);

    if (m_spPropertyInfoSupport.get() == nullptr)
    {
        return false;
    }
    _PropertyClassInfoInternal propertyInfoInternal;
    LookupPropertyInfoSupport_NoLock(propertyId, propertyInfoInternal);
    return IsOptionEnabled(propertyInfoInternal._propertyRuntimeClassOptions, propertyOption);
}

HRESULT CObservableObject::SetBindingInternal(_In_ UINT32 propertyId,_In_ IBindingBase *pPropertyBinding)
{
    foundation_assert(pPropertyBinding);

    // lock protection required 
    _FoundationBaseLock csl(*this);

    EnsurePropertyInfoSupport_NoLock();
    ClearBindingInternal_NoLock(propertyId);

    // Add to our Map
    _PropertyClassInfoInternal propertyInfoInternal;
    LookupPropertyInfoSupport_NoLock(propertyId, propertyInfoInternal);
    propertyInfoInternal._spPropertyBinding = pPropertyBinding;

    m_spPropertyInfoSupport->operator[](propertyId) = propertyInfoInternal;
    if(IsObserved())
    {
        IFR_ASSERT(EnsureBindingValueChangedDelegate());
        IFR_ASSERT(AttachBinding(pPropertyBinding));

        IFR(FireBindingValue(csl,pPropertyBinding,propertyId));
    }
    return S_OK;
}
HRESULT CObservableObject::ClearBindingInternal(_In_ UINT32 propertyId)
{
    // lock protection required 
    _FoundationBaseLock csl(*this);

    return ClearBindingInternal_NoLock(propertyId);
}

HRESULT CObservableObject::ClearBindingInternal_NoLock(_In_ UINT32 propertyId)
{
    if (m_spPropertyInfoSupport.get() != nullptr)
    {
        _MapPropertyInfoInternalType::iterator iter = m_spPropertyInfoSupport->find(propertyId);
        if (iter != m_spPropertyInfoSupport->end())
        {
            if (IsObserved() && iter->second._spPropertyBinding != nullptr)
            {
                IFR_ASSERT(DetachBinding(iter->second._spPropertyBinding));
            }
            iter->second._spPropertyBinding.Release();
            if (iter->second.IsEmpty())
            {
                m_spPropertyInfoSupport->erase(iter);
            }
            return S_OK;
        }
    }
    // no binding found to clear
    return S_FALSE;
}

HRESULT CObservableObject::EnsureBindingValueChangedDelegate()
{
    foundation_assert(m_spPropertyInfoSupport.get());

    if(!m_spPropertyInfoSupport->m_pBindingValueChangedDelegate)
    {
        IFR_ASSERT(CBindingValueChangedDelegateFactory::CreateInstance(
            this,
            &CObservableObject::OnBindingValueChanged,
            foundation::ctl::inspectable::CastToInspectable(this),
            &m_spPropertyInfoSupport->m_pBindingValueChangedDelegate));
    }
    return S_OK;
}

void CObservableObject::OnIsObservedChanged(bool newValue)
{
    _ObservableObjectBase_Type::OnIsObservedChanged(newValue);
    IGNOREHR(EvaluateBindingConnections());
}

bool CObservableObject::IsObservedInternal() 
{
    // Note: instead of calling _Base::IsObservedInternal we need to split in 
    // adapter or composed observers
    if (this->HasAdapterEventHandlers() || this->IsComposedObserved())
    {
        return true;
    }
    else if (!this->HasEventHandlersInternal())
    {
        // no internal handlers
        return false;
    }

    // lock protection required 
    _FoundationBaseLock csl(*this);

    // IsObserved is reported true from our base class which means we do have some event handlers
    // Note: next code will evaluate if this event handlers are coming from our internal bindings
    if(m_spPropertyInfoSupport.get() &&
        m_spPropertyInfoSupport->size() && 
        m_spPropertyInfoSupport->m_pBindingValueChangedDelegate)
    {
        size_t coundBindingEventProperties = 0;
        for(_MapPropertyInfoInternalType::const_iterator iter = m_spPropertyInfoSupport->begin();
            iter != m_spPropertyInfoSupport->end();
            ++iter)
        {
            CPropertyBinding *pPropertyBinding = static_cast<CPropertyBinding *>(iter->second._spPropertyBinding.Get());
            
            if(pPropertyBinding != nullptr &&
                this->_ObservableObjectBase_Type::GetEventSourceImpl()->ContainsEventHandler(pPropertyBinding->m_pEventHandlerDelegate))
            {
                ++coundBindingEventProperties;
            }
        }
        // Note:
        // If we end up in counting the same number of event handlers than the binding properties
        // we should consider that we are not being 'Observed' by external entities
        if (this->_ObservableObjectBase_Type::GetCountHandlerStorage() == coundBindingEventProperties)
        {
            // Observed but only from Internal bindings
            return false;
        }
    }
    return true;
}

HRESULT CObservableObject::EvaluateBindingConnections()
{
    // lock protection required 
    _FoundationBaseLock csl(*this);

    if(m_spPropertyInfoSupport.get() && m_spPropertyInfoSupport->size())
    {
        if(IsObserved())
        {
            foundation_assert(m_spPropertyInfoSupport->m_pBindingValueChangedDelegate == nullptr);
            IFR_ASSERT(EnsureBindingValueChangedDelegate());

            IFR_ASSERT(AttachBindings());
        }
        else
        {
            IFR_ASSERT(DetachBindings());
            foundation_assert(m_spPropertyInfoSupport->m_pBindingValueChangedDelegate != nullptr);
            m_spPropertyInfoSupport->m_pBindingValueChangedDelegate->SetCallback(nullptr);
            foundation::ctl::ReleaseInterface(m_spPropertyInfoSupport->m_pBindingValueChangedDelegate);
        }
    }
    return S_OK;
}

size_t CObservableObject::GetCountEventHandlers()
{
    return _ObservableObjectBase_Type::GetCountEventHandlers() +
        this->_EventModelSourceType::GetCountHandlerStorage();
}

HRESULT CObservableObject::GetIsInModelChangesInternal(_Outptr_ bool& value)
{
    bool bVal = false;
    if(IsComposed())
    {
        IObjectNodeClass *pInnerClassBase = nullptr;
        IFR_ASSERT(foundation::QueryWeakReference(this->AsInspectable(), &pInnerClassBase));
        IFR_ASSERT(pInnerClassBase->GetIsInModelChanges(&bVal));
    }
    else
    {
        IFR_ASSERT(GetIsInModelChanges(&bVal));
    }
    value = bVal ? true:false;
    return S_OK;
}

HRESULT CObservableObject::OnBeginModelChangesInternal(pmod::library::BatchModelChangesOptions options)
{
    foundation_assert(m_ModelChanges.get() == nullptr);
    m_ModelChanges.reset(new _MapValueChanges_Type());
    m_UseModelTransaction = IsOptionEnabled(options,pmod::library::BatchModelChangesOptions::Transaction);
    return S_OK;
}

HRESULT CObservableObject::OnEndModelChangesInternal(pmod::library::EndModelChangesOptions options)
{
    UNREFERENCED_PARAMETER(options);

    _FoundationBaseLock csl(*this);

    foundation_assert(m_ModelChanges.get() != nullptr);
    _MapValueChanges_Type  modelChanges;
    modelChanges.swap(*m_ModelChanges);
    // delete our changes container
    m_ModelChanges.reset();

    if (this->IsObserved() && modelChanges.size())
    {
        // Create the pmod::library::BatchPropertyChangedEventArgsImpl Class
        foundation::ComPtr<pmod::library::BatchPropertyChangedEventArgsImpl, foundation::ctl::_ComBasePtrImpl> spBatchPropertyChangedEventArgs;
        IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(spBatchPropertyChangedEventArgs.GetAddressOf()));

        for (_MapValueChanges_Type::iterator iter = modelChanges.begin();
            iter != modelChanges.end();
            ++iter)
        {
            UINT32 propertyId = (*iter).first;

            IObservableObject *pSource = static_cast<IObservableObject *>(this);

            foundation::ComPtr<IPropertyChangedEventArgs> spPropertyChangedEventArgs;
            if (foundation::pv_util::IsValueEmpty((*iter).second.second))
            {
                IFR_ASSERT(pmod::library::PropertyChangedEventArgsWIthCallbackImpl::CreateInstance(
                    pSource,
                    propertyId,
                    spPropertyChangedEventArgs.GetAddressOf()));
            }
            else
            {
                IFR_ASSERT(CreatePropertyChangedEventArgs(
                    pSource,
                    propertyId,
                    (*iter).second.first,
                    (*iter).second.second,
                    spPropertyChangedEventArgs.GetAddressOf()));
            }
            IFR_ASSERT(spBatchPropertyChangedEventArgs->add_property_changed_event_args(spPropertyChangedEventArgs));
            
            // Update the value now if we use Model Transaction Mode
            if(m_UseModelTransaction)
            {
                foundation::IPropertyInfo *pPropertyInfo = nullptr;
                IFR_ASSERT(GetPropertyInfoById(propertyId,&pPropertyInfo));

                // TODO: 
                // Since we change the Container Data here if we failed during this transaction we will probably
                // leave the Model in an uncompleted State violating the transaction requirement
                foundation::InspectablePtr spOldValue, spNewValue;
                spPropertyChangedEventArgs->GetOldValue(spOldValue.GetAddressOf());
                spPropertyChangedEventArgs->GetNewValue(spNewValue.GetAddressOf());

                IFR(StorePropertyInternal(
                    pPropertyInfo,
                    spOldValue,
                    spNewValue));
            }
        }
        // Fire notification changes
        FirePropertyChangedEvent(csl,spBatchPropertyChangedEventArgs);
    }
    return S_OK;
}

STDMETHODIMP CObservableObject::GetProperties(foundation::IInspectable *pProperties, UINT32* size, foundation::IPropertyIdPair ***ppProperties)
{
    PERF_CONTEXT(Core_PerfId_PropertyModel_GetProperties_Id)

    foundation::library::IDispatcher *pWorkingDispatcher = nullptr;
    if (m_UseGetPropertiesDispatcher && 
        (pWorkingDispatcher = this->GetWorkingDispatcherInternal()) != nullptr &&
        !foundation::HasThreadAccess(pWorkingDispatcher))
    {
        return foundation::RunSync(pWorkingDispatcher,
            [=] { return this->GetPropertiesInternal(pProperties, size, ppProperties); });
    }
    // fallback invoke on the same thread
    return GetPropertiesInternal(pProperties, size, ppProperties);
}

bool CObservableObject::_IsPropertyIdInContainerCallback(void *_Context, UINT32 propertyId)
{
    return reinterpret_cast<CObservableObject *>(_Context)->IsPropertyIdInContainer(propertyId);
}

HRESULT CObservableObject::GetPropertiesInternal(foundation::IInspectable *pProperties, UINT32* size, foundation::IPropertyIdPair ***ppProperties)
{
    foundation::IObjectDispatchInfo *pPropertyDataInfo = foundation::CastTo<foundation::IObjectDispatchInfo>(this->GetPropertyModelInfoClass());
    return foundation::library::_GetProperties(
        pPropertyDataInfo,
        this,
        this,
        &_IsPropertyIdInContainerCallback,
        pProperties,
        size,
        ppProperties);
}

HRESULT CObservableObject::_GetView(UINT32* size, foundation::IDictionaryPair ***ppProperties)
{
    return CObjectDispatch::_GetView(
        GetPropertyModelInfoClass(),
        this,
        size,
        ppProperties);
}

HRESULT CObservableObject::_GetSize(_Outptr_ UINT32* size)
{
    IFR_ASSERT(EnsurePropertiesContainer());
    _FoundationBaseLock csl(*this);
    m_pProperties->GetSize(size);
    return S_OK;
}

HRESULT CObservableObject::_Lookup(HSTRING propertyName, foundation::IInspectable** value)
{
    IFCPTR(value);

    UINT32 propertyId;
    IFR_PARAM(this->GetPropertyId(propertyName, &propertyId), propertyName);

    return this->GetProperty(propertyId,value);
}

HRESULT CObservableObject::_HasKey(HSTRING propertyName, bool* found)
{
    IFCPTR(found);
    *found = false;

    UINT32 propertyId;
    HRESULT hr = this->GetPropertyId(propertyName, &propertyId);
    if (hr != foundation::Errors::E_PROPERTY_NOT_FOUND)
    {
        // The dictionary class once it QI and we try has keys this needs to exist.
        IFR_ASSERT(EnsurePropertiesContainer());

        IFHR(hr);
        if (!IsPropertyIdInContainer(propertyId))
        {
            // typically intrinisc or reserved properties
            *found = true;
        }
        else if (m_pProperties != nullptr)
        {
            // in case the conatiner support removal we want to check if it is there
            IFHR_ASSERT(m_pProperties->HasProperty(propertyId, found));
        }
    }
    return S_OK;
}

HRESULT CObservableObject::_Insert(HSTRING propertyName, foundation::IInspectable *value,bool* replaced)
{
    IFCPTR(replaced);
    *replaced = false;

    UINT32 propertyId;
    HRESULT hr = this->GetPropertyId(propertyName,&propertyId);
    if(hr == foundation::Errors::E_PROPERTY_NOT_FOUND)
    {
        // attempt to automatically add a new property info
        foundation::PropertyInfo propertyInfo(
            0,
            _pal_GetStringRawBuffer(propertyName,nullptr),
            foundation::PropertyType_Inspectable,
            foundation::PropertyFlagType::CanRead | foundation::PropertyFlagType::CanWrite,
            nullptr
            );

        IFR_ASSERT(GetPropertyModelInfoClass()->AddPropertyInternal(&propertyInfo, foundation_GUID_NULL));
        // get the generated property Id
        propertyId = propertyInfo.m_id;
    }
    else
    {
        // property was defined in our meta data
        if (m_pProperties != nullptr)
        {
            IFHR_ASSERT(m_pProperties->HasProperty(propertyId, replaced));
        }
    }
    return this->SetProperty(propertyId,value);
}

HRESULT CObservableObject::_Remove(HSTRING propertyName)
{
    UINT32 propertyId;
    IFR(this->GetPropertyId(propertyName,&propertyId));

    IFR_ASSERT(EnsurePropertiesContainer());
    if(!GetPropertyModelInfoClass()->IsAllowChanges())
    {
        return E_UNEXPECTED;
    }
    _FoundationBaseLock csl(*this);

    m_pProperties->RemoveProperty(propertyId);
    return S_OK;
}

HRESULT CObservableObject::_Clear()
{
    IFR_ASSERT(EnsurePropertiesContainer());
    _FoundationBaseLock csl(*this);

    m_pProperties->Dispose();
    return S_OK;
}

HRESULT CObservableObject::_Keys(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first)
{
    foundation_assert(false);
    return E_NOTIMPL;
}

void CObservableObject::LogInvokeMethod(
    _In_ UINT32 uniqueId,
    _In_ foundation::IMethodInfo *pMethodInfo,
    _In_ UINT32 methodId,
    _In_ UINT32 size,
    _In_ foundation::IInspectable **parameters)
{
    foundation::logger_util::Log(
        foundation::logger_util::GetFoundationLogger(),
        this->AsInspectable(),
        foundation::LoggingLevel::Info,
        foundation::ObjectDispatch_Category_Invoke_Id,
        CPropertyModelLogFactory::GetLogCategory()->AsInterface(),
        foundation::ObjectDispatchInvoke_LogRecordEnum::TypeId,
        // Unique Id
        pmod::Model_LogRecord::UniqueId,
        foundation::pv_util::CreateValue(uniqueId).Get(),
        // Method Id
        foundation::ObjectDispatchInvoke_LogRecord::MethodId,
        foundation::pv_util::CreateValue(methodId).Get(),
        foundation::ObjectDispatchInvoke_LogRecord::ParamSize,
        foundation::pv_util::CreateValue(size).Get(),
        foundation::ObjectDispatchInvoke_LogRecord::Parameters,
        foundation::pv_util::CreateValue(size, parameters).Get(),
        -1);
}

void CObservableObject::LogGetProperty(
    _In_ UINT32 uniqueId,
    _In_ UINT32 propertyId)
{
    foundation::logger_util::Log(
        foundation::logger_util::GetFoundationLogger(),
        this->AsInspectable(),
        foundation::LoggingLevel::Trace,
        foundation::ObjectDispatch_Category_GetProperty_Id,
        CPropertyModelLogFactory::GetLogCategory()->AsInterface(),
        foundation::ObjectDispatchGetProperty_LogRecordEnum::TypeId,
        // Unique Id
        pmod::Model_LogRecord::UniqueId,
        foundation::pv_util::CreateValue(uniqueId).Get(),
        // property id
        foundation::ObjectDispatchGetProperty_LogRecord::PropertyId,
        foundation::pv_util::CreateValue((UINT32)propertyId).Get(),
        -1);
}

void CObservableObject::LogSetProperty(
    _In_ UINT32 uniqueId,
    _In_ UINT32 propertyId)
{
    foundation::logger_util::Log(
        foundation::logger_util::GetFoundationLogger(),
        this->AsInspectable(),
        foundation::LoggingLevel::Trace,
        foundation::ObjectDispatch_Category_SetProperty_Id,
        CPropertyModelLogFactory::GetLogCategory()->AsInterface(),
        foundation::ObjectDispatchSetProperty_LogRecordEnum::TypeId,
        // Unique Id
        pmod::Model_LogRecord::UniqueId,
        foundation::pv_util::CreateValue(uniqueId).Get(),
        // property id
        foundation::ObjectDispatchSetProperty_LogRecord::PropertyId,
        foundation::pv_util::CreateValue((UINT32)propertyId).Get(),
        -1);
}

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

void CObservableObject::DumpObject()
{
    foundation::HStringPtr typeName;
    GetPropertyModelInfoClass()->As<IObservableObjectInfo>()->GetName(typeName.GetAddressOf());

    this->DumpObjectBase(_pal_GetStringRawBuffer(typeName,nullptr));
    foundation::IObject *pNextParent = nullptr;
    _traceMessage(U(" "));
    HRESULT hr = GetParentNode(&pNextParent);
    if (SUCCEEDED(hr) && pNextParent)
    {
        _ObservableObjectBase_Type::DumpHierarchy(pNextParent);
    }
}

#endif


class CObservableObjectClassFactory :
    public foundation::ctl::Implements<IObservableObjectClassFactory,&IID_IObservableObjectClassFactory>
{
public:
    static IObservableObjectClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CObservableObjectClassFactory, IObservableObjectClassFactory>();
        }
    }

    STDMETHOD(CreateObservableObjectInstance)(
        _In_ const ObservableObjectCreateParameters *pCreateParams,
        _In_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance)
    {
        return CObservableObject::CreateInstance(pCreateParams, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.ObservableObjectClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObservableObjectClassFactory::GetInstance)
);

