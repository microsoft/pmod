/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseObjectProxy.cpp
****/
#include "pch.h"
#include "BaseObjectProxy.h"
#include "ProxyObjectFactory.h"
#include "BaseModelSource.h"

#include <pmod/errors.h>
#include <pmod/interfaces/source_model.h>
#include <Proxy/ProxyTypeInfo.g.h>

// {71FD10E6-3991-442E-8D59-20B078D2EAB4}
EXTERN_C const GUID IID_IBaseObjectProxyInternal =
{ 0x71fd10e6, 0x3991, 0x442e,{ 0x8d, 0x59, 0x20, 0xb0, 0x78, 0xd2, 0xea, 0xb4 } };

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::proxy;


class CProxyObjectImpl :
    public _ProxyObjectImplBase<>
{
public:
    HRESULT _Initialize(BaseObjectProxy *pBaseObjectProxy)
    {
        foundation_assert(pBaseObjectProxy != nullptr);
        m_pBaseObjectProxy = pBaseObjectProxy;
        return S_OK;
    }
protected:
    CProxyObjectImpl() :
        m_pBaseObjectProxy(nullptr)
    {}

    HRESULT GetProxyUniqueId(UINT32* pValue) override
    {
        *pValue = m_pBaseObjectProxy->m_ProxyUniqueId;
        return S_OK;
    }

    HRESULT GetFactory(IProxyObjectFactory ** pValue) override
    {
        *pValue = static_cast<IProxyObjectFactory *>(m_pBaseObjectProxy->m_pProxyObjectFactory);
        if (*pValue != nullptr)
        {
            (*pValue)->AddRef();
        }
        return S_OK;
    }

    BaseObjectProxy *m_pBaseObjectProxy;
};

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

FoundationTracker& CBaseProxyTrackerTracker::_GetInstance()
{
    static CBaseProxyTrackerTracker _FoundationTracker;
    return _FoundationTracker;
}
CBaseProxyTrackerTracker::CBaseProxyTrackerTracker() :
    FoundationTracker(U("CBaseProxyTrackerTracker"))
{
}

CBaseProxyTrackerTracker::~CBaseProxyTrackerTracker()
{
    SetIsTrackEnabled(false);
}

bool CBaseProxyTrackerTracker::IsTraceCountEnabled()
{
    return foundation::debug_util::IsOptionsFlag(0x0100 /*_OptionTrackProxyCountObjects*/);
}

template<> bool _FoundationTrackerInstance<CBaseProxyTrackerTracker>::_isTrackEnabled = true;

template<> void _FoundationTrackerInstance<CBaseProxyTrackerTracker>::SetIsTrackEnabled(bool value)
{
    _isTrackEnabled = value;
}

template<> bool _FoundationTrackerInstance<CBaseProxyTrackerTracker>::IsTrackEnabled()
{
    return _isTrackEnabled;
}
#endif

static void GetProxyRuntimeClass(
    _In_ string_t& typeName,
    _In_ const CHAR_t* proxyClassName)
{
    typeName = U("Microsoft.PropertyModel.Proxy.");
    typeName.append(proxyClassName);
}
static HRESULT ArrayToInspectableVector(
    foundation::InspectablePtr& inspectable_array_value,
    _Inspectable_ptr_vector_type& values)
{
    // unbox array and convert back from Proxy Values
    InspectableArrayWrapper array_values;
    IFR_ASSERT(pv_util::GetValueArray(
        inspectable_array_value,
        array_values.GetSizeAddressOf(),
        array_values.GetBufferAddressOf()));
    // copy into a vector array for further proxy conversion
    values.resize(array_values.GetSize());

    for (UINT32 index = 0; index < array_values.GetSize(); ++index)
    {
        values[index] = array_values[index];
    }
    return S_OK;
}

BaseObjectProxy::BaseObjectProxy():
    m_pProxyObjectFactory(nullptr),
    m_pInnerProxyObject(nullptr),
    m_UniqueId(0),
    m_ProcessId(0),
    m_Disposed(false),
    m_IIDTypeCache(GUID_NULL),
    m_isPurgeable(false),
    m_PurgeState(false),
    m_PurgeStateEnabled(false),
    m_SourceConnectState(true),
    m_InSourceConnectState(false),
    m_IsViewModelProxy(false),
    m_IsProxyObserved(false),
    m_ProxyUniqueId(0)
{
}

