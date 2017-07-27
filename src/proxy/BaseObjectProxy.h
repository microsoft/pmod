/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseObjectProxy.h
****/
#pragma once

#ifdef _MSC_VER
#pragma warning( disable : 4503 )
#endif

//#define TRACK_REF_COUNT

#include "FoundationTypes.h"
#include "FoundationUtilHelper.h"
#include "CriticalSection.h"
#include "FoundationBaseTracker.h"

#include <foundation/ctl/composable_weak_reference_source.h>
#include <foundation/delegate_library.h>
#include <foundation/library/base_adapter_host.h>
#include <foundation/library/fire_event_support.h>
#include <foundation/library/interfaces/event_source_class.h>
#include <foundation/library/interfaces/debug_util.h>
#include <foundation/library/debug_library.h>

class CProxyObjectFactory;

#include <vector>
#include <map>

struct IBaseObjectProxyInternal;
typedef std::vector<std::pair<IBaseObjectProxyInternal *,foundation::ComPtr<foundation::IUnknown>>> _SyncSourceInfo_Vector_Type;

BEGIN_DECLARE_INTERFACE(IBaseObjectProxyInternal,foundation::IUnknown,_VOID_MACRO)
    STDMETHOD(IsAttachToEventSourceNeeded)(_In_ bool attach,_Out_ bool& result) = 0;

    // about to disconnect from Source
    STDMETHOD(DisconnectFromSource)(foundation::IInspectable **ppSourceInfo) = 0;
    // connection to source has just finished
    STDMETHOD(ConnectToSource)(foundation::IInspectable *pSourceInfo, _SyncSourceInfo_Vector_Type& syncSourceInfo) = 0;

    // the Source is being connected/disconnected
    STDMETHOD(SetSourceConnectState)( bool newValue) = 0;
    // return if this Proxy is in Source connect State
    STDMETHOD_(bool,IsSourceConnectState)() = 0;

    // return if this Proxy is a ViewModel Proxy
    STDMETHOD_(bool,IsViewModelProxy)() = 0;

    // re sync a Source object
    STDMETHOD(ResyncSource)(foundation::IInspectable *pSource, _SyncSourceInfo_Vector_Type& syncSourceInfo) = 0;
    STDMETHOD(FireResyncEvent)(foundation::IUnknown *pEventArg) = 0;

    STDMETHOD(SetPurgeStateEnabled)(_In_ bool newValue) = 0;
    STDMETHOD(IsProxyComposed)(_Out_ bool& isComposed) = 0;
    STDMETHOD(Dispose)() = 0;
END_DECLARE_INTERFACE()

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
//------------------------------------------------------------------------------
// Class:   CBaseProxyTrackerTracker
//
// Purpose: A tracking class for all our proxies
//
//------------------------------------------------------------------------------
class CBaseProxyTrackerTracker :
    protected FoundationTracker,
    public _FoundationTrackerInstance<CBaseProxyTrackerTracker>
{
public:
    static FoundationTracker& _GetInstance();
private:
    CBaseProxyTrackerTracker();
        ~CBaseProxyTrackerTracker();
protected:
    bool is_trace_count_enabled() override;
};
#endif

#if defined(DEBUG) && defined(TRACK_REF_COUNT)
const UINT32 _OptionTrackRefCountAllProxies = 0x0800;
const UINT32 _OptionTrackRefCountPropertyModelProxies = 0x1000;
const UINT32 _OptionTrackRefCountCollectionModelProxies = 0x2000;
const UINT32 _OptionTrackRefCountCommandModelProxies = 0x4000;
const UINT32 _OptionTrackRefCountAsyncOperationProxies = 0x8000;
#endif

//------------------------------------------------------------------------------
// Class:   BaseObjectProxy
//
// Purpose: Define a base class for all the Model based proxy we will support
//
//------------------------------------------------------------------------------
class BaseObjectProxy :
    public foundation::library::_BaseAdapterHost<IBaseObjectProxyInternal, foundation::ctl::ComposableWeakRefSourceInspectableBase, BaseObjectProxy>,
    public IBaseObjectProxyInternal,
