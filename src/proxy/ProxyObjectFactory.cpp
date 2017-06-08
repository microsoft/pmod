/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ProxyObjectFactory.cpp
****/
#include "pch.h"

#include "ProxyObjectFactory.h"
#include "BaseObjectProxy.h"

#include "ObservableObjectProxy.h"
//#include "ViewModelProxy.h"
#include "ObservableCollectionProxy.h"
#include "CommandProxy.h"
#include "AsyncOperationProxy.h"

//#include "ObjectFactoryProxy.h"
#include <foundation/library/key_value_pair_impl.h>
#include <foundation/library/class_options.h>
#include <pmod/library/interfaces/viewmodel_factory_class.h>
#include <pmod/library/observable_statics_util.h>


#include <vector>
#include <algorithm>

#define MAX_PURGEABLE_SIZE          250
#define MIN_PURGEABLE_SIZE          50
#define DEFAULT_PURGE_TIMEOUT         2000  // 2 secs
#define DEFAULT_MAX_PURGE_TIMESTAMP   3000  // 3 secs

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::library;
using namespace pmod::proxy;


BEGIN_DECLARE_IID(IProxyDisconnectInfoPair, _VOID_MACRO);

// {D79FC81D-2AAE-4E72-9B49-3F75259F6413}
const GUID IID_IProxyDisconnectInfoPair =
{ 0xd79fc81d, 0x2aae, 0x4e72, { 0x9b, 0x49, 0x3f, 0x75, 0x25, 0x9f, 0x64, 0x13 } };

// {681BEA62-E169-4B58-A291-F5C9D1009F49}
const GUID __IID_IObjectNodeClass =
{ 0x681bea62, 0xe169, 0x4b58,{ 0xa2, 0x91, 0xf5, 0xc9, 0xd1, 0x0, 0x9f, 0x49 } };

// {5DA95F29-E571-424B-8423-C487DD316FF4}
const GUID __IID_IViewModelFactoryClass =
{ 0x5da95f29, 0xe571, 0x424b,{ 0x84, 0x23, 0xc4, 0x87, 0xdd, 0x31, 0x6f, 0xf4 } };

typedef IKeyValuePair<IBaseObjectProxyInternal *, foundation::IInspectable *, IID_IProxyDisconnectInfoPair> IProxyDisconnectInfoPair;
typedef _InspectableKeyValuePair<IProxyDisconnectInfoPair,_RefCountedValueAlloc<IBaseObjectProxyInternal *>> CProxyDisconnectInfoPair;

static _pal_AtomicIntegerType _countProxyFactoryObjects = 0;

// used by internal libraries
UINT32 GetCountProxyFactoryObjects()
{
    return (UINT32)_pal_InterlockedCompareExchange(&_countProxyFactoryObjects, 0, 0);
}
//------------------------------------------------------------------------------
// Class:   CProxyObjectFactory
//
// Purpose: Class that Implements IPropertyModelFactory interface
//
//-----------------------------------------------------------------------------
CProxyObjectFactory::CProxyObjectFactory():
    m_IsFreeThreaded(false),
    m_IsModelCacheEnabled(false),
    m_IsCacheValuesEnabled(false),
    m_IsModelTypeInfoProxyEnabled(true),
    m_AutoPurge(false),
    m_Disposed(false),
    m_PurgeTimeoutId(0),
    m_PurgeTimeout(DEFAULT_PURGE_TIMEOUT),
    m_MaxPurgeTimestamp(DEFAULT_MAX_PURGE_TIMESTAMP),
    m_InPurgingProcess(false),
    m_MaxPurgeableSize(MAX_PURGEABLE_SIZE),
    m_MinPurgeableSize(MIN_PURGEABLE_SIZE),
    m_PurgeIfAttached(false),
    m_NoPurgeState(false),
    m_SourceConnectState(true),
    m_ToProxyValueDisabled(false),
    m_FromProxyValueDisabled(false),
    m_NextProxyUniqueId(0)
{
    _pal_InterlockedIncrement(&_countProxyFactoryObjects);
}

void CProxyObjectFactory::FinalRelease()
{
    _pal_InterlockedDecrement(&_countProxyFactoryObjects);

    // there are case were the FinalRelease is called without a 'Dispose' call to happen
    // so we want to replicate some Dispose functionality
    if(!m_Disposed)
    {
        this->clear_cache();
        m_ViewModelFactory.Release();
    }
    _ProxyObjectFactoryBaseType::FinalRelease();
}

HRESULT CProxyObjectFactory::_Initialize(
    pmod::proxy::ProxyObjectFactoryOptions options,
    foundation::IInspectable *pWorkingDispatcher,
    foundation::IInspectable *pEventDispatcher)
{
    //if (pPropertyModelFactoryParams->_maxPurgeableSize != (UINT32)-1)
    //{
    //    m_MaxPurgeableSize = pPropertyModelFactoryParams->_maxPurgeableSize;
    //}
    //if (pPropertyModelFactoryParams->_minPurgeableSize != (UINT32)-1)
    //{
    //    m_MinPurgeableSize = pPropertyModelFactoryParams->_minPurgeableSize;
    //}
    //if (pPropertyModelFactoryParams->_purgeTimeout != (UINT32)-1)
    //{
    //    m_PurgeTimeout = pPropertyModelFactoryParams->_purgeTimeout;
    //}
    //if (pPropertyModelFactoryParams->_maxPurgeTimestamp != (UINT32)-1)
    //{
    //    m_MaxPurgeTimestamp = pPropertyModelFactoryParams->_maxPurgeTimestamp;
    //}
    
    m_PurgeIfAttached = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::PurgeIfAttached);
    m_IsModelTypeInfoProxyEnabled = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::UseModelTypeInfoProxy);
    m_IsModelCacheEnabled = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::IsModelCacheEnable);
    m_IsCacheValuesEnabled = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::IsCacheValuesEnabled);
    m_NoPurgeState = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::NoPurgeState);
    m_AutoPurge = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::AutoPurge);

    m_IsSourceFreeThreaded = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::IsSourceFreeThreaded);

    m_IsFreeThreaded = IsOptionEnabled(
        options,
        FoundationClassOptions_IsFreeThreadApartment);

    m_ToProxyValueDisabled = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::ToProxyValueDisabled);

    m_FromProxyValueDisabled = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::FromProxyValueDisabled);

    m_UseCacheStrongRefForProperties = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::UseCacheStrongRefForProperties);

    m_UseCacheStrongRefForItems = IsOptionEnabled(
        options,
        ProxyObjectFactoryOptions::UseCacheStrongRefForItems);

    // init the critical section if needed.
    // Note: both IsFreeThreaded | IsFreeThreadApartment will protect this Object
    if (this->IsThreadSafeLocking())
    {
        // Create the 'shared' protection section
        IFR_ASSERT(m_critsec.Init());
    }
    if (pEventDispatcher != nullptr)
    {
        IFR_ASSERT(foundation::QueryInterface(pEventDispatcher,m_spEventDispatcher.GetAddressOf()));
    }
    if (pWorkingDispatcher != nullptr)
    {
        IFR_ASSERT(foundation::QueryInterface(pWorkingDispatcher, m_spDispatcher.GetAddressOf()));
    }

    if (logger_util::IsAvailable())
    {
        IFR_ASSERT(logger_util::create_foundation_logger(
            Proxy_CategoryEnum::TypeId,
            (foundation::LoggerOptions)FoundationClassOptions_IsFreeThreadApartment,
            m_ProxyLogger.GetAddressOf()));

        IFR_ASSERT(logger_util::GetCategory(m_ProxyLogger,
            m_ProxyFactoryCategory.GetAddressOf(),
            Factory_CategoryEnum::TypeId,
            -1));

        IFR_ASSERT(logger_util::GetCategory(m_ProxyLogger,
            m_CommandProxyCategory.GetAddressOf(),
            BaseProxy_CategoryEnum::TypeId,
            pmod::proxy::Command_CategoryEnum::TypeId,
            -1));

        IFR_ASSERT(logger_util::GetCategory(m_ProxyLogger,
            m_CollectionProxyCategory.GetAddressOf(),
            BaseProxy_CategoryEnum::TypeId,
            pmod::proxy::Collection_CategoryEnum::TypeId,
            -1));

        IFR_ASSERT(logger_util::GetCategory(m_ProxyLogger,
            m_AsyncOperationProxyCategory.GetAddressOf(),
            BaseProxy_CategoryEnum::TypeId,
            pmod::proxy::AsyncOperation_CategoryEnum::TypeId,
            -1));

        IFR_ASSERT(logger_util::GetCategory(m_ProxyLogger,
            m_PropertyProxyCategory.GetAddressOf(),
            BaseProxy_CategoryEnum::TypeId,
            Property_CategoryEnum::TypeId,
            -1));
    }

    return S_OK;
}


