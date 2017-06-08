/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectClassAdapter.cpp
****/

#include "pch.h"

#include "MoObservableObjectClassAdapter.h"
#include "MoObservableObjectFactory.h"


// This class adapts from code delegate callbkack to the winrt version of it to allow overriding from winrt code.
class CInvokeDelegate :
    public foundation::ctl::Implements <pmod::library::IMethodOnInvokeDelegate, &pmod::library::IID_IMethodOnInvokeDelegate >
{
public:
 
    HRESULT _Initialize(ABI::Microsoft::PropertyModel::Library::IInvokeDelegate *pInvokeMethodDelegate)
    {
        _spMoOnInvokeMethodDelegate = pInvokeMethodDelegate;
        return S_OK;
    }

    static HRESULT GetModernDelegateFromCore(pmod::library::IMethodOnInvokeDelegate *pDelegate,
        ABI::Microsoft::PropertyModel::Library::IInvokeDelegate **ppModernDelegate)
    {
        foundation_assert(ppModernDelegate);
        if (pDelegate)
        {
            CInvokeDelegate * pInternalDelegate = static_cast<CInvokeDelegate *>(pDelegate);
            pInternalDelegate->_spMoOnInvokeMethodDelegate.CopyTo(ppModernDelegate);
        }
        else
        {
            *ppModernDelegate = nullptr;
        }
        return S_OK;
    }

protected:
    STDMETHOD(Invoke) (
        _In_ foundation::IMethodInfo *pMethodInfo, 
        _In_ UINT32 methodId, 
        _In_ UINT32 size, 
        _In_ foundation::IInspectable **parameters, 
        _Outptr_ foundation::IInspectable **ppValue)
    {
        bool isAsync;
        pMethodInfo->GetIsAsync(&isAsync);

        if (isAsync)
        {
            // winRT is not able to see the IAsyncOperation we are passing
            std::vector<IInspectable *> parameters_;
            IFR_ASSERT(CMoObservableObjectDelegateAdapter::CreateAsyncParameters(size, parameters, *ppValue, parameters_));
            foundation::InspectablePtr ignore_result_ptr;
            return _spMoOnInvokeMethodDelegate->Invoke(
                methodId,
                size + 1,
                (IInspectable **)&parameters_.front(),
                ignore_result_ptr.GetAddressOf());
        }
        else
        {
            return _spMoOnInvokeMethodDelegate->Invoke(methodId, size, parameters, ppValue);
        }
    }

private:
    foundation::ComPtr<ABI::Microsoft::PropertyModel::Library::IInvokeDelegate> _spMoOnInvokeMethodDelegate;
};


HRESULT STDMETHODCALLTYPE CMoObservableObjectClassAdapter::put_InvokeDelegate(ABI::Microsoft::PropertyModel::Library::IInvokeDelegate* value)
{
    foundation::ComPtr<pmod::library::IMethodOnInvokeDelegate> spOnInvokeMethodDelegate;
    if (value != nullptr)
    {
        IFR_ASSERT(foundation::ctl::CreateInstanceWithInitialize<CInvokeDelegate>(spOnInvokeMethodDelegate.GetAddressOf(), value));
    }
    return m_pInner->SetMethodOnInvokeDelegate(spOnInvokeMethodDelegate);
}

HRESULT STDMETHODCALLTYPE CMoObservableObjectClassAdapter::get_InvokeDelegate(ABI::Microsoft::PropertyModel::Library::IInvokeDelegate** value)
{
    foundation::ComPtr<pmod::library::IMethodOnInvokeDelegate> spOnInvokeMethodDelegate;
    IFR_ASSERT(m_pInner->GetMethodOnInvokeDelegate(spOnInvokeMethodDelegate.GetAddressOf()));
    return CInvokeDelegate::GetModernDelegateFromCore(spOnInvokeMethodDelegate, value);
}

HRESULT STDMETHODCALLTYPE CMoObservableObjectClassAdapter::SetPropertyInternal(unsigned int propertyId, IInspectable *value)
{
    return m_pInner->SetPropertyInternal(propertyId, value, true);
}

HRESULT STDMETHODCALLTYPE CMoObservableObjectClassAdapter::FirePropertyChanged(unsigned int propertyId, boolean useCallback)
{
    return m_pInner->FirePropertyChanged(propertyId, useCallback ? true : false);
}

HRESULT STDMETHODCALLTYPE CMoObservableObjectClassAdapter::FireEventModel(unsigned int eventId, IInspectable *eventArgs)
{
    return m_pInner->FireEventModel(eventId, eventArgs);
}

HRESULT STDMETHODCALLTYPE CMoObservableObjectClassAdapter::SetBinding(
    unsigned int propertyId,
    IBindingBase *propertyBinding) {
    IFCPTR(propertyBinding);

    foundation::ComPtr<pmod::IBindingBase> spPropertyBinding;
    IFR(foundation::QueryInterface(propertyBinding, spPropertyBinding.GetAddressOf()));
    return m_pInner->SetBinding(propertyId, spPropertyBinding);
}

HRESULT STDMETHODCALLTYPE CMoObservableObjectClassAdapter::ClearBinding(
    unsigned int propertyId)
{
    return m_pInner->ClearBinding(propertyId);
}

