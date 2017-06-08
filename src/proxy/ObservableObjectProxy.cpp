/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectProxy.cpp
****/

#include "pch.h"
#include "ObservableObjectProxy.h"

#include "ObjectDispatchInfoBase.h"

#include "ArrayHelper.h"

#include <foundation/pv_util.h>
#include <foundation/library/key_value_pair_impl.h>
#include <foundation/library/dictionary_impl.h>

#include <foundation/library/dictionary_class_ptr.h>
#include <foundation/interfaces/object_serializer.h>
#include <foundation/library/interfaces/fast_properties_container_parameters.h>
#include <pmod/library/event_model_event_args_impl.h>
#include <pmod/library/property_changed_event_args_impl.h>

#include <foundation/library/perf_counter_util.h>
#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#endif

using namespace pmod;

const UINT8 _PROPERTY_IS_CACHED_MASK = 0x01;
const UINT8 _PROPERTY_IS_REFERENCE_MASK = 0x02;

typedef foundation::library::_InspectableKeyValuePair<foundation::IPropertyIdPair> CPropertyIdPair;

// {42B49865-C22E-4CB7-9F4E-F4A039DCB1EA}
EXTERN_C const GUID  IID_IObjectDispatchInfoInternalClass =
{ 0x42b49865, 0xc22e, 0x4cb7,{ 0x9f, 0x4e, 0xf4, 0xa0, 0x39, 0xdc, 0xb1, 0xea } };

// Private imports
EXTERN_C FOUNDATION_LIB_API HRESULT  STDAPICALLTYPE CreateFastPropertiesContainer(
    const foundation::library::FastPropertiesContainerParameters *pParameters,
	foundation::library::IFastPropertiesContainer **ppPropertiesContainer);

//------------------------------------------------------------------------------
// Class:   CProxyPropertyChangedEventArgs
// A specialized IPropertyChangedEventArgs implemenation for a Model Proxy
//------------------------------------------------------------------------------
class CProxyPropertyChangedEventArgs :
	public pmod::library::PropertyChangedEventArgsImpl
{
public:
    HRESULT _Initialize(
        _In_ CObservableObjectProxy *pThis,
        _In_ UINT32 propertyId,
        _In_ IPropertyChangedEventArgs* pSourceEventArgs,
        bool bNeedToProxyValue)
    {
		pmod::library::PropertyChangedEventArgsImpl::_Initialize(
            static_cast<IObservableObject *>(pThis),
            propertyId, 
            nullptr, 
            nullptr);
        _pThis = pThis;
        foundation::ctl::inspectable::AddRefInterface(pThis);
        _spSourceEventArgs = pSourceEventArgs;
        _bNeedToProxyValue = bNeedToProxyValue;
        return S_OK;
    }

protected:
    CProxyPropertyChangedEventArgs():
        _pThis(nullptr),
        _bNeedToProxyValue(false),
        _bNewValueConverted(false),
        _bOldValueConverted(false)
    {}

    void FinalRelease() override
    {
        foundation::ctl::inspectable::ReleaseInterface(_pThis);
		pmod::library::PropertyChangedEventArgsImpl::FinalRelease();
    }

    STDMETHOD(GetNewValue)(_Outptr_ foundation::IInspectable ** ppValue)
    {
        if (!_bNewValueConverted)
        {
            IFR(_spSourceEventArgs->GetNewValue(this->_new_value_ptr.GetAddressOf()));
            if (_bNeedToProxyValue)
            {
                IFR(_pThis->ToProxyValue(this->_new_value_ptr));
            }
            _bNewValueConverted = true;
        }
        return pmod::library::PropertyChangedEventArgsImpl::GetNewValue(ppValue);
    }

    STDMETHOD(GetOldValue)(_Outptr_ foundation::IInspectable ** ppValue)
    {
        if (!_bOldValueConverted)
        {
            IFR(_spSourceEventArgs->GetOldValue(this->_old_value_ptr.GetAddressOf()));
            if (_bNeedToProxyValue)
            {
                IFR(_pThis->ToProxyValue(this->_old_value_ptr));
            }
            _bOldValueConverted = true;
        }
        return pmod::library::PropertyChangedEventArgsImpl::GetOldValue(ppValue);
    }
private:
    CObservableObjectProxy *_pThis;
    foundation::ComPtr<IPropertyChangedEventArgs> _spSourceEventArgs;
    bool _bNeedToProxyValue;
    bool _bNewValueConverted;
    bool _bOldValueConverted;
};

HRESULT CObservableObjectProxy::_Initialize(
    _In_ UINT32 proxyUniqueId,
    _In_ IObservableObject *pSourceObject,
    _In_opt_ pmod::proxy::IProxyObjectDelegate *pProxyModelDelegateBase,
    _In_ CProxyObjectFactory *pProxyObjectFactory,
    _In_ bool isFreeThreadedEnabled)
{
    IFR_ASSERT(_PropertyModelProxyBaseType::_Initialize(
        proxyUniqueId,
        pSourceObject,
        pProxyModelDelegateBase,
        pProxyObjectFactory,
        isFreeThreadedEnabled));

    if (IsModelCacheEnabled() && pProxyObjectFactory->UseCacheStrongRefForProperties())
    {
        // by Force the Attach we will ensure the caching mechanism is always present
        // and so we preserve the properties strong referenced as long as needed
        this->SetForceAttached(true);
    }
    return S_OK;
}

// IObservableObject Interface
STDMETHODIMP CObservableObjectProxy::GetProperty(UINT32 propertyId, IInspectable** value)
{
    PERF_CONTEXT(ProxyFactory_PerfId_PropertyModel_GetProperty_Id)

    return GetPropertyValue(propertyId, value);
}

STDMETHODIMP CObservableObjectProxy::SetProperty(UINT32 propertyId,IInspectable *value)
{
    PERF_CONTEXT(ProxyFactory_PerfId_PropertyModel_SetProperty_Id)
    return SetPropertyValue(propertyId, value);
}

STDMETHODIMP CObservableObjectProxy::GetProperties(foundation::IInspectable* pProperties, UINT32* size, foundation::IPropertyIdPair ***ppProperties)
{
    PERF_CONTEXT(ProxyFactory_PerfId_PropertyModel_GetProperties_Id)

    IFR(VerifyModelSource());

    foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> properties;
    IFR(m_spObjectSource->GetProperties(
        pProperties,
        properties.GetSizeAddressOf(),
        properties.GetBufferAddressOf()));

    // returned properties
    std::vector<foundation::ComPtr<foundation::IPropertyIdPair> > proxyProperties;

    for(UINT32 index = 0;index < properties.GetSize(); ++index)
    {
        foundation::InspectablePtr spValue;
        UINT32 propertyId;

        IFR_ASSERT(properties[index]->GetKey(&propertyId));
        IFR_ASSERT(properties[index]->GetValue(spValue.GetAddressOf()));
        IFR_ASSERT(ToPropertyProxyValue(propertyId, spValue));

        // cache if possible
        _IFR_(OnGetSourceProperty(propertyId, spValue, this->CanCacheProperty(propertyId)));

        foundation::ComPtr<foundation::IPropertyIdPair> spPropertyIdPair;
        IFR_ASSERT(foundation::library::CreateKeyValuePair<CPropertyIdPair>(
            propertyId,
            spValue,
            spPropertyIdPair.GetAddressOf()));

        proxyProperties.push_back(spPropertyIdPair);
    }
    // copy now to public API
    foundation::RefCountCopyTo(proxyProperties,size,ppProperties);
    return S_OK;
}

STDMETHODIMP CObservableObjectProxy::GetTypeInfo(_COM_Outptr_ foundation::IObjectTypeInfo** ppValue)
{
    IFCPTR(ppValue);

    IFR_ASSERT(EnsureObservableObjectInfo());

    return _spCachedObservableObjectInfo.CopyTo(reinterpret_cast<IObservableObjectInfo **>(ppValue));
}

STDMETHODIMP CObservableObjectProxy::GetPropertyChangedEventSource(_COM_Outptr_ IPropertyChangedEventSource** ppEventSource)
{
    IFCEXPECT(m_spEventHandler);

    * ppEventSource =  this->m_spEventHandler;
    (*ppEventSource)->AddRef();
    return S_OK;
}