HRESULT BaseObjectProxy::_Initialize(CProxyObjectFactory *pProxyObjectFactory, bool IsFreeThreadedEnabled)
{
#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
    if (CBaseProxyTrackerTracker::IsTrackEnabled())
    {
        CBaseProxyTrackerTracker::_GetInstance().AddInstance(this);
    }
#endif

    foundation_assert(pProxyObjectFactory);
    // Weak reference source support
    this->_InitializeComposableWeakReferenceSource(false,false);

    m_pProxyObjectFactory = pProxyObjectFactory;
    // Add Factory Class Ref
    pProxyObjectFactory->CastToInspectable()->AddRef();

    // if 'MultiThreaded' support
    m_IsFreeThreaded = m_pProxyObjectFactory->IsFreeThreaded() || IsFreeThreadedEnabled;
    if (m_IsFreeThreaded)
    {
        // Create the 'shared' protection section
        IFR_ASSERT(m_critsec.Init());
    }

    IFR_ASSERT(ctl::inspectable::AggregableCreator<CProxyObjectImpl>::CreateInstanceWithInitialize(
        this->CastToInspectable(),
        &m_pInnerProxyObject,
        this));
    return S_OK;
}

void BaseObjectProxy::ReleaseProxyFactory()
{
    if(m_pProxyObjectFactory)
    {
        // Release the Proxy Factory
        foundation::ctl::inspectable::CastToInspectable(m_pProxyObjectFactory)->Release();
        m_pProxyObjectFactory = nullptr;
    }
}

void BaseObjectProxy::FinalRelease()
{
#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
    if (CBaseProxyTrackerTracker::IsTrackEnabled())
    {
        CBaseProxyTrackerTracker::_GetInstance().RemoveInstance(this);
    }
#endif
    this->ReleaseInterfaceAdapters();
    ReleaseProxyFactory();
    RELEASE_INTERFACE(m_pInnerProxyObject);

    _BaseObjectProxyBaseType::FinalRelease();
}

HRESULT BaseObjectProxy::Dispose()
{
    m_Disposed = true;

	_InterfaceAdapterContainerType adapters = this->_GetInterfaceAdapters();
    for (_InterfaceAdapterContainerType::const_iterator iter = adapters.begin();
        iter != adapters.end();
        ++iter)
    {
        foundation::library::IDisposable *pDisposable = nullptr;
        if(SUCCEEDED(QueryWeakReference(*iter, &pDisposable)))
        {
            pDisposable->Dispose();
        }
    }
    // Dispose will release our Source Model so there is no point in mantaining the proxy with
    // a Factory reference
    ReleaseProxyFactory();
    return S_OK;
}

HRESULT BaseObjectProxy::VerifyDisposed()
{
    IFCEXPECTEx(!m_Disposed, pmod::E_PROXY_DISPOSED);
    IFCEXPECTEx(m_pProxyObjectFactory != nullptr, pmod::E_PROXY_NO_FACTORY);
    return S_OK;
}

HRESULT BaseObjectProxy::VerifyModelSource()
{
    IFCEXPECTEx(this->GetObjectSource() != nullptr, pmod::E_PROXY_NO_SOURCE);
    return this->VerifyDisposed();
}

HRESULT BaseObjectProxy::DisconnectFromSource(IInspectable **ppSourceInfo)
{
    UNREFERENCED_PARAMETER(ppSourceInfo);
    return S_OK;
}

HRESULT BaseObjectProxy::ConnectToSource(IInspectable *pSourceInfo, _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    UNREFERENCED_PARAMETER(pSourceInfo);
    UNREFERENCED_PARAMETER(syncSourceInfo);
    return S_OK;
}

HRESULT BaseObjectProxy::AddToCachedInstances()
{
    foundation_assert(m_pProxyObjectFactory);
    IFR_ASSERT(this->m_pProxyObjectFactory->AddToCachedInstances(static_cast<IObject *>(this)));
    return S_OK;
}

HRESULT BaseObjectProxy::ClearFromCachedInstances()
{
    // at this point we could have been already dispose and so our 
    // ProxyObjectFactory could be already null
    if (m_pProxyObjectFactory != nullptr &&
        m_pProxyObjectFactory->IsRemoveProxyFromCacheEnabled() && 
        this->m_UniqueId != 0)
    {
        IFR_ASSERT(this->m_pProxyObjectFactory->RemoveProxyFromCache(
            this->m_ProcessId,
            this->m_UniqueId));
    }
    // clear cached source properties
    this->m_UniqueId = 0;
    this->m_ProcessId = 0;
    this->m_IIDTypeCache = foundation_GUID_NULL;
    return S_OK;
}

