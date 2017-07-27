/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationBase.h
****/
#pragma once

#include <set>
#include "CriticalSection.h"
#include "EventSourceBase.h"
#include <foundation/library/base_adapter_host.h>
#include <foundation/library/class_options.h>
#include <foundation/library/interfaces/event_source_class.h>
#include <foundation/library/foundation_lib_api.h>
#include <foundation/ctl/composable_weak_reference_source.h>

#include "../foundation/FoundationTracker.h"

using namespace foundation_lib;

EXTERN_C FOUNDATION_LIB_API const GUID IID_IBaseModelProxyPrivate;

//------------------------------------------------------------------------------
// Class:   _FoundationBase
//
// Purpose: Base template class for all our created objects
//
//------------------------------------------------------------------------------
template <class Interface,
            class InterfaceClass,
            class TDelegate,
            class TInitializeParameter,
            class TEventSource,
            class TEventHandler,
            class TSender,
            class TEventArgs,
            bool isTrackingEnabled = false,
            class TBASE = foundation::ctl::ComposableWeakRefSourceInspectableBase
        >
class _FoundationBase :
    public Interface,
    public InterfaceClass,
    public _EventSourceBase<TEventSource, TEventHandler, TSender, TEventArgs, _FoundationBase<Interface, InterfaceClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, isTrackingEnabled, TBASE>, foundation::library::ICriticalSection>,
    public foundation::library::_FireEventHandlerContainerWithCacheImpl<_FoundationBase<Interface, InterfaceClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, isTrackingEnabled, TBASE>>,
    public foundation::library::_BaseAdapterHost<Interface, TBASE, _FoundationBase<Interface, InterfaceClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, isTrackingEnabled, TBASE> >
#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
    ,
    public _FoundationTrackerBase<isTrackingEnabled>