HRESULT CProxyObjectFactory::QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
{
    if (iid == IDisposable::GetIID())
    {
        *ppInterface = static_cast<IDisposable *>(this);
    }
    else
    {
        return _ProxyObjectFactoryBaseType::QueryInterfaceImpl(iid,ppInterface);
    }
    return S_OK;
}

HRESULT CProxyObjectFactory::Lock() throw()
{
    if (IsThreadSafeLocking())
    {
        return m_critsec.Lock();
    }
    return S_OK;
}

HRESULT CProxyObjectFactory::Unlock() throw()
{
    if (IsThreadSafeLocking())
    {
        return m_critsec.Unlock();
    }
    return S_OK;
}

bool CProxyObjectFactory::IsRemoveProxyFromCacheEnabled()
{
    // we allow Removing Proxy from Cache when 
    // no yet disposed and no purge state
    return !m_Disposed && m_NoPurgeState;
}


HRESULT CProxyObjectFactory::GetProxies(UINT32 *size, _COM_Outptr_ IProxyObject ***ppProxies)
{
    foundation_lib::_FoundationLock csl(*this);

    std::vector<foundation::ComPtr<IProxyObject>> proxies;
    ForEachBaseModelProxyInternal(
        [&proxies](_In_ IBaseObjectProxyInternal *pBaseModelProxyClass)
    {
        foundation::ComPtr<IProxyObject> pBaseObjectProxy = nullptr;
        foundation::QueryInterface(pBaseModelProxyClass, pBaseObjectProxy.GetAddressOf());
        proxies.push_back(pBaseObjectProxy);
        return S_OK;
    });
    return RefCountCopyTo(proxies, size, ppProxies);
}

HRESULT CProxyObjectFactory::DisconnectProxySources(foundation::IInspectable **ppDisconnectInfo)
{
    typedef std::vector<foundation::ComPtr<IProxyDisconnectInfoPair>> _ProxyDisconnectInfoPair_Vector_type;
    _ProxyDisconnectInfoPair_Vector_type proxiesDisconnectInfo;

    IFR_ASSERT(ForEachBaseModelProxyInternal(
        [&proxiesDisconnectInfo](_In_ IBaseObjectProxyInternal *pBaseModelProxyClass)
    {
        foundation::InspectablePtr spDisconnectInfo;
        IFR_ASSERT(pBaseModelProxyClass->DisconnectFromSource(spDisconnectInfo.GetAddressOf()));
        IFR_ASSERT(pBaseModelProxyClass->SetSourceConnectState(false));
      
        foundation::ComPtr<IProxyDisconnectInfoPair> spProxyDisconnectInfoPair;
        IFR_ASSERT(foundation::library::CreateKeyValuePair<CProxyDisconnectInfoPair>(
            pBaseModelProxyClass,
            spDisconnectInfo.Get(),
            spProxyDisconnectInfoPair.GetAddressOf()));
        proxiesDisconnectInfo.push_back(spProxyDisconnectInfoPair);

        return S_OK;
    }));

    IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
        (UINT32)proxiesDisconnectInfo.size(),
        proxiesDisconnectInfo.size() ? (foundation::IInspectable **)proxiesDisconnectInfo.front().GetAddressOfPtr() : nullptr,
        ppDisconnectInfo
        ));

    return S_OK;
}

HRESULT CProxyObjectFactory::ConnectProxySources(foundation::IInspectable *pDisconnectInfo)
{
    foundation::InspectableArrayWrapper proxiesDisconnectInfoArray;
    IFR_ASSERT(foundation::pv_util::GetInspectableArray(
        pDisconnectInfo,
        proxiesDisconnectInfoArray.GetSizeAddressOf(),
        proxiesDisconnectInfoArray.GetBufferAddressOf()
        ));

    _SyncSourceInfo_Vector_Type syncSourceInfo;
    for (UINT32 index = 0; index < proxiesDisconnectInfoArray.GetSize(); ++index)
    {
        foundation::ComPtr<IProxyDisconnectInfoPair> spProxyDisconnectInfoPair;
        IFR_ASSERT(foundation::QueryInterface(
            proxiesDisconnectInfoArray[index], 
            IID_IProxyDisconnectInfoPair, 
            spProxyDisconnectInfoPair.GetAddressOf()));
        foundation::ComPtr<IBaseObjectProxyInternal> spBaseObjectProxyInternal;
        spProxyDisconnectInfoPair->GetKey(spBaseObjectProxyInternal.GetAddressOf());
        foundation::InspectablePtr spDisconnectInfo;
        spProxyDisconnectInfoPair->GetValue(spDisconnectInfo.GetAddressOf());
        
        // always connecte the proxy if it was in the list.
        spBaseObjectProxyInternal->SetSourceConnectState(true);
        
        // Only refresh the properties if there is cache info.
        if (spDisconnectInfo != nullptr)
        {
            IFR_ASSERT(spBaseObjectProxyInternal->ConnectToSource(spDisconnectInfo, syncSourceInfo));
        }
    }
    if (syncSourceInfo.size())
    {
        for (_SyncSourceInfo_Vector_Type::iterator
            iter = syncSourceInfo.begin();
            iter != syncSourceInfo.end();
        ++iter)
        {
            (*iter).first->FireResyncEvent((*iter).second);
        }
    }
    return S_OK;
}

template <class _Func>
HRESULT CProxyObjectFactory::ForEachBaseModelProxyInternal(const _Func& func)
{
    return for_each_cached_object([&](foundation::IObject *p) -> HRESULT {
        foundation::ComPtr<IBaseObjectProxyInternal> pBaseObjectProxyInternal;
        IFR_ASSERT(foundation::QueryInterface(p, pBaseObjectProxyInternal.GetAddressOf()));
        return func(pBaseObjectProxyInternal);
    });
}

template <class _Func>
HRESULT CProxyObjectFactory::ForEachBaseModelProxyInternal(const _Map_cache_type& cache, const _Func& func)
{
    return for_each_cached_object(cache,[&](foundation::IObject *p) -> HRESULT {
        foundation::ComPtr<IBaseObjectProxyInternal> pBaseObjectProxyInternal;
        IFR_ASSERT(foundation::QueryInterface(p, pBaseObjectProxyInternal.GetAddressOf()));
        return func(pBaseObjectProxyInternal);
    });
}

void CProxyObjectFactory::SetPurgeTimer(UINT32 timeout)
{
    // Queue a Purge from the Timer Service
    _pal_SetTimer(
        m_PurgeTimeoutId,
        static_cast<ITimerCallback *>(this),
        timeout);
}

void CProxyObjectFactory::KillPurgeTimer()
{
    if(m_PurgeTimeoutId)
    {
        _pal_KillTimer(m_PurgeTimeoutId);
    }
}