HRESULT BaseObjectProxy::SetSourceConnectState( bool newValue)
{
    m_InSourceConnectState = true;
    HRESULT hr = SetSourceConnectStateInternal(newValue);
    m_InSourceConnectState = false;
    IFHR(hr);
    m_SourceConnectState = newValue;
    return S_OK;
}

bool BaseObjectProxy::IsSourceConnectState()
{
    return m_SourceConnectState;
}

HRESULT BaseObjectProxy::SetSourceConnectStateInternal( bool newValue)
{
    IFR(As<pmod::ISourceModelBase>()->SetAttachEnabled(newValue));
    return S_OK;
}

bool BaseObjectProxy::IsViewModelProxy()
{
    return m_IsViewModelProxy;
}

void BaseObjectProxy::SetIsViewModelProxy(bool newValue)
{
    m_IsViewModelProxy = newValue;
}

bool BaseObjectProxy::_IsEventSupportEnabled()
{
    return this->_GetEventDispatcher() != nullptr;
}

bool BaseObjectProxy::_IsContinueOnFireEventError()
{
    return false;
}

foundation::library::IDispatcher *BaseObjectProxy::_GetEventDispatcher()
{
    return m_pProxyObjectFactory != nullptr ? m_pProxyObjectFactory->GetEventDispatcherInternal():nullptr;
}

HRESULT BaseObjectProxy::IsProxyComposed(bool& isComposed)
{
    isComposed = IsComposed() ? true:false;
    return S_OK;
}

void BaseObjectProxy::EvaluateIsObservedChanged(bool isObserved)
{
    if (isObserved != m_IsProxyObserved)
    {
        this->m_IsProxyObserved = isObserved;
        // invoke virtual method to handle the observation change
        this->OnIsObservedChanged(isObserved);
        // ensure _TInterfaceAdapterHostBase class is aware
        this->FireOnObservedChanged(isObserved);
    }
}

void BaseObjectProxy::EvaluateIsObservedChanged()
{
    EvaluateIsObservedChanged(HasProxyEventHandlers());
}

bool BaseObjectProxy::HasProxyEventHandlers()
{
    if (this->GetProxyCountEventHandlers() > 0)
    {
        return true;
    }
    bool hasAnyAdapterEventHandler = false;
    IInterfaceAdapterHost *pModelAdapterHost = nullptr;
    if (SUCCEEDED(QueryWeakReference(this->AsInspectable(), &pModelAdapterHost)))
    {
        pModelAdapterHost->HasAnyAdapterEventHandlers(&hasAnyAdapterEventHandler);
    }
    return hasAnyAdapterEventHandler;
}

HRESULT BaseObjectProxy::_OnCustomFireEvent(
	_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
	_In_ IUnknown *pEventArgs)
{
    return FireModelAdaptersEvent(pFireEventHandlerContainer,pEventArgs);
}

HRESULT BaseObjectProxy::FireModelAdaptersEvent(
	_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
	IUnknown *pEventArgs)
{
    IInterfaceAdapterHost *pModelAdapterHost = nullptr;
    IFR(QueryWeakReference(this->AsInspectable(),&pModelAdapterHost));
    return pModelAdapterHost->FireAdaptersEventArgs(pFireEventHandlerContainer,pEventArgs);
}

HRESULT BaseObjectProxy::_QueryInheritedAdapterInterface(REFIID iid, IUnknown **ppInterface)
{
    return BaseObjectProxy::_ComInspectableBaseType::QueryInterfaceImpl(iid, ppInterface);
}

#if defined(_WINDOWS_RUNTIME)
HRESULT BaseObjectProxy::GetRuntimeClassNameImpl(_Out_ HSTRING *className)
{
    IObject *pAsObject = As<IObject>();

    foundation::ComPtr<IObjectTypeInfo> spTypeInfo;
    IFR(pAsObject->GetTypeInfo(spTypeInfo.GetAddressOf()));

    IID iidType;
    IFR(spTypeInfo->GetType(&iidType));

    string_t typeName;
    if(iidType == ICommand::GetIID())
    {
        GetProxyRuntimeClass(typeName,U("CommandProxy"));
    }
    else if (IsTypeOf<IEnumerableTypeInfo>(spTypeInfo))
    {
        GetProxyRuntimeClass(typeName,U("ObservableCollectionProxy"));
    }
    else if(iidType == IObservableObject::GetIID())
    {
        GetProxyRuntimeClass(typeName,U("ObservableObjectProxy"));
    }
    else if(iidType == IAsyncOperation::GetIID())
    {
        GetProxyRuntimeClass(typeName,U("AsyncOperationProxy"));
    }
    //else if(iidType == SimpleCommandInfo::IIDType)
    //{
    //    typeName = U("Microsoft.Internal.PropertyModel.ISimpleCommandModel");
    //}
    else
    {
        IFR(spTypeInfo->GetName(className));
        return S_OK;
    }

    IFR(_pal_CreateString(typeName.c_str(), (UINT32)typeName.size(),className));
    return S_OK;
}
#endif

