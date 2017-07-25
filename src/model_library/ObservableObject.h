/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObject.h
****/
#pragma once

#include "ObjectNode.h"
#include "BatchPropertyChangedHelper.h"
#include "ObjectDispatchDictionaryAdapter.h"

#include <memory>
#include <map>
//from Public
#include <pmod/event_connection.h>

// from model API
#include "../model/ObservableObjectInfo.h"

class CObservableObjectInfo;
class CObservableObject;

typedef ObjectDispatchDictionaryAdapter<pmod::library::IObservableObjectClass, CObservableObject> CObservableObjectDictionaryAdapter;

namespace pmod {
	namespace library {
	class BatchPropertyChangedEventArgsImpl;
}}

// Our Logger Category Factory
class CPropertyModelLogFactory : public _ObjectBaseLogFactory<>
{
public:
    static const CLogCategory*GetLogCategory();
};
//------------------------------------------------------------------------------
// Class:   CObservableObject
//
// Purpose: Implementation of IObservableObject Interface
//
//------------------------------------------------------------------------------
typedef ObjectNode
<
	pmod::IObservableObject,
	_CObservableObjectInfoBase,
	pmod::library::IObservableObjectClass,
	pmod::library::IObservableObjectDelegate,
	pmod::library::ObservableObjectCreateParameters,
	pmod::IPropertyChangedEventSource,
	pmod::IPropertyChangedEventHandler,
	foundation::IUnknown,
	pmod::IPropertyChangedEventArgs,
	CPropertyModelLogFactory
>
_ObservableObjectBase_Type;

