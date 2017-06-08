/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectProxy.h
****/

#pragma once

// from public
#include <foundation/assert.h>
#include <foundation/library/dictionary_class.h>
#include <foundation/library/logger_macros.h>

#include "Microsoft.PropertyModel.h"
#include "MoAdapterProxyInterface.h"
#include "MoModelProxy.h"
#include <pmod/library/model_library.h>
#include "MoEventSourceProxy.h"
#include "MoCoreProxyFactory.h"
#include <foundation/library/key_value_pair_impl.h>
#include <pmod/library/property_changed_event_args_impl.h>

template <class TCoreInterface, class TMoInterface, class THostInterface = TCoreInterface>
class TMoPropertyDataProxy : public
	TMoObjectProxy
	<
	TCoreInterface,
	TMoInterface
	>
{
protected:
	TMoPropertyDataProxy():
		_pMoPropertyData(nullptr)
	{}
public:
	HRESULT SetSourceInterface(TMoInterface *pMoInterface)
	{
		IFR_ASSERT(__super::SetSourceInterface(pMoInterface));
		IFR_ASSERT(foundation::QueryInterface(pMoInterface, &this->_pMoPropertyData));
		return S_OK;
	}
	void FinalRelease() override
	{
        RELEASE_INTERFACE(_pMoPropertyData);
		__super::FinalRelease();
	}

	HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
	{
		if (iid == foundation::IObjectDispatch::GetIID())
		{
			*ppInterface = static_cast<foundation::IObjectDispatch *>(this);
		}
		else
		{
			return __super::QueryInterfaceImpl(iid, ppInterface);
		}
		return S_OK;
	}
	STDMETHOD(GetProperty)(UINT32 propertyId, IInspectable** value)
	{
		PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_PropertyModel_GetProperty_Id)

		foundation::InspectablePtr spValue;
		HRESULT hr = this->_pMoPropertyData->GetProperty(propertyId, spValue.GetAddressOf());
		IFHR(hr);
		IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spValue, value));
		return S_OK;
	}

	STDMETHOD(SetProperty)(UINT32 propertyId, IInspectable *value)
	{
		PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_PropertyModel_SetProperty_Id)

		foundation::InspectablePtr spValue;
		IFR_ASSERT(CMoCoreProxyFactory::FromProxy(value, spValue.GetAddressOf()));
		return _pMoPropertyData->SetProperty(propertyId, spValue);
	}

	STDMETHOD(GetProperties)(foundation::IInspectable *pProperties, UINT32 *size, foundation::IPropertyIdPair ***ppProperties)
	{
		PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_PropertyModel_GetProperties_Id)

		typedef foundation::library::_InspectableKeyValuePair<foundation::IPropertyIdPair> CPropertyIdPair;

		foundation::ArrayRefCountWrapper<__FIKeyValuePair_2_UINT32_IInspectable> kvpMoProperties;
		HRESULT hr = this->_pMoPropertyData->GetProperties(
			pProperties,
			kvpMoProperties.GetSizeAddressOf(),
			kvpMoProperties.GetBufferAddressOf());
		IFHR(hr);

		std::vector<foundation::ComPtr<foundation::IPropertyIdPair> > properties;

		for (UINT32 index = 0; index < kvpMoProperties.GetSize(); ++index)
		{
			UINT32 propertyId;
			kvpMoProperties[index]->get_Key(&propertyId);
			foundation::InspectablePtr spValue;
			kvpMoProperties[index]->get_Value(spValue.GetAddressOf());
			foundation::InspectablePtr spCoreProxy;
			IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spValue, spCoreProxy.GetAddressOf()));

			foundation::ComPtr<foundation::IPropertyIdPair> spPropertyIdPair;
			IFR_ASSERT(foundation::library::CreateKeyValuePair<CPropertyIdPair>(
				propertyId,
				spCoreProxy,
				spPropertyIdPair.GetAddressOf()));
			properties.push_back(spPropertyIdPair);
		}

		// copy now to public API
		ContainerRefCountToArray(properties, size, ppProperties);
		return S_OK;
	}

    // Methods.
    STDMETHOD(InvokeMethod)(UINT32 methodId, _In_ UINT32 size, IInspectable **parameters, IInspectable **ppResult)
    {
        PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_PropertyModel_Invoke_Id)

        foundation::InspectableArrayWrapper parameterValues;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxyArray(
            size, parameters,
            parameterValues.GetSizeAddressOf(), parameterValues.GetBufferAddressOf()));

        foundation::InspectablePtr spResultValue;
        IFR(this->_pMoPropertyData->Invoke(
            methodId,
            parameterValues.GetSize(),
            (IInspectable **)parameterValues.GetBuffer(),
            spResultValue.GetAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spResultValue, ppResult));
        return S_OK;
    }
