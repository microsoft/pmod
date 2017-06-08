/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelModelFactory.cpp
****/

#include "pch.h"

#include "MoModelModelFactory.h"

#include <foundation\type_info_statics_util.h>
#include <foundation\node_statics_util.h>
#include <foundation\activation_factory.h>
#include <foundation\library\object_factory_util.h>
#include <foundation\library\disposable_util.h>
//#include <pmod\library\object_statics_util.h>
#include <pmod\library\observable_statics_util.h>

STDMETHODIMP MoModelModelFactory::IsFactoryAvailable(
    HSTRING activationId,
    boolean *value)
{
    *value = foundation::IsActivationFactoryAvailable(_pal_GetStringRawBuffer(activationId, nullptr));
    return S_OK;
}

STDMETHODIMP MoModelModelFactory::GetActivationFactory(
    HSTRING activationId,
    IInspectable **value)
{
    IUnknown *pUnk = foundation::GetActivationFactoryInstance(_pal_GetStringRawBuffer(activationId, nullptr));
    return foundation::QueryInterfaceIf(pUnk, value);
}

STDMETHODIMP MoModelModelFactory::ActivateObject(
    GUID iidInstanceType,
    IInspectable *pContext,
    IObject **value)
{
    return foundation::library::ActivateObject(iidInstanceType, pContext, value);
}

STDMETHODIMP MoModelModelFactory::IsInstanceOfType( 
                            IObject *pModel,
                            GUID iidInstanceType,
                            boolean *pResult)
{
    return IsInstanceOfTypeWithOptions(
        pModel,
        iidInstanceType,
        InstanceTypeOptions_All,
        pResult);
}

STDMETHODIMP MoModelModelFactory::IsInstanceOfTypeWithOptions( 
                            IObject *pModel,
                            GUID iidInstanceType,
                            InstanceTypeOptions options,
                            boolean *pResult)
{
    IFCPTR(pModel);
    IFCPTR(pResult);

    foundation::ComPtr<foundation::IObject> spModel;
    IFR(foundation::QueryInterface(pModel,spModel.GetAddressOf()));
    bool result;
    IFR(foundation::IsInstanceOfTypeWithOptions(
        spModel,
        iidInstanceType,
        (foundation::InstanceTypeOptions)options,
        &result));
    *pResult = result ? true:false;
    return S_OK;
}
                        
STDMETHODIMP MoModelModelFactory::IsEqual( 
                            IObject *pItem1,
                            IObject *pItem2,
                            boolean *pResult)
{
    IFCPTR(pItem1);
    IFCPTR(pItem2);
    IFCPTR(pResult);

    foundation::ComPtr<foundation::IObject> spModel1,spModel2;
    IFR_ASSERT(foundation::QueryInterface(pItem1,spModel1.GetAddressOf()));
    IFR_ASSERT(foundation::QueryInterface(pItem2,spModel2.GetAddressOf()));
    bool result;
    IFR_ASSERT(IsObjectEqualsInternal(spModel1, spModel2, &result));
    *pResult = result ? true:false;
    return S_OK;
}
                        
STDMETHODIMP MoModelModelFactory::GetAncestor( 
                            IObject *pModel,
                            GUID iidAncestorType,
                            IObject **ppResult)
{
    IFCPTR(pModel);
    IFCPTR(ppResult);

    foundation::ComPtr<foundation::IObject> spModel;
    IFR(foundation::QueryInterface(pModel,spModel.GetAddressOf()));
    return foundation::GetObjectAncestor(spModel,iidAncestorType, ppResult);
}

STDMETHODIMP MoModelModelFactory::GetParent(
    IObject *pModel,
    IObject **ppParent)
{
    IFCPTR(pModel);
    IFCPTR(ppParent);

    foundation::ComPtr<foundation::IObject> spModel;
    IFR(foundation::QueryInterface(pModel, spModel.GetAddressOf()));
    return foundation::GetObjectParent(spModel, ppParent);
}

STDMETHODIMP MoModelModelFactory::GetChilds(
    ABI::Microsoft::PropertyModel::IObject *pModel,
    UINT32 *length,
    ABI::Microsoft::PropertyModel::IObject ***value)
{
    foundation::ComPtr<foundation::IObject> spModel;
    IFR(foundation::QueryInterface(pModel, spModel.GetAddressOf()));
    return foundation::GetObjectChilds(spModel, length, value);
}

STDMETHODIMP MoModelModelFactory::GetRoot( 
                            IObject *pModel,
                            IObject **ppRoot)
{
    IFCPTR(pModel);
    IFCPTR(ppRoot);

    foundation::ComPtr<foundation::IObject> spModel;
    IFR(foundation::QueryInterface(pModel,spModel.GetAddressOf()));
    return foundation::GetObjectRoot(spModel, ppRoot);
}

STDMETHODIMP MoModelModelFactory::GetSource( 
                            IInspectable *pSourceModel,
                            IObject **ppSource)
{
    IFCPTR(pSourceModel);
    IFCPTR(ppSource);

    foundation::ComPtr<foundation::IObject> spSource;
    IFR(pmod::library::GetSource(pSourceModel,spSource.GetAddressOf()));
    if(spSource != nullptr)
    {
        IFR(foundation::QueryInterface(spSource,ppSource));
    }
    else
    {
        *ppSource = nullptr;
    }
    return S_OK; 
}

STDMETHODIMP MoModelModelFactory::GetSourceOfType( 
                            IInspectable *pSourceModel,
                            GUID iidInstanceType,
                            InstanceTypeOptions options,
                            boolean isModelClassType,
                            IObject **ppSource)
{
    IFCPTR(pSourceModel);
    IFCPTR(ppSource);

    foundation::ComPtr<foundation::IObject> spSource;
    IFR(pmod::library::get_source_of_type(
        pSourceModel,
        iidInstanceType,
        (foundation::InstanceTypeOptions)options,
        isModelClassType ? true:false,
        spSource.GetAddressOf()));
    if(spSource != nullptr)
    {
        IFR(foundation::QueryInterface(spSource,ppSource));
    }
    else
    {
        *ppSource = nullptr;
    }
    return S_OK; 

}
STDMETHODIMP MoModelModelFactory::Dispose(
    IInspectable *pObject)
{
    return foundation::library::Dispose(pObject);
}