STDMETHODIMP CProxyObjectFactory::Dispose()
{
    // no matter we end up in this call we will consider Disposed
    m_Disposed = true;

    _Map_cache_type currentCache;
    //do a swap while protecting the cache
    {
        foundation_lib::_FoundationLock csl(*this);       
        this->GetCache().swap(currentCache);
    }

    // Disable Purge State Iterator Func
    CProxyObjectFactory::ForEachBaseModelProxyInternal(currentCache,
        [](_In_ IBaseObjectProxyInternal *pBaseModelProxyClass)
    {
        pBaseModelProxyClass->SetPurgeStateEnabled(false);
        return S_OK;
    });

    // Kill Purge Timer if exists
    KillPurgeTimer();

    // next block will push weak references of every cached proxy
    typedef std::vector<foundation::WeakReferencePtr> _Weak_ref_vector_type;
    _Weak_ref_vector_type weakRefProxies;
    CProxyObjectFactory::ForEachBaseModelProxyInternal(currentCache,
        [&weakRefProxies](_In_ IBaseObjectProxyInternal *pBaseModelProxyClass)
    {
        foundation::WeakReferencePtr weak_ref_ptr;
        IFR_ASSERT(foundation::AsWeakRef(weak_ref_ptr, pBaseModelProxyClass));
        weakRefProxies.push_back(weak_ref_ptr);
        return S_OK;
    });

    DiposedProxyMapType disposedProxies;
    // Iterate on our Cached proxies collected in the last run 
    // and see how many of them 'survive'
    for (_Weak_ref_vector_type::iterator iter = weakRefProxies.begin();
        iter != weakRefProxies.end();
        ++iter)
    {
        IBaseObjectProxyInternal *pBaseModelProxyInternal = nullptr;
        foundation::ResolveWeakRef(*iter, pBaseModelProxyInternal);
        if (pBaseModelProxyInternal != nullptr)
        {
            UINT32 uniqueId = BaseObjectProxy::AsBaseProxyClass(pBaseModelProxyInternal)->m_UniqueId;
            // Insert into our private Map 
            disposedProxies.insert(std::make_pair(uniqueId, pBaseModelProxyInternal));
            // Dipose will happen now
            pBaseModelProxyInternal->Dispose();
        }
    }

    // Remove all our Purgeable Models
    m_PurgeableModels.clear();

    // Release COM Ptr References
    m_ViewModelFactory.Release();

#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    typedef std::map<UINT32,foundation::WeakReferencePtr> _Weak_ref_map_type;

    // Next Container will allow to collect 'potential' leak proxies
    _Weak_ref_map_type  proxyLeakReferences;
#endif

    // Iterate and FinalRelease the disposed proxies
    for(DiposedProxyMapType::iterator iter = disposedProxies.begin();
        iter != disposedProxies.end();
        ++iter)
    {
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
        UINT32 refCount =
#endif
        iter->second->Release();
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
        if(refCount)
        {
            // if we arrive here it does not mean the proxy is leaking
            // The main reason of this is that 'Aggregate' proxies like
            // Command & Collection can have a 'Inner' proxy that will be destroyed
            // only when the FinalRelease is being invoked

            // Add the proxies into our Container
            foundation::WeakReferencePtr weak_ref_ptr;
            HRESULT hr = foundation::AsWeakRef(weak_ref_ptr, iter->second);
            foundation_assert(SUCCEEDED(hr));

            proxyLeakReferences.insert(std::make_pair(iter->first, weak_ref_ptr));
        }
#endif
    }

#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    // Iterate to find still remaining proxies that are 'alive'
    for (_Weak_ref_map_type::iterator iter = proxyLeakReferences.begin();
        iter != proxyLeakReferences.end();
        ++iter)
    {
        // if the Weak Reference Hodler is able to resolve
        // then we have a proxy that 'survive' all the Dispose mechanism
        // and we want to dump it here
        IBaseObjectProxyInternal *pBaseModelProxyInternal = nullptr;
        foundation::ResolveWeakRef(iter->second, pBaseModelProxyInternal);
        if (pBaseModelProxyInternal != nullptr)
        {
            // Dump potential leaked proxies
            foundation::HStringPtr modelName;
            foundation::IObjectTypeInfo *pTypeInfo = BaseObjectProxy::AsBaseProxyClass(pBaseModelProxyInternal)->GetTypeInfo();
            if (pTypeInfo)
            {
                pTypeInfo->GetName(modelName.GetAddressOf());
            }
            UINT32 refCount = pBaseModelProxyInternal->Release();
            foundation::ostringstream_t os;
            os << "Dispose Proxy Type:" << modelName.GetRawBuffer()
                << " Id:" << iter->first
                << " RefCount:" << refCount
                << std::endl;
            _pal_Trace(os.str().c_str());
        }
    }
#endif

    return S_OK;
}

bool CProxyObjectFactory::LookupModelProxyInternal(_In_ foundation::IObject *pModel, _Out_ foundation::ComPtr<foundation::IObject>& spProxyModel)
{
    foundation_assert(pModel);

    if(!LookupModelProxyHelper(pModel,spProxyModel))
    {
        //// ViewModel replacements could be passed instead of the 'mapped' Model
        //foundation::ComPtr<ISourceModel> spSourceModel;
        //if(SUCCEEDED(foundation::QueryInterface(pModel,spSourceModel.GetAddressOf())))
        //{
        //    foundation::ComPtr<foundation::IObject> spSource;
        //    spSourceModel->GetSource(spSource.GetAddressOf());
        //    IPropertyViewModelProxy *pPropertyViewModelProxy = nullptr;
        //    if(
        //        spSource != nullptr && 
        //        // found it in our 'cache'
        //        LookupModelProxyHelper(spSource,spProxyModel) &&
        //        // type has to be 'IPropertyViewModelProxy'
        //        SUCCEEDED(foundation::QueryWeakReference(spProxyModel,&pPropertyViewModelProxy)))
        //    {
        //        // we found a Proxy mapped on the 'Source'
        //        spSource.Release();
        //        // find the VM source and compare it with the passed Model
        //        IFRF(pPropertyViewModelProxy->GetSource(spSource.GetAddressOf()));
        //        if (object_statics_util::is_object_equals(spSource, pModel))
        //        {
        //            return true;
        //        }
        //    }
        //    // Ensure we 'clean' the returned Proxy
        //    spProxyModel.Release();
        //}
        return false;
    }
    return true;
}

bool CProxyObjectFactory::LookupModelProxyHelper(_In_ foundation::IObject *pModel, _Out_ foundation::ComPtr<foundation::IObject>& spProxyModel)
{
    foundation_assert(pModel);

	UINT32 processId = 0;
    pModel->GetProcessId(&processId);
	UINT32 uniqueId = 0;
    pModel->GetUniqueId(&uniqueId);

    return LookupModelProxyHelper(processId,uniqueId,spProxyModel );
}

bool CProxyObjectFactory::LookupModelProxyHelper(_In_ UINT32 processId, _In_ UINT32 uniqueId, _Out_ foundation::ComPtr<foundation::IObject>& spProxyModel)
{
    foundation_lib::_FoundationLock csl(*this);

    return this->lookup(std::make_pair(processId, uniqueId), spProxyModel.GetAddressOf()) == S_OK;
}

std::vector<CProxyObjectFactory::PurgeInfoType>::iterator CProxyObjectFactory::FindPurgedProxy(IBaseObjectProxyInternal *pBaseModelProxy)
{
    for( std::vector<PurgeInfoType>::iterator iter = m_PurgeableModels.begin();
        iter != m_PurgeableModels.end();
        ++iter)
    {
        if(pBaseModelProxy == (*iter).first)
        {
            return iter;
        }
    }
    return m_PurgeableModels.end();
}

void CProxyObjectFactory::DeletePurgeableModel(IBaseObjectProxyInternal *pBaseModelProxy)
{
    std::vector<PurgeInfoType>::iterator iter = FindPurgedProxy(pBaseModelProxy);

    if(iter != m_PurgeableModels.end())
    {
        m_PurgeableModels.erase(iter);
    }
    else
    {
        foundation_assert(false);
    }
}

