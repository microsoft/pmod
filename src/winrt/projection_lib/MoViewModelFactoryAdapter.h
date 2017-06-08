/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModelFactoryAdapter.h
****/

#pragma once

#include "MoBaseAdapter.h"

#include "Microsoft.PropertyModel.Library.h"

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;


// CMoCreateSourceModelInstanceAdapter Class
class CMoCreateSourceModelInstanceAdapter:
    public foundation::ctl::Implements
    <
        pmod::ICreateSingleSourceModelDelegate,
        &pmod::IID_ICreateSingleSourceModelDelegate
    >
{
public:
    static HRESULT CreateInstance(
        ICreateSourceModelInstance *pMoCreateSourceModelInstance,
        pmod::ICreateSingleSourceModelDelegate **ppCreateSourceModelDelegate)
    {
        foundation_assert(pMoCreateSourceModelInstance);

        foundation::ctl::ComObject<CMoCreateSourceModelInstanceAdapter> *pMoCreateSourceModelInstanceAdapter = nullptr;
        IFR(foundation::ctl::ComObject<CMoCreateSourceModelInstanceAdapter>::CreateInstance(&pMoCreateSourceModelInstanceAdapter));
        pMoCreateSourceModelInstanceAdapter->m_MoCreateSourceModelInstance = pMoCreateSourceModelInstance;
        *ppCreateSourceModelDelegate = static_cast<pmod::ICreateSingleSourceModelDelegate *>(pMoCreateSourceModelInstanceAdapter);
        return S_OK;
    }
protected:
    STDMETHOD (Invoke)(_In_ foundation::IObject *pModel,_COM_Outptr_ pmod::ISourceModel **ppSourceModel) override
    {
        foundation_assert(m_MoCreateSourceModelInstance);
        foundation::ComPtr<IObject> spMoModel;

        IFR(foundation::QueryInterface(pModel,spMoModel.GetAddressOf()));
        foundation::ComPtr<ISourceModel> spMoSourceModel;

        IFR(m_MoCreateSourceModelInstance->Invoke(spMoModel,spMoSourceModel.GetAddressOf()));
        IFR(foundation::QueryInterface(spMoSourceModel,ppSourceModel));
        return S_OK;
    }

private:
    foundation::ComPtr<ICreateSourceModelInstance> m_MoCreateSourceModelInstance;
};

//------------------------------------------------------------------------------
// Class:   CMoViewModelFactoryAdapter
//
// Purpose: Implementation of IViewModelFactory Interface
//
//------------------------------------------------------------------------------
class CMoViewModelFactoryAdapter :
    public CMoBaseAdapter
    <
        pmod::IViewModelFactory,
        ABI::Microsoft::PropertyModel::IViewModelFactory,
        IViewModelCreatedEventHandler,
        IViewModelCreatedEventArgs
    >
{
public:
    // Interface IViewModelFactory
    HRESULT STDMETHODCALLTYPE RegisterSingleSourceViewModel( 
        GUID iidModelType,
        ICreateSourceModelInstance *pCreator)
    {
        IFCPTR(pCreator);
        foundation::ComPtr<pmod::ICreateSingleSourceModelDelegate> spCreateSingleSourceModelDelegate;
        IFR(CMoCreateSourceModelInstanceAdapter::CreateInstance(pCreator,spCreateSingleSourceModelDelegate.GetAddressOf()));

        IFR(m_pInner->RegisterSingleSourceViewModelCreator(iidModelType,spCreateSingleSourceModelDelegate));
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE CreateSingleSourceViewModel( 
        IObject *pSource,
        ISourceModel **ppNewInstance)
    {
        IFCPTR(pSource);
        IFCPTR(ppNewInstance);
        return E_NOTIMPL;
    }
                        
    HRESULT STDMETHODCALLTYPE add_ViewModelCreated( 
        IViewModelCreatedEventHandler *value,
        EventRegistrationToken *token)
    {
        return this->add_Handler(value,token);
    }
                        
    HRESULT STDMETHODCALLTYPE remove_ViewModelCreated( 
        EventRegistrationToken token)
    {
        return this->remove_Handler(token);
    }

};


//------------------------------------------------------------------------------
// Class:   CMoViewModelFactoryProtectedAdapter
//
// Purpose: Implementation of IViewModelFactoryProtected Interface
//
//------------------------------------------------------------------------------
class CMoViewModelFactoryProtectedAdapter :
    public foundation::AdapterBase
        <
        pmod::library::IViewModelFactoryClass,
        IViewModelFactoryProtected,
        &__uuidof(IViewModelFactoryProtected)
        >
{
public:
    // Interface IViewModelFactoryProtected
protected:
    
};

//------------------------------------------------------------------------------
// Class:   CMoViewModelFactoryOverridesAdapter
//
// Purpose: Implementation of IPropertyBindingOverrides Interface
//
//------------------------------------------------------------------------------
class CMoViewModelFactoryOverridesAdapter :
    public foundation::AdapterBase
        <
        pmod::library::IViewModelFactoryClass,
        IViewModelFactoryOverrides,
        &__uuidof(IViewModelFactoryOverrides)
        >
{
public:
    // Interface IViewModelFactoryOverrides
};

//------------------------------------------------------------------------------
// Class:   CMoViewModelCreatedEventArgsAdapter
//
// Purpose: Implementation of IObjectCreatedEventArgs Interface
//
//------------------------------------------------------------------------------
class CMoViewModelCreatedEventArgsAdapter :
    public foundation::AdapterBase
        <
        pmod::IViewModelCreatedEventArgs,
        IViewModelCreatedEventArgs,
        &__uuidof(IViewModelCreatedEventArgs)
        >
{
public:
    HRESULT STDMETHODCALLTYPE get_Object( 
        IObject **value) override
    {
        IFCPTR(value);
        foundation::ComPtr<foundation::IObject> spObject;
        IFR(m_pInner->GetObject(spObject.GetAddressOf()));
        IFR(foundation::QueryInterface(spObject, value));
        return S_OK;
    }
};