private:
	ABI::Microsoft::PropertyModel::IObjectDispatch *_pMoPropertyData;
};

typedef TMoPropertyDataProxy
<
	foundation::IObjectDispatch,
	ABI::Microsoft::PropertyModel::IObjectDispatch
>
CMoPropertyDataProxy;
//------------------------------------------------------------------------------
// Class:   CMoPropertyChangedEventArgsProxy
//
// Purpose: Implementation of pmod::IPropertyChangedEventArgs
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoPropertyChangedEventArgsProxy :
    public TAdapterProxyInterface
    <
        pmod::IPropertyChangedEventArgs,
        ABI::Microsoft::PropertyModel::IPropertyChangedEventArgs
    >,
    public pmod::library::IPropertyChangedWithCallbackEventArgs,
    public foundation::library::ILoggerSerialize
{
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == pmod::library::IPropertyChangedWithCallbackEventArgs::GetIID())
        {
            if (foundation::IsTypeOf(
                this->m_pSourceInterface,
                __uuidof(ABI::Microsoft::PropertyModel::IPropertyChangedWithCallbackEventArgs)))
            {
                *ppInterface = static_cast<IPropertyChangedWithCallbackEventArgs *>(this);
            }
            else
            {
                return E_NOINTERFACE;
            }
        }
        else if (iid == foundation::library::ILoggerSerialize::GetIID())
        {
            *ppInterface = static_cast<foundation::library::ILoggerSerialize *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    STDMETHOD(GetProperty)(UINT32* pVal)
    {
        return this->m_pSourceInterface->get_Property(pVal);
    }

    STDMETHOD(GetPropertyName)(HSTRING* pValue)
    {
        return this->m_pSourceInterface->get_PropertyName(pValue);
    }

    STDMETHOD(GetOldValue)(IInspectable **ppValue)
    {
        foundation::InspectablePtr spValue;
        IFR(this->m_pSourceInterface->get_OldValue(spValue.GetAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spValue,ppValue));
        return S_OK;
    }
    STDMETHOD(GetNewValue)(IInspectable **ppValue)
    {
        foundation::InspectablePtr spValue;
        IFR(this->m_pSourceInterface->get_NewValue(spValue.GetAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spValue,ppValue));
        return S_OK;
    }
    // Interface library::ILoggerSerialize
    STDMETHOD(CreateLogParameters)(UINT32 *size, foundation::ILogParameterPair ***ppLogParameterPair)
    {
        foundation::logger_util::log_parameter_pair_ptr_vector_type logParameters;
        IFR_ASSERT(foundation::logger_util::AppendLogParameter(
            foundation::BaseEventArgs_LogRecord_TypeIID_Id,
            foundation::pv_util::CreateValue(pmod::IPropertyChangedEventArgs::GetIID()),
            logParameters
            ));

        // property id
        UINT32 propertyId;
        GetProperty(&propertyId);
        IFR_ASSERT(foundation::logger_util::AppendLogParameter(
            pmod::PropertyChangedEventArgs_LogRecord_PropertyId_Id,
            foundation::pv_util::CreateValue(propertyId),
            logParameters
            ));

        foundation::InspectablePtr oldValue, newValue;
        this->GetOldValue(oldValue.GetAddressOf());
        this->GetNewValue(newValue.GetAddressOf());
        // Old Value
        IFR_ASSERT(foundation::logger_util::AppendLogParameter(
            pmod::PropertyChangedEventArgs_LogRecord_OldValue_Id,
            oldValue,
            logParameters
            ));
        // New Value
        IFR_ASSERT(foundation::logger_util::AppendLogParameter(
            pmod::PropertyChangedEventArgs_LogRecord_NewValue_Id,
            newValue,
            logParameters
            ));

        ContainerRefCountToArray(logParameters, size, ppLogParameterPair);
        return S_OK;
    }

};

