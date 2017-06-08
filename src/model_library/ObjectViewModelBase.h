/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectViewModelBase.h
****/

#pragma once

#include "ViewModelBase.h"
#include <pmod/interfaces.h>


template <class TSourceBase,class TDelegate,class TModelBase = CObservableObject>
class CObjectViewModelBase :
    public CViewModelSourceBase
    <
        TModelBase,
        TSourceBase,
        pmod::IPropertyChangedEventHandler,
        TDelegate
    >
{

protected:
    HRESULT OnSourcePropertyChangedHandler(foundation::IUnknown* pSender,pmod::IPropertyChangedEventArgs* pArgs)
    {
        UINT32 propertyId;
        IFR(pArgs->GetProperty(&propertyId));
        foundation::InspectablePtr oldValue,newValue;
        IFR(pArgs->GetOldValue(oldValue.GetAddressOf()));
        IFR(pArgs->GetNewValue(newValue.GetAddressOf()));

        foundation::ComPtr<pmod::IObservableObject> spSource;
        IFR(foundation::QueryInterface(pSender,spSource.GetAddressOf()));
        return OnSourcePropertyChanged(spSource,propertyId,oldValue,newValue);
    }

    // Pure methods
    virtual HRESULT OnSourcePropertyChanged(
        pmod::IObservableObject* pSource,
        UINT32 propertyId,
        foundation::InspectablePtr& oldValue,
        foundation::InspectablePtr& newValue) = 0;

};


