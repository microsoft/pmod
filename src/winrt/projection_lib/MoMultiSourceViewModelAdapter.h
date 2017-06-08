/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceViewModelAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoMultiSourcePropertyViewModelAdapter
//
// Purpose: Implementation of IMultiSourcePropertyViewModel Interface
//
//------------------------------------------------------------------------------
class CMoMultiSourcePropertyViewModelAdapter :
    public foundation::AdapterBase
    <
        pmod::library::IObservableObjectClass,
        IMultiSourceViewModel,
        &__uuidof(IMultiSourceViewModel)
    >
{
public:
};
//------------------------------------------------------------------------------
// Class:   CMoMultiSourcePropertyViewModelOverridesAdapter
//
// Purpose: Implementation of IMultiSourcePropertyViewModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoMultiSourcePropertyViewModelOverridesAdapter :
    public foundation::AdapterBase
        <
        pmod::library::IObservableObjectClass,
        IMultiSourceViewModelOverrides,
        &__uuidof(IMultiSourceViewModelOverrides)
        >
{
public:
    HRESULT STDMETHODCALLTYPE OnMultiSourceSourceChanged( 
        IObservableObject *pSource,
        int propertyId,
        IInspectable *oldValue,
        IInspectable *newValue) override
    {
        UNREFERENCED_PARAMETER(pSource);
        UNREFERENCED_PARAMETER(propertyId);
        UNREFERENCED_PARAMETER(oldValue);
        UNREFERENCED_PARAMETER(newValue);
        return S_OK;
    }
};