HRESULT BaseObjectProxy::QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
{
    if (iid == IBaseObjectProxyInternal::GetIID())
    {
        *ppInterface = static_cast<IBaseObjectProxyInternal *>(this);
    }  
    else if (iid == IProxyObject::GetIID())
    {
        return ::QueryWeakReference(m_pInnerProxyObject, iid, ppInterface);
    }
    else if (iid == IObject::GetIID())
    {
        *ppInterface = static_cast<IObject *>(this);
    }
    else if (iid == IFireEventHandlerContainer::GetIID())
    {
        *ppInterface = static_cast<IFireEventHandlerContainer *>(this);
    }
    else if (iid == foundation::library::ICriticalSection::GetIID())
    {
        *ppInterface = static_cast<foundation::library::ICriticalSection *>(this);
    }
    else if (iid == foundation::library::IEventSourceClass::GetIID())
    {
        *ppInterface = static_cast<foundation::library::IEventSourceClass *>(this);
    }
    else if (iid == IID_IUnknown || iid == IID_IInspectable)
    {
        return _BaseObjectProxyBaseType::QueryInterfaceImpl(iid,ppInterface);
    }
    else
    {
        return _BaseObjectProxyBaseType::QueryInterfaceImpl(iid, ppInterface);
    }
    return S_OK;
}

HRESULT BaseObjectProxy::_QueryCreateAdapterInterfaceImpl(
        IInspectable* pOuter,
        IInspectable* pInner,
        REFIID iid,
        REFIID iidHostType,
        IInspectable **ppModeAdapterNewInstance)
{
    //if (m_pProxyObjectFactory)
    //{
    //    return m_pProxyObjectFactory->QueryCreateInterfaceAdapter(
    //            pOuter,
    //            pInner,
    //            iidHostType,
    //            iid,
    //            ppModeAdapterNewInstance);
    //}
    return foundation::library::_AdapterHostResolverBase::_QueryCreateAdapterInterfaceImpl(pOuter, pInner, iid, iidHostType, ppModeAdapterNewInstance);
}


_pal_AtomicIntegerType BaseObjectProxy::AddRefImpl()
{
    _pal_AtomicIntegerType refCnt = _BaseObjectProxyBaseType::AddRefImpl();

#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    debug_util::TrackRefCount(
        GetTypeInfo(),
        _OptionTrackRefCountAllProxies | this->GetTrackRefCountOption(),
        TrackingInfoType_AddRef,
        U("BaseObjectProxy::AddRefImpl"), 
        m_UniqueId, 
        refCnt);
#endif
    HandleRefCountChanged(true,refCnt);
    return refCnt;
}

_pal_AtomicIntegerType BaseObjectProxy::ReleaseImpl()
{
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    IObjectTypeInfo *pTypeInfo = GetTypeInfo();
    AddRefInterface(pTypeInfo);
    ULONG uniqueId = m_UniqueId;
    UINT32 tracking_ref_count_option = this->GetTrackRefCountOption();
#endif
    _pal_AtomicIntegerType refCnt = _BaseObjectProxyBaseType::ReleaseImpl();
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    debug_util::TrackRefCount(
        pTypeInfo,
        _OptionTrackRefCountAllProxies | tracking_ref_count_option,
        refCnt ? TrackingInfoType_Release : TrackingInfoType_FinalRelease,
        U("BaseObjectProxy::ReleaseImpl"),
        uniqueId,
        refCnt);
    ReleaseInterface(pTypeInfo);
#endif
    if(refCnt)
    {
        bool purgeState = m_PurgeState;
        HandleRefCountChanged(false,refCnt);
        // if after this call we are in Purge State
        if(m_PurgeState && !purgeState)
        {
            foundation_assert(m_pProxyObjectFactory);
            m_pProxyObjectFactory->OnProxyObjectPurgeReady(As<IProxyObject>());
        }
    }
    return refCnt;
}
#if defined(DEBUG) && (defined(TRACK_REF_COUNT) || defined(TRACK_FOUNDATION_ENABLED))

