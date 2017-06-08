/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectAdapters.cpp
****/
#include "pch.h"

#include "FoundationBaseTracker.h"

#include <foundation/library/adapter_base.h>

#include <pmod/library/mtl.h>

#include "ObservableObject.h"
using namespace pmod;
using namespace pmod::library;

struct IDummyInterface : foundation::IUnknown
{
};
extern const IID IID_IDummyInterface;

// {3B6ED946-93C9-4037-89D9-7041E63D5141}
const GUID IID_IDummyInterface =
{ 0x3b6ed946, 0x93c9, 0x4037, { 0x89, 0xd9, 0x70, 0x41, 0xe6, 0x3d, 0x51, 0x41 } };

//------------------------------------------------------------------------------
// Class:   CObservableObjectAdapterBase
//
// Purpose: Adapter base class to host a IObservableObject instance in an adapter
//
//------------------------------------------------------------------------------
template <class T, const IID& iidType, class TInner>
class CObservableObjectAdapterBase : public 
    foundation::AdapterBase
    <
        TInner,
        IDummyInterface,
        &IID_IDummyInterface
    >,
    protected _ObservableObjectClassBase
{
private:
    typedef foundation::AdapterBase
        <
        TInner,
        IDummyInterface,
        &IID_IDummyInterface
        > _Base_Type;
public:
    HRESULT _Initialize(
    _In_ foundation::IInspectable *pOuter,
    _In_ foundation::IInspectable *pInner
    )
    {
        IFR_ASSERT(_Base_Type::_Initialize(pOuter, pInner));

        // create the inner class
        IFR_ASSERT(this->_CreateInnerInstance(
            iidType,
            pOuter,
            library::ObservableObjectOptions::None));

        T *pThis = static_cast<T *>(this);
        IFR_ASSERT(pThis->_InitializeAdapter());

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
        // Note: we want to remove the tracking on this since the leak will be reported
        // by the aggregated 'pOuter'
        GetFoundationTracker()->RemoveInstance(this->GetObservableObjectClass());
#endif
        return S_OK;
    }

    HRESULT _InitializeAdapter()
    {
        this->EvaluateIsObservedChangedInternal();
        return S_OK;
    }
protected:

    void FinalRelease() override
    {
        this->_ReleaseInner();
       _Base_Type::FinalRelease();
    }

    STDMETHOD(QueryAdapterInterface)(REFIID iid, foundation::IUnknown **ppInterface)
    {
        if (iid == IObservableObject::GetIID() || iid == foundation::IObjectDispatch::GetIID())
        {
            return foundation::QueryWeakReference(this->GetInnerRaw(), iid, ppInterface);
        }
        return _Base_Type::QueryAdapterInterface(iid, ppInterface);
    }

    STDMETHOD(HasEventHandlers)(_Outptr_ bool *pValue)
    {
        *pValue = this->GetObservableObjectClass()->_HasEventHandlersInternal();
        return S_OK;
    }

    STDMETHOD(OnHostObservedChanged)(_In_ bool isObserved)
    {
        // if a Property Model instance is created we need to ensure its 'OnAddHandlerInternal'
        // so its 'IsObserved' state is being properly mantained.
        this->EvaluateIsObservedChangedInternal();
        return S_OK;
    }

    class _CPropertiesContainer :
        public foundation::ctl::ImplementsInspectable<foundation::library::IPropertiesContainer, &foundation::library::IID_IPropertiesContainer>
    {
    public:
        static HRESULT CreateInstance(
            T *pThis,
            foundation::library::IPropertiesContainer **ppPropertiesContainer)
        {
            _CPropertiesContainer *pPropertiesContainer = nullptr;
            foundation::ctl::ComInspectableObject<_CPropertiesContainer>::CreateInstance(&pPropertiesContainer);
            pPropertiesContainer->_pThis = pThis;
            *ppPropertiesContainer = pPropertiesContainer;
            return S_OK;
        }
    protected:
        _CPropertiesContainer() :
            _pThis(nullptr)
        {}

        STDMETHOD(HasProperty)(_In_ UINT32 /*propertyId*/, _Out_ bool *found)
        {
            IFCPTR_ASSERT(found);
            // TODO: we may need to delegate into our 'T' template parameter
            *found = true;
            return S_OK;
        }

        STDMETHOD(GetValue)(_In_ UINT32 propertyId, _COM_Outptr_ foundation::IInspectable **value)
        {
            foundation_assert(_pThis);

            return _pThis->_GetPropertyValue(propertyId, value);
        }
        STDMETHOD(SetValue)(_In_ UINT32 propertyId, _In_ foundation::IInspectable *value)
        {
            foundation_assert(false);
            return E_NOTIMPL;
        }
        STDMETHOD(RemoveProperty)(_In_ UINT32 propertyId)
        {
            foundation_assert(false);
            return E_NOTIMPL;
        }
        STDMETHOD(GetSize)(_Outptr_ UINT32* size)
        {
            foundation_assert(false);
            return E_NOTIMPL;
        }
        STDMETHOD(Dispose)()
        {
            return S_OK;
        }
    private:
        T *_pThis;
    };

    friend class _CPropertiesContainer;

    // overrides from Delegate callback
    STDMETHOD(OnCreatePropertiesContainer)(
        _In_ IObservableObjectInfo* pPropertyModelInfo,
        _In_ UINT32 size,
        _COM_Outptr_ foundation::library::IPropertiesContainer **ppValue) override
    {
        UNREFERENCED_PARAMETER(pPropertyModelInfo);
        UNREFERENCED_PARAMETER(size);
        return _CPropertiesContainer::CreateInstance(static_cast<T *>(this), ppValue);
    }
    STDMETHOD(OnAddEventHandler)(
        _In_ pmod::IPropertyChangedEventHandler* pHandler) override
    {
        this->_OnAdapterAddEventHandler(pHandler);
        return S_OK;
    }
    STDMETHOD(OnRemoveEventHandler)(
        _In_ pmod::IPropertyChangedEventHandler* pHandler) override
    {
        this->_OnAdapterRemoveEventHandler(pHandler);
        return S_OK;
    }
private:
    void EvaluateIsObservedChangedInternal()
    {
        // use the 'CObservableObject' class internal method
        this->GetObservableObjectClass()->EvaluateIsObservedChanged();
    }
protected:

    CObservableObject *GetObservableObjectClass()
    {
        return static_cast<CObservableObject *>(this->GetInterfaceProtected());
    }
};
//------------------------------------------------------------------------------
// Class:   CEnumObservableObjectAdapter
//
// Purpose: Support IObservableObject interface in an IEnumValue
//
//------------------------------------------------------------------------------