class CObservableObject :
    public _ObservableObjectBase_Type,
    protected _EventSourceBase<
		pmod::IEventModelEventSource,
		pmod::IEventModelEventHandler,
		foundation::IUnknown,
		pmod::IEventModelEventArgs,
		CObservableObject,
		foundation::library::ICriticalSection>
	{
 public:

	 typedef _EventSourceBase<
		 pmod::IEventModelEventSource,
		 pmod::IEventModelEventHandler,
		 foundation::IUnknown,
		 pmod::IEventModelEventArgs,
		 CObservableObject,
		 foundation::library::ICriticalSection>
		 _EventModelSourceType;

	 HRESULT _Initialize(const pmod::library::ObservableObjectCreateParameters *modelImpl);

	 void _OnAddHandler(_In_ pmod::IEventModelEventHandler* pHandler)
	 {
		 _ObservableObjectBase_Type::OnAddHandlerInternal();
	 }

	 void _OnRemoveHandler(_In_ pmod::IEventModelEventHandler* pHandler)
	 {
		 _ObservableObjectBase_Type::OnRemoveHandlerInternal();
	 }

	 // ObjectNode Overrides
	 HRESULT OnFireNotifyInModelChangesChanged(bool newValue) override
	 {
		 UNREFERENCED_PARAMETER(newValue);
		 return FirePropertyChanged(pmod::Property_IsInModelChanges, false);
	 }
	 HRESULT OnBeginModelChangesInternal(pmod::library::BatchModelChangesOptions options)  override;
	 HRESULT OnEndModelChangesInternal(pmod::library::EndModelChangesOptions options) override;

    static HRESULT CreateInstance(
        _In_ const pmod::library::ObservableObjectCreateParameters *pModelImpl,
    _In_ foundation::IInspectable *pOuter,
    _Outptr_ foundation::IInspectable **ppNewInstance);

protected:
	friend CObservableObjectDictionaryAdapter;
#if defined(_WINDOWS_RUNTIME)
    INSPECTABLE_CLASS(U("Microsoft.PropertyModel.Library.PropertyModel"), foundation::TrustLevel::BaseTrust);
#endif
    CObservableObject();
    // ComBase Override
	HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
	{
		HRESULT hr = S_OK;
		if (iid == foundation::IObjectDispatch::GetIID())
		{
			*ppInterface = static_cast<foundation::IObjectDispatch *>(this);
		}
		else
		{
			hr = _ObservableObjectBase_Type::QueryInterfaceImpl(iid, ppInterface);
		}
		return hr;
	}

    void OnFinalRelease() override;

    // IObservableObject Interface
    STDMETHOD( GetProperty )(UINT32 propertyId, foundation::IInspectable** value) ;
    STDMETHOD( SetProperty )(UINT32 propertyId,foundation::IInspectable *value);
    STDMETHOD(GetProperties)(foundation::IInspectable *pProperties,UINT32 *size, foundation::IPropertyIdPair ***ppProperties);

    STDMETHOD( GetPropertyChangedEventSource )(_Outptr_ pmod::IPropertyChangedEventSource** ppEventSource);
    STDMETHOD( GetEventModelEventSource )(_Outptr_ pmod::IEventModelEventSource** ppEventSource);
    STDMETHOD(InvokeMethod) (UINT32 methodId,_In_ UINT32 size,foundation::IInspectable **parameters,foundation::IInspectable **ppResult);

    // IDictionary Delegate Interface
    HRESULT _GetView(UINT32* size,foundation::IDictionaryPair ***ppProperties);
	HRESULT _GetSize(_Outptr_ UINT32* size);
	HRESULT _Lookup(HSTRING propertyName, foundation::IInspectable** value);
	HRESULT _HasKey(HSTRING propertyName, bool* found);

	HRESULT _Insert(HSTRING propertyName, foundation::IInspectable *value,bool* replaced);
	HRESULT _Remove(HSTRING propertyName);
	HRESULT _Clear();
	HRESULT _Keys(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first);

    // IObservableObjectClass Interface
    // method on invoke delegate
    STDMETHOD(GetMethodOnInvokeDelegate)(_COM_Outptr_ pmod::library::IMethodOnInvokeDelegate **ppExecuteDelegate);
    STDMETHOD(SetMethodOnInvokeDelegate)(_In_opt_ pmod::library::IMethodOnInvokeDelegate *pExecuteDelegate);

    STDMETHOD(SetPropertyInternal)(_In_ UINT32 propertyId, _In_ foundation::IInspectable *newValue,_In_ bool fireNotifyChanged);
    STDMETHOD(FirePropertyChanged)(_In_ UINT32 propertyId, _In_ bool useCallback);
    STDMETHOD(FireEventModel)(_In_ UINT32 eventId,_In_ foundation::IInspectable *pEventArgs);
    STDMETHOD(SetBinding)(_In_ UINT32 propertyId,_In_ pmod::IBindingBase *pPropertyBinding);
    STDMETHOD(ClearBinding)(_In_ UINT32 propertyId);
    STDMETHOD(SetPropertyConverter)(_In_ UINT32 propertyId,_In_ pmod::library::IValueConverter *pValueConverter);
    STDMETHOD(GetPropertyRuntimeClassOptions)(_In_ UINT32 propertyId, _Out_ pmod::library::PropertyRuntimeClassOptions* propertyOptions) override;
    STDMETHOD(SetPropertyRuntimeClassOptions)(_In_ UINT32 propertyId, _In_ pmod::library::PropertyRuntimeClassOptions propertyOptions) override
    {
        return SetPropertyRuntimeClassOptionsInternal(propertyId, propertyOptions);
    }
    STDMETHOD(EnsurePropertiesContainer)() override;
    STDMETHOD(GetPropertiesContainer)(_COM_Outptr_ foundation::library::IPropertiesContainer **ppPropertiesContainer) override;

    // Interface IObjectNodeClass
	STDMETHOD(_SetParent)(foundation::library::_INode_impl *pThisNode, foundation::library::_INode_impl *pParent) override;
  
    // Overrides from _FoundationBase
     size_t GetCountEventHandlers() override;

    // Override from _ObjectNode
	STDMETHOD(_OnParentChanged)(_In_opt_ foundation::library::_INode_impl *pParent) override;
	STDMETHOD(_OnAncestorChanged)(
		_In_ foundation::library::_INode_impl *pChild,
		_In_opt_ foundation::library::_INode_impl *pParent) override;


	// _FoundationBase Overrides
	HRESULT DisposeInternal() override;
	HRESULT InitializeEventSourceModel() override;
	HRESULT SetTypeInfo(_In_ const IID& iidType) override;

    // return true if the property id is being contained by this model instance
    // Override this method if you include properties that are resolved outside our container
    virtual bool IsPropertyIdContained(UINT32 propertyId)
    {
        UNREFERENCED_PARAMETER(propertyId);
        return true;
    }
public:
    // Overrides from _FoundationBase
    void OnIsObservedChanged(bool newValue) override;
    bool IsObservedInternal() override;

    // Private for internal adapters support
    bool _HasEventHandlersInternal()
    {
        return _ObservableObjectBase_Type::HasEventHandlersInternal() ||
            this->_EventModelSourceType::GetCountHandlerStorage() > 0;
    }
protected:
    HRESULT FirePropertyChangedEvent(
        _FoundationBaseLock& csl,
        _In_ IObservableObject *pSource,
        _In_ UINT32 propertyId,
        _In_ foundation::IInspectable *oldValue,
        _In_ foundation::IInspectable *newValue);

    HRESULT FirePropertyChangedEvent(
        _FoundationBaseLock& csl,
        _In_ UINT32 propertyId,
        _In_ foundation::IInspectable *oldValue,
        _In_ foundation::IInspectable *newValue);

    // Standard IPropertyChangedEventArgs notification
    HRESULT FirePropertyChangedEvent(
        _FoundationBaseLock& csl,
        _In_ pmod::IPropertyChangedEventArgs* pEventArgs);

    // Batch IBatchPropertyChangedEventArgs notification
    HRESULT FirePropertyChangedEvent(
        _FoundationBaseLock& csl,
        _In_ pmod::IBatchPropertyChangedEventArgs* pEventArgs);
    HRESULT PerformBatchFirePropertyChanged(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ pmod::IBatchPropertyChangedEventArgs* pEventArgs);

    // Standard IEventModelEventArgs notification
    HRESULT PerformFireEventModel(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ pmod::IEventModelEventArgs* pEventArgs);

private:
    HRESULT InitializeProperties();

    struct _PropertyClassInfoInternal {
        _PropertyClassInfoInternal() :
        _propertyRuntimeClassOptions(static_cast<UINT16>(pmod::library::PropertyRuntimeClassOptions::None))
        {}

        bool IsEmpty() {
            return _spPropertyBinding == nullptr &&
                _spValueConverter == nullptr &&
                _propertyRuntimeClassOptions == static_cast<UINT16>(pmod::library::PropertyRuntimeClassOptions::None);
        }
        foundation::ComPtr<pmod::IBindingBase> _spPropertyBinding;
        foundation::ComPtr<pmod::library::IValueConverter> _spValueConverter;
        UINT16              _propertyRuntimeClassOptions;
    };
    static HRESULT IsPropertyAvailable(HRESULT hr, bool& propertyAvailable);
    typedef std::map<UINT32,_PropertyClassInfoInternal > _MapPropertyInfoInternalType;

    HRESULT AttachBinding(_In_ pmod::IBindingBase *pPropertyBinding);
    HRESULT DetachBinding(_In_ pmod::IBindingBase *pPropertyBinding);
    HRESULT FireBindingValue(
        _FoundationBaseLock& csl,
        _In_ pmod::IBindingBase *pPropertyBinding,
        _In_ int propertyId);

    HRESULT AttachBindings();
    HRESULT DetachBindings();
    HRESULT DetachBindings(const _MapPropertyInfoInternalType& propertySupport);
    
    void EnsurePropertyInfoSupport_NoLock();
    bool LookupPropertyInfoSupport_NoLock(
        _In_ UINT32 propertyId, 
        _Out_ _PropertyClassInfoInternal& propertyInfoInternal);

    HRESULT SetPropertyRuntimeClassOptionsInternal(_In_ UINT32 propertyId, _In_ pmod::library::PropertyRuntimeClassOptions propertyOptions);
    bool IsPropertyRuntimeClassOptionEnabled(_In_ UINT32 propertyId, _In_ pmod::library::PropertyRuntimeClassOptions propertyOption);

    // return working dispatcher (or null) if runtime class option is enabled
    foundation::library::IDispatcher *GetWorkingDispatcherForRuntimeClassOption(
        _In_ UINT32 propertyId,
        _In_ bool localOption,
        _In_ pmod::library::PropertyRuntimeClassOptions propertyRuntimeOptions);

    HRESULT SetBindingInternal(_In_ UINT32 propertyId, _In_ pmod::IBindingBase *pPropertyBinding);
    HRESULT ClearBindingInternal(_In_ UINT32 propertyId);
    HRESULT ClearBindingInternal_NoLock(_In_ UINT32 propertyId);

    // Handler method for Binding Value changed
    HRESULT OnBindingValueChanged(foundation::IUnknown *pSender, pmod::IBindingValueChangedEventArgs *pArgs);

    // Force disconnect my binding objects
    void ForceDisconnectBindings();

    // Evaluate Binding Connections
    HRESULT EvaluateBindingConnections();
    HRESULT EnsureBindingValueChangedDelegate();

    HRESULT EnsurePropertiesContainerInternal();
    HRESULT InitializeProperties(CObservableObjectInfo *pPropertyModelInfo);

    static HRESULT CreatePropertyChangedEventArgs(
        _In_ pmod::IObservableObject *pSource,
        _In_ UINT32 propertyId,
        _In_ foundation::IInspectable *oldValue,
        _In_ foundation::IInspectable *newValue,
        _Outptr_ pmod::IPropertyChangedEventArgs **ppPropertyChangedEventArgs);

    // Internal method to set the property with type safe checking
    HRESULT InternalSetProperty(_In_ UINT32 propertyId, _In_ foundation::IInspectable *newValue,_In_ bool isProtected,_In_ bool fireNotifyChanged);

    HRESULT InternalSetProperty(
        _In_ UINT32 propertyId,
        _In_ bool isWorkingDispatcher,
        _In_ foundation::IInspectable *newVal,
        _In_ bool isProtected,
        _In_ bool fireNotifyChanged);

    // Set the Property in our bag container without any critical section protection
    HRESULT StorePropertyInternal(_In_ foundation::IPropertyInfo *pPropertyInfo,_In_ foundation::IInspectable * oldValue,_In_ foundation::IInspectable * newValue);
 
    // Try get a property from our bag container
    HRESULT TryGetPropertyInternal(
        _In_ UINT32 propertyId,
        _In_ bool initialLock,
        _Outptr_ foundation::InspectablePtr& value);

    HRESULT GetPropertyInternal(
        _In_ UINT32 propertyId,
        _In_ bool initialLock,
        _Outptr_ foundation::IInspectable** ppValue);

    // Return an Ancestor for a Property Id
    HRESULT GetAncestorInternal(
		_In_ foundation::IPropertyInfo *pPropertyInfo, 
		_Outptr_ foundation::IObject **ppAncestor);

    // Recalc Ancestor for a Property id
    HRESULT RecalcAncestorInternal(
		_In_ foundation::IPropertyInfo *pPropertyInfo, 
		_Outptr_ foundation::IObject **ppAncestor);
    HRESULT RecalcAncestorProperties(_In_ CObservableObjectInfo *pPropertyModelInfo);
    HRESULT RecalcAncestorProperties(
        _In_ CObservableObjectInfo *pPropertyModelInfo, 
        _In_ pmod::library::BatchPropertyChangedEventArgsImpl *pBatchPropertyChangedEventArgs);

    
    void RecalcAncestorPropertiesIf();

	static bool _IsPropertyIdInContainerCallback(void *_Context, UINT32 propertyId);
    bool IsPropertyIdInContainer(UINT32 propertyId);

    HRESULT GetIsInModelChangesInternal(_Outptr_ bool& value);

    HRESULT GetPropertyId(_In_ HSTRING propertyName,_Outptr_ UINT32 *propertyId);

    HRESULT GetPropertiesInternal(foundation::IInspectable *pProperties, UINT32* size, foundation::IPropertyIdPair ***ppProperties);

    HRESULT ConvertPropertyValue(
        _In_ pmod::library::IValueConverter *pValueConverter,
        _In_ foundation::IPropertyInfo *pPropertyInfo,
        _In_ foundation::IInspectable *pValue,
        _In_ bool isConvertBack,
        _COM_Outptr_ foundation::IInspectable **ppConvertedValue);
    HRESULT ConvertPropertyValue(
        _In_ foundation::IPropertyInfo *pPropertyInfo,
        _In_ foundation::IInspectable *pValue,
        _In_ bool isConvertBack,
        _COM_Outptr_ foundation::IInspectable **ppConvertedValue);

    HRESULT InvokeAsyncInternal(
        _In_ bool bReturnAsyncOperation,
        _In_ foundation::IMethodInfo *pMethodInfo,
        _In_ UINT32 methodId,
        IInspectable *pAsyncOperationClass,
        const _Inspectable_ptr_vector_type& typeParameters,
        foundation::IInspectable **ppResult);

    HRESULT InvokeInternal(
        _In_ bool isDispatched,
        _In_ bool isAsync,
        _In_ foundation::IMethodInfo *pMethodInfo,
        _In_ UINT32 methodId,
        _In_ UINT32 size,
        _In_ foundation::IInspectable **parameters,
        foundation::IInspectable **ppResult);

    HRESULT SetAsParentIf(foundation::IInspectable *newValue);
    
    HRESULT GetPropertyInfoById(_In_ UINT32 propertyId,_Outptr_ foundation::IPropertyInfo **ppPropertyInfo);
   
protected:
    #if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override;

    #endif
    // Root Model Support
    bool HasRootModel() { return m_pRoot != nullptr; }
    bool IsSupportRootProperty() { return m_SupportRootProperty; }
    HRESULT RecalcRoot(); 

    CObservableObjectInfo *GetPropertyModelInfoClass();
    CObservableObjectInfo *EnsurePropertyModelInfoClassAndContainer();

private:

    // methods for logging
    void LogInvokeMethod(
        _In_ UINT32 uniqueId,
        _In_ foundation::IMethodInfo *pMethodInfo,
        _In_ UINT32 methodId,
        _In_ UINT32 size,
        _In_ foundation::IInspectable **parameters);

    void LogGetProperty(
        _In_ UINT32 uniqueId,
        _In_ UINT32 propertyId);

    void LogSetProperty(
        _In_ UINT32 uniqueId,
        _In_ UINT32 propertyId);

    typedef std::pair<foundation::InspectablePtr,foundation::InspectablePtr> _TValueChanged;
    typedef std::map<UINT32,_TValueChanged> _MapValueChanges_Type;
    typedef pmod:: BindingValueChangedConnection::_DelegateFactory<CObservableObject> CBindingValueChangedDelegateFactory;
    typedef PropertyChangedEventHandlerBase<_ObservableObjectBase_Type::_TNotifyEventSourceImpl_Type> PropertyChangedEventHandler;
    
    // Our internal Binding support struct
    struct _PropertyInfoSupport:
        _MapPropertyInfoInternalType
    {
        _PropertyInfoSupport():
            m_pBindingValueChangedDelegate(nullptr)
        {}
        CBindingValueChangedDelegateFactory::_type_eventhandler_delegate   *m_pBindingValueChangedDelegate;
    };
    bool                        m_SupportRootProperty:1;
    bool                        m_NoAutoParent:1;
    bool                        m_UseGetPropertyDispatcher : 1;
    bool                        m_UseSetPropertyDispatcher : 1;
    bool                        m_UseGetPropertiesDispatcher : 1;
    bool                        m_UseModelTransaction:1;
    bool                        m_UseFastPropertiesContainer : 1;
    bool                        m_ClassOnSetProperty : 1;
    bool                        m_AllowReturnAsyncOnVoidResult : 1;
	bool                        m_IsContainerWeakRef : 1;
	pmod::library::IObjectNodeClass             *m_pRoot;       // weak reference to our root

    // Our Property Container Ptr
    foundation::library::IPropertiesContainer           *m_pProperties;

    // For binding support
    std::unique_ptr<_PropertyInfoSupport>                 m_spPropertyInfoSupport;

    // Begin/End Model Changes support
    std::unique_ptr<_MapValueChanges_Type>               m_ModelChanges;

    // MethodOnInvoke delegate object
    foundation::ComPtr<pmod::library::IMethodOnInvokeDelegate>         m_MethodOnInvokeDelegate;

};