IObjectTypeInfo *BaseObjectProxy::GetTypeInfo()
{
    if (m_spTypeInfoCache == nullptr)
    {
        IObject *pAsObject = As<IObject>();
        HRESULT hr = pAsObject->GetTypeInfo(m_spTypeInfoCache.GetAddressOf());
        foundation_assert(SUCCEEDED(hr));
    }
    return m_spTypeInfoCache;
}

#endif

STDMETHODIMP BaseObjectProxy::GetUniqueId(UINT32 *uniqueId)
{
    *uniqueId = m_UniqueId;
    return S_OK;
}

STDMETHODIMP BaseObjectProxy::GetProcessId(UINT32 *processId)
{
    *processId = m_ProcessId;
    return S_OK;
}


STDMETHODIMP BaseObjectProxy::GetTypeInfo(foundation::IObjectTypeInfo** ppValue)
{
    IFCPTR(ppValue);
    IFR(this->VerifyModelSource());

    // TODO: we may want to Proxy the returned references
    return this->GetObjectSource()->GetTypeInfo(ppValue);
}



STDMETHODIMP BaseObjectProxy::OnAdapterAddHandler(_In_ IUnknown* pSender,_In_ IUnknown* pHandler)
{
    UNREFERENCED_PARAMETER(pSender);
    UNREFERENCED_PARAMETER(pHandler);
    this->EvaluateIsObservedChanged(true);
    return S_OK;
}

STDMETHODIMP BaseObjectProxy::OnAdapterRemoveHandler(_In_ IUnknown* pSender,_In_ IUnknown* pHandler)
{
    this->EvaluateIsObservedChanged();
    return S_OK;
}

HRESULT BaseObjectProxy::SetPurgeStateEnabled(bool newValue)
{
    m_PurgeStateEnabled = newValue;
    return S_OK;
}

void BaseObjectProxy::SetPurgeState(bool newValue)
{
    if(newValue != m_PurgeState)
    {
        m_PurgeState = newValue;
        foundation_assert(m_pProxyObjectFactory);
        m_pProxyObjectFactory->OnProxyModelPurgeStateChanged(
            static_cast<IBaseObjectProxyInternal *>(this),newValue);
    }
}

void BaseObjectProxy::SetIsPurgeable(bool newValue)
{
    if(newValue != m_isPurgeable)
    {        
        m_isPurgeable = newValue;
        SetPurgeState(EvaluatePurgeState(As<_IBaseModelSource>()->IsAttached()));
    }
}

bool BaseObjectProxy::EvaluatePurgeState(bool isAttached)
{
    return m_isPurgeable ? 
            (
            IsPurgeIfAttached()     // If Purge on Attached is true we follow the 'IsPurgeable' State
            ||
            !isAttached             // we require not be attached to be in Purge State
            ) 
            : false;                // we are not in Purgeable State
}

void BaseObjectProxy::HandleRefCountChanged(bool addRef,ULONG refCnt)
{
    if(IsPurgeStateEnabledInternal())
    {
        if(addRef && refCnt == 2)
        {
            // Move out from Purge State
            SetIsPurgeable(false);
        }
        else if(!addRef && refCnt == 1)
        {
            // Move Into Purge State
            SetIsPurgeable(true);
        }
    }
}

bool BaseObjectProxy::IsAttachEnabledInternal()
{
    return m_pProxyObjectFactory != nullptr ?
        m_pProxyObjectFactory->IsSourceConnectState() ? true:false:
        false;
}

bool BaseObjectProxy::IsFreeThreaded()
{
    foundation_assert(m_pProxyObjectFactory);
    return m_pProxyObjectFactory->IsFreeThreaded();
}

bool BaseObjectProxy::IsSourceFreeThreaded()
{
    foundation_assert(m_pProxyObjectFactory);
    return m_pProxyObjectFactory->IsSourceFreeThreaded();
}

bool BaseObjectProxy::IsModelCacheEnabled()
{
    foundation_assert(m_pProxyObjectFactory);
    return m_pProxyObjectFactory->IsModelCacheEnabled();
}

bool BaseObjectProxy::IsProxyModelCacheEnabled()
{
    return IsModelCacheEnabled() && As<_IBaseModelSource>()->IsAttached();
}