class CEnumObservableObjectAdapter :
    public CObservableObjectAdapterBase
    <
    CEnumObservableObjectAdapter,
        EnumValue::IIDType,
        foundation::IEnumValue
    >
{
public:
    HRESULT _InitializeAdapter()
    {
        return S_OK;
    }
    HRESULT _GetPropertyValue(
        _In_ UINT32 propertyId,
        _COM_Outptr_ foundation::IInspectable **ppValue)
    {
        switch (propertyId)
        {
        case EnumValue::Property_Value:
            {
                UINT32 enumValue;
                this->GetInner()->GetValue(&enumValue);
                IFR_ASSERT(foundation::pv_util::CreateUInt32Value(enumValue, ppValue));
                break;
            }
        case EnumValue::Property_Name:
            {
                foundation::HStringPtr name;
                GetInner()->ToString(name.GetAddressOf());
                IFR_ASSERT(foundation::pv_util::CreateStringValue(name, ppValue));
                break;
            }
        default:
            foundation_assert(false);
            return E_UNEXPECTED;
        }
        return S_OK;
    }

};

//------------------------------------------------------------------------------
// Class:   CAsyncOperationObservableObjectAdapter
//
// Purpose: Support IObservableObject interface in an IAsyncOperation
//
//------------------------------------------------------------------------------
class CAsyncOperationObservableObjectAdapter :
    public CObservableObjectAdapterBase
    <
    CAsyncOperationObservableObjectAdapter,
    AsyncOperation::IIDType,
    foundation::IAsyncOperation
    >
{
public:
    HRESULT _GetPropertyValue(
        _In_ UINT32 propertyId,
        _COM_Outptr_ foundation::IInspectable **ppValue)
    {
        switch (propertyId)
        {
            case AsyncOperation::Property_ResultStatus:
            {
                foundation::ResultStatus resultStatus;
                GetInner()->GetStatus(&resultStatus);
                IFR_ASSERT(foundation::pv_util::CreateUInt32Value(static_cast<UINT32>(resultStatus), ppValue));
                break;
            }
            case AsyncOperation::Property_Error:
            {
                HRESULT hResult;
                GetInner()->GetError(&hResult);
                IFR_ASSERT(foundation::pv_util::CreateUInt32Value(hResult, ppValue));
                break;
            }
            case AsyncOperation::Property_Result:
            {
                foundation::InspectablePtr spResult;
                GetInner()->GetResult(foundation::_InfiniteTimeout,spResult.GetAddressOf());
                *ppValue = spResult.Detach();
                break;
            }
            case AsyncOperation::Property_Progress:
            {
                _spCurrentProgressValue.CopyTo(ppValue);
                break;
            }
            default:
                foundation_assert(false);
                return E_UNEXPECTED;
        }
        return S_OK;
    }
private:
    STDMETHOD(FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pIFireEventHandlerContainer,
		_In_ foundation::IUnknown* pEventArgs) override
    {
        foundation::ComPtr<foundation::IProgressEventArgs> spProgressEventArgs;
        foundation::ComPtr<foundation::ICompletedEventArgs> spCompletedEventArgs;
        if (SUCCEEDED(foundation::QueryInterface(pEventArgs, spCompletedEventArgs.GetAddressOf())))
        {
            // completed event so update non progress properties
            this->FirePropertyChanged(AsyncOperation::Property_ResultStatus);
            this->FirePropertyChanged(AsyncOperation::Property_Error);
            this->FirePropertyChanged(AsyncOperation::Property_Result);
        }
        else if (SUCCEEDED(foundation::QueryInterface(pEventArgs, spProgressEventArgs.GetAddressOf())))
        {
            spProgressEventArgs->GetProgressValue(_spCurrentProgressValue.ReleaseAndGetAddressOf());
            this->FirePropertyChanged(AsyncOperation::Property_Progress);
        }
        return S_OK;
    }
private:
    foundation::InspectablePtr _spCurrentProgressValue;
};