STDMETHODIMP CObservableObjectProxy::GetEventModelEventSource(_COM_Outptr_ pmod::IEventModelEventSource** ppEventSource)
{
    foundation_lib::_FoundationLock csl(*this);

    if(m_spEventModelProxyEventHandler == nullptr)
    {
        IFR(_EventModelProxyEventHandler::CreateInstance(this,this,this->m_spEventModelProxyEventHandler.GetAddressOf()));
    }
    foundation_assert(m_spEventModelProxyEventHandler);
    * ppEventSource =  this->m_spEventModelProxyEventHandler;
    (*ppEventSource)->AddRef();
    return S_OK;
}

STDMETHODIMP CObservableObjectProxy::InvokeMethod(UINT32 methodId,_In_ UINT32 size,IInspectable **parameters,IInspectable **ppResult)
{
    PERF_CONTEXT(ProxyFactory_PerfId_PropertyModel_Invoke_Id)
    return InvokeMethodInternal(methodId, size, parameters, ppResult);
}

HRESULT CObservableObjectProxy::GetSourceEventSource(IPropertyChangedEventSource **ppEventSource)
{
    IFR(VerifyModelSource());
    return m_spObjectSource->GetPropertyChangedEventSource(ppEventSource);
}

STDMETHODIMP CObservableObjectProxy::Dispose()
{
    _spCachedFastPropertiesContainer.Release();
    return _PropertyModelProxyBaseType::Dispose();
}


HRESULT CObservableObjectProxy::QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
{
    HRESULT hr = S_OK;
    if(iid == foundation::IDictionary::GetIID())
    {
        *ppInterface = static_cast<foundation::IDictionary *>(this);
    }
	else if (iid == foundation::IObjectDispatch::GetIID())
	{
		*ppInterface = static_cast<foundation::IObjectDispatch *>(this);
	}
	else
    {
        hr = _PropertyModelProxyBaseType::QueryInterfaceImpl(iid, ppInterface);
    }
    return hr;
}

void CObservableObjectProxy::FinalRelease()
{
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    if (foundation::debug_util::IsOptionsFlag(_OptionTrackRefCountAllProxies | _OptionTrackRefCountPropertyModelProxies))
    {
        pal::library::TraceMessage(U("CObservableObjectProxy::FinalRelease unique_id:%d\n"), this->m_UniqueId);
    }
#endif

    _PropertyModelProxyBaseType::FinalRelease();
    RELEASE_INTERFACE(m_pEventModelProxyDelegate);
}

size_t CObservableObjectProxy::GetProxyCountEventHandlers()
{
    foundation_lib::_FoundationLock csl(*this);

    return _PropertyModelProxyBaseType::GetProxyCountEventHandlers() +
        (this->m_spEventModelProxyEventHandler != nullptr ?
        this->m_spEventModelProxyEventHandler->GetEventHandlers().size() :
        0);
}

foundation::ILogCategory *CObservableObjectProxy::GetLogCategory()
{
    return this->m_pProxyObjectFactory->GetPropertyProxyCategory();
}

