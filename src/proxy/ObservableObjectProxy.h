/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectProxy.h
****/

#pragma once

#include "TBaseModelProxy.h"
#include "../model_library/BatchPropertyChangedHelper.h"
#include "../foundation_library/EventHandler.h"

#include <pmod/event_connection.h>
#include <foundation/library/interfaces/fast_properties_container.h>

#include <map>

struct IObjectDispatchInfoInternalClass;

typedef PropertyChangedEventHandlerBase
<
	TNotifyEventSourceImpl
    <
    pmod::IPropertyChangedEventSource,
    pmod::IPropertyChangedEventHandler,
    foundation::IUnknown,
    pmod::IPropertyChangedEventArgs,
    BaseObjectProxy,
	foundation::library::ICriticalSection
    >
>
ProxyPropertyChangedEventHandler;

//------------------------------------------------------------------------------
// Class:   CObservableObjectProxy
//
// Purpose: Define a base class for all the proxy generated classes using IPropertyModel Interface
//
//------------------------------------------------------------------------------
typedef TBaseModelProxy
    <
        pmod::IObservableObject,
        pmod::IPropertyChangedEventSource,
        pmod::IPropertyChangedEventHandler,
        pmod::IPropertyChangedEventArgs,
        pmod::proxy::IObservableObjectProxyDelegate,
        ProxyPropertyChangedEventHandler
    > _PropertyModelProxyBaseType;

class CObservableObjectProxy:
    public _PropertyModelProxyBaseType,
//    public pmod::property_model_wrapper_type<CObservableObjectProxy>,
    public foundation::IDictionary
  //  protected INotifyEventHandlerDelegate<pmod::IEventModelEventHandler>
{
public:
    HRESULT _Initialize(
        _In_ UINT32 proxyUniqueId,
        _In_ pmod::IObservableObject *pSourceObject,
        _In_opt_ pmod::proxy::IProxyObjectDelegate *pProxyModelDelegateBase,
        _In_ CProxyObjectFactory *pProxyFactory,
        _In_ bool IsFreeThreadedEnabled);

protected:

    CObservableObjectProxy():
         m_pCachedPropertyModelInfoClass(nullptr),
         m_pEventModelProxyDelegate(nullptr),
         _isViewModelTypeInfo(false)
    {
    }
public:

    // IObjectNode Interface
    STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue);

    // IPropertyModel Interface
    STDMETHOD( GetProperty )(UINT32 propertyId, IInspectable** value) override;
    STDMETHOD( SetProperty )(UINT32 propertyId,IInspectable *value) override;
    STDMETHOD(GetProperties)(foundation::IInspectable* pProperties, UINT32* size, foundation::IPropertyIdPair ***ppProperties) override;

    STDMETHOD( GetPropertyChangedEventSource )(_COM_Outptr_ pmod::IPropertyChangedEventSource** ppEventSource) override;
    STDMETHOD( GetEventModelEventSource )(_COM_Outptr_ pmod::IEventModelEventSource** ppEventSource) override;
    STDMETHOD( InvokeMethod ) (UINT32 methodId,_In_ UINT32 size,IInspectable **parameters,IInspectable **ppResult) override;

    // IDictionary Interface
    STDMETHOD(GetView)(UINT32* size,foundation::IDictionaryPair ***ppProperties);
    STDMETHOD(GetSize)(_Outptr_ UINT32* size) override;
    STDMETHOD(Lookup)(HSTRING propertyName, IInspectable** value) override;
    STDMETHOD(HasKey)(HSTRING propertyName, bool* found) override;

    STDMETHOD(Insert)(HSTRING propertyName, IInspectable *value,bool* replaced) override;
    STDMETHOD(Remove)(HSTRING propertyName) override;
    STDMETHOD(Clear)() override;
    STDMETHOD(Keys)(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first) override;

    // Methods needed from _PropertyModelWrapper
    HRESULT SetPropertyValue(_In_ UINT32 propertyId,_In_ IInspectable *pInspectable);
    HRESULT GetPropertyValue(_In_ UINT32 propertyId,_In_ IInspectable **ppInspectable);

    friend class CProxyObjectFactory;
    friend class CProxyPropertyChangedEventArgs;
