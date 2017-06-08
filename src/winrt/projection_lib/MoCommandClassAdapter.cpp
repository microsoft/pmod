/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandClassAdapter.cpp
****/

#include "pch.h"

#include "MoCommandClassAdapter.h"
#include "MoCommandFactory.h"
//#include "../mocoreproxy/MoCoreProxyFactory.h"

class CCanExecuteDelegateAdapter : 
    public foundation::ctl::Implements <pmod::library::ICanExecuteDelegate,&pmod::library::IID_ICanExecuteDelegate >
{
public:
    static HRESULT CreateInstance(
        ABI::Microsoft::PropertyModel::Library::ICanExecuteDelegate *pCanExecuteDelegate,
        pmod::library::ICanExecuteDelegate **ppCanExecuteDelegate)
    {
        foundation_assert(pCanExecuteDelegate != nullptr);

        CCanExecuteDelegateAdapter *pCanExecuteDelegateAdapter = nullptr;
        foundation::ctl::ComObject<CCanExecuteDelegateAdapter>::CreateInstance(&pCanExecuteDelegateAdapter);
        pCanExecuteDelegateAdapter->_spMoCanExecuteDelegate = pCanExecuteDelegate;
        *ppCanExecuteDelegate = static_cast<pmod::library::ICanExecuteDelegate *>(pCanExecuteDelegateAdapter);
        return S_OK;
    }
protected:
    STDMETHOD(Invoke)(_In_ foundation::IInspectable *parameter, _Outptr_ bool *pValue)
    {
        boolean bValue;

        HRESULT hr = S_OK;
        IFHR(_spMoCanExecuteDelegate->Invoke(parameter, &bValue));
        *pValue = bValue ? true : false;
        return hr;
    }
private:
    foundation::ComPtr<ABI::Microsoft::PropertyModel::Library::ICanExecuteDelegate> _spMoCanExecuteDelegate;
};

class CExecuteDelegateAdapter :
    public foundation::ctl::Implements <pmod::library::IExecuteDelegate, &pmod::library::IID_IExecuteDelegate >
{
public:
    static HRESULT CreateInstance(
        ABI::Microsoft::PropertyModel::Library::IExecuteDelegate *pExecuteDelegate,
        pmod::library::IExecuteDelegate **ppCanExecuteDelegate)
    {
        foundation_assert(pExecuteDelegate != nullptr);

        CExecuteDelegateAdapter *pExecuteDelegateAdapter = nullptr;
        foundation::ctl::ComObject<CExecuteDelegateAdapter>::CreateInstance(&pExecuteDelegateAdapter);
        pExecuteDelegateAdapter->_spMoExecuteDelegate = pExecuteDelegate;
        *ppCanExecuteDelegate = static_cast<pmod::library::IExecuteDelegate *>(pExecuteDelegateAdapter);
        return S_OK;
    }
protected:
    STDMETHOD(Invoke)(
        _In_ foundation::IInspectable *parameter, 
        _Outptr_ foundation::IInspectable* *ppResult)
    {
        foundation_assert(ppResult != nullptr);

        if (*ppResult != nullptr)
        {
            // we assume is a IAsyncOperation
            foundation::InspectablePtr parameter_;
            IFR_ASSERT(CMoCommandDelegateAdapter::CreateAsyncParameters(parameter, *ppResult, parameter_.GetAddressOf()));
            foundation::InspectablePtr ignore_result_ptr;
            HRESULT hr = S_OK;
            IFHR(_spMoExecuteDelegate->Invoke(parameter_, ignore_result_ptr.GetAddressOf()));
            return hr;
        }
        else
        {
            // non async
            return _spMoExecuteDelegate->Invoke(parameter, ppResult);
        }
    }
private:
    foundation::ComPtr<ABI::Microsoft::PropertyModel::Library::IExecuteDelegate> _spMoExecuteDelegate;
};


// Interface ICommandModelProtected
HRESULT STDMETHODCALLTYPE CMoCommandClassAdapter::SetCommandState( int commandState,boolean fireCanExecute) 
{
    return m_pInner->SetCommandState(commandState,fireCanExecute ? true:false);
}
                    
HRESULT STDMETHODCALLTYPE CMoCommandClassAdapter::FireCanExecuteChanged( void)
{
    return m_pInner->FireCanExecuteChanged();
}

HRESULT STDMETHODCALLTYPE CMoCommandClassAdapter::get_CanExecuteDelegate(ICanExecuteDelegate **value)
{

    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CMoCommandClassAdapter::put_CanExecuteDelegate(ICanExecuteDelegate *value)
{
    foundation::ComPtr<pmod::library::ICanExecuteDelegate> spCanExecuteDelegate;
    if (value != nullptr)
    {
        IFR_ASSERT(CCanExecuteDelegateAdapter::CreateInstance(value, spCanExecuteDelegate.GetAddressOf()));
    }
    return m_pInner->SetCanExecuteDelegate(spCanExecuteDelegate);
}

HRESULT STDMETHODCALLTYPE CMoCommandClassAdapter::get_ExecuteDelegate(IExecuteDelegate **value)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CMoCommandClassAdapter::put_ExecuteDelegate(IExecuteDelegate *value)
{
    foundation::ComPtr<pmod::library::IExecuteDelegate> spExecuteDelegate;
    if (value != nullptr)
    {
        IFR_ASSERT(CExecuteDelegateAdapter::CreateInstance(value, spExecuteDelegate.GetAddressOf()));
    }
    return m_pInner->SetExecuteDelegate(spExecuteDelegate);
}