HRESULT CProxyObjectFactory::PurgeProxyObjects(_In_ UINT32 max_purge_size, _In_ UINT64 timeStamp)
{
    if (m_spDelegate)
    {
        HRESULT hr = m_spDelegate->OnPurgeInvoked(max_purge_size, timeStamp);
        IFHR(hr);
        if (hr == S_OK)
        {
            // Delegate callback take care
            return S_OK;
        }
    }
    return PurgeProxyObjectsInternalHelper(max_purge_size, timeStamp);
}

HRESULT CProxyObjectFactory::PurgeProxyObjectsInternalHelper(_In_ UINT32 max_purge_size,_In_ UINT64 timeStamp)
{

    m_InPurgingProcess = true;

    UINT64 currentTimestamp = _pal_GetTickCount64();

    // Start the purge process
    while(m_PurgeableModels.size())
    {
        IBaseObjectProxyInternal *pBaseModelProxy = m_PurgeableModels[0].first;
        UINT64 proxyTimestamp = m_PurgeableModels[0].second;

        if(
            // Purge Size condition
            (max_purge_size != (UINT32)-1 && m_PurgeableModels.size() <= max_purge_size)
            ||
            // Timestamp condition
            (timeStamp != -1 && ((UINT64)(currentTimestamp - proxyTimestamp) < timeStamp))
          )
        {
            break;
        }

        foundation::IObject *pProxyAsModel = BaseObjectProxy::GetAsModel(pBaseModelProxy);
        // Dispose first so the Purge State mechanism won't happen again
        IGNOREHR(pBaseModelProxy->Dispose());

        // AddRef + Remove + Release
        if (VerifyCachedInstance(pProxyAsModel))
        {
            UINT32 refCnt;
            refCnt = pBaseModelProxy->AddRef();
            IFR(PurgeProxyModel(pBaseModelProxy));
            refCnt = pBaseModelProxy->Release();
            foundation_assert(refCnt == 0);
        }
        else
        {
            // This shouldn't happen.  We have a weak reference to this proxy
            // on our purge list, but no strong reference on our cache list.
            // Since the weak pointer may be dangling, we cannot safely call
            // in to the object, so we simple delete this entry from the 
            // purgable list.
            
            foundation_assert(false);
            DeletePurgeableModel(pBaseModelProxy);
        }
    }

    // Purge also Weak References proxies
    PurgeWeakReferencesProxies();

    m_InPurgingProcess = false;

    return S_OK;
}

void CProxyObjectFactory::PurgeWeakReferencesProxies()
{
    // Purge Weak reference that are null
    this->remove_all_empty_weak_ref();
}

HRESULT CProxyObjectFactory::ToProxyObject(foundation::IObject * pSource, _COM_Outptr_result_maybenull_ foundation::IObject * *pResult)
{
    return ToProxyObjectInternal(pSource, pResult);
}

HRESULT CProxyObjectFactory::CreateProxyObject(
    ProxyType proxyType,
    foundation::IObject * pSource,
    IProxyObjectDelegate * pProxyObjectDelegate,
    foundation::IInspectable * pOuter,
    _COM_Outptr_result_maybenull_ foundation::IInspectable * *pResult)
{
    return CreateObjectProxyInstanceInternal(proxyType, pSource, pProxyObjectDelegate, pOuter, pResult);
}

HRESULT CProxyObjectFactory::LookupObjectProxy(
    foundation::IObject *pSourceObject,
    IProxyObject **ppModelProxy)
{
    IFCPTR(pSourceObject);
    IFCPTR(ppModelProxy);

    foundation::ComPtr<foundation::IObject> spProxyModel;
    if (LookupModelProxyInternal(pSourceObject, spProxyModel))
    {
        return foundation::QueryInterface(spProxyModel,ppModelProxy);
    }
    else
    {
        *ppModelProxy = nullptr;
    }
    return S_OK;

}

void CProxyObjectFactory::OnProxyModelPurgeStateChanged(IBaseObjectProxyInternal *pBaseModelProxy,bool isPurgeReady)
{
    if(isPurgeReady)
    {
        std::vector<PurgeInfoType>::iterator iter = FindPurgedProxy(pBaseModelProxy);

        if(iter == m_PurgeableModels.end())
        {
            // Before adding this proxy to the purge list (which is a weak reference), verify that
            // we do indeed have this proxy in our cache (which is a strong reference).
            if (VerifyCachedInstance(BaseObjectProxy::GetAsModel(pBaseModelProxy)))
            {
                m_PurgeableModels.push_back(PurgeInfoType(pBaseModelProxy,_pal_GetTickCount64()));
            }
            else
            {
                // we should never arrive here
                foundation_assert(false);
            }
        }
        else
        {
            // we should never arrive here
            foundation_assert(false);
        }
    }
    else
    {
        DeletePurgeableModel(pBaseModelProxy);
    }
}

void CProxyObjectFactory::OnProxyObjectPurgeReady(_In_ IProxyObject *pBaseModelProxy)
{
    if(m_InPurgingProcess)
    {
        // already in Purge process
        return;
    }

    if (m_spDelegate)
    {
        HRESULT hr = m_spDelegate->OnProxyObjectPurgeReady(pBaseModelProxy);
        if (hr == S_OK)
        {
            // the Callback delegate take care of the Purge attempt
            return;
        }
    }

    // Default processing will be executed here
    if(m_AutoPurge)
    {
        if(!IsPurgeTimerRunning())
        {
            SetPurgeTimer(m_PurgeTimeout);
        }
    }
    else if ((m_PurgeableModels.size() + 1) > m_MaxPurgeableSize)
    {
        PurgeProxyObjects(m_MinPurgeableSize,-1);
    }
}

HRESULT CProxyObjectFactory::PurgeProxyModel(IBaseObjectProxyInternal *pBaseModelProxy)
{
    // remove weak reference in our purgeable list
    DeletePurgeableModel(pBaseModelProxy);
    
    // remove from our Cached Instances
    HRESULT hr = RemoveFromCachedInstances(pBaseModelProxy);
    foundation_assert(S_OK == hr);
    IFR(hr);

    return S_OK;
}

HRESULT CProxyObjectFactory::AddToCachedInstances(foundation::IObject *pModel)
{
    foundation_lib::_FoundationLock csl(*this);

    // we should not allow 'Cache' any Model after being disposed
    foundation_assert(!m_Disposed);

    foundation_assert(pModel);
//    _ObjectReferenceMap_Type& modelInstanceMap = GetProcessModelInstanceMap(pModel);

    UINT32 uniqueId;
    IFR(pModel->GetUniqueId(&uniqueId));

    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        Factory_CategoryEnum::TypeId,
        Factory_Category::AddToCache);

    if (IsLoggingAvailable() && logger_util::IsInfoEnabled(
        this->m_ProxyFactoryCategory,
        logUniqueId))
    {
        logger_util::Log(
            this->m_ProxyLogger,
            nullptr,
            foundation::LoggingLevel::Info,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->m_ProxyFactoryCategory,
            Factory_BaseProxyCache_LogRecordEnum::TypeId,
            // UniqueId
            Factory_BaseProxyCache_LogRecord::UniqueId,
            foundation::pv_util::CreateValue((UINT32)uniqueId).Get(),
            -1);
    }

    foundation::ComPtr<IBaseObjectProxyInternal> spBaseModelProxyClass;
    IFR(foundation::QueryInterface(pModel,spBaseModelProxyClass.GetAddressOf()));

    bool isProxyComposed;
    IFR(spBaseModelProxyClass->IsProxyComposed(isProxyComposed));

    bool isPurgeStateEnabled = !(isProxyComposed || m_NoPurgeState);
 
    this->add_to_cache(pModel, !isPurgeStateEnabled);

    // start tracking Purge State
    if (isPurgeStateEnabled)
    {
        spBaseModelProxyClass->SetPurgeStateEnabled(true);
    }

    return S_OK;
}

