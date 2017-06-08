/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ProxyObjectFactory.h
****/

#pragma once

#include <Proxy/ProxyInterfaces.g.h>
#include <Proxy/ProxyImplInternal.g.h>

#include <foundation/library/object_cache.h>

#include "InterfaceAdapterFactoryBase.h"
#include <foundation/weak_reference.h>
#include <foundation/library/base_adapter_host.h>
#include <pmod/library/interfaces/object_statics.h>
#include "CriticalSection.h"

#include "palTimer.h"

#include <map>
#include <vector>

struct IBaseObjectProxyInternal;

//------------------------------------------------------------------------------
// Class:   CProxyObjectFactory
//
// Purpose: A factory class to create Property Model instances
//
//------------------------------------------------------------------------------

typedef pmod::proxy::_ProxyObjectFactoryImplBase<> _ProxyObjectFactoryBaseType;

class CProxyObjectFactory:
    public _ProxyObjectFactoryBaseType,
    public foundation::library::IDisposable,
    public ITimerCallback,
    protected foundation::library::ICriticalSection,
    protected foundation::library::object_proxy_cache<foundation::IObject>
{
public:
    HRESULT _Initialize(
        pmod::proxy::ProxyObjectFactoryOptions options,
        foundation::IInspectable *pWorkingDispatcher,
        foundation::IInspectable *pEventDispatcher);

    void _BeginInitialize() {}
    void _EndInitialize() {}

    inline bool _IsFreeThreaded() { return m_IsFreeThreaded;}

   // Override from ComBase
   void FinalRelease() override;
   HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override;

   // Interface IProxyObjectFactory
   HRESULT ToProxyObject(foundation::IObject * pSource, _COM_Outptr_result_maybenull_ foundation::IObject * *pResult) override;
   HRESULT GetProxies(_Out_ UINT32 *size, _Outptr_result_maybenull_ pmod::proxy::IProxyObject ** *pResult) override;
   HRESULT CreateProxyObject(pmod::proxy::ProxyType proxyType, foundation::IObject * pSource, pmod::proxy::IProxyObjectDelegate * pProxyObjectDelegate, foundation::IInspectable * pOuter, _COM_Outptr_result_maybenull_ foundation::IInspectable * *pResult) override;
   HRESULT LookupObjectProxy(foundation::IObject * pSourceObject, _COM_Outptr_result_maybenull_ pmod::proxy::IProxyObject * *pResult) override;
   HRESULT CreateViewModel(GUID iidViewModel, foundation::IInspectable * contextParameter, _COM_Outptr_result_maybenull_ foundation::IObject * *pResult) override;
   HRESULT PurgeProxyObjects(UINT32 maxPurgeSize, UINT64 time) override;
   HRESULT DisconnectProxySources(_COM_Outptr_result_maybenull_ foundation::IInspectable * *pResult) override;
   HRESULT ConnectProxySources(foundation::IInspectable * pDisconnectInfo) override;

   HRESULT PurgeProxyObjectsInternalHelper(UINT32 maxPurgeSize, UINT64 time);

   //// Override from CInterfaceAdapterFactoryBase
   //HRESULT QueryCreateAdapterInterfaceInternal(
   //     _In_ IInspectable *pOuter,
   //     _In_ IInspectable *pInner,
   //     _In_ REFIID riid_outer,
   //     _In_ REFIID riid,
   //     _Outptr_ IInspectable **ppModelAdapterInnerUnk) override;

    // Interface IDisposable
    STDMETHOD(Dispose)();

protected:
    CProxyObjectFactory();
public:

    // static Methods

    // Return true if the Value support QI for IObject
    static bool IsObject(_In_ IInspectable *pValue, foundation::ComPtr<foundation::IObject>& spModel);

    // Return true if the Value support QI for IBaseObjectProxy
    static bool IsProxyObject(_In_ IInspectable *pValue,foundation::ComPtr<pmod::proxy::IProxyObject>& spModelProxy);
    
    // Overrides from ICriticalSection
    HRESULT Lock() throw() override;
    HRESULT Unlock() throw() override;

private:
    friend class BaseObjectProxy;

    inline bool IsThreadSafeLocking()
    {
        return m_IsFreeThreaded || m_IsSourceFreeThreaded;
    }

    inline bool IsLoggingAvailable()
    {
        return this->m_ProxyFactoryCategory != nullptr;
    }

    // Internal methods
    HRESULT ToProxyObjectInternal(_In_ foundation::IObject* pObject, foundation::IObject** ppProxyObject);

    // Convert a 'Raw' Value that could contain Proxy Model
    HRESULT ToProxyValueInternal(_In_ pmod::proxy::IProxyObject *pBaseModelProxy,_Inout_ foundation::InspectablePtr& value);

    // Convert a Variant back into a 'Raw' Model if needed
    HRESULT FromProxyValueInternal(_In_ pmod::proxy::IProxyObject *pBaseModelProxy,_In_opt_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,_Inout_ foundation::InspectablePtr& value);

    // When a Proxy Model inform us that is ready or not to be purged
    void OnProxyModelPurgeStateChanged(
        _In_ IBaseObjectProxyInternal *pBaseModelProxy,
        _In_ bool isPurgeReady);

    // A Proxy Model attempt to Purge after change its internal State to 'Purgeable'
    void OnProxyObjectPurgeReady(_In_ pmod::proxy::IProxyObject *pBaseModelProxy);

    bool IsFreeThreaded() { return m_IsFreeThreaded; }
    bool IsSourceFreeThreaded(){ return m_IsSourceFreeThreaded; }
    bool IsModelCacheEnabled() { return m_IsModelCacheEnabled; }
    bool IsValuesCacheEnabled() { return m_IsCacheValuesEnabled; }
    bool IsModelTypeInfoProxyEnabled() { return m_IsModelTypeInfoProxyEnabled; }

    bool UseCacheStrongRefForProperties() { return m_UseCacheStrongRefForProperties; }
    bool UseCacheStrongRefForItems() { return m_UseCacheStrongRefForItems; }

    bool IsPurgeIfAttached() { return m_PurgeIfAttached; }
    bool IsSourceConnectState() { return m_SourceConnectState; }
    bool IsDisposed() { return m_Disposed; }

    bool IsRemoveProxyFromCacheEnabled();

    foundation::library::IDispatcher *GetEventDispatcherInternal() { return m_spEventDispatcher; }

    bool IsInvokeDispatcherRequired();
    HRESULT InvokeUsingDispatcher(foundation::library::IDelegateDispatchable *pDispatchable);

    // Core logger
    foundation::ILogger *GetProxyLogger() { return m_ProxyLogger; }
    // Proxy Logger categories
    foundation::ILogCategory *GetProxyFactoryCategory() { return m_ProxyFactoryCategory; }
    // Proxy Instance Logger categories
    foundation::ILogCategory *GetCommandProxyCategory() { return m_CommandProxyCategory; }
    foundation::ILogCategory *GetCollectionProxyCategory() { return m_CollectionProxyCategory; }
    foundation::ILogCategory *GetAsyncOperationdProxyCategory() { return m_AsyncOperationProxyCategory; }
    foundation::ILogCategory *GetPropertyProxyCategory() { return m_PropertyProxyCategory; }

protected:
    // ITimerCallback Interface
    STDMETHOD (OnTimeout)(UINT64 timerId);

    // Template Methods
    template <class Q>
    HRESULT CreateOrLookupModelProxyInstanceInternal(
        _In_ pmod::proxy::ProxyType type,
        _In_ foundation::IObject *pSourceObject,
        _COM_Outptr_ Q              **ppNewInstance);

    template <class Q>
    HRESULT CreateObjectProxyInstanceInternal(
        _In_ pmod::proxy::ProxyType type,
        _In_ foundation::IObject *pSourceObject,
        _COM_Outptr_ Q              **ppNewInstance);

    template <class T,class TSourceType>
    HRESULT CreateObjectProxyInstanceInternal(
        _In_ foundation::IObject *pSourceObject,
        _In_ pmod::proxy::IProxyObjectDelegate *pProxyObjectDelegateBase,
        _In_ IInspectable    *pOuter,
        _COM_Outptr_ IInspectable    **ppUnknown);

    HRESULT CreateObjectProxyInstanceInternalAndCache(
        _In_ pmod::proxy::ProxyType type,
        _In_ foundation::IObject *pSourceObject,
        _COM_Outptr_ foundation::IObject  **ppProxyObject);

private:
   typedef std::pair<IBaseObjectProxyInternal *,UINT64> PurgeInfoType;
   typedef std::vector<foundation::ComPtr<IBaseObjectProxyInternal> > _BaseObjectProxyInternalVector_Type;
   typedef std::map<UINT32,IBaseObjectProxyInternal *> DiposedProxyMapType;

   // Private from/to proxy value conversion
   HRESULT FromProxyValueInternal(_In_opt_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,_Inout_ foundation::InspectablePtr& value );
   HRESULT ToProxyValueInternal(_Inout_ foundation::InspectablePtr& value);

   HRESULT CreateObjectProxyInstanceInternal(
        pmod::proxy::ProxyType type,
        foundation::IObject *pSourceModel,
        pmod::proxy::IProxyObjectDelegate *pProxyModelDelegateBase,
        IInspectable *pOuter,
        IInspectable **ppUnknown);


   HRESULT CreatePropertyModelProxyInstanceInternal(
            _In_ pmod::IObservableObject *pSourceModel,
            _Out_ pmod::IObservableObject **ppProxyModelInstance);

    HRESULT AddToCachedInstances(foundation::IObject *model);
    HRESULT RemoveProxyFromCache(UINT32 processId,UINT32 uniqueId);
    HRESULT RemoveFromCachedInstances(foundation::IObject *model);
    HRESULT RemoveFromCachedInstances(IUnknown *pUnk);
    bool VerifyCachedInstance(_In_ foundation::IObject *pModel);

    void DeletePurgeableModel(IBaseObjectProxyInternal *pBaseModelProxy);
    
    void PurgeWeakReferencesProxies();
    HRESULT PurgeProxyModel(IBaseObjectProxyInternal *pBaseModelProxy);

    bool LookupModelProxyInternal(_In_ foundation::IObject *pModel, _Out_ foundation::ComPtr<foundation::IObject>& proxyModel);
    bool LookupModelProxyHelper(_In_ foundation::IObject *pModel, _Out_ foundation::ComPtr<foundation::IObject>& proxyModel);
    bool LookupModelProxyHelper(_In_ UINT32 processId, _In_ UINT32 uniqueId, _Out_ foundation::ComPtr<foundation::IObject>& proxyModel);
    HRESULT RaiseViewModelCreated(_In_ foundation::IObject *pViewModel);

    bool IsProxyObjectContained(_In_ const foundation::InspectablePtr& value,foundation::ComPtr<pmod::proxy::IProxyObject>& spModelProxy);

    // Auto Purge private methods
    void SetPurgeTimer(UINT32 timeout);
    void KillPurgeTimer();
    bool IsPurgeAvailable() { return m_PurgeableModels.size() != 0; }
    bool IsPurgeTimerRunning() { return m_PurgeTimeoutId != 0; }

    std::vector<PurgeInfoType>::iterator FindPurgedProxy(IBaseObjectProxyInternal *pBaseModelProxy);

    friend class CObservableCollectionProxy;
    friend class CObservableObjectProxy;
    friend class CCommandProxy;
    friend class CAsyncOperationProxy;
private:
    template <class _Func>
    HRESULT ForEachBaseModelProxyInternal(const _Func& func);

    template <class _Func>
    static HRESULT ForEachBaseModelProxyInternal(const _Map_cache_type& cache, const _Func& func);

    HRESULT TryCreateSingleSourceViewModel(
        foundation::IObjectTypeInfo *pObjectTypeInfo,
        IID& iidModelType,
        foundation::IObject *pModel,
        pmod::ISourceModel **ppViewModel);

private:
    // The current dispatcher
    foundation::ComPtr<foundation::library::IDispatcher>                      m_spDispatcher;
    // The event dispatcher
    foundation::ComPtr<foundation::library::IDispatcher>                      m_spEventDispatcher;

    // The callback Delegate
    foundation::ComPtr<pmod::proxy::IProxyObjectFactoryDelegate> m_spDelegate;

    // An optional View Model Factory
    foundation::ComPtr<pmod::IViewModelFactory> m_ViewModelFactory;

    // The proxy logger
    foundation::ComPtr<foundation::ILogger> m_ProxyLogger;
    foundation::ComPtr<foundation::ILogCategory> m_ProxyFactoryCategory;
    foundation::ComPtr<foundation::ILogCategory> m_CommandProxyCategory;
    foundation::ComPtr<foundation::ILogCategory> m_CollectionProxyCategory;
    foundation::ComPtr<foundation::ILogCategory> m_AsyncOperationProxyCategory;
    foundation::ComPtr<foundation::ILogCategory> m_PropertyProxyCategory;

    // Control if Factory Model Free Threaded
    bool m_IsFreeThreaded : 1;

    // Control if Root Sorurce Free Threaded
    bool m_IsSourceFreeThreaded : 1;

    // Control if Factory Model Cache is Enabled
    bool m_IsModelCacheEnabled : 1;

    // Control if Factory Values Cache is Enabled
    bool m_IsCacheValuesEnabled : 1;

    // If we want to have proxy for Model Type Info
    bool m_IsModelTypeInfoProxyEnabled:1;

    // If Auto Purge is desired
    bool m_AutoPurge:1;

    // if ToProxyValue is enabled
    bool m_ToProxyValueDisabled : 1;

    // if FromProxyValue is enabled
    bool m_FromProxyValueDisabled : 1;

    // if UseCacheStrongRefForProperties is enabled
    bool m_UseCacheStrongRefForProperties : 1;

    // if UseCacheStrongRefForItems is enabled
    bool m_UseCacheStrongRefForItems : 1;

    // If Factory was disposed
    bool m_Disposed:1;

    // The Purge Timeout Id
    UINT64 m_PurgeTimeoutId;
    UINT32     m_PurgeTimeout;
    UINT32     m_MaxPurgeTimestamp;

    _pal_AtomicIntegerType          m_NextProxyUniqueId;
    // list of proxy models ready to be purged
    std::vector<PurgeInfoType>      m_PurgeableModels;
    bool                            m_InPurgingProcess:1;
    // next values control the purge mechanism
    UINT32                          m_MaxPurgeableSize;
    UINT32                          m_MinPurgeableSize;
    bool                            m_PurgeIfAttached:1;
    bool                            m_NoPurgeState:1;
    bool                            m_SourceConnectState:1;

    // critical section to protect the proxy factory
    foundation_lib::CAutoDeleteCriticalSection          m_critsec; 
};