//    public pmod::proxy::library::IBaseObjectProxyClass,
    public foundation::library::_FireEventHandlerContainerWithCacheImpl<BaseObjectProxy>,
    protected foundation::IObject,
    protected foundation::library::IEventSourceClass,
    protected foundation::library::ICriticalSection
#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
    ,
    public IFoundationTrackerInstance
#endif
{
public:
    // Set if this Proxy is a ViewModel
    void SetIsViewModelProxy(bool newValue);

    foundation::IObject *GetObjectInstance() { return static_cast<foundation::IObject *>(this); }

    void _OnAddHandler(_In_ foundation::IUnknown* pHandler)
    {
        UNREFERENCED_PARAMETER(pHandler);
        this->EvaluateIsObservedChanged(true);
    }

    void _OnRemoveHandler(_In_ foundation::IUnknown* pHandler)
    {
        UNREFERENCED_PARAMETER(pHandler);
        this->EvaluateIsObservedChanged();
    }

protected:
    BaseObjectProxy();

    friend class CProxyObjectFactory;
    friend class foundation::library::_FireEventHandlerContainerImpl<BaseObjectProxy>;
    friend class foundation::library::_FireEventSupportUtil;

    HRESULT _Initialize(CProxyObjectFactory *pProxyFactory, bool IsFreeThreadedEnabled);

    IInspectable *AsInspectable()
    {
        return static_cast<BaseObjectProxy::_ComInspectableBaseType *>(this);
    }

    HRESULT FireModelAdaptersEvent(
        _In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
        _In_ foundation::IUnknown *pEventArgs);

    // Interface IFoundationTracker
#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
    void DumpObject() override;
#endif

    // Override ComBase
#if defined(_WINDOWS_RUNTIME)
    HRESULT GetRuntimeClassNameImpl(_Out_ HSTRING *className) override;
#endif

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;
    _pal_AtomicIntegerType AddRefImpl() override;
    _pal_AtomicIntegerType ReleaseImpl() override;
    void FinalRelease() override;

     // Override from _FireEventHandlerContainerImpl
    bool _IsEventSupportEnabled();
    bool _IsContinueOnFireEventError();
    foundation::library::IDispatcher *_GetEventDispatcher();
    inline bool _IsFreeThreaded() { return IsFreeThreadApartment(); }

    // IObject Interface
    STDMETHOD(GetUniqueId)(UINT32 *uniqueId) override;
    STDMETHOD(GetProcessId)(UINT32 *processId) override;
    STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue) override;

    // =============== Interface IBaseObjectProxyInternal =======================
    STDMETHOD(DisconnectFromSource)(foundation::IInspectable **ppSourceInfo) override;
    STDMETHOD(ConnectToSource)(foundation::IInspectable *pSourceInfo,_SyncSourceInfo_Vector_Type& syncSourceInfo) override;
    STDMETHOD( SetSourceConnectState)( bool newValue) override;
    STDMETHOD_(bool,IsSourceConnectState)() override;
    STDMETHOD_(bool,IsViewModelProxy)() override;
    STDMETHOD( SetPurgeStateEnabled)(bool newValue) override;
    STDMETHOD( IsProxyComposed)(bool& isComposed) override;
    STDMETHOD( Dispose)() override;

    // =============== Interface IModelAdapterOverrides =======================
    STDMETHOD (OnAdapterAddHandler)(_In_ foundation::IUnknown* pSender,_In_ foundation::IUnknown* pHandler) override;
    STDMETHOD (OnAdapterRemoveHandler)(_In_ foundation::IUnknown* pSender,_In_ foundation::IUnknown* pHandler) override;


    //  overrides from IInterfaceAdapterHost
    STDMETHOD(IsObserved)(bool *bValue) override
    {
        *bValue = this->m_IsProxyObserved;
        return S_OK;
    }

    template <class Q>
    Q *As()
    {
        Q *pQ = nullptr;
        HRESULT hr = QueryInterfaceImpl(Q::GetIID(),(foundation::IUnknown **)&pQ);
        foundation_assert(SUCCEEDED(hr));
        return pQ;
    }

    static BaseObjectProxy *AsBaseProxyClass(IBaseObjectProxyInternal *pBaseObjectProxyInternal)
    {
        return static_cast<BaseObjectProxy *>(pBaseObjectProxyInternal);
    }

    static foundation::IObject *GetAsModel(IBaseObjectProxyInternal *pBaseObjectProxyInternal)
    {
        return AsBaseProxyClass(pBaseObjectProxyInternal)->As<foundation::IObject>();
    }

    static bool IsProxyValueConversionNeeded(foundation::PropertyType propertyType)
    {
        return (propertyType & 0xff) == foundation::PropertyType_Inspectable;
    }

    typedef foundation::library::_BaseAdapterHost<IBaseObjectProxyInternal, foundation::ctl::ComposableWeakRefSourceInspectableBase, BaseObjectProxy> _BaseObjectProxyBaseType;

    friend class foundation::library::_BaseAdapterHost<IBaseObjectProxyInternal, foundation::ctl::ComposableWeakRefSourceInspectableBase, BaseObjectProxy>;