HRESULT CProxyObjectFactory::RemoveFromCachedInstances(IUnknown *pUnk)
{
    foundation_assert(pUnk);

    foundation::ComPtr<foundation::IObject> spObject;
    IFR_ASSERT(foundation::QueryInterface(pUnk, spObject.GetAddressOf()));
    return RemoveFromCachedInstances(spObject);
}

HRESULT CProxyObjectFactory::RemoveFromCachedInstances(foundation::IObject *pModel)
{
    foundation_assert(pModel);

    UINT32 processId = 0;
    pModel->GetProcessId(&processId);
    UINT32 uniqueId;
    IFR(pModel->GetUniqueId(&uniqueId));
    return RemoveProxyFromCache(processId, uniqueId);
}

HRESULT CProxyObjectFactory::RemoveProxyFromCache(UINT32 processId, UINT32 uniqueId)
{
    foundation_lib::_FoundationLock csl(*this);

    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        Factory_CategoryEnum::TypeId,
        Factory_Category::RemoveFromCache);

    if (IsLoggingAvailable() && logger_util::IsInfoEnabled(
        this->m_ProxyFactoryCategory,
        logUniqueId))
    {
        logger_util::Log(
            this->m_ProxyLogger,
            nullptr,
            foundation::LoggingLevel::Info,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->m_ProxyFactoryCategory,
            Factory_BaseProxyCache_LogRecordEnum::TypeId,
            // UniqueId
            Factory_BaseProxyCache_LogRecord::UniqueId,
            foundation::pv_util::CreateValue((UINT32)uniqueId).Get(),
            -1);
    }
    return this->remove_from_cache(std::make_pair(processId, uniqueId));
}

bool CProxyObjectFactory::VerifyCachedInstance(_In_ foundation::IObject *pModel)
{
    UINT32 processId,uniqueId = 0;
    IFREx(pModel->GetProcessId(&processId), false);
    IFREx(pModel->GetUniqueId(&uniqueId), false);
    return this->is_cached(std::make_pair(processId, uniqueId));
}


HRESULT CProxyObjectFactory::TryCreateSingleSourceViewModel(
    foundation::IObjectTypeInfo *pObjectTypeInfo,
    IID& iidModelTypeForView,
    foundation::IObject *pModel,
    ISourceModel **ppViewModel)
{
    HRESULT hr = S_OK;
    foundation_assert(m_ViewModelFactory);
    foundation_assert(pModel);
    foundation_assert(ppViewModel);

    foundation::ComPtr<pmod::library::IViewModelFactoryClass> spViewModelFactoryClass;
    IFR_ASSERT(foundation::QueryInterface(m_ViewModelFactory,__IID_IViewModelFactoryClass,spViewModelFactoryClass.GetAddressOf()));

    foundation::ComPtr<IObjectTypeInfo> spObjectTypeInfo(pObjectTypeInfo);

    while(spObjectTypeInfo != nullptr)
    {
        IFR_ASSERT(spObjectTypeInfo->GetType(&iidModelTypeForView));

        hr = spViewModelFactoryClass->CreateSingleSourceViewModelInternal(iidModelTypeForView,pModel,ppViewModel);
        if(hr != (HRESULT)foundation::Errors::E_OBJECT_ENTRY_NOTFOUND)
        {
            IFHR(hr);
        }
        if(hr == S_OK)
        {
            foundation_assert(*ppViewModel);
            // found it and create successfully
            return S_OK;
        }
        foundation::ComPtr<foundation::IObjectTypeInfo> spBaseType;
        IFR_ASSERT(spObjectTypeInfo->GetBaseType(spBaseType.GetAddressOf()));
        spObjectTypeInfo = spBaseType;
    }
    // not able to found a View Model
    return S_FALSE;
}

HRESULT CProxyObjectFactory::CreatePropertyModelProxyInstanceInternal(
    IObservableObject *pSourceModel,
    IObservableObject **ppNewInstance)
{
    IID iidModelType = foundation_GUID_NULL;
    foundation::ComPtr<IObservableObjectInfo> spObservableObjectInfo;
    IFR(foundation::GetObjectTypeInfo(pSourceModel, spObservableObjectInfo.GetAddressOf()));
    IFR_ASSERT(spObservableObjectInfo->GetType(&iidModelType));
    
    foundation::ComPtr<foundation::IObject> spModelProxy; // Instance of the created Model Proxy

    // A ViewModel to be fired in case it was created
    foundation::ComPtr<foundation::IObject> spViewModel;

    // Create the Proxy Model for the passed instance
    IObservableObject *pObservableObjectProxy = nullptr;
    IFR(CreateObjectProxyInstanceInternal(
        ProxyType::ObservableObject,
        pSourceModel,
        &pObservableObjectProxy));

    // assign directly to the raw pointer
    spModelProxy.Attach(pObservableObjectProxy);

    // check if we have a runtime View Model replacement
    if(m_ViewModelFactory != nullptr)
    {
        foundation::ComPtr<ISourceModel> spSourceViewModel;

        IID iidModelTypeForView = foundation_GUID_NULL;        // the Model Type GUID that match the View
        HRESULT hr = TryCreateSingleSourceViewModel(
                spObservableObjectInfo,
                iidModelTypeForView,
                pObservableObjectProxy,
                spSourceViewModel.GetAddressOf());
        IFHR(hr);
        if(hr == S_OK)
        {
            IFR(foundation::QueryInterface(spSourceViewModel,spViewModel.GetAddressOf()));

            foundation_assert(spViewModel);

            IObservableObject *pViewModelProxy = nullptr;
            IFR(CreateObjectProxyInstanceInternal(
                ProxyType::ViewModel,
                spViewModel,
                &pViewModelProxy));

            // Release previous instance
            spModelProxy.Release();
            // assign directly to the raw pointer
            spModelProxy.Attach(pViewModelProxy);
        }
    }

    if(!m_Disposed)
    {
        foundation::ComPtr<IUnknown> spUnknown(spModelProxy); // prevent the Model Proxy to move into a purge state

        // Cache this Model proxy instance
        foundation::ComPtr<foundation::IObject> spModel;
        IFR(foundation::QueryInterface(spModelProxy,spModel.GetAddressOf()));
        AddToCachedInstances(spModel);
    }

    if(spViewModel != nullptr)
    {
        // we invoke RuntimeInitialize for custom initialize
        pmod::library::IObjectNodeClass *pObjectNodeClass = nullptr;
        foundation::QueryWeakReference(spViewModel,__IID_IObjectNodeClass,&pObjectNodeClass);

        if (pObjectNodeClass != nullptr)
        {
            IFR(pObjectNodeClass->RuntimeInitialize());
        }

        // Fire View Model Creation...
        IFR(RaiseViewModelCreated(spViewModel));
    }

    // Return the QI for the created proxy model
    return foundation::QueryInterface(spModelProxy,ppNewInstance);
}

HRESULT CProxyObjectFactory::RaiseViewModelCreated(_In_ foundation::IObject *pViewModel)
{
    foundation_assert(m_ViewModelFactory != nullptr);
    foundation_assert(pViewModel != nullptr);

    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        Factory_CategoryEnum::TypeId,
        Factory_Category::ViewModelCreated);

    if (IsLoggingAvailable() && logger_util::IsInfoEnabled(
        this->m_ProxyFactoryCategory,
        logUniqueId))
    {
        UINT32 viewModelUniqueId = 0;
        GUID iidType;

        pViewModel->GetUniqueId(&viewModelUniqueId);
        foundation::GetObjectType(pViewModel, iidType);

        logger_util::Log(
            this->m_ProxyLogger,
            nullptr,
            foundation::LoggingLevel::Info,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->m_ProxyFactoryCategory,
            Factory_ViewModelCreated_LogRecordEnum::TypeId,
            // UniqueId
            Factory_Model_LogRecord::UniqueId,
            foundation::pv_util::CreateValue((UINT32)viewModelUniqueId).Get(),
            // Type
            Factory_Model_LogRecord::TypeId,
            foundation::pv_util::CreateValue(iidType).Get(),
            -1);
    }

    foundation::ComPtr<pmod::library::IViewModelFactoryClass> spObjectFactoryClass;
    IFR(foundation::QueryInterface(m_ViewModelFactory, __IID_IViewModelFactoryClass, spObjectFactoryClass.GetAddressOf()));

    return spObjectFactoryClass->RaiseObjectCreated(pViewModel);
}

