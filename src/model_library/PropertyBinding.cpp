/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyBinding.cpp
****/

#include "pch.h"

#include "PropertyBinding.h"
#include "ObservableCollectionStatics.h"
#include <foundation/pv_util.h>
#include <foundation/hstring_wrapper.h>
#include "CreateFoundationHelper.h"

#include <foundation/type_info_statics_util.h>
#include <pmod/eventhandler_util.h>
#include <foundation/library/com_ptr_singleton.h>
#include <pmod/library/object_statics_util.h>

// {5233654F-8148-4D09-A046-8B788F1A8201}
const GUID pmod::library::IID_IPropertyBindingClassFactory =
{ 0x5233654f, 0x8148, 0x4d09,{ 0xa0, 0x46, 0x8b, 0x78, 0x8f, 0x1a, 0x82, 0x1 } };

using namespace pmod;
using namespace foundation;
using namespace pmod::library;
using namespace foundation::library;

const UINT32 _IndexPropertyDefault = 0x6000;
const UINT32 _IndexPropertyOffest = 0x6100;

// Note: Index based properties must be >= _IndexPropertyDefault
static inline bool IsIndexProperty(UINT32 propertyId)
{
    return propertyId >= _IndexPropertyDefault;
}

static bool IsIndexPropertyPath(
    HSTRING path)
{
    // start with a '['
    return foundation::get_raw_buffer(path)[0] == U('[');
}