//------------------------------------------------------------------------------
// Class:   CMoBatchPropertyChangedEventArgsProxy
//
// Purpose: Implementation of pmod::IBatchPropertyChangedEventArgs
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoBatchPropertyChangedEventArgsProxy :
    public TAdapterProxyInterface
    <
        pmod::IBatchPropertyChangedEventArgs,
        ABI::Microsoft::PropertyModel::IBatchPropertyChangedEventArgs
    >,
    public foundation::library::ILoggerSerialize
{
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == foundation::library::ILoggerSerialize::GetIID())
        {
            *ppInterface = static_cast<foundation::library::ILoggerSerialize *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    STDMETHOD(GetPropertyChanges)(UINT32* size, pmod::IPropertyChangedEventArgs ***ppPropertyChangedEventArgs)
    {
        std::vector<foundation::ComPtr<pmod::IPropertyChangedEventArgs>> propertyChangesVector;

        foundation::ArrayRefCountWrapper<ABI::Microsoft::PropertyModel::IPropertyChangedEventArgs> propertyChangesArray;
        IFR_ASSERT(this->m_pSourceInterface->get_PropertyChanges(
            propertyChangesArray.GetSizeAddressOf(),
            propertyChangesArray.GetBufferAddressOf()));

        for (UINT32 index = 0; index < propertyChangesArray.GetSize(); ++index)
        {
            foundation::ComPtr<pmod::IPropertyChangedEventArgs> spPropertyChangedEventArgs;
            HRESULT hr = CMoPropertyChangedEventArgsProxy::CreateInstance<CMoPropertyChangedEventArgsProxy>
                (propertyChangesArray[index], spPropertyChangedEventArgs.GetAddressOf());
            IFR_ASSERT(hr);
            propertyChangesVector.push_back(spPropertyChangedEventArgs);
        }
        ContainerRefCountToArray(propertyChangesVector, size, ppPropertyChangedEventArgs);
        return S_OK;
    }

    // Interface library::ILoggerSerialize
    STDMETHOD(CreateLogParameters)(UINT32 *size, foundation::ILogParameterPair ***ppLogParameterPair)
    {
        foundation::logger_util::log_parameter_pair_ptr_vector_type logParameters;
        IFR_ASSERT(foundation::logger_util::AppendLogParameter(
            foundation::BaseEventArgs_LogRecord_TypeIID_Id,
            foundation::pv_util::CreateValue(pmod::IBatchPropertyChangedEventArgs::GetIID()),
            logParameters
            ));

        ContainerRefCountToArray(logParameters, size, ppLogParameterPair);
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CMoEventModelEventArgsProxy
//
// Purpose: Implementation of pmod::IEventModelEventArgs
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoEventModelEventArgsProxy :
    public TAdapterProxyInterface
    <
        pmod::IEventModelEventArgs,
        ABI::Microsoft::PropertyModel::IEventModelEventArgs
    >
{
protected:
    STDMETHOD(GetEvent)(UINT32* pVal)
    {
        return this->m_pSourceInterface->get_Event(pVal);
    }

    STDMETHOD(GetEventArgs)(IInspectable **ppValue)
    {
        foundation::InspectablePtr spEventArgs;
        IFR_ASSERT(this->m_pSourceInterface->get_EventArgs(spEventArgs.GetAddressOf()));
        if(spEventArgs != nullptr)
        {
            IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spEventArgs,ppValue));
        }
        else
        {
            *ppValue = nullptr;
        }
        return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   CPropertyChangedEventHandlerProxy
//
// Purpose: Implementation of Modern IPropertyChangedEventHandler
//          Interface using the Core Interface callback
//------------------------------------------------------------------------------
class CPropertyChangedEventHandlerProxy :
    public TFreeThreadedAdapterProxyInterface
    <
        ABI::Microsoft::PropertyModel::IPropertyChangedEventHandler,
        pmod::IPropertyChangedEventHandler
    >,
    public ABI::Microsoft::PropertyModel::IBatchPropertyChangedEventHandler
{
protected:
    // Interface IPropertyChangedEventHandler
    HRESULT STDMETHODCALLTYPE Invoke(
        IInspectable *sender,
        ABI::Microsoft::PropertyModel::IPropertyChangedEventArgs *e) override
    {
        foundation::ComPtr<pmod::IPropertyChangedEventArgs> spPropertyChangedEventArgs;
        HRESULT hr = CMoPropertyChangedEventArgsProxy::CreateInstance<CMoPropertyChangedEventArgsProxy>
           (e,spPropertyChangedEventArgs.GetAddressOf());
        IFR_ASSERT(hr);
        return this->m_pSourceInterface->Invoke(sender,spPropertyChangedEventArgs);
    }
    // Interface IBatchPropertyChangedEventHandler
    HRESULT STDMETHODCALLTYPE Invoke(
        IInspectable *sender,
        ABI::Microsoft::PropertyModel::IBatchPropertyChangedEventArgs *e) override
    {
        foundation::ComPtr<pmod::IBatchPropertyChangedEventHandler> spBatchPropertyChangedEventHandler;
        if (SUCCEEDED(foundation::QueryInterface(this->m_pSourceInterface, spBatchPropertyChangedEventHandler.GetAddressOf())))
        {
            foundation::ComPtr<pmod::IBatchPropertyChangedEventArgs> spBatchPropertyChangedEventArgs;
            HRESULT hr = CMoBatchPropertyChangedEventArgsProxy::CreateInstance<CMoBatchPropertyChangedEventArgsProxy>
                (e, spBatchPropertyChangedEventArgs.GetAddressOf());
            IFR_ASSERT(hr);
            return spBatchPropertyChangedEventHandler->Invoke(sender, spBatchPropertyChangedEventArgs);
        }
        else
        {
            // our handler callback does not support IBatchPropertyChangedEventHandler interface
            foundation::ArrayRefCountWrapper<ABI::Microsoft::PropertyModel::IPropertyChangedEventArgs> propertyChangesArray;
            IFR_ASSERT(e->get_PropertyChanges(
                propertyChangesArray.GetSizeAddressOf(),
                propertyChangesArray.GetBufferAddressOf()));
            for (UINT32 index = 0; index < propertyChangesArray.GetSize(); ++index)
            {
                foundation::ComPtr<pmod::IPropertyChangedEventArgs> spPropertyChangedEventArgs;
                HRESULT hr = CMoPropertyChangedEventArgsProxy::CreateInstance<CMoPropertyChangedEventArgsProxy>
                    (propertyChangesArray[index], spPropertyChangedEventArgs.GetAddressOf());
                IFR_ASSERT(hr);
                IFR(this->m_pSourceInterface->Invoke(sender, spPropertyChangedEventArgs));
            }
        }
        return S_OK;
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == __uuidof(ABI::Microsoft::PropertyModel::IBatchPropertyChangedEventHandler))
        {
            *ppInterface = static_cast<ABI::Microsoft::PropertyModel::IBatchPropertyChangedEventHandler *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CEventModelEventHandlerProxy
//
// Purpose: Implementation of Modern IEventModelEventHandler
//          Interface using the Core Interface callback
//------------------------------------------------------------------------------
class CEventModelEventHandlerProxy :
    public TFreeThreadedAdapterProxyInterface
    <
        ABI::Microsoft::PropertyModel::IEventModelEventHandler,
        pmod::IEventModelEventHandler
    >
{
protected:
    HRESULT STDMETHODCALLTYPE Invoke(
        IInspectable *sender,
        ABI::Microsoft::PropertyModel::IEventModelEventArgs *e) override
    {
        foundation::ComPtr<pmod::IEventModelEventArgs> spEventModelEventArgs;
        HRESULT hr = CMoEventModelEventArgsProxy::CreateInstance<CMoEventModelEventArgsProxy>
           (e,spEventModelEventArgs.GetAddressOf());
        IFR_ASSERT(hr);
        return this->m_pSourceInterface->Invoke(sender,spEventModelEventArgs);
    }
};

typedef 
    TMoEventSourceProxy<
        pmod::IPropertyChangedEventSource,
        pmod::IPropertyChangedEventHandler,
        ABI::Microsoft::PropertyModel::IPropertyChangedEventHandler,
        ABI::Microsoft::PropertyModel::IObservableObject,
        &ABI::Microsoft::PropertyModel::IObservableObject::add_PropertyChanged,
        &ABI::Microsoft::PropertyModel::IObservableObject::remove_PropertyChanged,
        CPropertyChangedEventHandlerProxy>
    CMoPropertyChangedEventSourceProxy;

typedef 
    TMoEventSourceProxy<
        pmod::IEventModelEventSource,
        pmod::IEventModelEventHandler,
        ABI::Microsoft::PropertyModel::IEventModelEventHandler,
        ABI::Microsoft::PropertyModel::IObservableObject,
        &ABI::Microsoft::PropertyModel::IObservableObject::add_EventModel,
        &ABI::Microsoft::PropertyModel::IObservableObject::remove_EventModel,
        CEventModelEventHandlerProxy>
    CMoEventModelEventSourceProxy;

//------------------------------------------------------------------------------
// Class:   CMoObservableObjectProxy
//
// Purpose: Implementation of pmod::IObservableObject
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoObservableObjectProxy :
    public TMoPropertyDataProxy
    <
        pmod::IObservableObject,
        ABI::Microsoft::PropertyModel::IObservableObject
    >,
    public foundation::IDictionary
{
protected:
    CMoObservableObjectProxy():
        _pInnerMap(nullptr)
    {
    }
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == foundation::IDictionary::GetIID())
        {
            *ppInterface = static_cast<foundation::IDictionary *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    void FinalRelease() override
    {
        RELEASE_INTERFACE(_pInnerMap);
        _spPropertyChangedEventSource.Release();
        _spEventModelEventSource.Release();
        __super::FinalRelease();
    }

    // Interface pmod::IObservableObject

    // Event methods.
    STDMETHOD(GetPropertyChangedEventSource)(_COM_Outptr_ pmod::IPropertyChangedEventSource** ppEventSource)
    {
        if(_spPropertyChangedEventSource == nullptr)
        {
            HRESULT hr =  CMoPropertyChangedEventSourceProxy::CreateInstance<CMoPropertyChangedEventSourceProxy>
            (this->m_pSourceInterface,_spPropertyChangedEventSource.GetAddressOf());
        }
        return _spPropertyChangedEventSource.CopyTo(ppEventSource);
    }

    STDMETHOD(GetEventModelEventSource)(_COM_Outptr_ pmod::IEventModelEventSource** ppEventSource)
    {
        if(_spEventModelEventSource == nullptr)
        {
            HRESULT hr =  CMoEventModelEventSourceProxy::CreateInstance<CMoEventModelEventSourceProxy>
                (this->m_pSourceInterface,_spEventModelEventSource.GetAddressOf());
        }
        return _spEventModelEventSource.CopyTo(ppEventSource);
    }

    // Interface foundation::IDictionary
    STDMETHOD(GetView)(UINT32* size,foundation::IDictionaryPair ***ppProperties) override
    {
        IFR_ASSERT(EnsureInnerMap());

        foundation::ComPtr<ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,IInspectable *> *>> spIterable;
        IFR_ASSERT(foundation::QueryWeakReference(this->_pInnerMap,spIterable.GetAddressOf()));

        foundation::ComPtr<ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,IInspectable *> *>> spFirst;
        IFR_ASSERT(spIterable->First(spFirst.GetAddressOf()));

        std::vector<foundation::ComPtr<foundation::IDictionaryPair> > properties;

        boolean hasCurrent = false;
        spFirst->get_HasCurrent(&hasCurrent);
        while(hasCurrent)
        {
            foundation::ComPtr<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,IInspectable *>> spKvp;
            IFR_ASSERT(spFirst->get_Current(spKvp.GetAddressOf()));

            foundation::HStringPtr key;
            spKvp->get_Key(key.GetAddressOf());
            foundation::InspectablePtr spValue;
            spKvp->get_Value(spValue.GetAddressOf());
            foundation::InspectablePtr spCoreProxy;

            IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spValue,spCoreProxy.GetAddressOf()));

            foundation::ComPtr<foundation::IDictionaryPair> spPropertyNamePair;
            IFR_ASSERT(foundation::library::CreateDictionaryPair(
                key,
                spCoreProxy,
                spPropertyNamePair.GetAddressOf()));

            properties.push_back(spPropertyNamePair);

            spFirst->MoveNext(&hasCurrent);
        }
        // copy now to public API
        ContainerRefCountToArray(properties,size,ppProperties);
        return S_OK;
    }

    STDMETHOD(GetSize)(_Outptr_ UINT32* size) override
    {
        IFR_ASSERT(EnsureInnerMap());
        return _pInnerMap->get_Size(size);
    }

    STDMETHOD(Lookup)(HSTRING propertyName, IInspectable** value) override
    {
        IFCPTR_ASSERT(value);

        IFR_ASSERT(EnsureInnerMap());
        foundation::InspectablePtr spValue;
        IFR(_pInnerMap->Lookup(propertyName,spValue.GetAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spValue,value));
        return S_OK;
    }

    STDMETHOD(HasKey)(HSTRING propertyName, bool* found) override
    {
        IFCPTR_ASSERT(found);

        IFR_ASSERT(EnsureInnerMap());
        foundation::InspectablePtr spValue;
        boolean bFound;
        IFR(_pInnerMap->HasKey(propertyName, &bFound));
        *found = bFound ? true : false;
        return S_OK;
    }

    STDMETHOD(Insert)(HSTRING propertyName, IInspectable *value,bool* replaced) override
    {
        IFR_ASSERT(EnsureInnerMap());
        foundation::InspectablePtr spValue;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxy(value, spValue.GetAddressOf()));
        boolean bReplaced;
        IFR(_pInnerMap->Insert(propertyName, spValue, &bReplaced));
        *replaced = bReplaced ? true : false;
        return S_OK;
    }

    STDMETHOD(Remove)(HSTRING propertyName) override
    {
        IFR_ASSERT(EnsureInnerMap());
        IFR(_pInnerMap->Remove(propertyName));
        return S_OK;
    }

    STDMETHOD(Clear)() override
    {
        IFR_ASSERT(EnsureInnerMap());
        IFR(_pInnerMap->Clear());
        return S_OK;
    }

    STDMETHOD(Keys)(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first) override
    {
        return E_NOTIMPL;
    }

private:
    HRESULT EnsureInnerMap()
    {
        if(_pInnerMap == nullptr)
        {
            IFR_ASSERT(foundation::QueryInterface(this->m_pSourceInterface,&_pInnerMap));
        }
        return S_OK;
    }
private:
    ABI::Windows::Foundation::Collections::__FIMap_2_HSTRING_IInspectable_t *_pInnerMap;
    foundation::ComPtr<pmod::IPropertyChangedEventSource>    _spPropertyChangedEventSource;
    foundation::ComPtr<pmod::IEventModelEventSource>         _spEventModelEventSource;
};

//------------------------------------------------------------------------------
// Class:   TBaseObservableObjectAggregationProxy
//
// Purpose: Base Aggregation Proxy
//------------------------------------------------------------------------------
template <class TBASE>
class TBaseObservableObjectAggregationProxy :
        public TBASE
{
protected:
    TBaseObservableObjectAggregationProxy():
        m_pInnerUnknown(nullptr)
    {
    }

protected:
    // Overrides from ComBase
    void FinalRelease() override
    {
        RELEASE_INTERFACE(m_pInnerUnknown);
        TBASE::FinalRelease();
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        HRESULT hr = S_OK;
        if(IsPropertyModelInterface(iid))
        {
            return QueryObservableObjectInterface(iid,ppInterface);
        }
        else
        {
            hr = TBASE::QueryInterfaceImpl(iid, ppInterface);
        }
        return hr;
    }
    // If an iid can be QI into our Proxy Property Model
    bool IsPropertyModelInterface(REFIID iid)
    {
        return iid == pmod::IObservableObject::GetIID();
    }

    bool IsPropertyModelAggregateCreated()
    {
        return m_pInnerUnknown != nullptr;
    }

    // route a QI to the aggregated Property Model object
    HRESULT QueryObservableObjectInterface(REFIID iid, IUnknown **ppInterface)
    {
        IFR(EnsureCreatePropertyModelProxyAggregation());
        return foundation::QueryWeakReference(m_pInnerUnknown,iid,ppInterface);
    }

    // Ensure creating a Property Model Proxy aggregation
    HRESULT EnsureCreatePropertyModelProxyAggregation()
    {
        if(m_pInnerUnknown == nullptr)
        {
            foundation::ComPtr<ABI::Microsoft::PropertyModel::IObservableObject> spMoPropertyModel;
            IFR_ASSERT(foundation::QueryInterface(this->m_pSourceInterface,spMoPropertyModel.GetAddressOf()));

            IFR_ASSERT(CMoObservableObjectProxy::CreateAggregableInstance<CMoObservableObjectProxy>(
                    (foundation::ctl::ComInspectableBase *)this,    // pOuter
                    spMoPropertyModel,
                    &m_pInnerUnknown));                 // ppNewInstance
        }
        return S_OK;
    }
private:
    IInspectable        *m_pInnerUnknown;             // Inner IUnknown to a Proxy Property Model
};