HRESULT CProxyObjectFactory::CreateViewModel(
    GUID iidViewModel,
    foundation::IInspectable *pContextParameter ,
    foundation::IObject **ppNewInstance)
{
    if(m_ViewModelFactory == nullptr)
        return E_UNEXPECTED;
    foundation::InspectablePtr contextParameterValue(pContextParameter);
    if(pContextParameter)
    {
        IFR(FromProxyValueInternal(nullptr,contextParameterValue));
    }

    foundation::ComPtr<pmod::library::IViewModelFactoryClass> spObjectFactoryClass;
    IFR_ASSERT(foundation::QueryInterface(m_ViewModelFactory, __IID_IViewModelFactoryClass, spObjectFactoryClass.GetAddressOf()));

    foundation::ComPtr<foundation::IObject> spViewModelObject;
    IFR(spObjectFactoryClass->CreateObjectInternal(
            iidViewModel,
            contextParameterValue,
            spViewModelObject.GetAddressOf()));

    foundation::ComPtr<foundation::IObject> spViewModel;
    IFR_ASSERT(foundation::QueryInterface(spViewModelObject, spViewModel.GetAddressOf()));

    IFR(CreateObjectProxyInstanceInternal(
        ProxyType::ObservableObject,
        spViewModel,
        ppNewInstance));

    foundation_assert(*ppNewInstance);
    IFR(RaiseViewModelCreated(*ppNewInstance));

    return S_OK;
}

template <class T,class TSourceModel>
HRESULT CProxyObjectFactory::CreateObjectProxyInstanceInternal(
    foundation::IObject *pSourceObject,
    IProxyObjectDelegate *pProxyModelDelegateBase,
    IInspectable    *pOuter,
    IInspectable    **ppInspectable)
{
    foundation_assert(pSourceObject);
    foundation_assert(ppInspectable);

    UINT32 proxyUniqueId = _pal_InterlockedIncrement(&m_NextProxyUniqueId);

    foundation::ComPtr<TSourceModel> spSourceModel;
    IFR(foundation::QueryInterface(pSourceObject, spSourceModel.GetAddressOf()));
    if(pOuter)
    {
        IInspectable *pOuterInspectable = nullptr;
        IFR(foundation::QueryWeakReference(pOuter,foundation_IID_IInspectable,&pOuterInspectable));

        foundation::ctl::AggregableComInspectableObject<T> *pNewInstance;
        IFR(foundation::ctl::AggregableComInspectableObject<T>::CreateInstance(pOuterInspectable, &pNewInstance));
        IFR(pNewInstance->_Initialize(proxyUniqueId,spSourceModel, pProxyModelDelegateBase, this, T::_IsProxyFreeThreaded(this)));
        *ppInspectable = reinterpret_cast<IInspectable*>(static_cast<foundation::ctl::INonDelegatingInspectable *>(pNewInstance));
    }
    else
    {
        foundation::ctl::ComInspectableObject<T> *pNewInstance;
        IFR(foundation::ctl::ComInspectableObject<T>::CreateInstance(&pNewInstance));
        IFR(pNewInstance->_Initialize(proxyUniqueId, spSourceModel, pProxyModelDelegateBase, this, T::_IsProxyFreeThreaded(this)));
        *ppInspectable = static_cast<IInspectable *>(static_cast<typename T::_ComInspectableBaseType *>(pNewInstance));
    }
    return S_OK;
}

HRESULT CProxyObjectFactory::CreateObjectProxyInstanceInternal(
        ProxyType type,
        foundation::IObject *pSourceObject,
        pmod::proxy::IProxyObjectDelegate *pProxyModelDelegateBase,
        IInspectable *pOuter,
        IInspectable **ppUnknown)
{
    foundation_assert(pSourceObject);
    foundation_assert(ppUnknown);

    UINT32 logUniqueId = logger_util::ToLogMessageUniqueId(
        Factory_CategoryEnum::TypeId,
        Factory_Category::CreateObjectProxyInstance);


    if (IsLoggingAvailable() && logger_util::IsInfoEnabled(
        this->m_ProxyFactoryCategory,
        logUniqueId))
    {
        UINT32 sourceUniqueId;
        pSourceObject->GetUniqueId(&sourceUniqueId);

        logger_util::Log(
            this->m_ProxyLogger,
            nullptr,
            foundation::LoggingLevel::Info,
            // Message Enum Value
            logUniqueId,
            // Log Category
            this->m_ProxyFactoryCategory,
            Factory_CreateObjectProxyInstance_LogRecordEnum::TypeId,
            // ProxyType
            Factory_CreateObjectProxyInstance_LogRecord::ProxyType,
            foundation::pv_util::CreateValue((UINT32)type).Get(),
            // SourceUniqueId
            Factory_CreateObjectProxyInstance_LogRecord::SourceUniqueId,
            foundation::pv_util::CreateValue((UINT32)sourceUniqueId).Get(),
            -1);
    }

    if(type == ProxyType::ObservableObject)
    {
        return CreateObjectProxyInstanceInternal<CObservableObjectProxy, IObservableObject>
            (
             pSourceObject,
                pProxyModelDelegateBase,
                pOuter,
                ppUnknown
            );
    }
    else if(type == ProxyType::ObservableCollection)
    {
        return CreateObjectProxyInstanceInternal<CObservableCollectionProxy, IObservableCollection>
            (
            pSourceObject,
                pProxyModelDelegateBase,
                pOuter,
                ppUnknown
            );
    }
    else if (type == ProxyType::Command)
    {
        return CreateObjectProxyInstanceInternal<CCommandProxy,ICommand>
            (
            pSourceObject,
                pProxyModelDelegateBase,
                pOuter,
                ppUnknown
            );
    }
    else if(type == ProxyType::AsyncOperation)
    {
        return CreateObjectProxyInstanceInternal<CAsyncOperationProxy,foundation::IAsyncOperation>
            (
            pSourceObject,
                pProxyModelDelegateBase,
                pOuter,
                ppUnknown
            );
    }
    else if(type == ProxyType::ViewModel)
    {
        foundation_assert(false);
        //return CreateObjectProxyInstanceInternal<CPropertyViewModelProxy,IPropertyModel>
        //    (
        //    pSourceObject,
        //        pProxyModelDelegateBase,
        //        pOuter,
        //        ppUnknown
        //    );
    }
    return E_UNEXPECTED;
}


template <class Q>
HRESULT CProxyObjectFactory::CreateOrLookupModelProxyInstanceInternal(
    _In_ ProxyType type,
    _In_ foundation::IObject *pSourceObject,
    _COM_Outptr_ Q              **ppNewInstance)
{
    foundation_lib::_FoundationLock csl(*this);

    foundation::ComPtr<foundation::IObject> spProxyObject;
    if (LookupModelProxyInternal(pSourceObject, spProxyObject))
    {
        if(Q::GetIID() == foundation::IObject::GetIID())
        {
            return spProxyObject.CopyTo(ppNewInstance);
        }
        else
        {
            return foundation::QueryInterface(spProxyObject, ppNewInstance);
        }
    }

    if(type == ProxyType_PropertyModel)
    {
        foundation::ComPtr<IPropertyModel> spSourceModel;
        IFR(foundation::QueryInterface(pSourceObject, spSourceModel.GetAddressOf()));
        foundation::ComPtr<IPropertyModel> spProxyPropertyModel;
        IFR(CreatePropertyModelProxyInstanceInternal(spSourceModel,spProxyPropertyModel.GetAddressOf()));
        IFR(foundation::QueryInterface(spProxyPropertyModel,ppNewInstance));
    }
    else
    {
        IFR(CreateObjectProxyInstanceInternalAndCache(type, pSourceObject, ppNewInstance));
    }
    return S_OK;
}