static HRESULT ResolveIndexPropertyPath(
    HSTRING path,
    string_t& indexProperty)
{
    if (IsIndexPropertyPath(path))
    {
        indexProperty = get_string_t(path);
        size_t pos;
        if ((pos = indexProperty.find_last_of(U(']'))) == string_t::npos)
        {
            // wrong index property path format
            return E_UNEXPECTED;
        }
        indexProperty = indexProperty.substr(1, pos - 1);
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CPropertyBinding::CreateBinding(
    _In_            IObservableObject *pSource,
    _In_            foundation::IInspectable *path,
    CPropertyBinding **ppPropertyBinding)
{
    foundation_assert(ppPropertyBinding != nullptr);

    pmod::library::PropertyBindingCreateParameters bindingParam;
    bindingParam.m_p_path = path;
    bindingParam.m_p_source = pSource;
    bindingParam.m_options = PropertyBindingOptions::None;

    foundation::InspectablePtr spNewInstance;
    IFR_ASSERT(CPropertyBinding::CreateInstance(&bindingParam, nullptr, spNewInstance.GetAddressOf()));

    *ppPropertyBinding = static_cast<CPropertyBinding *>(static_cast<CPropertyBinding::_ComBaseType *>(spNewInstance.Detach()));
    return S_OK;
}

HRESULT CPropertyBinding::GetValue(
    _In_            pmod::IObservableObject *pSource,
    _In_            foundation::IInspectable *path,
    _COM_Outptr_    foundation::IInspectable **ppValue)
{
    CPropertyBinding *pPropertyBinding;
    IFR_ASSERT(CreateBinding(pSource, path, &pPropertyBinding));

    // release context ptr
    foundation::InspectablePtr release_ptr;
    release_ptr.Attach(pPropertyBinding->CastToInspectable());

    return pPropertyBinding->GetValue(ppValue);
}

HRESULT CPropertyBinding::ResolvePath(
        _In_            IObservableObject *pSource,
        _In_            foundation::IInspectable *path,
        _Outptr_        UINT32 *propertyId,
        _COM_Outptr_    IObservableObject **ppResolvedReference)
{
    foundation_assert(pSource);
    foundation_assert(path);
    foundation_assert(propertyId);
    foundation_assert(path);
    foundation_assert(ppResolvedReference);

    CPropertyBinding *pPropertyBinding;
    IFR_ASSERT(CreateBinding(pSource, path, &pPropertyBinding));

    // release context ptr
    foundation::InspectablePtr release_ptr;
    release_ptr.Attach(pPropertyBinding->CastToInspectable());

    _BindingStepsVector bindings;
    IFR(pPropertyBinding->ResolveBindings(bindings));

    if (bindings.size() && bindings.size() == pPropertyBinding->m_PropertyPath.size())
    {
        // path was correctly resolved
        size_t nLastStepIndex = bindings.size() - 1;

        *propertyId = bindings[nLastStepIndex].second;
        foundation::ComPtr<IObservableObject> spPropertyModel;
        IFR(ResolveBindingReference(bindings[nLastStepIndex],false,ppResolvedReference));
        return S_OK;
    }
    *propertyId = (UINT32)-1;
    *ppResolvedReference = nullptr;
    return S_FALSE;

}

HRESULT CPropertyBinding::_Initialize(
    const pmod::library::PropertyBindingCreateParameters *pInitializeParameters)
{
    IFR_ASSERT(_BindingBase::_Initialize(pInitializeParameters));
    IFR_ASSERT(_SingleBaseModelSourceRefHolderT::_InitializeSourceEventHandler(this->IsFreeThreadApartment()));

    m_bUseWeakReferenceSource = IsOptionEnabled(pInitializeParameters->m_options,PropertyBindingOptions::UseSourceWeakReference);
    m_bUseWeakReferenceSteps = IsOptionEnabled(pInitializeParameters->m_options,PropertyBindingOptions::UsePathStepWeakReference);
    // Initialize Weak/Strong Reference option
    SetIsWeakReference(m_bUseWeakReferenceSource);

    if (pInitializeParameters->m_p_source)
    {
        IFR(SetSourceInternal(pInitializeParameters->m_p_source));
    }
    if (pInitializeParameters->m_p_path)
    {
        IFR(SetPath(pInitializeParameters->m_p_path));
    }
    
    return S_OK;
}

void CPropertyBinding::OnFinalRelease()
{
    _BindingBase::OnFinalRelease();
    CleanupBindings();
    m_PropertyPath.clear();
}

HRESULT CPropertyBinding::CreateInstance(
    _In_ const pmod::library::PropertyBindingCreateParameters *pParameters,
    _In_ foundation::IInspectable *pOuter,
    _Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CPropertyBinding>(pParameters, pOuter, ppNewInstance);
}

HRESULT CPropertyBinding::GetValueInternal(const _BindingStepsVector& bindings,foundation::IInspectable **ppValue)
{
    HRESULT hr;
    if(bindings.size() && bindings.size() == m_PropertyPath.size())
    {
        size_t nLastStepIndex = bindings.size() - 1;

        UINT32 propertyId = bindings[nLastStepIndex].second;
        foundation::ComPtr<IObservableObject> spPropertyModel;
        IFHR(ResolveBindingReference(bindings[nLastStepIndex],false,spPropertyModel.GetAddressOf()));

        foundation::InspectablePtr value;
        hr = GetPropertyValueStep(spPropertyModel, propertyId, value.GetAddressOf());
        if (SUCCEEDED(hr))
        {
            // Apply Converter to get the value
            IFR(ConvertValue(value));
            *ppValue = value.Detach();
            return S_OK;
        }
        else if (hr != E_BOUNDS)
        {
            // log & return the error unless is an unresolved 
            // 'E_BOUNDS' for Indexed property on the last step
            IFHR(hr);
        }
    }
    // unresolved path on this binding so create an Empty Value
    IFR_ASSERT(pv_util::CreateEmptyValue(ppValue));
    return S_OK;
}

STDMETHODIMP CPropertyBinding::GetValue(foundation::IInspectable **ppValue)
{
    IFCPTR(ppValue);
    IFCEXPECT(IsValueValid());

    PERF_CONTEXT(Core_PerfId_BindingBase_GetValue_Id)

    if(IsAttached())
    {
        if (IsCachedBindingValue())
        {
            return GetCachedBindingValue(ppValue);
        }
        IFR(GetValueInternal(m_ConnectedBindings, ppValue));
        SetCachedBindingValue(*ppValue);
        return S_OK;
    }
    else
    {
        _BindingStepsVector bindings;
        IFR(ResolveBindings(bindings));
        return GetValueInternal(bindings,ppValue);
    }
}

STDMETHODIMP CPropertyBinding::GetPath(foundation::IInspectable **ppValue)
{
    foundation::string_t propertiesPath;

    for(_Inspectable_ptr_vector_type::iterator iter = m_PropertyPath.begin();
                iter != m_PropertyPath.end();
                ++iter)
    {
        foundation::InspectablePtr nextPath = *iter;
        foundation::ComPtr<IPropertyValue> spPathValue;
        IFR(pv_util::CastToPropertyValue(nextPath,spPathValue.GetAddressOf()));

        PropertyType propertyType;
        IFR(spPathValue->get_Type(&propertyType));

        if(propertyType == PropertyType_String)
        {
            foundation::HStringPtr hString;
            IFR(spPathValue->GetString(hString.GetAddressOf()));

            if (propertiesPath.size() && !IsIndexPropertyPath(hString))
            {
                propertiesPath += U(".");
            }
            propertiesPath += hString.GetRawBuffer();
        }
    }

    if(propertiesPath.size())
    {
        return pv_util::CreateStringValue(
            propertiesPath.c_str(),
            ppValue);
    }
    else
    {

        return pv_util::CreateInspectableArrayValue(
            (UINT32)m_PropertyPath.size(),
            reinterpret_cast<foundation::IInspectable **>(m_PropertyPath.data()),
            ppValue);
    }
}

HRESULT CPropertyBinding::GetBindingsStepsInternal(
    const _BindingStepsVector& bindings,
    _Out_ UINT32 *size_path_steps,
    IObservableObject ***ppPathSteps,
    _Out_ UINT32 *size_property_steps,
    UINT32 **ppPropertySteps)
{
    std::vector<foundation::ComPtr<IObservableObject>> pathVector;
    std::vector<UINT32> propertiesVector;

    pathVector.resize(bindings.size());
    propertiesVector.resize(bindings.size());

    for (_BindingStepsVector::const_iterator iter = bindings.begin();
        iter != bindings.end();
        ++iter)
    {
        size_t index = iter - bindings.begin();
        propertiesVector[index] = (*iter).second;
        IFR(ResolveBindingReference(*iter, false, pathVector[index].GetAddressOf()));
    }
    ContainerRefCountToArray(pathVector, size_path_steps, ppPathSteps);
    ContainerToArray(propertiesVector, size_property_steps, ppPropertySteps);

    return S_OK;
}

STDMETHODIMP CPropertyBinding::GetBindingsSteps(
    _Out_ UINT32 *size_path_steps,
    IObservableObject ***ppPathSteps,
    _Out_ UINT32 *size_property_steps,
    UINT32 **ppPropertySteps)
{
    if (IsAttached())
    {
        return GetBindingsStepsInternal(
            m_ConnectedBindings,
            size_path_steps,
            ppPathSteps,
            size_property_steps,
            ppPropertySteps);
    }
    else
    {
        _BindingStepsVector bindings;
        IFR(ResolveBindings(bindings));
        return GetBindingsStepsInternal(
            bindings,
            size_path_steps,
            ppPathSteps,
            size_property_steps,
            ppPropertySteps);
    }
}

HRESULT CPropertyBinding::AddPathStep(foundation::string_t& pathSetp)
{
    foundation::InspectablePtr spValue;
    // let's see if there is an index property access on this sub path
    size_t index = pathSetp.find(U('['));
    if (index != std::string::npos && index > 0)
    {
        IFR_ASSERT(pv_util::CreateStringValue(
            pathSetp.substr(0, index).c_str(),
            spValue.GetAddressOf()));
        m_PropertyPath.push_back(spValue);

        pathSetp = pathSetp.substr(index);
    }
    IFR_ASSERT(pv_util::CreateStringValue(
        pathSetp.c_str(),
        spValue.ReleaseAndGetAddressOf()));
    m_PropertyPath.push_back(spValue);
    return S_OK;
}

STDMETHODIMP CPropertyBinding::SetPath(foundation::IInspectable *value)
{
    IFCPTR(value);
    foundation::ComPtr<IPropertyValue> spPropertyValue;
    IFR(pv_util::CastToPropertyValue(value,spPropertyValue.GetAddressOf()));

    PropertyType propertyType;
    IFR(spPropertyValue->get_Type(&propertyType));

    IFCEXPECT( 
        propertyType == PropertyType_Int16 ||
        propertyType == PropertyType_UInt16 ||
        propertyType == PropertyType_Int32 ||
        propertyType == PropertyType_UInt32 ||
        propertyType == PropertyType_String || 
        propertyType == PropertyType_UInt32Array);
    
    m_PropertyPath.clear();
    if( propertyType == PropertyType_Int16 ||
        propertyType == PropertyType_UInt16 ||
        propertyType == PropertyType_Int32 ||
        propertyType == PropertyType_UInt32)
    {
        UINT32 propertyId;
        IFR_ASSERT(pv_util::GetValue(value, &propertyId));
        m_PropertyPath.push_back(pv_util::CreateValue(propertyId));
    }
    else if(propertyType == PropertyType_String)
    {
        foundation::HStringPtr hString;
        IFR_ASSERT(spPropertyValue->GetString(hString.GetAddressOf()));

        foundation::string_t path = hString.GetRawBuffer();
        while(path.length())
        {
            size_t index = path.find(U("."));
            foundation::InspectablePtr spValue;
            if(index == std::string::npos)
            {
                IFR_ASSERT(AddPathStep(path));
                break;
            }
            else
            {
                foundation::string_t sub_path = path.substr(0, index);
                path = path.substr(index + 1);
                IFR_ASSERT(AddPathStep(sub_path));
            }
        }
    }
    else
    {
        UINT32 size;
        UINT32 *pValues;
        IFR(spPropertyValue->GetUInt32Array(&size,&pValues));

        for(UINT32 i=0; i < size;++i)
        {
            foundation::InspectablePtr spValue;
            IFR(pv_util::CreateUInt32Value(pValues[i],spValue.GetAddressOf()));

            m_PropertyPath.push_back(spValue);
        }
    }

    return EnsureAddSourceHandler();
}

bool CPropertyBinding::NeedAttachToEventSource()
{
    return 
        m_spReferenceHolder->IsReferenceAttached() &&
        IsPropertyPathValid() &&
        this->_BindingSourceModelBase::NeedAttachToEventSource();
}

HRESULT CPropertyBinding::AttachToSource()
{
    foundation_assert(IsPropertyPathValid());
    IFR(ConnectBindings());
    IFR(on_source_attached_internal());
    return S_OK;
}

HRESULT CPropertyBinding::DetachFromSource()
{
    IFR(CleanupBindings());
    IFR(on_source_detached_internal());

    return S_OK;
}

HRESULT CPropertyBinding::ResolveSender(IObservableObject *pSender,size_t& nIndex)
{
    foundation_assert(pSender != nullptr);

    size_t indexPath = 0;
    for(std::vector<_BindingPathType>::iterator iter = m_ConnectedBindings.begin();
            iter != m_ConnectedBindings.end();
            ++iter,++indexPath)
    {
        foundation::ComPtr<IObservableObject> spPropertyModel;
        // we could expect some connected steps to be weak references
        // so we will not enforce a not null value being resolved
        IFR(ResolveBindingReference(*iter,true,spPropertyModel.GetAddressOf()));
        if(
            // Use 'Raw' comparission
            pSender == spPropertyModel ||
            // Or in case 'real' sender is the aggregated object not itself
            pmod::library::is_object_equals(pSender,spPropertyModel))
        {
            nIndex = indexPath;
            return S_OK;
        }
    }
    foundation_assert(false);
    return E_UNEXPECTED;
}

HRESULT CPropertyBinding::InvokeInternal(foundation::IUnknown *pSender,IPropertyChangedEventArgs *pArgs)
{
    size_t nIndex;
    IFR(ResolveSender(static_cast<IObservableObject *>(pSender),nIndex));

    UINT32 propertyId;
    IFR(pArgs->GetProperty(&propertyId));

    UINT32 connectedPropertyId = m_ConnectedBindings[nIndex].second;

    // check if the property that was changed was part of our property steps
    if (propertyId == Property_All ||           // all properties on a 'Model'
        propertyId == connectedPropertyId ||   // match exact connected property
        IsIndexProperty(connectedPropertyId)   // any Index based property used in connection
        )
    {
        bool isLastStep = nIndex == (m_PropertyPath.size() - 1);
        // a property in our binding steps was changed
        if (isLastStep)
        {
            // force recalc binding value
            InvalidateCachedBindingValue();

            // the property change comes from the last step in our binding
            IFR(InternalFireBindingValueChanged());
        }
        else
        {
            // Property Changed came from one of the path setps objects
            // or we receive a 'Property_All' property change from a path step

            // reconstruct all the bindings
            IFR(UpdateBindings());
        }
    }
    return S_OK;
}

HRESULT CPropertyBinding::InternalFireBindingValueChanged()
{
    foundation::InspectablePtr spValue;
    IFR(GetValue(spValue.GetAddressOf()));

    if(m_pDelegate)
    {
        IFR(m_pDelegate->OnValueChanged(spValue,IsValueSet()));
    }
    IFR(_CPropertyBindingBase_Type::InternalFireBindingValueChanged());
    return S_OK;
}

HRESULT CPropertyBinding::UpdateBindings()
{
    CleanupBindings();

    IFR(ConnectBindings());
    IFR(InternalFireBindingValueChanged());

    return S_OK;
}

HRESULT CPropertyBinding::ConnectBindings()
{
    foundation_assert(m_ConnectedBindings.size()==0);
    HRESULT hr = S_OK;

    this->EnableCallback(true, this->AsInspectable());

    IFHR(ResolveBindings(m_ConnectedBindings));
    for(std::vector<_BindingPathType>::iterator iter = m_ConnectedBindings.begin();
        iter != m_ConnectedBindings.end();
        ++iter)
    {
        foundation::ComPtr<IObservableObject> spSource;
        IFHR(ResolveBindingReference(*iter,false,spSource.GetAddressOf()));
        IFHR(ObservableObjectListenerBase::AttachEventHandler(
            spSource,
            m_pEventHandlerDelegate));
    }
    return S_OK;
}

HRESULT CPropertyBinding::GetPropertyValueStep(
    IObservableObject *pPropertyModelStep,
    UINT32 propertyId,
    foundation::IInspectable **ppValue)
{
    foundation_assert(pPropertyModelStep != nullptr);

    if (propertyId >= _IndexPropertyDefault)
    {
        foundation::ComPtr<IObservableCollection> spCollectionModel;
        // we expect the underlying 'source' step to support this interface
        IFR_ASSERT(foundation::QueryInterface(pPropertyModelStep, spCollectionModel.GetAddressOf()));

        // if we did not complete value step
        *ppValue = nullptr;

        if (propertyId >= _IndexPropertyOffest)
        {
            UINT32 indexItem = (propertyId - _IndexPropertyOffest);
            UINT32 count;
            IFR_ASSERT(spCollectionModel->GetCount(&count));
            if (indexItem >= count)
            {
                // the step can't be completed at this point but should not be consider as
                // an error
                return S_OK;
            }
            return spCollectionModel->GetItem(indexItem, ppValue);
        }
        else
        {
            UINT32 indexPath = (propertyId - _IndexPropertyDefault);
            HStringPtr path;
            IFR_ASSERT(pv_util::GetValue(m_PropertyPath[indexPath], &path));

            string_t keyValue;
            IFR_ASSERT(ResolveIndexPropertyPath(path, keyValue));
            // TODO: we only support a string value, we could attempt
            // a better smart conversion to other types
            size_t pos_start = keyValue.find_first_of(U('\''));
            size_t pos_end = keyValue.find_last_of(U('\''));
            if (pos_start == string_t::npos || pos_end == string_t::npos)
            {
                IFR(E_UNEXPECTED);
            }
            keyValue = keyValue.substr(pos_start + 1, pos_end - pos_start -1);
            foundation::InspectablePtr spDefaultValueLookup = pv_util::CreateValue(keyValue.c_str());

            return CObservableCollectionStatics::LookupItemInternal(
                spCollectionModel,
                spDefaultValueLookup,
                ppValue);
        }
    }
    return pPropertyModelStep->GetProperty(propertyId, ppValue);
}

HRESULT CPropertyBinding::ResolveBindings(_Outptr_ _BindingStepsVector& bindings)
{
    foundation_assert(m_spReferenceHolder->IsReferenceAttached());
    HRESULT hr;

    size_t indexPath = 0;
    foundation::ComPtr<IObservableObject> spNextSource;
    IFHR(m_spReferenceHolder->ResolveReferencePtr(spNextSource));
    // should we ASSERT ?
    // Return if Resolve Reference is nullptr
    IFCEXPECT(spNextSource.Get());

    for(_Inspectable_ptr_vector_type::const_iterator iter = m_PropertyPath.begin();
                iter != m_PropertyPath.end();
                ++iter,
                ++indexPath)
    {
        foundation::ComPtr<IPropertyValue> spNextPathValue;
        IFHR(pv_util::CastToPropertyValue(*iter, spNextPathValue.GetAddressOf()));
        PropertyType propertyType;
        IFHR(spNextPathValue->get_Type(&propertyType));

        UINT32 nextPropertyId;

        if(propertyType == PropertyType_String)
        {
            foundation::HStringPtr hString;
            IFHR(spNextPathValue->GetString(hString.GetAddressOf()));

            // resolve the property from the TypeInfo
            IFHR(ResolveProperty(spNextSource, hString, &nextPropertyId));
            if (nextPropertyId == _IndexPropertyDefault)
            {
                nextPropertyId += (UINT32)indexPath;
            }
        }
        else
        {
            IFHR(spNextPathValue->GetUInt32(&nextPropertyId));
        }
        // Add to the Connected bindings

        foundation::ReferenceHolderBase<IObservableObject> *pReferenceHolderBase = nullptr;
        bool useWeakReference = indexPath==0 ? m_bUseWeakReferenceSource:m_bUseWeakReferenceSteps;

        if(useWeakReference)
        {
            pReferenceHolderBase = new foundation::WeakReferenceHolder<IObservableObject>();
        }
        else
        {
            pReferenceHolderBase = new foundation::StrongReferenceHolder<IObservableObject>();
        }

        IFR(pReferenceHolderBase->Attach(spNextSource));

        bindings.push_back(_BindingPathType(pReferenceHolderBase,nextPropertyId));

        // we need the next source
        if( (indexPath+1) < m_PropertyPath.size())
        {
            foundation::InspectablePtr spValue;
            HRESULT hr = GetPropertyValueStep(spNextSource, nextPropertyId, spValue.GetAddressOf());
            if (hr != E_BOUNDS && FAILED(hr))
            {
                IFHR(hr);
            }
            spNextSource.Release();

            // we expect the value to be nullptr || IObservableObject Interface
            if(spValue == nullptr)
            {
                // this would happen if the Binding is not fully resolved yet
                break;
            }
            else
            {
                IFHR(foundation::QueryInterface(spValue, spNextSource.GetAddressOf()));
            }
        }
    }
    return S_OK;
}

bool CPropertyBinding::IsValueSet()
{
    return IsPropertyPathValid() &&
            m_ConnectedBindings.size() == m_PropertyPath.size();
}

HRESULT CPropertyBinding::DisconnectBindings()
{
    HRESULT hr = S_OK;

    // Prevent further invocations to arrive
    this->EnableCallback(false, nullptr);

    for(std::vector<_BindingPathType>::iterator iter = m_ConnectedBindings.begin();
        iter != m_ConnectedBindings.end();
        ++iter)
    {
        foundation::ComPtr<IObservableObject> spSource;
        IFHR(ResolveBindingReference(*iter,true,spSource.GetAddressOf()));
        if(spSource != nullptr)
        {
            IFHR(ObservableObjectListenerBase::DetachEventHandler(
                spSource,
                m_pEventHandlerDelegate));
        }
    }

    return hr;
}

HRESULT CPropertyBinding::CleanupBindings()
{
    HRESULT hr = S_OK;
    InvalidateCachedBindingValue();
    IFCL(DisconnectBindings());
Cleanup:
    m_ConnectedBindings.clearAndDelete();
    return hr;
}

HRESULT CPropertyBinding::ResolveProperty(
        IObservableObject *pObservableObject,
        HSTRING propertyName,
        UINT32 *propertyId)
{
    foundation_assert(pObservableObject);
    foundation_assert(propertyId);

    string_t indexProperty;
    HRESULT hr = ResolveIndexPropertyPath(propertyName, indexProperty);
    IFHR(hr);

    if (hr == S_OK)
    {
        // an index property from a collection
        CHAR_t *pEndPtr = nullptr;
        // an Indexed property being found
        *propertyId = (UINT32)_pal_strtoul(indexProperty.c_str(), &pEndPtr, 10) + _IndexPropertyOffest;
        if (*pEndPtr != 0)
        {
            // use default property resolution. Please note the default property 
            // needs to match the current path index. This will be calculated from the
            // caller method
            *propertyId = _IndexPropertyDefault;
        }
        return S_OK;
    }
    else
    {
        foundation::ComPtr<IObservableObjectInfo> spPropertyModelInfo;
        IFR(foundation::GetObjectTypeInfo(pObservableObject, spPropertyModelInfo.GetAddressOf()));

        foundation::ComPtr<IPropertyInfo> spPropertyInfo;
        IFR(spPropertyModelInfo->GetPropertyByName(propertyName,spPropertyInfo.GetAddressOf()));

        // we expect a non null value
        foundation_assert(spPropertyInfo != nullptr);
        IFR(spPropertyInfo->GetId(propertyId));
    }
    return S_OK;
}

HRESULT CPropertyBinding::ResolveBindingReference(
        _In_ const _BindingPathType &bindingPath,
        _In_ bool ignoreNullReference,
        _Outptr_ IObservableObject **ppWeakReferenceModel)
{
    foundation_assert(ppWeakReferenceModel);
    IFCPTR(ppWeakReferenceModel); // just to make OACR happy

    return foundation::ResolveReferenceHolder(
        *bindingPath.first,
        ignoreNullReference,
        *ppWeakReferenceModel);
}

#if PMOD_LIB_LIBRARY_STATIC_LIB
EXTERN_C void _ModelLibraryInstanceInitializerStub();
#endif

class CPropertyBindingClassFactory :
    public foundation::ctl::Implements<IPropertyBindingClassFactory, &IID_IPropertyBindingClassFactory>
{
public:
    static IPropertyBindingClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CPropertyBindingClassFactory, IPropertyBindingClassFactory>();
        }
    }

    STDMETHODIMP CreatePropertyBindingInstance(
        _In_ const PropertyBindingCreateParameters *pBindingParam,
        _In_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CPropertyBinding::CreateInstance(pBindingParam, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.PropertyBindingClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CPropertyBindingClassFactory::GetInstance)
);