protected:

    // Overrides from BaseModelSource
    void SetIsAttached(bool isAttached) override;
    HRESULT on_invoke_internal(foundation::IUnknown* pSender,pmod::IPropertyChangedEventArgs* pArgs) override;
   
    // Support for IBatchPropertyChangedEventHandler Delegate
    HRESULT InvokeInternal(foundation::IUnknown* pSender,pmod::IBatchPropertyChangedEventArgs* pArgs);
    HRESULT OnSourceBatchPropertyChanged(foundation::IUnknown* pSender,pmod::IBatchPropertyChangedEventArgs* pArgs);

    // Overrides from EventSourceBase
    HRESULT CreateSourceEventHandlerDelegate(
        bool isFreeThreaded,
        pmod::IPropertyChangedEventHandler **ppEventHandlerDelegate) override;

    // Pure Virtual Methods Override
    HRESULT GetSourceEventSource(pmod::IPropertyChangedEventSource **ppEventSource) override;

    // Interface IBaseObjectProxyInternal Overrides
    STDMETHOD(Dispose)()  override;
    STDMETHOD(ConnectToSource)(foundation::IInspectable *pSourceInfo, _SyncSourceInfo_Vector_Type& syncSourceInfo) override;
    STDMETHOD(DisconnectFromSource)(foundation::IInspectable **ppSourceInfo) override;
    STDMETHOD(ResyncSource)(foundation::IInspectable *pSource, _SyncSourceInfo_Vector_Type& syncSourceInfo) override;
    STDMETHOD(FireResyncEvent)(foundation::IUnknown *pEventArg) override;

    // Overrides from ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;
    void FinalRelease() override;

    // Overrides from BaseModelProxy
    bool IsProxyModelCacheEnabled();
    size_t GetProxyCountEventHandlers() override;
    foundation::ILogCategory *GetLogCategory() override;
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    UINT32 GetTrackRefCountOption() override { return _OptionTrackRefCountPropertyModelProxies; }
#endif

    void InvalidatePropertiesCache();
    HRESULT RestoreDisconnectedProperties(
        foundation::InspectableArrayWrapper& propertiesIdPair,
        _SyncSourceInfo_Vector_Type& syncSourceInfo
        );

    // Standard IPropertyChangedEventArgs notification
    HRESULT FirePropertyChangedEvent(_In_ pmod::IPropertyChangedEventArgs* pEventArgs);
    HRESULT PerformFirePropertyChangedEventArgs(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ pmod::IPropertyChangedEventArgs *pEventArgs);

    // Batch IBatchPropertyChangedEventArgs notification
    HRESULT FirePropertyChangedEvent(_In_ pmod::IBatchPropertyChangedEventArgs* pEventArgs);
    HRESULT PerformBatchFirePropertyChangedEventArgs(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ pmod::IBatchPropertyChangedEventArgs *pEventArgs);

    HRESULT InvokeMethodInternal(UINT32 methodId,_In_ UINT32 size,IInspectable **parameters,IInspectable **ppResult);

    HRESULT GetProxyModelInstance(_In_ UINT32 propertyId,_In_ REFIID riid,void** pValue);
    HRESULT GetModelCommandInstance(_In_ UINT32 propertyId,pmod::ICommand** pValue);
    HRESULT GetModelCollectionInstance(_In_ UINT32 propertyId,pmod::IObservableCollection** pValue);

    foundation::PropertyType GetPropertyType(_In_ UINT32 propertyId);
    bool IsProxyValueConversionNeeded(_In_ UINT32 propertyId);

    bool CanCacheProperty(_In_ UINT32 propertyId);
    bool CanStrongRefCacheProperty(_In_ UINT32 propertyId);
    HRESULT EnsureObservableObjectInfoClass();
    HRESULT EnsureObservableObjectInfo();

    HRESULT CreateProxyPropertyChangedEventArgs(
        _In_ pmod::IPropertyChangedEventArgs* pArgs,
        _COM_Outptr_ pmod::IPropertyChangedEventArgs** ppPropertyChangedEventArgs);

    HRESULT on_source_property_changed_internal(_In_ pmod::IPropertyChangedEventArgs *pProxyPropertyChangedEventArgs);

    // Convert to a Proxy a Property accessed value
    virtual HRESULT ToPropertyProxyValue(_In_ UINT32 propertyId,_Inout_ foundation::InspectablePtr& value);

    typedef BatchPropertyChangedEventHandlerDelegateBase<
        CObservableObjectProxy,
        _EventHandlerDelegate
    > PropertyChangedEventHandlerDelegate;

    friend class TBatchPropertyChangedEventInvoker;
    friend  class TBatchPropertyChangedEventWeakRefInvoker;

    typedef TNotifyEventSourceImpl
    <
        pmod::IEventModelEventSource,
        pmod::IEventModelEventHandler,
        foundation::IUnknown,
        pmod::IEventModelEventArgs,
		BaseObjectProxy,
		foundation::library::ICriticalSection
    > _EventModelProxyEventHandler;

    typedef pmod::EventModelConnection::_DelegateFactory<CObservableObjectProxy> CEventModelProxyConnection;

    // raw entry point from 'Source' event model
    HRESULT OnSourceEventModel(foundation::IUnknown *pSender, pmod::IEventModelEventArgs *pEventArgs);
    // safe entry point for event model
    HRESULT on_invoke_internal(foundation::IUnknown* pSender, pmod::IEventModelEventArgs* pArgs);

    // Fire support for Event model
    HRESULT FireEventModelEvent(_In_ pmod::IEventModelEventArgs* pEventArgs);
    HRESULT PerformFireEventModelEventArgs(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ pmod::IEventModelEventArgs *pEventArgs);

    // attach/detach from Event Model Source
    HRESULT AttachToEventModelSource();
    HRESULT DetachFromEventModelSource();

    // Override from TBaseModelProxy
    HRESULT AttachToSource() override;
    HRESULT DetachFromSource() override;