//------------------------------------------------------------------------------
// Class:   CCollectionObservableObjectAdapter
//
// Purpose: Support IObservableObject interface in an ICollection
//
//------------------------------------------------------------------------------

class CCollectionObservableObjectAdapter :
    public CObservableObjectAdapterBase
    <
        CCollectionObservableObjectAdapter,
        ObservableCollection::IIDType,
        IObservableCollection
    >
{
public:
    HRESULT _GetPropertyValue(
        _In_ UINT32 propertyId,
        _COM_Outptr_ foundation::IInspectable **ppValue)
    {
        switch (propertyId)
        {
            case ObservableCollection::Property_Count:
            {
                IFR_ASSERT(foundation::pv_util::CreateUInt32Value(get_inner_count(), ppValue));
                break;
            }
            case ObservableCollection::Property_IsEmpty:
            {
                // Note: we never return the cached 'IsEmpty' since we are not necesary observed
                IFR_ASSERT(foundation::pv_util::CreateBooleanValue(get_inner_count() == 0 ? true : false, ppValue));
                break;
            }
            default:
                foundation_assert(false);
                return E_UNEXPECTED;
        }
        return S_OK;
    }
    HRESULT _InitializeAdapter()
    {
        IFR_ASSERT(_Base_Type::_InitializeAdapter());
        // Note: when the Adapter is being created it could be already 'Observed'
        // so we will cache or 'IsEmpty' value
        _isEmptyCache = get_inner_count() == 0;
        return S_OK;
    }
protected:
    typedef CObservableObjectAdapterBase
        <
        CCollectionObservableObjectAdapter,
        ObservableCollection::IIDType,
        IObservableCollection
        > _Base_Type;

    CCollectionObservableObjectAdapter():
        _isEmptyCache(false) // only to not allow garbage since is not relevant
    {}
private:
    UINT32 get_inner_count()
    {
        UINT32 count;
        GetInner()->GetCount(&count);
        return count;
    }
    STDMETHOD(FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pIFireEventHandlerContainer,
		_In_ foundation::IUnknown* pEventArgs) override
    {
        foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        if (SUCCEEDED(foundation::QueryInterface(pEventArgs, spNotifyCollectionChangedEventArgs.GetAddressOf())))
        {
            // Note: we always notify changes in the following order Property_Count -> Property_IsEmpty
            // Our unit test code is taking this into consideration

            // 'Property_Count' firing filter
            foundation::NotifyCollectionModelChangedAction action;
            spNotifyCollectionChangedEventArgs->GetAction(&action);
            if (action != foundation::NotifyCollectionModelChangedAction::ItemReplaced)
            {
                this->FirePropertyChanged(ObservableCollection::Property_Count);
            }

            // 'Property_IsEmpty' firing filter
            bool isEmpty = get_inner_count() == 0;
            if (isEmpty != _isEmptyCache)
            {
                _isEmptyCache = isEmpty;
                this->FirePropertyChanged(ObservableCollection::Property_IsEmpty);
            }
        }
        return S_OK;
    }
    STDMETHOD(OnHostObservedChanged)(_In_ bool isObserved) override
    {
        _Base_Type::OnHostObservedChanged(isObserved);
        if (isObserved)
        {
            // when we start observing we will cache the current IsEmpty state
            _isEmptyCache = get_inner_count() == 0;
        }
        return S_OK;
    }

private:
    foundation::InspectablePtr _spCurrentProgressValue;
    bool _isEmptyCache;     // to store the current value of IsEmpty property (only valid when observed)
};