bool BaseObjectProxy::IsPurgeStateEnabledInternal()
{
    return m_PurgeStateEnabled && !m_Disposed && m_UniqueId;
}

bool BaseObjectProxy::IsPurgeIfAttached()
{
    foundation_assert(m_pProxyObjectFactory);
    return m_pProxyObjectFactory->IsPurgeIfAttached();
}

bool BaseObjectProxy::IsInvokeDispatcherRequired()
{
    foundation_assert(m_pProxyObjectFactory);
    return m_pProxyObjectFactory->IsInvokeDispatcherRequired();
}

HRESULT BaseObjectProxy::InvokeUsingDispatcher(foundation::library::IDelegateDispatchable *pDispatchable)
{
    foundation_assert(m_pProxyObjectFactory);
    return m_pProxyObjectFactory->InvokeUsingDispatcher(pDispatchable);
}

// Convert a 'Raw' Variant to Variant that could contain Proxy Model
HRESULT BaseObjectProxy::ToProxyValue(_Inout_ foundation::InspectablePtr& value)
{
    foundation_assert(m_pProxyObjectFactory);
    IFR(VerifyDisposed());
    if (pv_util::IsValueArray(value))
    {
        _Inspectable_ptr_vector_type values;
        IFR_ASSERT(ArrayToInspectableVector(value, values));

        IFR(ToProxyArray(values));
        return pv_util::CreateInspectableArrayValue(
            (UINT32)values.size(),
            values.size() ? values.front().GetAddressOfPtr() : nullptr,
            value.ReleaseAndGetAddressOf());
    }
    return m_pProxyObjectFactory->ToProxyValueInternal(
            As<IProxyObject>(),
            value);
}

// Convert the Items Array to Proxy Models
HRESULT BaseObjectProxy::ToProxyArray(_Inspectable_ptr_vector_type& items)
{
    for(_Inspectable_ptr_vector_type::iterator iter = items.begin();
        iter != items.end();
        ++iter)
    {
        InspectablePtr value(*iter);
        // Convert
        IFR(ToProxyValue(value));
        // Put back into the array
        (*iter) = value;
    }
    return S_OK;
}

// Convert the Items Array to Proxy Models
HRESULT BaseObjectProxy::FromProxyArray(_Inspectable_ptr_vector_type& items)
{
    for(_Inspectable_ptr_vector_type::iterator iter = items.begin();
        iter != items.end();
        ++iter)
    {
        foundation::InspectablePtr value(*iter);
        // Convert
        IFR(FromProxyValue(value));
        // Put back into the array
        (*iter) = value;
    }
    return S_OK;
}

// Convert an already converted Proxy Variant back to a 'Raw' Variant to be passed back
HRESULT BaseObjectProxy::FromProxyValue(foundation::InspectablePtr& value)
{
    return FromProxyValue(nullptr,value);
}

HRESULT BaseObjectProxy::FromProxyValue(IPropertyTypeInfo *pPropertyTypeInfo,foundation::InspectablePtr& value)
{
    foundation_assert(m_pProxyObjectFactory);
    IFR(VerifyDisposed());

    // if Inspectable array
    if (pv_util::IsValueArray(value))
    {
        _Inspectable_ptr_vector_type values;
        IFR_ASSERT(ArrayToInspectableVector(value, values));

        IFR(FromProxyArray(values));
        return pv_util::CreateInspectableArrayValue(
            (UINT32)values.size(),
            values.size() ? values.front().GetAddressOfPtr() : nullptr,
            value.ReleaseAndGetAddressOf());
    }

    return m_pProxyObjectFactory->FromProxyValueInternal(
        As<IProxyObject>(),
        pPropertyTypeInfo,
        value);

}

HRESULT BaseObjectProxy::ToProxyObjectInternal(_In_ IObject* pObject, IObject** ppProxyObject)
{
    foundation_assert(m_pProxyObjectFactory);
    IFR(VerifyDisposed());
    return m_pProxyObjectFactory->ToProxyObjectInternal(pObject, ppProxyObject);
}

bool BaseObjectProxy::IsLevelEnabled(
        LoggingLevel level,
        UINT32 messageEnumValue)
{
    if(m_pProxyObjectFactory == nullptr || !m_pProxyObjectFactory->IsLoggingAvailable())
    {
        return false;
    }

    IID iid = this->GetModelIId();
    return logger_util::IsLevelEnabled(
        this->GetLogCategory(),
        level,
        messageEnumValue,
        &iid);
}

