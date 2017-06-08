/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectFactory.cpp
****/

#include "pch.h"

#include "MoObservableObjectFactory.h"

#include <pmod/library/library_util.h>
#include <pmod/library/observable_statics_util.h>

//------------------------------------------------------------------------------
// Class:   CMoBatchEventHandlerAdapter
//
// Purpose: Implementation of IBatchPropertyChangedEventHandler Interface Adapter
//
//------------------------------------------------------------------------------
class CMoBatchEventHandlerAdapter : 
    public foundation::ctl::ComBase,
    public IPropertyChangedEventHandler,
    public IBatchPropertyChangedEventHandler
{
public:
    static HRESULT CreateInstance(
        IPropertyChangedEventHandler *pPropertyChangedEventHandler,
        IBatchPropertyChangedEventHandler *pBatchPropertyChangedEventHandler,
        IPropertyChangedEventHandler **ppPropertyChangedEventHandler)
    {
        CMoBatchEventHandlerAdapter *pMoBatchEventHandlerAdapter = nullptr;
        foundation::ctl::ComObject<CMoBatchEventHandlerAdapter>::CreateInstance(&pMoBatchEventHandlerAdapter);
        pMoBatchEventHandlerAdapter->_spPropertyChangedEventHandler = pPropertyChangedEventHandler;
        pMoBatchEventHandlerAdapter->_spBatchPropertyChangedEventHandler = pBatchPropertyChangedEventHandler;
        *ppPropertyChangedEventHandler = pMoBatchEventHandlerAdapter;
        return S_OK;
    }
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == __uuidof(IPropertyChangedEventHandler))
        {
            *ppInterface = static_cast<IPropertyChangedEventHandler *>(this);
        }
        else if (iid == __uuidof(IBatchPropertyChangedEventHandler))
        {
            *ppInterface = static_cast<IBatchPropertyChangedEventHandler *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // IPropertyChangedEventHandler
    HRESULT STDMETHODCALLTYPE Invoke(
        IInspectable *sender,
        IPropertyChangedEventArgs *e) override
    {
        return _spPropertyChangedEventHandler->Invoke(sender, e);
    }

    // IBatchPropertyChangedEventHandler
    HRESULT STDMETHODCALLTYPE Invoke(
        IInspectable *sender,
        IBatchPropertyChangedEventArgs *e) override
    {
        return _spBatchPropertyChangedEventHandler->Invoke(sender, e);
    }
private:
    foundation::ComPtr<IPropertyChangedEventHandler> _spPropertyChangedEventHandler;
    foundation::ComPtr<IBatchPropertyChangedEventHandler> _spBatchPropertyChangedEventHandler;
};

//------------------------------------------------------------------------------
// Class:   CMoObservableObjectDelegateAdapter
//
// Purpose: Implementation of IObservableObjectDelegate Interface Adapter
//
//------------------------------------------------------------------------------
STDMETHODIMP CMoObservableObjectDelegateAdapter::OnCreatePropertiesContainer(
    _In_ pmod::IObservableObjectInfo* pPropertyModelInfo,
    _In_ UINT32 size,
    _COM_Outptr_ foundation::library::IPropertiesContainer **ppValue)
{
    UNREFERENCED_PARAMETER(pPropertyModelInfo);
    UNREFERENCED_PARAMETER(size);
    *ppValue = nullptr;
    return S_OK;
}

STDMETHODIMP CMoObservableObjectDelegateAdapter::OnGetProperty(_In_ UINT32 propertyId,_Inout_ IInspectable **ppValue)
{
    foundation_assert(ppValue);
    if(IsOverrideComposed())
    {
        IObservableObjectOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));
        foundation::ComPtr<IInspectable> spValue;
        IFR(pOverrides->OnGetProperty(propertyId,spValue.GetAddressOf()));
        if (spValue != nullptr)
        {
            // this is needed since the CORE API will pass a value
            if(*ppValue)
            {
                (*ppValue)->Release();
                *ppValue = nullptr;
            }
            return spValue.CopyTo(ppValue);
        }
    }
    // we could resolve the property
    return S_FALSE;
}

STDMETHODIMP CMoObservableObjectDelegateAdapter::OnSetProperty(
    _In_ UINT32 propertyId,
    _In_ bool isPublic,
    _In_ IInspectable *oldValue,
    _In_ IInspectable *newValue)
{
    if(IsOverrideComposed())
    {
        IObservableObjectOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));

        IFR(pOverrides->OnSetProperty(propertyId, isPublic ? TRUE:FALSE,oldValue, newValue));
        return S_OK;
    }
    else
    {
        return S_OK;
    }	
}