HRESULT CObservableObjectProxy::GetProxyModelInstance(_In_ UINT32 propertyId,_In_ REFIID riid,void** pValue)
{
    foundation::InspectablePtr value;
    IFR(GetPropertyValue(propertyId,value.GetAddressOf()));
    if(IsInspectableValue(value))
    {
        return value->QueryInterface(riid,(void **)pValue);
    }
    else
    {
        // TODO: should verify that the Var Type is VT_UNKNOWN ?
        * pValue = nullptr;
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::GetModelCommandInstance(_In_ UINT32 propertyId,ICommand** pValue)
{
    return GetProxyModelInstance(propertyId,ICommand::GetIID(),(void **)pValue);
}

HRESULT CObservableObjectProxy::GetModelCollectionInstance(_In_ UINT32 propertyId,IObservableCollection** pValue)
{
    return GetProxyModelInstance(propertyId, IObservableCollection::GetIID(),(void **)pValue);
}

HRESULT CObservableObjectProxy::InvokeMethodInternal(UINT32 methodId,_In_ UINT32 size,IInspectable **parameters,IInspectable **ppResult)
{
    IFR(VerifyModelSource());

    foundation::InspectableArrayWrapper _parameters;

    // If there are parameters
    if(size)
    {
        _Inspectable_ptr_vector_type parametersArray;
        CopyArrayToVector(size,parameters,parametersArray);
        // Convert the Items Array to Proxy Models
        IFR(FromProxyArray(parametersArray));

        ContainerToArray(parametersArray,_parameters.GetSizeAddressOf(),_parameters.GetBufferAddressOf());
    }

    foundation::InspectablePtr result;
    HRESULT hr = S_OK;
    IFHR(m_spObjectSource->InvokeMethod(
            methodId,
            _parameters.GetSize(),
            (IInspectable **)_parameters.GetBuffer(),
            result.GetAddressOf()));
    IFR_ASSERT(ToProxyValue(result));
    IFR(result.CopyTo(ppResult));
    return hr;
}

HRESULT CObservableObjectProxy::SetPropertyValue(_In_ UINT32 propertyId,_In_ IInspectable *pNewValue)
{
    IFR(VerifyModelSource());

    foundation::InspectablePtr value(pNewValue);

    IFR(EnsureObservableObjectInfo());

    foundation_assert(_spCachedObservableObjectInfo != nullptr);
    foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
    IFR(_spCachedObservableObjectInfo->GetPropertyById(propertyId, spPropertyInfo.GetAddressOf()));

    IFR(FromProxyValue(spPropertyInfo,value));
    IFR(m_spObjectSource->SetProperty(propertyId,value));

    // Note: we could cache here but we prefer to Wait for the next 'GetPropertyValue'
    // or a Fire Event Notification since the 'Source' could be in a Model Changes State

    return S_OK;
}

HRESULT CObservableObjectProxy::CacheSerializedValues()
{
    foundation::ComPtr<foundation::IObjectSerializer> object_serializer_ptr;
    IFR_ASSERT(foundation::QueryInterface(
        this->m_spObjectSource,
        object_serializer_ptr.GetAddressOf()));

    foundation::library::DictionaryClassPtr content_options_ptr;
    content_options_ptr.SetValue(U("properties"), foundation::CreateValue(
				(UINT32)foundation::InstanceFlags::All |
				(UINT32)foundation::InstanceFlags::NoReserved |
				(UINT32)foundation::InstanceFlags::NoInspectable));

    LPCSTR_t binarySerializerFactoryid = U("foundation.BinaryStreamProvider");

    foundation::IStreamProvider *p_stream_provider = static_cast<foundation::IStreamProvider *>(foundation::GetActivationFactoryInstance(
        binarySerializerFactoryid));
    foundation_assert(p_stream_provider != nullptr);

    foundation::InspectablePtr serialized_content_options_ptr;
    IFR_ASSERT(p_stream_provider->Serialize(
        content_options_ptr,
        serialized_content_options_ptr.GetAddressOf()));

    // serialize the query properties from our 'Source' object
    foundation::InspectablePtr serialized_content_ptr;
    IFR(object_serializer_ptr->GetContent(
        foundation::HStringRef(binarySerializerFactoryid),
        serialized_content_options_ptr,
        serialized_content_ptr.GetAddressOf()
        ));

    foundation::InspectablePtr deserialized_content_ptr;
    IFR_ASSERT(p_stream_provider->Deserialize(serialized_content_ptr, deserialized_content_ptr.GetAddressOf()));
    foundation::DictionaryPtr content_properties_ptr;
    IFR_ASSERT(foundation::QueryInterface(deserialized_content_ptr, content_properties_ptr.GetAddressOf()));
    // with the deserialized properties let's fill our cache
    for (foundation::DictionaryPtr::keys_iterator keys_iter =
        content_properties_ptr.keys_begin();
        keys_iter != content_properties_ptr.keys_end();
    ++keys_iter)
    {
        foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
        IFR_ASSERT(_spCachedObservableObjectInfo->GetPropertyByName(
            *keys_iter,
            spPropertyInfo.GetAddressOf()));

        UINT32 cachedPropertyId;
        spPropertyInfo->GetId(&cachedPropertyId);

        foundation::InspectablePtr value_content_ptr;
        IFR_ASSERT(
            content_properties_ptr.GetValuePtr(
            _pal_GetStringRawBuffer(*keys_iter, nullptr), value_content_ptr));

        IFR_ASSERT(SetCachedPropertyValue(cachedPropertyId, value_content_ptr));
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::GetPropertyValue(_In_ UINT32 propertyId,_In_ IInspectable **ppInspectable)
{
    foundation::InspectablePtr value;

    IFR(VerifyModelSource());

    bool canCacheProperty = CanCacheProperty(propertyId);

    {
        foundation_lib::_FoundationLock csl(*this);
        if (canCacheProperty)
        {
            HRESULT hr = GetCachedPropertyValue(propertyId, value.GetAddressOf());
            if (hr == S_OK)
            {
                return value.CopyTo(ppInspectable);
            }
            IFHR(hr);
        }

        if (canCacheProperty &&
            this->m_pProxyObjectFactory->IsValuesCacheEnabled() &&
            (GetPropertyType(propertyId) & 0xff) != foundation::PropertyType_Inspectable)
        {
            // arriving here mean an attempt to serialize from 'Source'
            // all properties that are of type value. We should expect to arrive only
            // once in the life time of this Proxy
            IFR(CacheSerializedValues());
            return GetCachedPropertyValue(propertyId, ppInspectable);
        }
    }

    IFR(m_spObjectSource->GetProperty(propertyId,value.ReleaseAndGetAddressOf()));
    IFR_ASSERT(ToPropertyProxyValue(propertyId, value));
    _IFR_(OnGetSourceProperty(propertyId, value, canCacheProperty));

    IFR(value.CopyTo(ppInspectable));
    return S_OK;
}

HRESULT CObservableObjectProxy::OnGetSourceProperty(
    UINT32 propertyId,
    foundation::InspectablePtr& value,
    bool canCacheProperty)
{
    if (canCacheProperty)
    {
        // Cache this Property Id
        IFR_ASSERT(SetAndGetCachedPropertyValue(propertyId, value));
    }
    else if (IsReferencePropertyEnabled(propertyId))
    {
        // when global scope 'Cache' is enabled we would need to 
        // provide a way to track the used properties on a proxy
        // for properly recovering from a Source disconnect
        IFR_ASSERT(SetReferencePropertyValue(propertyId, value));
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::ToPropertyProxyValue(_In_ UINT32 propertyId,_Inout_ foundation::InspectablePtr& value)
{
    UNREFERENCED_PARAMETER(propertyId);

    if (this->IsProxyValueConversionNeeded(propertyId))
    {
        IFR(ToProxyValue(value));
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::CreateProxyPropertyChangedEventArgs(
    _In_ IPropertyChangedEventArgs* pArgs,
    _COM_Outptr_ IPropertyChangedEventArgs** ppPropertyChangedEventArgs)
{
    UINT32 propertyId;
    IFR(pArgs->GetProperty(&propertyId));

    bool bNeedToProxyValue = IsProxyValueConversionNeeded(propertyId);
    // Update my cached properties bag
    if(CanCacheProperty(propertyId))
    {
        foundation::InspectablePtr newValue;
        IFR(pArgs->GetNewValue(newValue.GetAddressOf()));
        if (bNeedToProxyValue)
        {
            IFR(ToProxyValue(newValue));
        }
        SetAndGetCachedPropertyValue(propertyId, newValue);
    }

    CProxyPropertyChangedEventArgs *pPropertyChangedEventArgs;
    foundation::ctl::ComInspectableObject<CProxyPropertyChangedEventArgs>::CreateInstance(&pPropertyChangedEventArgs);
    IFR_ASSERT(pPropertyChangedEventArgs->_Initialize(
        this,
        propertyId,
        pArgs,
        bNeedToProxyValue));

    *ppPropertyChangedEventArgs = pPropertyChangedEventArgs;
    return S_OK;
}

HRESULT CObservableObjectProxy::on_invoke_internal(IUnknown* pSender,IPropertyChangedEventArgs* pArgs)
{
    UNREFERENCED_PARAMETER(pSender);

    foundation::ComPtr<IPropertyChangedEventArgs> spProxyPropertyChangedEventArgs;

    if (foundation::IsTypeOf<pmod::library::IPropertyChangedWithCallbackEventArgs>(pArgs))
    {
        UINT32 propertyId;
        pArgs->GetProperty(&propertyId);
        if (CanCacheProperty(propertyId))
        {
            InvalidateCachedProperty(propertyId);
        }
        IFR_ASSERT(pmod::library::PropertyChangedEventArgsWIthCallbackImpl::CreateInstance(
            static_cast<IObservableObject *>(this),
            propertyId,
            spProxyPropertyChangedEventArgs.GetAddressOf()));
    }
    else
    {
        IFR_ASSERT(CreateProxyPropertyChangedEventArgs(pArgs, spProxyPropertyChangedEventArgs.GetAddressOf()));
    }

    IGNOREHR(FirePropertyChangedEvent(spProxyPropertyChangedEventArgs));

    IFR(on_source_property_changed_internal(spProxyPropertyChangedEventArgs));
    return S_OK;
}

HRESULT CObservableObjectProxy::on_source_property_changed_internal(_In_ IPropertyChangedEventArgs *pProxyPropertyChangedEventArgs)
{
    // Overrides to 'OnSourcePropertyChanged'
    if(m_spProxyDelegate != nullptr)
    {
        foundation::HStringPtr propertyName;
        pProxyPropertyChangedEventArgs->GetPropertyName(propertyName.GetAddressOf());
        IFR(m_spProxyDelegate->OnSourcePropertyChanged(propertyName));
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::PerformFirePropertyChangedEventArgs(
	_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
	IPropertyChangedEventArgs *pEventArgs)
{
    FireEventInternal(pEventHandlerContainer,pEventArgs);
    return S_OK;
}

HRESULT CObservableObjectProxy::PerformBatchFirePropertyChangedEventArgs(
	_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
	_In_ IBatchPropertyChangedEventArgs *pEventArgs)
{
    m_spEventHandler->FireEventWithInvoke<ProxyPropertyChangedEventHandler>(
		pEventHandlerContainer,
        AsInterface(),
        pEventArgs);
    return S_OK;
}

HRESULT CObservableObjectProxy::EnsureObservableObjectInfo()
{
    foundation_lib::_FoundationLock csl(*this);

    // Type IID_IObservableObject means the underlying Source is a Dictionary
    bool isTypeInfoDynamic = IsEqualGUID(m_IIDTypeCache, IID_IObservableObject) ? true : false;

    // and so it will automatically add dynamic properties
    bool hasIIDType = !IsEqualGUID(m_IIDTypeCache, foundation_GUID_NULL) && !isTypeInfoDynamic;

    if (_spCachedObservableObjectInfo == nullptr)
    {
        IFR(VerifyModelSource());

        // Next block will create the Property Model Info Hierarchy
        foundation::ComPtr<IObservableObjectInfo> spObservableObjectInfo;
        IFR(foundation::GetObjectTypeInfo(m_spObjectSource, spObservableObjectInfo.GetAddressOf()));
        foundation_assert(spObservableObjectInfo != nullptr);

        UINT32 typeId;
        IFR_ASSERT(spObservableObjectInfo->GetTypeId(&typeId));
        if (typeId  < 0x100)
        {
            isTypeInfoDynamic = true;
        }

        // our first attempt is to find an existing registered type if not Dictionary type
        IID iidType;
        spObservableObjectInfo->GetType(&iidType);
        
        if (!isTypeInfoDynamic)
        {
            // attempt to find in locally reserved
            foundation::GetTypeInfoAs(iidType, _spCachedObservableObjectInfo.GetAddressOf());
        }
        if(_spCachedObservableObjectInfo == nullptr)
        {
            // reference the 'Source' Type Info
            _spCachedObservableObjectInfo = spObservableObjectInfo;
        }
    }
    foundation_assert(_spCachedObservableObjectInfo != nullptr);
    _isViewModelTypeInfo = foundation::IsTypeOf<IViewModelInfo>(_spCachedObservableObjectInfo.Get());
    return S_OK;
}

HRESULT CObservableObjectProxy::EnsureObservableObjectInfoClass()
{
    foundation_lib::_FoundationLock csl(*this);

    if (!m_pCachedPropertyModelInfoClass)
    {
        foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
        IFR(this->GetTypeInfo(spModelTypeInfo.GetAddressOf()));
        foundation_assert(spModelTypeInfo != nullptr);
        IFR(foundation::QueryWeakReference(spModelTypeInfo,&m_pCachedPropertyModelInfoClass));
    }
    return S_OK;
}

bool CObservableObjectProxy::IsDictionary()
{
    return IsEqualGUID(m_IIDTypeCache, IID_IObservableObject) ? true:false;
}

bool CObservableObjectProxy::IsProxyModelCacheEnabled()
{
    return _PropertyModelProxyBaseType::IsProxyModelCacheEnabled() && !IsDictionary();
}

bool CObservableObjectProxy::IsPropertyInCacheContainer(_In_ UINT32 propertyId)
{
    EnsureObservableObjectInfoClass();

    foundation_assert(_spCachedObservableObjectInfo != nullptr);
    if (_isViewModelTypeInfo)
    {
        return propertyId >= PropertyModel_ViewModel_Start;
    }
    foundation_assert(propertyId < PropertyModel_ViewModel_Start);
    // TODO: check range
    return true;
}

bool CObservableObjectProxy::IsReferencePropertyEnabled(_In_ UINT32 propertyId)
{
    return IsModelCacheEnabled() &&
        propertyId < foundation::Property_ResevervedMask  &&
        IsPropertyInCacheContainer(propertyId);
}

foundation::PropertyType CObservableObjectProxy::GetPropertyType(_In_ UINT32 propertyId)
{
    foundation::PropertyType propertyType = foundation::PropertyType_Empty;
    IGNOREHR(EnsureObservableObjectInfoClass());

    foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
    if (SUCCEEDED(_spCachedObservableObjectInfo->GetPropertyById(propertyId, spPropertyInfo.GetAddressOf())))
    {
        spPropertyInfo->GetPropertyType(&propertyType);
    }
    return propertyType;
}

bool CObservableObjectProxy::IsProxyValueConversionNeeded(_In_ UINT32 propertyId)
{
    return BaseObjectProxy::IsProxyValueConversionNeeded(GetPropertyType(propertyId));
}

bool CObservableObjectProxy::CanCacheProperty(_In_ UINT32 propertyId)
{    
    // Note: check wide scope proxy and if Dictionary type
    if (!IsProxyModelCacheEnabled() ||
        propertyId >= foundation::Property_ResevervedMask ||
        !IsPropertyInCacheContainer(propertyId))
    {
        return false;
    }

    IFRF(EnsureObservableObjectInfoClass());
    UINT32 flags = 0;
    m_pCachedPropertyModelInfoClass->GetPropertyFlagsById(propertyId,&flags);

    bool bCacheEnabled = !IsOptionEnabled(flags,foundation::PropertyFlagType::NoCache);
    return bCacheEnabled;
}

bool CObservableObjectProxy::CanStrongRefCacheProperty(_In_ UINT32 propertyId)
{
    // Note: we assume the property can be cached
    IFRF(EnsureObservableObjectInfoClass());
    UINT32 flags = 0;
    m_pCachedPropertyModelInfoClass->GetPropertyFlagsById(propertyId, &flags);

	bool bCanStrongRef =
		!(IsOptionEnabled(flags, foundation::PropertyFlagType::IsWeakReference) ||
			IsOptionEnabled(flags, foundation::PropertyFlagType::IsAncestor));
    return bCanStrongRef;
}

void CObservableObjectProxy::SetIsAttached(bool isAttached)
{
    _PropertyModelProxyBaseType::SetIsAttached(isAttached);
}

void CObservableObjectProxy::InvalidatePropertiesCache()
{
    _spCachedFastPropertiesContainer.Release();
}

STDMETHODIMP CObservableObjectProxy::DisconnectFromSource(foundation::IInspectable **ppSourceInfo)
{
    if (IsAttachedNeeded() && IsProxyModelCacheEnabled() && _spCachedFastPropertiesContainer!= nullptr)
    {
        typedef std::vector<foundation::ComPtr<foundation::IPropertyIdPair>> _PropertyIdPair_Vector_type;
        _PropertyIdPair_Vector_type disconnectProperties;


        foundation::ArrayRefCountWrapper<foundation::IPropertyInfo> propertiesInfo;
        IFR_ASSERT(_spCachedObservableObjectInfo->GetProperties(
            static_cast<UINT32>(foundation::InstanceFlags::All),
            propertiesInfo.GetSizeAddressOf(),
            propertiesInfo.GetBufferAddressOf()));

        for (UINT32 index = 0; index < propertiesInfo.GetSize(); ++index)
        {
            UINT32 propertyId;
            propertiesInfo[index]->GetId(&propertyId);

            if (propertyId >= foundation::Property_ResevervedMask)
            {
                // skip reserved properties
                continue;
            }
            UINT8 cachedOptions;
            IFR_ASSERT(GetCachedPropertyOptions(propertyId, cachedOptions));

            foundation::InspectablePtr spPropertyValue;

            if (
                // not cached
                (cachedOptions & _PROPERTY_IS_CACHED_MASK) == 0 &&
                // and 'IsReference'
                (cachedOptions & _PROPERTY_IS_REFERENCE_MASK) != 0)
            {
                // we have a cache property that is 'refresh' only
                foundation_assert(m_spObjectSource);
                IFCEXPECT(m_spObjectSource != nullptr);
                ;
                IFR(m_spObjectSource->GetProperty(propertyId, spPropertyValue.GetAddressOf()));
            }
            else
            {
                HRESULT hr = GetCachedPropertyValue(propertyId, spPropertyValue.GetAddressOf());
                IFHR_ASSERT(hr);
                // avoid storing Weak References that already gone
                if (hr != S_OK)
                {
                    continue;
                }
            }
            foundation::ComPtr<foundation::IPropertyIdPair> spPropertyIdPair;
            IFR_ASSERT(foundation::library::CreateKeyValuePair<CPropertyIdPair>(
                propertyId,
                spPropertyValue.Get(),
                spPropertyIdPair.GetAddressOf()));

            disconnectProperties.push_back(spPropertyIdPair);
        }
        if (disconnectProperties.size())
        {
            IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
                (UINT32)disconnectProperties.size(),
                (foundation::IInspectable **)disconnectProperties.front().GetAddressOfPtr(),
                ppSourceInfo
                ));
        }
        return S_OK;
    }
    return _PropertyModelProxyBaseType::DisconnectFromSource(ppSourceInfo);
}

HRESULT CObservableObjectProxy::RestoreDisconnectedProperties(
    foundation::InspectableArrayWrapper& propertiesIdPair,
    _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    HRESULT hr = S_OK;
    for (UINT32 index = 0; index < propertiesIdPair.GetSize();++index)
    {
        foundation::ComPtr<foundation::IPropertyIdPair> spPropertyIdPair;
        IFR_ASSERT(foundation::QueryInterface(
            propertiesIdPair[index],
            spPropertyIdPair.GetAddressOf()));
        UINT32 propertyId;
        spPropertyIdPair->GetKey(&propertyId);
        foundation::InspectablePtr disconnectValue;
        spPropertyIdPair->GetValue(disconnectValue.GetAddressOf());

        // retrieve actual value
        foundation::InspectablePtr actualValue;
        IFHR(m_spObjectSource->GetProperty(propertyId,actualValue.GetAddressOf()));
        IFHR(ToProxyValue(actualValue));

        // Cached the value again
        if(CanCacheProperty(propertyId))
        {
            IFHR(SetCachedPropertyValue(propertyId, actualValue));
        }
        else
        {
            IFR(SetReferencePropertyValue(propertyId, actualValue));
        }

        if (foundation::pv_util::CompareInspectableValues(disconnectValue, actualValue) != 0)
        {
            foundation::ComPtr<IPropertyChangedEventArgs> spPropertyChangedEventArgs;
            IFHR_ASSERT(pmod::library::PropertyChangedEventArgsImpl::CreateInstance(
                static_cast<IObservableObject *>(this),
                propertyId,
                disconnectValue,
                actualValue,
                spPropertyChangedEventArgs.GetAddressOf()));

            syncSourceInfo.push_back(std::make_pair(this, spPropertyChangedEventArgs.Get()));
        }
    }
    return S_OK;
}

STDMETHODIMP CObservableObjectProxy::ConnectToSource(
    foundation::IInspectable *pSourceInfo,
    _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    if(IsProxyModelCacheEnabled())
    {
        IFR(VerifyModelSource());
        if (pSourceInfo != nullptr)
        {
            foundation::InspectableArrayWrapper propertiesIdPair;
            IFR_ASSERT(foundation::pv_util::GetInspectableArray(
                pSourceInfo,
                propertiesIdPair.GetSizeAddressOf(),
                propertiesIdPair.GetBufferAddressOf()
                ));
            IFR(RestoreDisconnectedProperties(propertiesIdPair, syncSourceInfo));
        }
    }
    else
    {
        // we don't have a way to know which properties were changed 
        foundation::InspectablePtr emptyValue;
        foundation::pv_util::CreateEmptyValue(emptyValue.GetAddressOf());

        foundation::ComPtr<IPropertyChangedEventArgs> spPropertyChangedEventArgs;
        IFR_ASSERT(pmod::library::PropertyChangedEventArgsImpl::CreateInstance(
            static_cast<IObservableObject *>(this),
            Property_All,
            emptyValue,
            emptyValue,
            spPropertyChangedEventArgs.GetAddressOf()));

        syncSourceInfo.push_back(std::make_pair(this, spPropertyChangedEventArgs.Get()));
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::CreateSourceEventHandlerDelegate(
    bool isFreeThreaded,
    pmod::IPropertyChangedEventHandler **ppEventHandlerDelegate)
{

    if (isFreeThreaded)
    {
        typedef foundation::EventHandlerDelegateWeakRefBase
            <
            IBaseObjectProxyInternal,
            CObservableObjectProxy,
            pmod::IPropertyChangedEventHandler,
            pmod::IPropertyChangedEventArgs
            >
            WeakRefHandlerDelegateBase;

        typedef BatchPropertyChangedEventHandlerDelegateBase<
            CObservableObjectProxy,
            WeakRefHandlerDelegateBase,
            TBatchPropertyChangedEventWeakRefInvoker
        > WeakRefPropertyChangedEventHandlerDelegate;
        return CreateEventHandlerDelegateWeakRefWithType<WeakRefPropertyChangedEventHandlerDelegate>(
            ppEventHandlerDelegate
            );
    }
    else
    {
        PropertyChangedEventHandlerDelegate *pPropertyChangedEventHandlerDelegate = nullptr;
        IFR_ASSERT(foundation::CreateDelegate(this, &pPropertyChangedEventHandlerDelegate));
        *ppEventHandlerDelegate = pPropertyChangedEventHandlerDelegate;
        return S_OK;
    }
}

HRESULT CObservableObjectProxy::InvokeInternal(IUnknown* pSender,IBatchPropertyChangedEventArgs* pArgs)
{
    if(this->IsInvokeDispatcherRequired())
    {
        return InvokeLater<
            CObservableObjectProxy,
            IBatchPropertyChangedEventArgs,
            &CObservableObjectProxy::OnSourceBatchPropertyChanged
        >(this,pSender,pArgs);
    }
    return OnSourceBatchPropertyChanged(pSender,pArgs);
}

HRESULT CObservableObjectProxy::OnSourceBatchPropertyChanged(IUnknown* pSender,pmod::IBatchPropertyChangedEventArgs* pArgs)
{
    UNREFERENCED_PARAMETER(pSender);

    // Create the CBatchPropertyChangedEventArgs Class
    foundation::ComPtr<pmod::library::BatchPropertyChangedEventArgsImpl, foundation::ctl::_ComBasePtrImpl> spBatchPropertyChangedEventArgs;
    IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(spBatchPropertyChangedEventArgs.GetAddressOf()));

    struct _ForEach : public CPropertyChangedEventArgsFunc {
    _ForEach(
        _In_ CObservableObjectProxy *pThis,
        _In_ pmod::library::BatchPropertyChangedEventArgsImpl *pBatchPropertyChangedEventArgsClass):
        m_pThis(pThis),
        m_pBatchPropertyChangedEventArgsClass(pBatchPropertyChangedEventArgsClass)
    {
    }

    HRESULT operator() (_In_ IPropertyChangedEventArgs *pPropertyChangedEventArgs)
    {
        foundation::ComPtr<IPropertyChangedEventArgs> spProxyPropertyChangedEventArgs;
        IFR_ASSERT(m_pThis->CreateProxyPropertyChangedEventArgs(pPropertyChangedEventArgs, spProxyPropertyChangedEventArgs.GetAddressOf()));
        IFR_ASSERT(m_pBatchPropertyChangedEventArgsClass->add_property_changed_event_args(spProxyPropertyChangedEventArgs));

        // Note: we do 'on_source_property_changed_internal' before we Fire the Batch Event
        IFR(m_pThis->on_source_property_changed_internal(spProxyPropertyChangedEventArgs));
        
        return S_OK;
    }
    private:
        CObservableObjectProxy                     *m_pThis;
		pmod::library::BatchPropertyChangedEventArgsImpl  *m_pBatchPropertyChangedEventArgsClass;
    };
    // Next Iteration wil populate the Batch Event Args
    _ForEach iter(this, spBatchPropertyChangedEventArgs);
    IFR(ForEachPropertyChangedEventArgs(pArgs,&iter));

    // Fire now
    IGNOREHR(FirePropertyChangedEvent(spBatchPropertyChangedEventArgs));

    return S_OK;
}

HRESULT CObservableObjectProxy::FirePropertyChangedEvent(_In_ IPropertyChangedEventArgs* pEventArgs)
{
    IFR(FireEventSupportInternal(
        &CObservableObjectProxy::PerformFirePropertyChangedEventArgs,
        pEventArgs));
    return S_OK;
}

HRESULT CObservableObjectProxy::FirePropertyChangedEvent(_In_ IBatchPropertyChangedEventArgs* pEventArgs)
{
    IFR(FireEventSupportInternal(
        &CObservableObjectProxy::PerformBatchFirePropertyChangedEventArgs,
        pEventArgs));
    return S_OK;
}

HRESULT CObservableObjectProxy::FireEventModelEvent(_In_ IEventModelEventArgs* pEventArgs)
{
    IFR(FireEventSupportInternal(
        &CObservableObjectProxy::PerformFireEventModelEventArgs,
        pEventArgs));
    return S_OK;
}

HRESULT CObservableObjectProxy::PerformFireEventModelEventArgs(
	_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
	_In_ IEventModelEventArgs *pEventArgs)
{
    if(m_spEventModelProxyEventHandler != nullptr)
    {
        IFR(m_spEventModelProxyEventHandler->FireEvent(pEventHandlerContainer,this->AsInspectable(),pEventArgs));
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::on_invoke_internal(foundation::IUnknown* pSender, pmod::IEventModelEventArgs* pEventArgs)
{
    UINT32 eventId;
    foundation::InspectablePtr spEventArgs;

    IFR_ASSERT(pEventArgs->GetEvent(&eventId));
    IFR_ASSERT(pEventArgs->GetEventArgs(spEventArgs.ReleaseAndGetAddressOf()));

    if (spEventArgs != nullptr)
    {
        IFR_ASSERT(ToProxyValue(spEventArgs));
    }
    foundation::ComPtr<IEventModelEventArgs> spEventModelEventArgs;
    IFR_ASSERT(pmod::library::EventModelEventArgsImpl::CreateInstance(
        eventId,
        spEventArgs,
        spEventModelEventArgs.GetAddressOf()));

    // Fire the Proxied Event
    IFR(FireEventModelEvent(spEventModelEventArgs));

    return S_OK;
}

HRESULT CObservableObjectProxy::OnSourceEventModel(IUnknown *pSender, IEventModelEventArgs *pEventArgs)
{
    if (this->IsInvokeDispatcherRequired())
    {
        return InvokeLater<
            CObservableObjectProxy,
            IEventModelEventArgs,
            &CObservableObjectProxy::on_invoke_internal
        >(this, pSender, pEventArgs);
    }
    return on_invoke_internal(pSender, pEventArgs);
}

HRESULT CObservableObjectProxy::AttachToEventModelSource()
{
    foundation_assert(this->m_spObjectSource);

    if(m_pEventModelProxyDelegate == nullptr)
    {
        if (this->IsFreeThreadApartment())
        {
            IFR_ASSERT(foundation::CreateEventHandlerDelegateWeakRef<IBaseObjectProxyInternal>(
                this,
                &CObservableObjectProxy::OnSourceEventModel,
                &this->m_pEventModelProxyDelegate));
        }
        else
        {
            CEventModelProxyConnection::_type_eventhandler_delegate *pT;
            IFR_ASSERT(CEventModelProxyConnection::CreateInstance(
                this,
                &CObservableObjectProxy::OnSourceEventModel,
                nullptr,
                &pT));
            this->m_pEventModelProxyDelegate = pT;
        }

    }
    foundation_assert(this->m_pEventModelProxyDelegate);

    // Ensure we set the Controller
    if (this->IsFreeThreadApartment())
    {
        EnableWeakRefCallbackType<foundation::DelegateWeakRefBase<IEventModelEventHandler>>(
            this->m_pEventModelProxyDelegate, 
            true);
    }
    else
    {
        SetCallback<CEventModelProxyConnection::_type_eventhandler_delegate>(
            this->m_pEventModelProxyDelegate,
            this,
            this->AsInspectable());
    }
    IFR(EventModelListenerBase::AttachEventHandler(this->m_spObjectSource,m_pEventModelProxyDelegate));
    return S_OK;
}

HRESULT CObservableObjectProxy::DetachFromEventModelSource()
{
    // Ensure the Event Model Delegate is nulled in case was created
    if(m_pEventModelProxyDelegate)
    {
        if (this->IsFreeThreadApartment())
        {
            EnableWeakRefCallbackType<foundation::DelegateWeakRefBase<IEventModelEventHandler>>(
                this->m_pEventModelProxyDelegate,
                false);
        }
        else
        {
            SetCallback<CEventModelProxyConnection::_type_eventhandler_delegate>(
                this->m_pEventModelProxyDelegate,
                nullptr,
                nullptr);
        }
    }

    foundation_assert(this->m_spObjectSource);
    if (this->IsSourceAlive())
    {
        IFR(EventModelListenerBase::DetachEventHandler(this->m_spObjectSource,m_pEventModelProxyDelegate));
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::AttachToSource()
{
    IFR(_PropertyModelProxyBaseType::AttachToSource());
    IFR(AttachToEventModelSource());

    if (IsProxyModelCacheEnabled() && _spCachedFastPropertiesContainer != nullptr)
    {
        // Initialize our Cache Fast Container
        std::vector<UINT32> referencedProperties;
        IFR_ASSERT(GetCachedPropertiesWithOptions(
            _spCachedFastPropertiesContainer,
            _PROPERTY_IS_REFERENCE_MASK,
            referencedProperties));
        if (referencedProperties.size())
        {
            // query reference properties from source
            foundation::InspectablePtr query_properties;
            IFR_ASSERT(foundation::pv_util::CreateUInt32ArrayValue(
                (UINT32)referencedProperties.size(),
                &referencedProperties.front(),
                query_properties.GetAddressOf()));

            foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> properties;
            IFR(this->m_spObjectSource->GetProperties(query_properties,
                properties.GetSizeAddressOf(),
                properties.GetBufferAddressOf()));

            for (UINT32 index = 0; index < properties.GetSize(); ++index)
            {
                UINT32 propertyId;
                foundation::InspectablePtr value;

                properties[index]->GetKey(&propertyId);
                properties[index]->GetValue(value.GetAddressOf());
                IFR(ToPropertyProxyValue(propertyId, value));
                // mark them as cached
                IFR_ASSERT(SetCachedPropertyValue(propertyId, value));
            }
        }
    }
    else
    {
        InvalidatePropertiesCache();
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::DetachFromSource()
{
    if (_spCachedFastPropertiesContainer != nullptr)
    {
        // we need to invalidate anything being cached until here
        UINT32 size;
        _spCachedFastPropertiesContainer->GetSize(&size);
        for (UINT32 propertyId = 1; propertyId <= size; ++propertyId)
        {
            UINT8 cachedOptions;
            IFR_ASSERT(GetCachedPropertyOptions(_spCachedFastPropertiesContainer, propertyId, cachedOptions));
            cachedOptions &= ~_PROPERTY_IS_CACHED_MASK;
            IFR_ASSERT(SetCachedPropertyOptions(_spCachedFastPropertiesContainer, propertyId, cachedOptions));
        }
    }

    IFR(DetachFromEventModelSource());
    IFR(_PropertyModelProxyBaseType::DetachFromSource());
    return S_OK;
}

// IDictionary Interface Impl
STDMETHODIMP CObservableObjectProxy::GetView(UINT32* size, foundation::IDictionaryPair ***ppProperties)
{
    IFR(VerifyModelSource());
    foundation::ComPtr<foundation::IDictionary> spDictionary;
    IFR(foundation::QueryInterface(m_spObjectSource,spDictionary.GetAddressOf()));

    foundation::ArrayRefCountWrapper<foundation::IDictionaryPair> properties;
    IFR(spDictionary->GetView(
            properties.GetSizeAddressOf(),
            properties.GetBufferAddressOf()));

    // returned properties
    std::vector<foundation::ComPtr<foundation::IDictionaryPair> > proxyProperties;

    for(UINT32 index = 0;index < properties.GetSize(); ++index)
    {
        foundation::InspectablePtr spValue;
        foundation::HStringPtr propertyName;

        IFR(properties[index]->GetKey(propertyName.GetAddressOf()));
        IFR(properties[index]->GetValue(spValue.GetAddressOf()));

        foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
        IFR(_spCachedObservableObjectInfo->GetPropertyByName(propertyName, spPropertyInfo.GetAddressOf()));

        UINT32 propertyId;
        IFR(spPropertyInfo->GetId(&propertyId));

        IFR(ToPropertyProxyValue(propertyId,spValue));

        foundation::ComPtr<foundation::IDictionaryPair> spPropertyNamePair;
        IFR_ASSERT(foundation::library::CreateDictionaryPairImpl(propertyName, spValue, spPropertyNamePair.GetAddressOf()));

        proxyProperties.push_back(spPropertyNamePair);
    }
    // copy now to public API
    ContainerRefCountToArray(proxyProperties,size,ppProperties);
    return S_OK;
}

STDMETHODIMP CObservableObjectProxy::GetSize(_Outptr_ UINT32* size)
{
    IFR(VerifyModelSource());

    foundation::ComPtr<foundation::IDictionary> spDictionary;
    IFR(foundation::QueryInterface(m_spObjectSource,spDictionary.GetAddressOf()));
    return spDictionary->GetSize(size);
}

HRESULT CObservableObjectProxy::GetPropertyId(_In_ HSTRING propertyName, _Outptr_ UINT32 *propertyId)
{
    foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
    IFR_PARAM_VALUE(_spCachedObservableObjectInfo->GetPropertyByName(propertyName, spPropertyInfo.GetAddressOf()), propertyName);
    return spPropertyInfo->GetId(propertyId);
}

STDMETHODIMP CObservableObjectProxy::Lookup(HSTRING propertyName, IInspectable** value)
{
    IFR(VerifyModelSource());

    if (IsDictionary())
    {
        foundation::ComPtr<foundation::IDictionary> spSourceDictionary;
        IFR(foundation::QueryInterface(m_spObjectSource, spSourceDictionary.GetAddressOf()));
        foundation::InspectablePtr spValue;
        IFR(spSourceDictionary->Lookup(propertyName, spValue.GetAddressOf()));
        IFR_ASSERT(ToProxyValue(spValue));
        return spValue.CopyTo(value);
    }
    else
    {
        UINT32 propertyId;
        IFR_PARAM(this->GetPropertyId(propertyName, &propertyId), propertyName);
        IFR(this->GetPropertyValue(propertyId, value));
    }
    return S_OK;
}

STDMETHODIMP CObservableObjectProxy::HasKey(HSTRING propertyName, bool* found)
{
    IFCPTR(found);

    IFR(VerifyModelSource());

    // validate we have PropertyModelInfo
    IFR(EnsureObservableObjectInfo());

    if (!IsDictionary())
    {
        UINT32 propertyId;
        HRESULT hr = this->GetPropertyId(propertyName, &propertyId);
        if (hr == foundation::E_PROPERTY_NOT_FOUND)
        {
            *found = false;
            return S_OK;
        }
        IFHR(hr);

        // if property could be cached we avoid the access to the 'Source'
        if (this->CanCacheProperty(propertyId))
        {
            foundation::InspectablePtr spValue;
            hr = GetCachedPropertyValue(propertyId, spValue.GetAddressOf());
            if (hr == S_OK)
            {
                *found = true;
                return S_OK;
            }
        }
    }
    // if not cached but the property id was resolved we need to ask the 'Source'
    foundation::ComPtr<foundation::IDictionary> spDictionary;
    IFR(foundation::QueryInterface(m_spObjectSource,spDictionary.GetAddressOf()));
    return spDictionary->HasKey(propertyName,found);
}

STDMETHODIMP CObservableObjectProxy::Insert(HSTRING propertyName, IInspectable *value,bool* replaced)
{
    foundation_lib::_FoundationLock csl(*this);

    IFR(VerifyModelSource());

    foundation::InspectablePtr spValue(value);

    IFR(EnsureObservableObjectInfo());

    foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
    // Note: when inserting properties it is possible that the next
    // call could fail by not finding the property
    _spCachedObservableObjectInfo->GetPropertyByName(propertyName, spPropertyInfo.GetAddressOf());
    IFR(FromProxyValue(nullptr,spValue));

    foundation::ComPtr<foundation::IDictionary> spDictionary;
    IFR(foundation::QueryInterface(m_spObjectSource,spDictionary.GetAddressOf()));
    return spDictionary->Insert(propertyName,spValue,replaced);

}

STDMETHODIMP CObservableObjectProxy::Remove(HSTRING propertyName)
{
    foundation_lib::_FoundationLock csl(*this);

    IFR(VerifyModelSource());

    foundation::ComPtr<foundation::IDictionary> spDictionary;
    IFR(foundation::QueryInterface(m_spObjectSource,spDictionary.GetAddressOf()));
    return spDictionary->Remove(propertyName);
}

STDMETHODIMP CObservableObjectProxy::Clear()
{
    foundation_lib::_FoundationLock csl(*this);

    IFR(VerifyModelSource());

    foundation::ComPtr<foundation::IDictionary> spDictionary;
    IFR(foundation::QueryInterface(m_spObjectSource,spDictionary.GetAddressOf()));
    return spDictionary->Clear();
}

STDMETHODIMP CObservableObjectProxy::Keys(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first)
{
    IFR(VerifyModelSource());

    foundation::ComPtr<foundation::IDictionary> spDictionary;
    IFR(foundation::QueryInterface(m_spObjectSource, spDictionary.GetAddressOf()));
    return spDictionary->Keys(first);
}

HRESULT CObservableObjectProxy::GetPropertyContainerValue(
    foundation::library::IFastPropertiesContainer *pFastPropertiesContainer,
    _In_ UINT32 propertyId,
    _In_ IInspectable **ppInspectable,
    _In_ UINT8 options)
{

    if (pFastPropertiesContainer != nullptr)
    {
        UINT8 cachedOptions;
        IFR_ASSERT(GetCachedPropertyOptions(pFastPropertiesContainer, propertyId, cachedOptions));

        if ((cachedOptions & options) != 0)
        {
            foundation::InspectablePtr spValue;
            IFR_ASSERT(pFastPropertiesContainer->GetValue(propertyId, spValue.GetAddressOf()));

            if (m_pProxyObjectFactory->UseCacheStrongRefForProperties() &&
                CanStrongRefCacheProperty(propertyId))
            {
                // object was already strong referenced and so return
                return spValue.CopyTo(ppInspectable);
            }

            foundation::PropertyType propertyType;
            IFR_ASSERT(pFastPropertiesContainer->GetPropertType(propertyId, &propertyType));

            HRESULT hr = FromCachedProxyValue(propertyType, spValue);
            IFHR_ASSERT(hr);
            if (hr == S_FALSE)
            {
                // weak reference is gone
                pFastPropertiesContainer->SetValue(propertyId, nullptr);
                // mark this property as not cached
                cachedOptions &= ~_PROPERTY_IS_CACHED_MASK;
                IFR_ASSERT(SetCachedPropertyOptions(pFastPropertiesContainer, propertyId, cachedOptions));
            }
            else
            {
                // object was stored weak referenced and was alive
                return spValue.CopyTo(ppInspectable);
            }
        }
    }
    // property was not cached or the weak reference was gone
    return S_FALSE;
}

HRESULT CObservableObjectProxy::GetCachedPropertyValue(_In_ UINT32 propertyId, _In_ IInspectable **ppInspectable)
{
    return GetPropertyContainerValue(
        _spCachedFastPropertiesContainer,
        propertyId, 
        ppInspectable,
        _PROPERTY_IS_CACHED_MASK);
}

HRESULT CObservableObjectProxy::GetPropertyContainerValue(
    _In_ UINT32 propertyId,
    _In_ IInspectable **ppInspectable,
    _In_ UINT8 options)
{
    foundation_lib::_FoundationLock csl(*this);

    return GetPropertyContainerValue(
        _spCachedFastPropertiesContainer, 
        propertyId, 
        ppInspectable,
        options);
}

HRESULT CObservableObjectProxy::EnsureCachedFastPropertiesContainer()
{
    foundation_lib::_FoundationLock csl(*this);

    if (_spCachedFastPropertiesContainer == nullptr)
    {
        EnsureObservableObjectInfo();
		foundation::library::FastPropertiesContainerParameters parameters;
        parameters._slotPrefixSize = sizeof(UINT8);
        parameters._pPropertyDataInfo = _spCachedObservableObjectInfo;
        IFR_ASSERT(CreateFastPropertiesContainer(
            &parameters,
            _spCachedFastPropertiesContainer.GetAddressOf()))
    }
    return S_OK;
}

HRESULT CObservableObjectProxy::SetCachedPropertyValue(_In_ UINT32 propertyId, _In_ IInspectable *pInspectable)
{
    foundation_lib::_FoundationLock csl(*this);

    return SetPropertyContainerValue(propertyId, pInspectable, _PROPERTY_IS_CACHED_MASK);
}

HRESULT CObservableObjectProxy::SetAndGetCachedPropertyValue(_In_ UINT32 propertyId, foundation::InspectablePtr& value)
{
    // this will guarantee that the object won't be released if it is a proxy just created
    foundation::InspectablePtr value_refcount = value;

    IFR_ASSERT(SetCachedPropertyValue(propertyId, value));
    return GetCachedPropertyValue(propertyId, value.ReleaseAndGetAddressOf());
}

HRESULT CObservableObjectProxy::SetPropertyContainerValue(
    _In_ UINT32 propertyId,
    _In_ IInspectable *pInspectable,
    _In_ UINT8 options)

{
    foundation_lib::_FoundationLock csl(*this);

    IFR_ASSERT(EnsureCachedFastPropertiesContainer());

    foundation_assert(propertyId < foundation::Property_ResevervedMask);

    foundation::PropertyType propertyType;
    IFR_ASSERT(_spCachedFastPropertiesContainer->GetPropertType(propertyId, &propertyType));

    // convert to storage value
    foundation::InspectablePtr spValue;

    bool allowWeakReference = !(
        m_pProxyObjectFactory->UseCacheStrongRefForProperties() &&
        CanStrongRefCacheProperty(propertyId));

    IFR_ASSERT(ToCachedProxyValue(propertyType, pInspectable, spValue, allowWeakReference));

    IFR_ASSERT(_spCachedFastPropertiesContainer->SetValue(propertyId, spValue));
    // mark this property as cached
    UINT8 cachedOptions;
    IFR_ASSERT(GetCachedPropertyOptions(propertyId, cachedOptions));
    cachedOptions |= options;
    IFR_ASSERT(SetCachedPropertyOptions(propertyId, cachedOptions));
    return S_OK;
}

HRESULT CObservableObjectProxy::InvalidateCachedProperty(_In_ UINT32 propertyId)
{
    foundation_lib::_FoundationLock csl(*this);

    if (_spCachedFastPropertiesContainer == nullptr)
    {
        return S_FALSE;
    }
    UINT8 cachedOptions;
    IFR_ASSERT(GetCachedPropertyOptions(propertyId, cachedOptions));
    cachedOptions &= ~_PROPERTY_IS_CACHED_MASK;
    IFR_ASSERT(SetCachedPropertyOptions(propertyId, cachedOptions));
    return S_OK;
}

HRESULT CObservableObjectProxy::SetReferencePropertyValue(_In_ UINT32 propertyId, _In_ IInspectable *pInspectable)
{
    return SetPropertyContainerValue(propertyId, pInspectable, _PROPERTY_IS_REFERENCE_MASK);
}

HRESULT CObservableObjectProxy::GetCachedPropertyOptions(
	foundation::library::IFastPropertiesContainer *pFastPropertiesContainer,
    _In_ UINT32 propertyId,
    UINT8& cachedOptions)
{
    foundation_assert(pFastPropertiesContainer != nullptr);
    foundation_assert(propertyId < foundation::Property_ResevervedMask);

    IFR_ASSERT(pFastPropertiesContainer->ReadCustomData(propertyId, &cachedOptions));
    return S_OK;
}
HRESULT CObservableObjectProxy::SetCachedPropertyOptions(
	foundation::library::IFastPropertiesContainer *pFastPropertiesContainer,
    _In_ UINT32 propertyId,
    UINT8 cachedOptions
    )
{
    foundation_assert(pFastPropertiesContainer != nullptr);
    foundation_assert(propertyId < foundation::Property_ResevervedMask);

    IFR_ASSERT(pFastPropertiesContainer->WriteCustomData(propertyId, &cachedOptions));
    return S_OK;
}


HRESULT CObservableObjectProxy::GetCachedPropertyOptions(_In_ UINT32 propertyId, UINT8& cachedOptions)
{
    return GetCachedPropertyOptions(_spCachedFastPropertiesContainer, propertyId, cachedOptions);
}

HRESULT CObservableObjectProxy::SetCachedPropertyOptions(_In_ UINT32 propertyId, UINT8 cachedOptions)
{
    return SetCachedPropertyOptions(_spCachedFastPropertiesContainer, propertyId, cachedOptions);
}

HRESULT CObservableObjectProxy::GetCachedPropertiesWithOptions(
	foundation::library::IFastPropertiesContainer *pFastPropertiesContainer,
    _In_ UINT32 mask,
    std::vector<UINT32>& cachedProperties)
{
    foundation::ArrayRefCountWrapper<foundation::IPropertyInfo> propertiesInfo;
    IFR_ASSERT(_spCachedObservableObjectInfo->GetProperties(
		static_cast<UINT32>(foundation::InstanceFlags::All),
        propertiesInfo.GetSizeAddressOf(),
        propertiesInfo.GetBufferAddressOf()));

    for (UINT32 index = 0; index < propertiesInfo.GetSize(); ++index)
    {
        UINT32 propertyId;
        propertiesInfo[index]->GetId(&propertyId);

        if (propertyId < foundation::Property_ResevervedMask)
        {
            UINT8 cachedOptions;
            IFR_ASSERT(GetCachedPropertyOptions(pFastPropertiesContainer, propertyId, cachedOptions));

            if ((cachedOptions & mask) != 0)
            {
                cachedProperties.push_back(propertyId);
            }
        }
    }
    return S_OK;
}

STDMETHODIMP CObservableObjectProxy::ResyncSource(
    IInspectable *pSource, 
    _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    foundation_lib::_FoundationLock csl(*this);

    foundation::ComPtr<foundation::library::IFastPropertiesContainer>
        spCurrentCachedFastPropertiesContainer = this->_spCachedFastPropertiesContainer;

    // early release so attach source does not attempt to access the source
    this->_spCachedFastPropertiesContainer.Release();

    IFR_ASSERT(_PropertyModelProxyBaseType::ResyncSource(pSource, syncSourceInfo));
    
    if (spCurrentCachedFastPropertiesContainer == nullptr)
    {
        // no caching was enabled on this
        return S_OK;
    }
    foundation_assert(_spCachedObservableObjectInfo != nullptr);

    // the event being posted later
    foundation::ComPtr<pmod::library::BatchPropertyChangedEventArgsImpl, foundation::ctl::_ComBasePtrImpl> spProxyBatchPropertyChangedEventArgs;
    
    std::vector<UINT32> cachedProperties;
    IFR_ASSERT(GetCachedPropertiesWithOptions(
        spCurrentCachedFastPropertiesContainer,
        (_PROPERTY_IS_CACHED_MASK | _PROPERTY_IS_REFERENCE_MASK),
        cachedProperties));

    // anything to ask
    if (cachedProperties.size())
    {
        foundation::InspectablePtr query_properties;
        IFR_ASSERT(foundation::pv_util::CreateUInt32ArrayValue(
            (UINT32)cachedProperties.size(),
            &cachedProperties.front(),
            query_properties.GetAddressOf()));

        foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> properties;
        IFR(this->m_spObjectSource->GetProperties(query_properties,
            properties.GetSizeAddressOf(),
            properties.GetBufferAddressOf()));

        for (UINT32 index = 0; index < properties.GetSize(); ++index)
        {
            UINT32 propertyId;
            foundation::InspectablePtr sourcePropertyValue;

            properties[index]->GetKey(&propertyId);
            properties[index]->GetValue(sourcePropertyValue.GetAddressOf());

            foundation::InspectablePtr containerPropertyValue;
            HRESULT hr = GetPropertyContainerValue(
                spCurrentCachedFastPropertiesContainer,
                propertyId, 
                containerPropertyValue.GetAddressOf(),
                (_PROPERTY_IS_CACHED_MASK | _PROPERTY_IS_REFERENCE_MASK));

            UINT8 cachedOptions;
            IFR_ASSERT(GetCachedPropertyOptions(spCurrentCachedFastPropertiesContainer, propertyId, cachedOptions));

            bool isCached = (cachedOptions & _PROPERTY_IS_CACHED_MASK) != 0;

            if (containerPropertyValue != nullptr &&
                sourcePropertyValue != nullptr &&
                foundation::IsTypeOf<IBaseObjectProxyInternal>(containerPropertyValue))
            {
                // for Proxy type properties we need to sync the 'Source'
                foundation::ComPtr<IBaseObjectProxyInternal> spBaseObjectProxyInternal;
                IFR_ASSERT(foundation::QueryInterface(containerPropertyValue, spBaseObjectProxyInternal.GetAddressOf()));
                IFR(spBaseObjectProxyInternal->ResyncSource(sourcePropertyValue,syncSourceInfo));
            }
            else if (isCached)
            {
                IFR(ToPropertyProxyValue(propertyId, sourcePropertyValue));

                if (foundation::pv_util::CompareInspectableValues(
                    containerPropertyValue,
                    sourcePropertyValue) != 0)
                {
                    // value has changed from the Source
                    containerPropertyValue = sourcePropertyValue;
                    // batch a new PropertyChanged Event
                    foundation::ComPtr<IPropertyChangedEventArgs> spPropertyChangedEventArgs;
                    IFHR_ASSERT(pmod::library::PropertyChangedEventArgsImpl::CreateInstance(
                        static_cast<IObservableObject *>(this),
                        propertyId,
                        containerPropertyValue,
                        sourcePropertyValue,
                        spPropertyChangedEventArgs.GetAddressOf()));
                    if (spProxyBatchPropertyChangedEventArgs == nullptr)
                    {
                        IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(spProxyBatchPropertyChangedEventArgs.GetAddressOf()));
                    }
                    spProxyBatchPropertyChangedEventArgs->add_property_changed_event_args(spPropertyChangedEventArgs);
                }
            }
            // cache the value again in our Container
            if (isCached)
            {
                IFHR_ASSERT(this->SetCachedPropertyValue(propertyId, containerPropertyValue));
            }
            else
            {
                IFHR_ASSERT(this->SetReferencePropertyValue(propertyId, containerPropertyValue));
            }
        }
    }
    if (spProxyBatchPropertyChangedEventArgs != nullptr)
    {
        syncSourceInfo.push_back(std::make_pair(
            static_cast<IBaseObjectProxyInternal *>(this),
            spProxyBatchPropertyChangedEventArgs->CastToInspectable()));
    }
    return S_OK;
}

STDMETHODIMP CObservableObjectProxy::FireResyncEvent(foundation::IUnknown *pEventArg)
{
    LogSourceEvent(
        foundation::LoggingLevel::Info,
        (UINT32)proxy::BaseProxy_Category::ResyncEvent,
        pEventArg);

    foundation::ComPtr<IPropertyChangedEventArgs> spPropertyChangedEventArgs;
    if (SUCCEEDED(foundation::QueryInterface(pEventArg, spPropertyChangedEventArgs.GetAddressOf())))
    {
        return this->FirePropertyChangedEvent(spPropertyChangedEventArgs);
    }
    foundation::ComPtr<IBatchPropertyChangedEventArgs> spProxyBatchPropertyChangedEventArgs;
    IFR_ASSERT(foundation::QueryInterface(pEventArg, spProxyBatchPropertyChangedEventArgs.GetAddressOf()));
    return this->FirePropertyChangedEvent(spProxyBatchPropertyChangedEventArgs);
}

