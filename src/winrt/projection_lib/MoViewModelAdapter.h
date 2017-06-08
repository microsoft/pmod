/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModelAdapter.h
****/

#pragma once

#include "MoSourceModelDelegateAdapter.h"

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoPropertyViewModelAdapter
//
// Purpose: Implementation of IPropertyViewModel Interface
//
//------------------------------------------------------------------------------
class CMoPropertyViewModelAdapter :
    public foundation::AdapterBase<
        pmod::library::IViewModelClass,
        IViewModel,
        &__uuidof(IViewModel)
        >
{
public:
};


//------------------------------------------------------------------------------
// Class:   CMoPropertyViewModelProtectedAdapter
//
// Purpose: Implementation of ICollectionViewModelProtected Interface
//
//------------------------------------------------------------------------------
class CMoPropertyViewModelProtectedAdapter :
    public foundation::AdapterBase
    <
        pmod::library::IViewModelClass,
        IViewModelProtected,
        &__uuidof(IViewModelProtected)
    >
{
public:
    // Interface IPropertyViewModelProtected

protected:

};

//------------------------------------------------------------------------------
// Class:   CMoPropertyViewModelOverridesAdapter
//
// Purpose: Implementation of IPropertyViewModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoPropertyViewModelOverridesAdapter :
    public foundation::AdapterBase
    <
        pmod::library::IViewModelClass,
        IViewModelOverrides,
        &__uuidof(IViewModelOverrides)
    >
{
public:
    virtual HRESULT STDMETHODCALLTYPE OnSourcePropertyChanged( 
                            /* [in] */ int propertyId,
                            /* [in] */ IInspectable *oldValue,
                            /* [in] */ IInspectable *newValue) override
    {
        UNREFERENCED_PARAMETER(propertyId);
        UNREFERENCED_PARAMETER(oldValue);
        UNREFERENCED_PARAMETER(newValue);
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE OnSourceEventModel(_In_ int eventId,_In_ IInspectable *pEventArgs) override
    {
        UNREFERENCED_PARAMETER(eventId);
        UNREFERENCED_PARAMETER(pEventArgs);
        return S_OK;
    }
};
