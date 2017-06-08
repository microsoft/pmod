/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModel.h
****/

#pragma once

#include "MoBase.h"
#include "Microsoft.Internal.PropertyModel.h"
#include "MoEventHandler.h"
#include "MoInteropHelper.h"
#include "MoSourceModelDelegateAdapter.h"

//------------------------------------------------------------------------------
// Class:   CMoPropertyViewModelDelegateAdapter
//
// Purpose: Implementation of IViewModelDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoPropertyViewModelDelegateAdapter :
    public CMoSourceModelDelegateAdapter<IViewModelDelegate,IPropertyChangedEventHandler,IObservableObject>
{
protected:
     // IViewModelDelegate Implementation
    STDMETHOD( OnSourcePropertyChanged )(__in int propertyId,__in const VARIANT& oldValue,__in const VARIANT& newValue)
    {
        if(IsOverrideComposed())
        {
            ABI::Microsoft::Internal::PropertyModel::IPropertyViewModelOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));

            CComPtr<IInspectable> spOldValue,spNewValue;
            IFR(CMoInteropHelper::ToItem(oldValue,&spOldValue));
            IFR(CMoInteropHelper::ToItem(newValue,&spNewValue));
            IFR(pOverrides->OnSourcePropertyChanged(propertyId,spOldValue,spNewValue));
        }
        return S_OK;
    }
};