//------------------------------------------------------------------------------
// Class:   CCommandObservableObjectAdapter
//
// Purpose: Support IObservableObject interface in an ICommand
//
//------------------------------------------------------------------------------
class CCommandObservableObjectAdapter :
    public CObservableObjectAdapterBase
    <
    CCommandObservableObjectAdapter,
    CommandModel::IIDType,
    ICommand
    >
{
public:
    HRESULT _GetPropertyValue(
        _In_ UINT32 propertyId,
        _COM_Outptr_ foundation::IInspectable **ppValue)
    {
        switch (propertyId)
        {
        case CommandModel::Property_IsEnabled:
        {
            UINT32 state;
            GetInner()->GetState(&state);
            IFR_ASSERT(foundation::pv_util::CreateBooleanValue(state == CommandStateEnabled, ppValue));
            break;
        }
        case CommandModel::Property_State:
        {
            UINT32 state;
            GetInner()->GetState(&state);
            IFR_ASSERT(foundation::pv_util::CreateUInt32Value(state, ppValue));
            break;
        }
        case CommandModel::Property_CommandId:
        {
            // TODO: no public API to retrieve the Command Id
            UINT32 commandId = 0;
            IFR_ASSERT(foundation::pv_util::CreateUInt32Value(commandId, ppValue));
            break;
        }
        default:
            foundation_assert(false);
            return E_UNEXPECTED;
        }
        return S_OK;
    }
private:
    STDMETHOD(FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pIFireEventHandlerContainer,
		_In_ foundation::IUnknown* pEventArgs) override
    {
        foundation::ComPtr<ICanExecuteChangedEventArgs> spCanExecuteChangedEventArgs;
        if (SUCCEEDED(foundation::QueryInterface(pEventArgs, spCanExecuteChangedEventArgs.GetAddressOf())))
        {
            // can executed changed event
            this->FirePropertyChanged(CommandModel::Property_IsEnabled);
            this->FirePropertyChanged(CommandModel::Property_State);
        }
        return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   CBindingBaseObservableObjectAdapter
//
// Purpose: Support IObservableObject interface in an ICommand
//
//------------------------------------------------------------------------------
class CBindingBaseObservableObjectAdapter :
    public CObservableObjectAdapterBase
    <
    CBindingBaseObservableObjectAdapter,
    PropertyBinding::IIDType,
    IBindingBase
    >
{
public:
    HRESULT _GetPropertyValue(
        _In_ UINT32 propertyId,
        _COM_Outptr_ foundation::IInspectable **ppValue)
    {
        switch (propertyId)
        {
        case PropertyBinding::Property_Value:
        {
            IFR_ASSERT(GetInner()->GetValue(ppValue));
            break;
        }
        default:
            foundation_assert(false);
            return E_UNEXPECTED;
        }
        return S_OK;
    }
private:
    STDMETHOD(FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pIFireEventHandlerContainer,
		_In_ foundation::IUnknown* pEventArgs) override
    {
        foundation::ComPtr<IBindingValueChangedEventArgs> spBindingValueChangedEventArgs;
        if (SUCCEEDED(foundation::QueryInterface(pEventArgs, spBindingValueChangedEventArgs.GetAddressOf())))
        {
            // binding value changed event
            this->FirePropertyChanged(PropertyBinding::Property_Value);
        }
        return S_OK;
    }
};

static HRESULT RegisterFactoryObservableObjectAdapter(
	_In_ foundation::library::IGenericAdapterFactory *pGenericAdapterFactory,
	GUID riid_outer,
	GUID iidSourceType,
	_In_ foundation::library::CreateAdapterInstanceCallback creatorCallback)
{
	pGenericAdapterFactory->AddAdapterEntry(
		riid_outer,
		foundation::IID_IObjectDispatch,
		iidSourceType,
		creatorCallback);
	pGenericAdapterFactory->AddAdapterEntry(
		riid_outer,
		pmod::IID_IObservableObject,
		iidSourceType,
		creatorCallback);
	return S_OK;
}

HRESULT RegisterFactoryObservableObjectAdapters(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
	foundation_assert(pGenericAdapterFactory != nullptr);

    // IID_IEnumValue Adapter
	RegisterFactoryObservableObjectAdapter(
		pGenericAdapterFactory,
		foundation::IID_IEnumValue,
		foundation::IID_IEnumValue,
		foundation::CreateAdapter<CEnumObservableObjectAdapter>);

    // IID_IAsyncOperation Adapter
	RegisterFactoryObservableObjectAdapter(
		pGenericAdapterFactory,
		foundation::IID_IAsyncOperation,
		foundation::IID_IAsyncOperation,
		foundation::CreateAdapter<CAsyncOperationObservableObjectAdapter>);

    // IID_ICollection Adapter
	RegisterFactoryObservableObjectAdapter(
		pGenericAdapterFactory,
		IID_IObservableCollection,
		IID_IObservableCollection,
		foundation::CreateAdapter<CCollectionObservableObjectAdapter>);

    // IID_ICollection Adapter
	RegisterFactoryObservableObjectAdapter(
		pGenericAdapterFactory,
		IID_ICommand,
		IID_ICommand,
		foundation::CreateAdapter<CCommandObservableObjectAdapter>);

    // IID_IPropertyBinding Adapter
	RegisterFactoryObservableObjectAdapter(
		pGenericAdapterFactory,
		IID_IPropertyBinding,
		IID_IBindingBase,
		foundation::CreateAdapter<CBindingBaseObservableObjectAdapter>);

    // IID_IMultiSourceBinding Adapter
	RegisterFactoryObservableObjectAdapter(
		pGenericAdapterFactory,
		IID_IMultiSourceBinding,
		IID_IBindingBase,
		foundation::CreateAdapter<CBindingBaseObservableObjectAdapter>);

    return S_OK;
}
