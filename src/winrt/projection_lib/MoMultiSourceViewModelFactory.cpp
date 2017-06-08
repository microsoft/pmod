/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceViewModelFactory.cpp
****/

#include "pch.h"

#include "MoMultiSourceViewModelFactory.h"
#include "MoObservableObjectFactory.h"
#include "MoSourceModelDelegateAdapter.h"

#include <pmod/library/library_util.h>

//------------------------------------------------------------------------------
// Class:   CMoMultiSourceViewModelDelegateAdapter
//
// Purpose: Implementation of IMultiSourceViewModelDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoMultiSourceViewModelDelegateAdapter :
    public CMoSourceModelBaseDelegateAdapter
    <
    pmod::library::IMultiSourceViewModelDelegate,
    pmod::IPropertyChangedEventHandler,
    pmod::IObservableObject>
{
protected:
     // IMultiSourceCollectionViewModelDelegate Implementation
    STDMETHOD( OnSourcePropertyChanged )(
        _In_ pmod::IObservableObject *pSource,
        _In_ UINT32 propertyId,
        _In_ IInspectable *oldValue,
        _In_ IInspectable *newValue) override
    {
        if(IsOverrideComposed())
        {
            IMultiSourceViewModelOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));

            foundation::ComPtr<IObservableObject> spMoPropertyModel;
            IFR(foundation::QueryInterface(pSource,spMoPropertyModel.GetAddressOf()));

            IFR(pOverrides->OnMultiSourceSourceChanged(spMoPropertyModel,propertyId,oldValue,newValue));
        }
        return S_OK;
    }
};

STDMETHODIMP MoMultiSourcePropertyViewModelFactory::CreateInstance( 
        ObservableObjectOptions modelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        IMultiSourceViewModel **ppInstance)

{
    return CreateInstanceWithType( 
        GUID_NULL,
        modelOptions,
        pOuter,
        ppInner,
        ppInstance);
}

STDMETHODIMP MoMultiSourcePropertyViewModelFactory::CreateInstanceWithType( 
        GUID iidType,
        ObservableObjectOptions modelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        IMultiSourceViewModel **ppInstance)
{
	// Find the Type...
    if (!::IsEqualIID(iidType, GUID_NULL) && !foundation::IsTypeInfoRegistered(iidType))
	{
		return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
	}
		HRESULT hr;

    foundation::ComPtr<CMoObservableObjectDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    foundation::ComPtr<CMoMultiSourceViewModelDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spMultiSourcePropertyViewModelDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spMultiSourcePropertyViewModelDelegateAdapter.GetAddressOf()));

    pmod::library::MultiSourceViewModelCreateParameters param;
    param.m_options = (pmod::library::ObservableObjectOptions)modelOptions;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_p_viewmodel_delegate = spMultiSourcePropertyViewModelDelegateAdapter;

    foundation::ComPtr<IInspectable> spInner;
    hr = pmod::library::GetMultiSourceViewModelClassFactory()->
        CreateMultiSourceViewModelInstance(&param,pOuter,spInner.GetAddressOf());
	IFHR(hr);
    IFHR(foundation::QueryInterface(spInner,ppInstance));
    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
	return S_OK;
}