template <class Q>
HRESULT CProxyObjectFactory::CreateObjectProxyInstanceInternal(
    _In_ ProxyType type,
    _In_ foundation::IObject *pSourceObject,
    _COM_Outptr_ Q              **ppNewInstance)
{
    foundation_assert(pSourceObject);
    foundation_assert(ppNewInstance);

    if(m_spDelegate)
    {
        foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
        IFR(pSourceObject->GetTypeInfo(spTypeInfo.GetAddressOf()));
        IID iidType;
        IFR(spTypeInfo->GetType(&iidType));

        foundation::ComPtr<IProxyObject> spProxyObject;
        IFR(m_spDelegate->OnCreateProxyObject(type, pSourceObject, iidType, spProxyObject.GetAddressOf()));
        if (spProxyObject != nullptr)
        {
            return foundation::QueryInterface(spProxyObject, ppNewInstance);
        }
    }

    foundation::ComPtr<IInspectable> spInspectable;

    IFR(CreateObjectProxyInstanceInternal(type, pSourceObject, nullptr, nullptr, spInspectable.GetAddressOf()));
    IFR(foundation::QueryInterface(spInspectable,ppNewInstance));
    return S_OK;

}
HRESULT CProxyObjectFactory::CreateObjectProxyInstanceInternalAndCache(
    _In_ pmod::proxy::ProxyType type,
    _In_ foundation::IObject *pSourceObject,
    _COM_Outptr_ foundation::IObject  **ppProxyObject)
{
    _IFR_(CreateObjectProxyInstanceInternal(type, pSourceObject, ppProxyObject));
    foundation_assert(*ppProxyObject != nullptr);
    if (!m_Disposed)
    {
        foundation::ComPtr<IUnknown> spUnknown(*ppProxyObject); // prevent the Model Proxy to move into a purge state
        AddToCachedInstances(*ppProxyObject);
    }
    return S_OK;
}

bool CProxyObjectFactory::IsObject(_In_ IInspectable *pValue, foundation::ComPtr<foundation::IObject>& spObject)
{
    foundation_assert(pValue);

    // TODO: change the IObjectNode -> IObject once we resolve the IEnumValue that 
    // also support the IObject interface now

    // Note that we are QI the IObjectNode Interface but ALSO the specific IXXModel
    // in case the underlying model does not route the IObjectNode interface
    if(IsInspectableValue(pValue) &&
        (
        SUCCEEDED(pValue->QueryInterface(ICommand::GetIID(), (void **)spObject.GetAddressOf())) ||
        SUCCEEDED(pValue->QueryInterface(IObservableCollection::GetIID(), (void **)spObject.GetAddressOf())) ||
        SUCCEEDED(pValue->QueryInterface(foundation::IAsyncOperation::GetIID(), (void **)spObject.GetAddressOf())) ||
        SUCCEEDED(pValue->QueryInterface(IObservableObject::GetIID(), (void **)spObject.GetAddressOf()))
        ))
    {
        return true;
    }
    return false;
}

bool CProxyObjectFactory::IsProxyObject(_In_ IInspectable *pValue, foundation::ComPtr<pmod::proxy::IProxyObject>& spModelProxy)
{
    foundation_assert(pValue);

    if(SUCCEEDED(pValue->QueryInterface(IProxyObject::GetIID(),(void **)spModelProxy.GetAddressOf())))
    {
        return true;
    }
    return false;
}

bool CProxyObjectFactory::IsProxyObjectContained(_In_ const foundation::InspectablePtr& value,foundation::ComPtr<pmod::proxy::IProxyObject>& spModelProxy)
{
    if (value != nullptr && IsProxyObject(value, spModelProxy))
    {
        foundation::ComPtr<IProxyObjectFactory> spFactory;
        spModelProxy->GetFactory(spFactory.GetAddressOf());
        if(spFactory.Get() == static_cast<IProxyObjectFactory *>(this))
        {
            // the Variant is a Proxy and also was created by this Factory Instance
            return true;
        }
    }
    return false;
}

HRESULT CProxyObjectFactory::ToProxyValueInternal(_In_ IProxyObject *pBaseModelProxy,_Inout_ foundation::InspectablePtr& value)
{
    if (m_ToProxyValueDisabled)
    {
        return S_FALSE;
    }
    foundation_lib::_FoundationLock csl(*this);

    //if(m_spDelegate)
    //{
    //    HRESULT hr = m_spDelegate->ToProxyValue(pBaseModelProxy,value.GetAddressOfPtr());
    //    IFHR(hr);
    //    if(hr == S_OK)
    //    {
    //        // callback take care of the proxy value conversion
    //        return S_OK;
    //    }
    //}
    return ToProxyValueInternal(value);
}