#endif
{
public:
    typedef _FoundationBase<Interface, InterfaceClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, isTrackingEnabled, TBASE> _BaseT;
    typedef CFoundationLock<_BaseT> _FoundationBaseLock;

    HRESULT _Initialize(const TInitializeParameter *modelImpl)
    {
#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
        if (IsOptionEnabled(modelImpl->m_options, foundation::library::FoundationClassOptions_NoTracking))
        {
            GetFoundationTracker()->RemoveInstance(this);
        }
#endif
        HRESULT hr;

        this->m_Options = (UINT16)modelImpl->m_options;
        // initialize 'm_IsContinueOnFireEventError' property
        m_IsContinueOnFireEventError = IsOptionEnabled(modelImpl->m_options, foundation::library::FoundationClassOptions_ContinueOnFireEventError);
        
        // if 'QuirksMode' mode
        m_UseQuirksMode = IsOptionEnabled(modelImpl->m_options, foundation::library::FoundationClassOptions_UseQuirksMode);
        
        // if 'MultiThreaded' support
        bool isFreeThreaded = IsOptionEnabled(modelImpl->m_options, foundation::library::FoundationClassOptions_IsFreeThreadApartment);
        if(isFreeThreaded)
        {
            // Create the 'shared' protection section impl
            IFHR_ASSERT(foundation::ctl::CreateInstanceWithInitialize(&m_pCritsec));
        }
        // Weak reference source support
        this->_InitializeComposableWeakReferenceSource(
            !isFreeThreaded,
            IsOptionEnabled(modelImpl->m_options, foundation::library::FoundationClassOptions_ComposedWeakReference)
            );

        m_IsDelegateWeakRef = IsOptionEnabled(modelImpl->m_options, foundation::library::FoundationClassOptions_IsDelegateWeakRef);
        m_pDelegate = modelImpl->m_p_delegate;
        if (!m_IsDelegateWeakRef)
        {
            ADDREF_INTERFACE(m_pDelegate);
        }
        return S_OK;
    }

    // This method will be called by the Factory Creator
    void _BeginInitialize()
    {
        m_IsInitialize = true;
    }

    void _EndInitialize()
    {
        m_IsInitialize = false;
    }

    // TNotifyEventHandlerCallback methods
    virtual void _OnAddHandler(_In_ TEventHandler* pHandler)
    {
        if (m_pDelegate)
        {
            m_pDelegate->OnAddEventHandler(pHandler);
        }
        OnAddHandlerInternal();
    }

    virtual void _OnRemoveHandler(_In_ TEventHandler* pHandler)
    {
        if (m_pDelegate)
        {
            m_pDelegate->OnRemoveEventHandler(pHandler);
        }
        OnRemoveHandlerInternal();
    }

      // IEventSourceClass Interface
      HRESULT _AddEventHandlerInternal(
          _In_ GUID eventSourceType,
          _In_opt_ foundation::IUnknown *pEventHandler)
      {
        _FoundationBaseLock csl(*this);

          if (eventSourceType == TEventSource::GetIID())
          {
              if (pEventHandler != nullptr)
              {
                  foundation::ComPtr<TEventSource> spEventSource;
                  IFR_ASSERT(this->GetEventSource(spEventSource.GetAddressOf()));
                  foundation::ComPtr<TEventHandler> spEventHandler;
                  IFR_ASSERT(foundation::QueryInterface(pEventHandler, spEventHandler.GetAddressOf()));
                  IFR_ASSERT(spEventSource->AddHandler(spEventHandler));
              }
          }
          else
          {
              return E_NOTIMPL;
          }
          return S_OK;
      }

      HRESULT _RemoveEventHandlerInternal(
          _In_ GUID eventSourceType,
          _In_ foundation::IUnknown *pEventHandler)
      {
          if (eventSourceType == TEventSource::GetIID())
          {
              foundation::ComPtr<TEventSource> spEventSource;
              IFR_ASSERT(this->GetEventSource(spEventSource.GetAddressOf()));
              foundation::ComPtr<TEventHandler> spEventHandler;
              IFR_ASSERT(foundation::QueryInterface(pEventHandler, spEventHandler.GetAddressOf()));
              IFR_ASSERT(spEventSource->RemoveHandler(spEventHandler));
          }
          else
          {
              return E_NOTIMPL;
          }
          return S_OK;
      }

      // Invoke when an Event is being fired by the 'Core' instance
      HRESULT _OnCustomFireEvent(
          _In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
          _In_ foundation::IUnknown *pEventArgs)
      {
          // make sure our Model Adapters are Fired too for every invocation of FireEevnt Support
          IFR_ASSERT(GetInterfaceAdapterHost()->FireAdaptersEventArgs(
              pFireEventHandlerContainer,
              pEventArgs));
          return S_OK;
      }

      bool _IsContinueOnFireEventError()
      {
          return m_IsContinueOnFireEventError;
      }

      bool _IsEventSupportEnabled()
      {
          return true;
      }

      virtual foundation::library::IDispatcher *_GetEventDispatcher()
      {
          return m_spEventDispatcher;
      }

      inline bool _IsFreeThreaded()
      {
          return IsFreeThreadApartment();
      }

      // Fires on our 'Foundation' Event Handler Container
      HRESULT FireEvent(
          _In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
          _In_ TEventArgs *pEventArgs)
      {
          foundation_assert(pEventArgs);
          // Fire the Event Args trough the Event Source
          if (this->GetCountHandlerStorage())
          {
              IFR_ASSERT(this->GetEventSourceImpl()->FireEvent(
                  pEventHandlerContainer,
                  static_cast<Interface*>(this),
                  pEventArgs));
          }
          return S_OK;
      }

protected:
    friend class foundation::library::_BaseAdapterHost<Interface, TBASE, _BaseT>;

    _FoundationBase():
        m_IsInitialize(false),
        m_IsObserved(false),
        m_IsFinalRelease(false),
        m_UseQuirksMode(false),
        m_IsDelegateWeakRef(false),
        m_IsContinueOnFireEventError(false),
        m_pModelAdapterHost(nullptr),
        m_Options(0),
        m_pDelegate(nullptr),
        m_pCritsec(nullptr)
    {
    }

    UINT16& GetOptions() 
    {
        return m_Options;
    }

    HRESULT EnsureEventSourceModel()
    {
        _FoundationBaseLock csl(*this);

        if (!this->IsEventHandlerSource())
        {
            // Initialize our Event Source Model with our critical section
            IFR_ASSERT(InitializeEventSourceModel());
        }
        return S_OK;
    }

    bool IsUseQuirksMode()
    {
        return m_UseQuirksMode;
    }

    bool IsInitialize() 
    {
        return m_IsInitialize;
    }

    bool IsObserved()
    {
        return m_IsObserved;
    }   

    bool IsFinalRelease() 
    {
        return m_IsFinalRelease;
    }

    bool IsDelegateAvailable()
    {
        return !((m_IsInitialize && !m_UseQuirksMode) || m_IsFinalRelease);
    }

    bool IsDelegateWeakRef()
    {
        return m_IsDelegateWeakRef;
    }
    
    Interface *AsInterface()
    {
        return static_cast<Interface *>(this);
    }

    foundation::IInspectable *AsInspectable()
    {
        return foundation::ctl::inspectable::CastToInspectable(this);
    }

    InterfaceClass *AsInterfaceClass()
    {
        return static_cast<InterfaceClass *>(this);
    }

    virtual HRESULT InitializeEventSourceModel()
    {
       // _EventHandlerStorageT *pT = nullptr;
        return this->_CreateEventSource(this,this->m_pCritsec);
    }

    virtual HRESULT GetEventSource(_COM_Outptr_ TEventSource **ppEventSource)
    {
        foundation_assert(false);
        *ppEventSource = nullptr;
        return E_NOTIMPL;
    }

    bool HasAdapterEventHandlers()
    {
        bool bHasModelAdapterHandlers = false;
        IGNOREHR(GetInterfaceAdapterHost()->HasAnyAdapterEventHandlers(&bHasModelAdapterHandlers));
        return bHasModelAdapterHandlers;
    }

    bool HasEventHandlers()
    {
        return HasEventHandlersInternal() || HasAdapterEventHandlers();
    }

    // Only 'Internal' handlers excluding Adapters
    inline bool HasEventHandlersInternal()
    {
        return this->GetCountHandlerStorage() > 0;
    }

    virtual size_t GetCountEventHandlers()
    {
        // Count 'Core' event handlers
        return this->GetCountHandlerStorage();
    }

    virtual void OnFinalRelease()
    {
    }

    virtual HRESULT _QueryCreateAdapterInterfaceImpl(
        foundation::IInspectable* pOuter,
        foundation::IInspectable* pInner,
        REFIID iid,
        REFIID iidHostType,
        foundation::IInspectable **ppModeAdapterNewInstance)
    {
        return foundation::library::_AdapterHostResolverBase::_QueryCreateAdapterInterfaceImpl(
            pOuter,
            pInner,
            iid,
            iidHostType,
            ppModeAdapterNewInstance);
    }

    virtual IID _GetHostInterfaceType(REFIID iid)
    {
        UNREFERENCED_PARAMETER(iid);
        return Interface::GetIID();
    }

#if defined(DEBUG)
    ~_FoundationBase()
    {
        foundation_assert(m_IsFinalRelease);
    }
#endif

    // Overrides from ComInspectableBase
    void FinalRelease() override
    {
        // release critical section
        if (m_pCritsec != nullptr)
        {
            m_pCritsec->CastToUnknown()->Release();
            m_pCritsec = nullptr;
        }
        if (!m_IsDelegateWeakRef)
        {
            RELEASE_INTERFACE(m_pDelegate);
        }
        // Clean Weak Reference Source
        TBASE::FinalRelease();
        // Prepare for Final Release
        m_IsFinalRelease = true;
        // Invoke Virtual method to be override by base classes
        OnFinalRelease();
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == Interface::GetIID())
        {
            *ppInterface = static_cast<Interface *>(this);
        }
        else if (iid == InterfaceClass::GetIID())
        {
            *ppInterface = static_cast<InterfaceClass *>(this);
        }
        else if (iid == foundation::library::IDisposable::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IDisposable *>(this);
        }
        else if (iid == foundation::library::IFireEventHandlerContainer::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IFireEventHandlerContainer *>(this);
        }
        else if (iid == foundation::library::IFoundationClassBase::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IFoundationClassBase *>(this);
        }
        else if(
            iid == foundation_IID_IPropertyValue || 
            iid == IID_IBaseModelProxyPrivate)
        {
            // this will skip any attempt to ask for any Adapter
            return E_NOINTERFACE;
        }
        else
        {
            return foundation::library::_BaseAdapterHost<Interface, TBASE, _BaseT>::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
    template <
        class TLocker,
        class T,
        class _TEventArgs
    >
    HRESULT FireWithCallback(
        TLocker& csl,
        _In_ HRESULT (T::*_FireEventDelegate)(foundation::library::IFireEventHandlerContainer *, _TEventArgs *),
        _In_ _TEventArgs *pEventArgs)
    {
        // During Initialization there should be no Event Handler in the 'Core' or 
        // Model Adapters
        // Also during FinalRelease we should not allow any Fire Event since it could potentially add 
        // unwanted references
        if(IsInitialize() || IsFinalRelease())
        {
            return S_FALSE;
        }

        // Note: the lock will be called by our base class
        // to have more control when can we unlock to avoid contention
        return foundation::library::_FireEventSupportUtil::_FireEventWithCallback(
            csl,
            static_cast<T *>(this),
            _FireEventDelegate,
            pEventArgs);
    }

    template <
        class T,
        class _TEventArgs
    >
    inline HRESULT FireWithCallback(
    _In_ HRESULT(T::*_FireEventDelegate)(foundation::library::IFireEventHandlerContainer *, _TEventArgs *),
    _In_ _TEventArgs *pEventArgs)
    {
        _FoundationBaseLock csl(*this);
        return FireWithCallback(csl, _FireEventDelegate, pEventArgs);
    }

    inline bool IsFreeThreadApartment()
    {
        return this->m_pCritsec != nullptr;
    }

    foundation::library::ICriticalSection *GetCriticalSectionInterface()
    {
        return static_cast<foundation::library::ICriticalSection *>(this->m_pCritsec);
    }

    // ICoreClassBase Interface
    STDMETHOD(CountEventHandlers)(_Out_ UINT32 *pValue)
    {     
        *pValue = (UINT32)GetCountEventHandlers();
        return S_OK;
    }

    STDMETHOD(GetIsObserved)(_Out_ bool *pValue)
    {
        *pValue = m_IsObserved;
        return S_OK;
    }
    STDMETHOD(GetFoundationOptions)(_Out_ UINT16 *pValue)
    {
        *pValue = this->m_Options;
        return S_OK;
    }

    STDMETHOD(GetEventDispatcher)(_COM_Outptr_result_maybenull_ foundation::library::IDispatcher **ppEventDispatcher)
    {
        IFCPTR_ASSERT(ppEventDispatcher);
        return m_spEventDispatcher.CopyTo(ppEventDispatcher);
    }

    STDMETHOD(SetEventDispatcher)(_In_opt_ foundation::library::IDispatcher *pEventDispatcher)
    {
        m_spEventDispatcher = pEventDispatcher;
        return S_OK;
    }
    STDMETHOD(GetCriticalSection)(_COM_Outptr_result_maybenull_ foundation::library::ICriticalSection **pp_CriticalSection)
    {
        IFCPTR_ASSERT(pp_CriticalSection);
        *pp_CriticalSection = GetCriticalSectionInterface();
        if (*pp_CriticalSection != nullptr)
        {
            (*pp_CriticalSection)->AddRef();
        }
        return S_OK;
    }

    // IDisposable Interface
    STDMETHOD(Dispose)()
    {
        return DisposeInternal();
    }

    // Override IInterfaceAdapterHost
    STDMETHOD(IsObserved)(bool *bValue) override
    {
        *bValue = this->m_IsObserved;
        return S_OK;
    }

    // Override IInterfaceAdapterOverrides
    STDMETHOD (OnAdapterAddHandler)(_In_ foundation::IUnknown* pSender,_In_ foundation::IUnknown* pHandler) override
    {
        UNREFERENCED_PARAMETER(pSender);
        UNREFERENCED_PARAMETER(pHandler);

        this->OnAddHandlerInternal();
        return S_OK;
    }

    STDMETHOD (OnAdapterRemoveHandler)(_In_ foundation::IUnknown* pSender,_In_ foundation::IUnknown* pHandler) override
    {
        UNREFERENCED_PARAMETER(pSender);
        UNREFERENCED_PARAMETER(pHandler);

        this->OnRemoveHandlerInternal();
        return S_OK;
    }
     
    class CRefCountHelper
    {
    public:
        CRefCountHelper(_BaseT *pThis):
            _pThis(pThis)
        {
            foundation_assert(pThis);
            foundation::ctl::inspectable::CastToInspectable(pThis)->AddRef();
        }
        ~CRefCountHelper()
        {
            foundation::ctl::inspectable::CastToInspectable(_pThis)->Release();
        }
    private:
        _BaseT *_pThis;
    };

protected:
    virtual HRESULT DisposeInternal()
    {
        // Override this Base behavior when needed
        return S_OK;
    }

    virtual void OnIsObservedChanged(bool newValue)
    {
        UNREFERENCED_PARAMETER(newValue);
    }

    // return true is our composed host object is being observed
    bool IsComposedObserved()
    {
        bool isObserved = false;
        if (this->IsComposed() &&
            this->GetInterfaceAdapterHost() != this->GetSelfModelAdapterHost())
        {
            IGNOREHR(GetInterfaceAdapterHost()->IsObserved(&isObserved));
        }
        return isObserved;
    }

    virtual bool IsObservedInternal()
    {
        return HasEventHandlers() ||
            IsComposedObserved();
    }

public:
    virtual void OnAddHandlerInternal()
    {
        EvaluateIsObservedChanged(true);
    }

    virtual void OnRemoveHandlerInternal()
    {
        EvaluateIsObservedChanged();
    }

    HRESULT Lock(){
        if(m_pCritsec != nullptr)
        {
            return m_pCritsec->Lock();
        }
        return S_OK;
    }
    HRESULT Unlock()
    {
        if (m_pCritsec != nullptr)
        {
            return m_pCritsec->Unlock();
        }
        return S_OK;
    }

    void EvaluateIsObservedChanged()
    {
        return EvaluateIsObservedChanged(IsObservedInternal());
    }
private:
inline foundation::library::IInterfaceAdapterHost *GetSelfModelAdapterHost()
    {
        return static_cast<foundation::library::IInterfaceAdapterHost *>(this);
    }
    foundation::library::IInterfaceAdapterHost *GetInterfaceAdapterHost()
    {
        if(this->IsInitialize() || !this->IsComposed() )
        {
            return this->GetSelfModelAdapterHost();
        }
        // cache the Composed Adapter Host
        if(!m_pModelAdapterHost)
        {
            // ask Outer object for the interface if
            foundation::QueryWeakReference(
                this->AsInspectable(),
                &m_pModelAdapterHost);
            if(m_pModelAdapterHost == nullptr)
            {
                // this mean the Outer does not support or during construction
                // phase it still does not have the Inner pointer assigned
                return this->GetSelfModelAdapterHost();
            }
        }
        return m_pModelAdapterHost;
    }

    void EvaluateIsObservedChanged(bool isObserved)
    {
        if(isObserved != m_IsObserved)
        {
            m_IsObserved = isObserved;
            // Invoke overrideable method to base classes
            this->OnIsObservedChanged(isObserved);
            if(m_pDelegate && this->IsDelegateAvailable())
            {
                if(isObserved)
                {
                    IGNOREHR(m_pDelegate->OnStartObserve());
                }
                else
                {
                    IGNOREHR(m_pDelegate->OnStopObserve());
                }
            }
            // ensure _InterfaceAdapterHostBase class is aware
            this->FireOnObservedChanged(isObserved);
        }
    }
private:
    bool m_IsInitialize:1;
    bool m_IsMultiThreadSupport:1;
    bool m_IsObserved:1;
    bool m_IsFinalRelease:1;
    bool m_UseQuirksMode:1;
    bool m_IsDelegateWeakRef : 1;
    bool m_IsContinueOnFireEventError : 1;

    UINT16 m_Options;
protected:
    TDelegate											*m_pDelegate;
    CCriticalSectionImpl								*m_pCritsec;
    foundation::library::IInterfaceAdapterHost          *m_pModelAdapterHost;
    foundation::ComPtr<foundation::library::IDispatcher>  m_spEventDispatcher;
};