private:
    HRESULT EnsureCachedFastPropertiesContainer();
    HRESULT CacheSerializedValues();
    HRESULT GetCachedPropertyValue(_In_ UINT32 propertyId, _In_ IInspectable **ppInspectable);
    HRESULT SetCachedPropertyValue(_In_ UINT32 propertyId, _In_ IInspectable *pInspectable);
    HRESULT SetAndGetCachedPropertyValue(_In_ UINT32 propertyId, foundation::InspectablePtr& value);
    HRESULT InvalidateCachedProperty(_In_ UINT32 propertyId);
    HRESULT OnGetSourceProperty(
        UINT32 propertyId, 
        foundation::InspectablePtr& value, 
        bool canCacheProperty);

    HRESULT GetPropertyContainerValue(
        _In_ UINT32 propertyId,
        _In_ IInspectable **ppInspectable,
        _In_ UINT8 options);
    HRESULT SetPropertyContainerValue(
        _In_ UINT32 propertyId,
        _In_ IInspectable *pInspectable,
        _In_ UINT8 options);

    bool IsPropertyInCacheContainer(_In_ UINT32 propertyId);
    bool IsReferencePropertyEnabled(_In_ UINT32 propertyId);
    bool IsDictionary();

    HRESULT GetPropertyId(_In_ HSTRING propertyName, _Outptr_ UINT32 *propertyId);

    // Mark a 'cache' property as Refresh only
    HRESULT SetReferencePropertyValue(_In_ UINT32 propertyId, _In_ IInspectable *pInspectable);

    HRESULT GetCachedPropertyOptions(_In_ UINT32 propertyId, UINT8& cachedOptions);
    HRESULT SetCachedPropertyOptions(_In_ UINT32 propertyId, UINT8 cachedOptions);

    HRESULT GetCachedPropertiesWithOptions(
        foundation::library::IFastPropertiesContainer *pFastPropertiesContainer,
        _In_ UINT32 mask, 
        std::vector<UINT32>& cachedProperties);


    HRESULT GetPropertyContainerValue(
		foundation::library::IFastPropertiesContainer *pFastPropertiesContainer,
        _In_ UINT32 propertyId,
        _In_ IInspectable **ppInspectable,
        _In_ UINT8 options);

    static HRESULT GetCachedPropertyOptions(
		foundation::library::IFastPropertiesContainer *pFastPropertiesContainer,
        _In_ UINT32 propertyId,
        UINT8& cachedOptions
        );
    static HRESULT SetCachedPropertyOptions(
		foundation::library::IFastPropertiesContainer *pFastPropertiesContainer,
        _In_ UINT32 propertyId,
        UINT8 cachedOptions
        );
private:
    foundation::ComPtr<foundation::library::IFastPropertiesContainer> _spCachedFastPropertiesContainer;
    // Property Info related fields
    foundation::ComPtr<pmod::IObservableObjectInfo>     _spCachedObservableObjectInfo;
	IObjectDispatchInfoInternalClass                      *m_pCachedPropertyModelInfoClass;
    bool                                                         _isViewModelTypeInfo:1;

    pmod::IEventModelEventHandler                  *m_pEventModelProxyDelegate;
    foundation::ComPtr<foundation::ctl::ComObject<_EventModelProxyEventHandler> > m_spEventModelProxyEventHandler;

};