HRESULT CProxyObjectFactory::ToProxyValueInternal(_Inout_ foundation::InspectablePtr& value)
{
    foundation::ComPtr<IProxyObject> spBaseModelProxy;
    foundation::ComPtr<foundation::IObject> spObject;

    // If value is IObjectNode we need to proxy
    // TODO: we may need to Proxy Enum values
    if(value != nullptr 
        && !IsProxyObjectContained(value, spBaseModelProxy)
        && !IsPropertyValue(value)      // not an IPropertyValue
        && IsObject(value, spObject)
        )
    {
        foundation::ComPtr<foundation::IObject> spProxyModel;
        IFR(ToProxyObjectInternal(spObject, spProxyModel.GetAddressOf()));
        value = spProxyModel;
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CProxyObjectFactory::FromProxyValueInternal(
        _In_ IProxyObject *pBaseModelProxy,
        _In_opt_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        _Inout_ foundation::InspectablePtr& value)
{
    if (m_FromProxyValueDisabled)
    {
        return S_FALSE;
    }

    //if(m_spDelegate)
    //{
    //    HRESULT hr = m_spDelegate->FromProxyValue(pBaseModelProxy,value.GetAddressOfPtr());
    //    IFHR(hr);
    //    if(hr == S_OK)
    //    {
    //        // callback take care of the proxy value conversion
    //        return S_OK;
    //    }
    //}
    return FromProxyValueInternal(pPropertyTypeInfo,value);
}

HRESULT CProxyObjectFactory::FromProxyValueInternal(
    _In_opt_ foundation::IPropertyTypeInfo *pPropertyTypeInfo, 
    _Inout_ foundation::InspectablePtr& value)
{
    foundation::ComPtr<IProxyObject> spBaseModelProxy;
    if(value != nullptr && IsProxyObject(value,spBaseModelProxy))
    {
        foundation::ComPtr<foundation::IObject> spObject;

        /*
        if(pPropertyTypeInfo)
        {
            // Note: we will attempt to verify the Var Type Info
            // and the 'Source' proxy. If they match we should expect
            // the unwrap is this instance

            foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
            IFR_ASSERT(pPropertyTypeInfo->GetTypeInfo(spTypeInfo.GetAddressOf()));
            if (spTypeInfo != nullptr)
            {
                IID iidVarType;
                IFR_ASSERT(spTypeInfo->GetType(&iidVarType));
                // we don't expect Proxy to have 'null' sources
                // so we will fail on this next method call
                ::get_source_of_type(
                    spBaseModelProxy,
                    iidVarType,
                    foundation::InstanceTypeOptions::AllowBaseType,  // don't allow inheritance
                    true,                                           // look for the 'class' type
                    spObject.GetAddressOf());
                // GetSourceOfType could return S_OK but a 'null' result
                // Note: we also could receive an E_NOINTERFACE if the underlying 'Source'
                // is not our Core class implementation or lives on another separate process
                if (spObject != nullptr)
                {
                    // the Var Type Info and 'Source' match the type
                    value = spObject;
                    return S_OK;
                }
            }
        }
        */
        // we will need to QI again since we will need to
        // extract the real 'Source' for Proxy Views created during runtime replacements
        // Note: Even that IProxyObject is an ISourceModel interface
        // we will QI again since the Property View Model Proxy has two entries
        // for ISourceModel, one coming from a direct call to IProxyObject and
        // another one from QI to ISourceModel
        foundation::ComPtr<ISourceModel> spSourceModel;
        IFR_ASSERT(foundation::QueryInterface(spBaseModelProxy, spSourceModel.GetAddressOf()));
        // Extract Source property
        IFR_ASSERT(spSourceModel->GetSource(spObject.GetAddressOf()));
        value = spObject;
    }
    return S_OK;
}

HRESULT CProxyObjectFactory::ToProxyObjectInternal(_In_ foundation::IObject* pObject, foundation::IObject** ppProxyObject)
{
    foundation_assert(pObject);
    foundation_assert(ppProxyObject);

    *ppProxyObject = nullptr;

    foundation::ComPtr<foundation::IObject> spProxyObject;
    if (LookupModelProxyInternal(pObject, spProxyObject))
    {
        return spProxyObject.CopyTo(ppProxyObject);
    }

    foundation::ComPtr<IObservableObject> spPropertyModel;
    foundation::ComPtr<ICommand> spCommandModel;
    foundation::ComPtr<IObservableCollection> spCollectionModel;
    foundation::ComPtr<foundation::IAsyncOperation> spAsyncOperation;
    
    if (SUCCEEDED(pObject->QueryInterface(ICommand::GetIID(), (void **)spCommandModel.GetAddressOf())))
    {
        return CreateObjectProxyInstanceInternalAndCache(ProxyType::Command, spCommandModel, ppProxyObject);
    }
    else if (SUCCEEDED(pObject->QueryInterface(IObservableCollection::GetIID(), (void **)spCollectionModel.GetAddressOf())))
    {
        return CreateObjectProxyInstanceInternalAndCache(ProxyType::ObservableCollection, spCollectionModel, ppProxyObject);
    }
    else if (SUCCEEDED(pObject->QueryInterface(foundation::IAsyncOperation::GetIID(), (void **)spAsyncOperation.GetAddressOf())))
    {
        return CreateObjectProxyInstanceInternalAndCache(ProxyType::AsyncOperation, spAsyncOperation, ppProxyObject);
    }
    else if (SUCCEEDED(pObject->QueryInterface(IObservableObject::GetIID(), (void **)spPropertyModel.GetAddressOf())))
    {
        return CreatePropertyModelProxyInstanceInternal(spPropertyModel, reinterpret_cast<IObservableObject **>(ppProxyObject));
    }
    else
    {
        foundation_assert(FALSE);
        // Unknown IObject Interface
        return E_FAIL;
    }
    return S_OK;
}

//HRESULT CProxyObjectFactory::QueryCreateAdapterInterfaceInternal(
//        _In_ IInspectable *pOuter,
//        _In_ IInspectable *pInner,
//        _In_ REFIID riid_outer,
//        _In_ REFIID riid,
//        _COM_Outptr_ IInspectable **ppModelAdapterInnerUnk)
//{
//    HRESULT hr = _ProxyObjectFactoryBaseType::QueryCreateAdapterInterfaceInternal(
//        pOuter,
//        pInner,
//        riid_outer,
//        riid,
//        ppModelAdapterInnerUnk);
//    if(hr == E_NOINTERFACE)
//    {
//        // the local proxy factory scope failed so let's try the global scope
//        hr = foundation::library_util::GetInterfaceAdapterFactoryHost()->QueryCreateInterfaceAdapter(pOuter,pInner,riid_outer,riid,ppModelAdapterInnerUnk);
//    }
//    return hr;
//}

//HRESULT CProxyObjectFactory::ToProxyValue(_Inout_ IInspectable **ppValue)
//{
//    foundation_assert(ppValue);
//    foundation_assert(*ppValue);
//
//    foundation::InspectablePtr value(*ppValue);
//    IFR(ToProxyValueInternal(value));
//    (*ppValue)->Release();
//    *ppValue = value.Detach();
//    return S_OK;
//}
//
//HRESULT CProxyObjectFactory::FromProxyValue(_Inout_ IInspectable **ppValue)
//{
//    foundation_assert(ppValue);
//    foundation_assert(*ppValue);
//
//    foundation::InspectablePtr value(*ppValue);
//    IFR(FromProxyValueInternal(nullptr,value));
//    (*ppValue)->Release();
//    *ppValue = value.Detach();
//    return S_OK;
//}
//
//HRESULT CProxyObjectFactory::GetPurgeableSize(_Out_ UINT32 *pValue)
//{
//    *pValue = (int)m_PurgeableModels.size();
//    return S_OK;
//}

STDMETHODIMP CProxyObjectFactory::OnTimeout(UINT64 timerId)
{
    UNREFERENCED_PARAMETER(timerId);

    foundation_assert(m_PurgeTimeoutId == timerId); 

    IFR(PurgeProxyObjects(-1,m_MaxPurgeTimestamp));
    // If Auto Purge is enabled but no Purge is available Kill the recurring timer
    if(m_AutoPurge && !IsPurgeAvailable())
    {
        KillPurgeTimer();
    }
    return S_OK;
}

//STDMETHODIMP CProxyObjectFactory::GetDispatcher(_COM_Outptr_ foundation::library::IDispatcher **ppDispatcher)
//{
//    IFCPTR(ppDispatcher);
//    return m_spDispatcher.CopyTo(ppDispatcher);
//}
//
//STDMETHODIMP CProxyObjectFactory::SetDispatcher(_In_ foundation::library::IDispatcher *pDispatcher)
//{
//    m_spDispatcher = pDispatcher;
//    return S_OK;
//}
//
//STDMETHODIMP CProxyObjectFactory::GetEventDispatcher(_COM_Outptr_ foundation::library::IDispatcher **ppEventDispatcher)
//{
//    IFCPTR(ppEventDispatcher);
//    return m_spEventDispatcher.CopyTo(ppEventDispatcher);
//}
//
//STDMETHODIMP CProxyObjectFactory::SetEventDispatcher(_In_ foundation::library::IDispatcher *pEventDispatcher)
//{
//    m_spEventDispatcher = pEventDispatcher;
//    return S_OK;
//}

bool CProxyObjectFactory::IsInvokeDispatcherRequired()
{
    bool bValue = false;

    if(m_spDispatcher != nullptr)
    {
        bool hasThreadAccess = true;
        IGNOREHR(m_spDispatcher->HasThreadAccess(&hasThreadAccess));
        bValue = !hasThreadAccess;
    }
    return bValue;
}

HRESULT CProxyObjectFactory::InvokeUsingDispatcher(foundation::library::IDelegateDispatchable *pDispatchable)
{
    foundation_assert(m_spDispatcher != nullptr);
    foundation::ComPtr<foundation::IAsyncOperation> spAsyncAction;
    return m_spDispatcher->RunAsync(pDispatchable,spAsyncAction.GetAddressOf());
}

//STDMETHODIMP CProxyObjectFactory::CreateObjectFactoryProxy(
//        foundation::IObjectFactoryBase *pModelFactoryBase,
//        foundation::IObjectFactoryBase **ppProxyObjectFactoryBase)
//{
//    IFCPTR(pModelFactoryBase);
//    IFCPTR(ppProxyObjectFactoryBase);
//
//    return CObjectFactoryProxy::CreateInstance(pModelFactoryBase,this,ppProxyObjectFactoryBase);
//}

