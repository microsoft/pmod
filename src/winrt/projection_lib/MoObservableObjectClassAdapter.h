/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectClassAdapter.h
****/

#pragma once
#include <foundation/library/logger_macros.h>

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoObservableObjectClassAdapter
//
// Purpose: Implementation of IObservableObjectClass Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoObservableObjectClassAdapter :
    public foundation::AdapterBase <
    pmod::library::IObservableObjectClass,
    IObservableObjectClass,
    &__uuidof(IObservableObjectClass)
    >
{
protected:
    // Interface IPropertyModelProtected for WinRT
    HRESULT STDMETHODCALLTYPE SetPropertyInternal(unsigned int propertyId, IInspectable *value) override;
    HRESULT STDMETHODCALLTYPE FirePropertyChanged(unsigned int propertyId, boolean useCallback) override;
    HRESULT STDMETHODCALLTYPE FireEventModel(unsigned int eventId, IInspectable *eventArgs) override;

    HRESULT STDMETHODCALLTYPE SetBinding(
        unsigned int propertyId,
        IBindingBase *propertyBinding) override;
    HRESULT STDMETHODCALLTYPE ClearBinding(
        unsigned int propertyId) override;

    // called during initialization from code generated classes
    HRESULT STDMETHODCALLTYPE InitializeModel()
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE put_InvokeDelegate(ABI::Microsoft::PropertyModel::Library::IInvokeDelegate* value);

    HRESULT STDMETHODCALLTYPE get_InvokeDelegate(ABI::Microsoft::PropertyModel::Library::IInvokeDelegate** value);

};
//------------------------------------------------------------------------------
// Class:   CMoObservableObjectProtectedAdapter
//
// Purpose: Implementation of IObservableObject Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoObservableObjectProtectedAdapter :
    public foundation::AdapterBase <
    pmod::library::IObservableObjectClass,
    IObservableObjectProtected,
    &__uuidof(IObservableObjectProtected)
    >
{
protected:
    // Interface IPropertyModelProtected for WinRT
};
//------------------------------------------------------------------------------
// Class:   CMoObservableObjectOverridesAdapter
//
// Purpose: Implementation of IPropertyModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoObservableObjectOverridesAdapter :
    public foundation::AdapterBase <
    pmod::library::IObservableObjectClass,
    IObservableObjectOverrides,
    &__uuidof(IObservableObjectOverrides)
    >
{
public:
    // Interface IPropertyModelOverrides
    HRESULT STDMETHODCALLTYPE OnGetProperty(int propertyId, IInspectable **value) override
    {
        UNREFERENCED_PARAMETER(propertyId);
        UNREFERENCED_PARAMETER(value);

        IFCPTR(value);
        *value = nullptr;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnSetProperty(int propertyId, boolean isPublic, IInspectable *oldValue, IInspectable *newValue) override
    {
        UNREFERENCED_PARAMETER(propertyId);
        UNREFERENCED_PARAMETER(isPublic);
        UNREFERENCED_PARAMETER(oldValue);
        UNREFERENCED_PARAMETER(newValue);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnInvoke(int methodId, UINT32 length, IInspectable **pParameters, IInspectable **result) override
    {
        UNREFERENCED_PARAMETER(methodId);
        UNREFERENCED_PARAMETER(length);
        UNREFERENCED_PARAMETER(pParameters);
        UNREFERENCED_PARAMETER(result);
        return E_NOTIMPL;
    }
};