protected:
     // overrides from BaseAdapterHost
    IID _GetHostInterfaceType(REFIID iid)
    {
        return this->GetModelIId();
    }

    HRESULT _QueryCreateAdapterInterfaceImpl(
        IInspectable* pOuter,
        IInspectable* pInner,
        REFIID iid,
        REFIID iidHostType,
        IInspectable **ppModeAdapterNewInstance);

    bool IsLevelEnabled(
        foundation::LoggingLevel level,
        UINT32 messageEnumValue);

    foundation::ILogger *GetCoreLogger();
    // logging methods
    HRESULT LogSourceEvent(
        foundation::LoggingLevel level,
        UINT32 messageEnumValue,
        foundation::IUnknown *pEventArgs);
    HRESULT LogInitialize();
    HRESULT LogFinalRelease();
    HRESULT LogAttachToSource();
    HRESULT LogDetachFromSource();
    HRESULT LogResyncSource(foundation::IObject *pSource);

    // Pure Methods
    virtual foundation::IObject *GetObjectSource() = 0;
    virtual REFIID GetModelIId() const = 0;
    virtual foundation::ILogCategory *GetLogCategory() = 0;
    virtual size_t GetProxyCountEventHandlers() = 0;
    virtual void OnIsObservedChanged(bool newValue) = 0;
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    virtual UINT32 GetTrackRefCountOption() = 0;
#endif

    // virtual methods
    virtual HRESULT SetSourceConnectStateInternal( bool newValue);

    // Override Event Handler Support
    HRESULT _OnCustomFireEvent(
        _In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
        _In_ foundation::IUnknown *pEventArgs);

#if defined(DEBUG) && (defined(TRACK_REF_COUNT) || defined(TRACK_FOUNDATION_ENABLED))
    foundation::IObjectTypeInfo *GetTypeInfo();
#endif

public:
    // Overrides from ICriticalSection
    HRESULT Lock() throw() override
    {
        if (m_IsFreeThreaded)
        {
            return m_critsec.Lock();
        }
        return S_OK;
    }

    HRESULT Unlock() throw() override
    {
        if (m_IsFreeThreaded)
        {
            return m_critsec.Unlock();
        }
        return S_OK;
    }