STDMETHODIMP CMoObservableObjectDelegateAdapter::OnInvokeMethod(
    _In_ foundation::IMethodInfo *pMethodInfo,
    _In_ UINT32 methodId,
    _In_ UINT32 size,
    _In_ IInspectable **parameters,
    _Outptr_ IInspectable **ppValue)
{
    UNREFERENCED_PARAMETER(pMethodInfo);

    if(IsOverrideComposed())
    {
        HRESULT hr;
        IObservableObjectOverrides *pOverrides;
        IFHR(QueryOverrides(pOverrides));

        bool isAsync;
        pMethodInfo->GetIsAsync(&isAsync);

        if (isAsync)
        {
            // winRT is not able to see the IAsyncOperation we are passing
            std::vector<IInspectable *> parameters_;
            IFR_ASSERT(CreateAsyncParameters(size, parameters, *ppValue, parameters_));
            foundation::InspectablePtr ignore_result_ptr;
            hr = pOverrides->OnInvoke(methodId, 
                size + 1, 
                (IInspectable **)&parameters_.front(), 
                ignore_result_ptr.GetAddressOf());
        }
        else
        {
            hr = pOverrides->OnInvoke(methodId, size, parameters, ppValue);
        }
        IFHR(hr);
        return S_OK;
    }
    else
    {
        return E_NOTIMPL;
    }
}

HRESULT CMoObservableObjectDelegateAdapter::CreateAsyncParameters(
    _In_ UINT32 size,
    _In_ IInspectable **parameters,
    _In_ IInspectable *pAsyncOperation,
    _Out_ std::vector<IInspectable *>& parameters_)
{
    parameters_.resize(size + 1);
    for (UINT32 index = 0; index < size; ++index)
    {
        parameters_[index] = parameters[index];
    }
    foundation_assert(pAsyncOperation != nullptr);
    IAsyncOperation *pMoAsyncOperation = nullptr;
    IFR_ASSERT(foundation::QueryWeakReference(pAsyncOperation, &pMoAsyncOperation));
    parameters_[size] = pMoAsyncOperation;
    return S_OK;
}


STDMETHODIMP MoObservableObjectFactory::CreateInstance(
        _In_ ObservableObjectOptions modelOptions,
        _In_opt_ IInspectable* pOuter,
        _COM_Outptr_ IInspectable** ppInner,
        _COM_Outptr_ IObservableObject** ppInstance)
{
    return CreateInstanceWithType(GUID_NULL,modelOptions,pOuter,ppInner,ppInstance);
}

STDMETHODIMP MoObservableObjectFactory::CreateInstanceWithType(
        GUID iidType,
        _In_ ObservableObjectOptions modelOptions,
        _In_opt_ IInspectable* pOuter,
        _COM_Outptr_ IInspectable** ppInner,
        _COM_Outptr_ IObservableObject** ppInstance)
{
    // Find the Type...
    if (!::IsEqualIID(iidType, GUID_NULL) && !foundation::IsTypeInfoRegistered(iidType))
    {
        return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
    }
    HRESULT hr;

    foundation::ComPtr<CMoObservableObjectDelegateAdapter,foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    pmod::library::ObservableObjectCreateParameters param;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_options = (pmod::library::ObservableObjectOptions)modelOptions;

    foundation::InspectablePtr spInner;
    hr =  pmod::library::GetObservableObjectClassFactory()->
        CreateObservableObjectInstance(&param,pOuter,spInner.GetAddressOf());

    IFHR(hr);

    IFHR(foundation::QueryInterface(spInner,ppInstance));

    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}

STDMETHODIMP MoObservableObjectFactory::GetPropertyValue( 
                IObservableObject *pObservableObject,
                IInspectable *pPath,
                IInspectable **ppValue)
{
    IFCPTR(pObservableObject);
    IFCPTR(pPath);
    IFCPTR(ppValue);

    foundation::ComPtr<pmod::IObservableObject> spPropertyModel;
    IFR_ASSERT(foundation::QueryInterface(pObservableObject,spPropertyModel.GetAddressOf()));

    IFR(pmod::library::get_instance()->GetPropertyValue(spPropertyModel, pPath, ppValue));
    return S_OK;
}

STDMETHODIMP MoObservableObjectFactory::CreateBatchEventHandler(
    IPropertyChangedEventHandler *pPropertyChangedEventHandler,
    IBatchPropertyChangedEventHandler *pBatchPropertyChangedEventHandler,
    IPropertyChangedEventHandler **ppPropertyChangedEventHandler)
{
    IFCPTR(pPropertyChangedEventHandler);
    IFCPTR(pBatchPropertyChangedEventHandler);
    IFCPTR(ppPropertyChangedEventHandler);

    return CMoBatchEventHandlerAdapter::CreateInstance(
        pPropertyChangedEventHandler,
        pBatchPropertyChangedEventHandler,
        ppPropertyChangedEventHandler);
}