ILogger *BaseObjectProxy::GetCoreLogger()
{
    foundation_assert(m_pProxyObjectFactory);
    return m_pProxyObjectFactory->GetProxyLogger();
}

HRESULT BaseObjectProxy::ToCachedProxyValue(
    _In_ PropertyType propertyType,
    _In_ foundation::IInspectable *pInspectable,
    foundation::InspectablePtr& spValue,
    bool allowWeakReference
    )
{
    if (allowWeakReference &&
        (pInspectable != nullptr && propertyType == PropertyType_Inspectable))
    {
        if (FAILED(foundation::AsWeakRef(
            *(reinterpret_cast<foundation::IWeakReference **>(spValue.ReleaseAndGetAddressOf())),
            pInspectable)))
        {
            spValue = pInspectable;
        }
    }
    else if (propertyType == PropertyType_String)
    {
        HStringPtr hstr;
        IFR_ASSERT(pv_util::GetStringValue(pInspectable, hstr.GetAddressOf()));
        HStringPtr hstr_copy(hstr.GetRawBuffer());
        IFR_ASSERT(pv_util::CreateStringValue(hstr_copy, spValue.GetAddressOf()));
    }
    else
    {
        spValue = pInspectable;
    }
    return S_OK;
}

HRESULT BaseObjectProxy::FromCachedProxyValue(
    _In_ PropertyType propertyType,
    foundation::InspectablePtr& spValue
    )
{
    if (spValue != nullptr && propertyType == PropertyType_Inspectable)
    {
        foundation::InspectablePtr spStrongRef;
        if (SUCCEEDED(foundation::ResolveWeakRefIf(
            spValue,
            foundation_IID_IInspectable,
            *spStrongRef.GetAddressOf())))
        {
            spValue = spStrongRef;
            if (spValue == nullptr)
            {
                // this will indicate that the weak reference was gone
                return S_FALSE;
            }
        }
    }
    return S_OK;
}
HRESULT BaseObjectProxy::LogInitialize()
{
    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        BaseProxy_CategoryEnum::TypeId,
        BaseProxy_Category::Initialize);

    if (this->IsLevelEnabled(
        LoggingLevel::Info,
        logUniqueId))
    {
        logger_util::Log(
            this->GetCoreLogger(),
            this->AsInspectable(),
            LoggingLevel::Info,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->GetLogCategory(),
            BaseProxy_Base_LogRecordEnum::TypeId,
            // Proxy UniqueId
            BaseProxy_Base_LogRecord::ProxyUniqueId,
            pv_util::CreateValue((UINT32)this->m_ProxyUniqueId).Get(),
            // UniqueId
            BaseProxy_Base_LogRecord::UniqueId,
            pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
            // TypeId
            BaseProxy_Base_LogRecord::TypeId,
            pv_util::CreateValue(this->GetModelIId()).Get(),
            -1);
    }
    return S_OK;
}
HRESULT BaseObjectProxy::LogFinalRelease()
{
    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        BaseProxy_CategoryEnum::TypeId,
        BaseProxy_Category::FinalRelease);

    if (this->IsLevelEnabled(
        LoggingLevel::Debug,
        logUniqueId))
    {
        logger_util::Log(
            this->GetCoreLogger(),
            nullptr,
            LoggingLevel::Debug,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->GetLogCategory(),
            BaseProxy_Base_LogRecordEnum::TypeId,
            // Proxy UniqueId
            BaseProxy_Base_LogRecord::ProxyUniqueId,
            pv_util::CreateValue((UINT32)this->m_ProxyUniqueId).Get(),
            // UniqueId
            BaseProxy_Base_LogRecord::UniqueId,
            pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
            // TypeId
            BaseProxy_Base_LogRecord::TypeId,
            pv_util::CreateValue(this->GetModelIId()).Get(),
            -1);
    }
    return S_OK;
}