protected:
    bool IsFreeThreadApartment()
    {
        return m_IsFreeThreaded;
    }
    bool IsProxyObserved() { return this->m_IsProxyObserved; }
    bool IsInvokeDispatcherRequired();
    HRESULT InvokeUsingDispatcher(foundation::library::IDelegateDispatchable *pDispatchable);

    bool IsAttachEnabledInternal();
    bool IsProxyModelCacheEnabled();
    bool IsPurgeStateEnabledInternal();
    bool IsInSourceConnectState()
    {
        return m_InSourceConnectState;
    }
    void EvaluateIsObservedChanged(bool isObserved);
    void EvaluateIsObservedChanged();
    bool EvaluatePurgeState(bool isAttached);

    // mantain the value of the Purge State field
    void SetPurgeState(bool newValue);

    // mantain the value of the IsPurgeable field
    void SetIsPurgeable(bool newValue);

    // Convert a 'Raw' Variant to Variant that could contain Proxy Model
    HRESULT ToProxyValue(_Inout_ foundation::InspectablePtr& value);

    // Convert the Items Array to Proxy Models
    HRESULT ToProxyArray(_Inspectable_ptr_vector_type& items);

    // Convert the Items Array to Proxy Models
    HRESULT FromProxyArray(_Inspectable_ptr_vector_type& items);

    // Convert an already converted Proxy Variant back to a 'Raw' Variant to be passed back
    HRESULT FromProxyValue(foundation::InspectablePtr& value);
    HRESULT FromProxyValue(foundation::IPropertyTypeInfo *pPropertyTypeInfo,foundation::InspectablePtr& value);

    HRESULT ToProxyObjectInternal(_In_ foundation::IObject* pObject, foundation::IObject** ppProxyObject);
    
    // Verify if this instance is already disposed
    HRESULT VerifyDisposed();

    // Verify if the 'Source' is available
    HRESULT VerifyModelSource();

    // add myself to the Cached Instances of my Proxy Factory
    HRESULT AddToCachedInstances();

    // remove this proxy from the Proxy Factory
    HRESULT ClearFromCachedInstances();

    bool IsFreeThreaded();
    bool IsSourceFreeThreaded();
    bool IsModelCacheEnabled();

    template <
        typename TSink,
        typename TEventArgs,
        HRESULT (TSink::*pMethod)(foundation::IUnknown *, TEventArgs *)
    >
    HRESULT InvokeLater(
    TSink *pSink,
    foundation::IUnknown *pSender,
    TEventArgs *pEventArgs)
    {
        typedef foundation::InvokeEventDispatchable<TSink,TEventArgs,pMethod> _TDelegateType;
        foundation::_InvokeEventParams<TEventArgs> invokeParams;
        invokeParams.m_spSender = pSender;
        invokeParams.m_spEventArgs = pEventArgs;

        _TDelegateType *pDelegateType = nullptr;
        _TDelegateType::CreateDelegateInstance(
            pSink,
            this->AsInspectable(),
            invokeParams,
            &pDelegateType);

        foundation_assert(pDelegateType);
        HRESULT hr = this->InvokeUsingDispatcher(pDelegateType);
        foundation::ctl::ReleaseInterface(pDelegateType);
        return hr;
    }

    static HRESULT ToCachedProxyValue(
        _In_ foundation::PropertyType propertyType,
        _In_ IInspectable *pInspectable,
        foundation::InspectablePtr& spValue,
        bool allowWeakReference
        );
    static HRESULT FromCachedProxyValue(
        _In_ foundation::PropertyType propertyType,
        foundation::InspectablePtr& spValue
        );
private:

    bool IsPurgeIfAttached();
    void HandleRefCountChanged(bool addRef,ULONG refCnt);

    HRESULT _QueryInheritedAdapterInterface(REFIID iid, foundation::IUnknown **ppInterface);
    void ReleaseProxyFactory();


    bool HasProxyEventHandlers();
    friend class CFireEventHandlerContainerBase;
    friend class CProxyObjectImpl;
protected:
    CProxyObjectFactory         *m_pProxyObjectFactory;
    foundation::IInspectable    *m_pInnerProxyObject;

    UINT32      m_ProxyUniqueId;
    UINT32 m_UniqueId;       // cached Unique Id from Source
    UINT32 m_ProcessId;      // cached process id from Source
    bool m_Disposed:1;              // if the Proxy has been Disposed
    IID  m_IIDTypeCache;            // cached type iid for this proxy
private:
    bool m_isPurgeable:1;           // if this Proxy is able to be purged
    bool m_IsFreeThreaded  : 1;// if multi thread is enabled for the proxy
    bool m_PurgeState:1;            // the 'real' purge state
    bool m_PurgeStateEnabled:1;     // if the Proxy should inform about his Purge State to the Proxy Factory
    bool m_SourceConnectState:1;	// true when SourceConnectState is being set
    bool m_InSourceConnectState:1;  // true when SetSourceConnectState is being called
    bool m_IsViewModelProxy:1;      // if this Proxy is wrapping a View Model
    bool m_IsProxyObserved : 1;     // if proxy is being observed
    foundation_lib::CAutoDeleteCriticalSection          m_critsec; // clitical section to protect the proxy
#if defined(DEBUG) && (defined(TRACK_REF_COUNT) || defined(TRACK_FOUNDATION_ENABLED))
    foundation::ComPtr<foundation::IObjectTypeInfo> m_spTypeInfoCache;
#endif
};