HRESULT BaseObjectProxy::LogSourceEvent(
    LoggingLevel level,
    UINT32 messageEnumValue,
    IUnknown *pEventArgs)
{
    if (this->IsLevelEnabled(
        level,
        logger_util::ToLogMessageUniqueId(
            BaseProxy_CategoryEnum::TypeId,
            messageEnumValue)))
    {
        logger_util::Log(
            this->GetCoreLogger(),
            this->AsInspectable(),
            LoggingLevel::Debug,
            // Message Enum Value
            logger_util::ToLogMessageUniqueId(
            BaseProxy_CategoryEnum::TypeId,
            messageEnumValue),
            // Log Category
            this->GetLogCategory(),
            BaseProxy_SourceEvent_LogRecordEnum::TypeId,
            // Proxy UniqueId
            BaseProxy_Base_LogRecord::ProxyUniqueId,
            pv_util::CreateValue((UINT32)this->m_ProxyUniqueId).Get(),
            // UniqueId
            BaseProxy_Base_LogRecord::UniqueId,
            pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
            // TypeId
            BaseProxy_Base_LogRecord::TypeId,
            pv_util::CreateValue(this->GetModelIId()).Get(),
            // EventArgs
            BaseProxy_SourceEvent_LogRecord::EventArgs,
            pEventArgs,
            -1);
    }
    return S_OK;
}
HRESULT BaseObjectProxy::LogResyncSource(IObject *pSource)
{
    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        BaseProxy_CategoryEnum::TypeId,
        BaseProxy_Category::ResyncSource);

    if (this->IsLevelEnabled(
        LoggingLevel::Info,
        logUniqueId))
    {
        UINT32 sourceUniqueId = 0;
        GUID sourceIIdType = foundation_GUID_NULL;

        if (pSource != nullptr)
        {
            pSource->GetUniqueId(&sourceUniqueId);
            GetObjectType(pSource, sourceIIdType);
        }

        logger_util::Log(
            this->GetCoreLogger(),
            this->AsInspectable(),
            LoggingLevel::Info,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->GetLogCategory(),
            BaseProxy_ResyncSource_LogRecordEnum::TypeId,
            // Proxy UniqueId
            BaseProxy_Base_LogRecord::ProxyUniqueId,
            pv_util::CreateValue((UINT32)this->m_ProxyUniqueId).Get(),
            // UniqueId
            BaseProxy_Base_LogRecord::UniqueId,
            pv_util::CreateValue((UINT32)sourceUniqueId).Get(),
            // TypeId
            BaseProxy_Base_LogRecord::TypeId,
            pv_util::CreateValue(sourceIIdType).Get(),
            -1);
    }
    return S_OK;
}

HRESULT BaseObjectProxy::LogAttachToSource()
{
    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        BaseProxy_CategoryEnum::TypeId,
        BaseProxy_Category::AttachToSource);

    if (this->IsLevelEnabled(
        LoggingLevel::Debug,
        logUniqueId))
    {
        logger_util::Log(
            this->GetCoreLogger(),
            this->AsInspectable(),
            LoggingLevel::Debug,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->GetLogCategory(),
            BaseProxy_Base_LogRecordEnum::TypeId,
            // Proxy UniqueId
            BaseProxy_Base_LogRecord::ProxyUniqueId,
            pv_util::CreateValue((UINT32)this->m_ProxyUniqueId).Get(),
            // UniqueId
            BaseProxy_Base_LogRecord::UniqueId,
            pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
            // TypeId
            BaseProxy_Base_LogRecord::TypeId,
            pv_util::CreateValue(this->GetModelIId()).Get(),
            -1);
    }
    return S_OK;
}
HRESULT BaseObjectProxy::LogDetachFromSource()
{
    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        BaseProxy_CategoryEnum::TypeId,
        BaseProxy_Category::DetachFromSource);

    if (this->IsLevelEnabled(
        LoggingLevel::Debug,
        logUniqueId))
    {
        logger_util::Log(
            this->GetCoreLogger(),
            nullptr,
            LoggingLevel::Debug,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->GetLogCategory(),
            BaseProxy_Base_LogRecordEnum::TypeId,
            // Proxy UniqueId
            BaseProxy_Base_LogRecord::ProxyUniqueId,
            pv_util::CreateValue((UINT32)this->m_ProxyUniqueId).Get(),
            // UniqueId
            BaseProxy_Base_LogRecord::UniqueId,
            pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
            // TypeId
            BaseProxy_Base_LogRecord::TypeId,
            pv_util::CreateValue(this->GetModelIId()).Get(),
            -1);
    }
    return S_OK;
}

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
void BaseObjectProxy::DumpObject()
{
    IObjectTypeInfo *pModelTypeInfo = GetTypeInfo();
    if (pModelTypeInfo)
    {
        HStringPtr typeName;
        pModelTypeInfo->GetName(typeName.GetAddressOf());
        _traceMessage(U("BaseObjectProxy => Name:%s ProxyUniqueId:%d UniqueId:%d RefCount=%d\n"),
            typeName.GetRawBuffer(),
            m_ProxyUniqueId,
            m_UniqueId,
            this->GetRefCount());
    }
}
#endif

