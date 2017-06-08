/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchFactory.cpp
****/
#include "pch.h"

#include "ObjectDispatchFactory.h"
#include "ObjectDispatch.h"
#include <foundation/library/key_value_pair_impl.h>
#include <foundation/library/activation_factory.h>

#include <foundation/library/com_ptr_singleton.h>

// {94BABC5E-8343-44EC-91A5-87ED9E4E0E64}
const GUID foundation::library::IID_IObjectDispatchFactory =
{ 0x94babc5e, 0x8343, 0x44ec,{ 0x91, 0xa5, 0x87, 0xed, 0x9e, 0x4e, 0xe, 0x64 } };

using namespace foundation;

// specialization for IPropertyIdPair interface
typedef library::_InspectableKeyValuePair<IPropertyIdPair> CPropertyIdPair;

library::IObjectDispatchFactory *CObjectDispatchFactory::GetInstance()
{
    return library::_GetFactoryInstance<CObjectDispatchFactory, IObjectDispatchFactory>();
}

STDMETHODIMP CObjectDispatchFactory::CreateObjectDispatch(
    _In_ const foundation::library::ObjectDispatchCreateParameters *pCreateParams,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CObjectDispatch::CreateInstance(pCreateParams, pOuter, ppNewInstance);
}

STDMETHODIMP CObjectDispatchFactory::CreatePropertyIdPair(
    UINT32 propertyId,
    _In_opt_ foundation::IInspectable *pValue,
    _COM_Outptr_ IPropertyIdPair **ppPropertyIdPair)
{
    return library::CreateKeyValuePair<CPropertyIdPair>(propertyId, pValue, ppPropertyIdPair);
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.ObjectDispatch"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObjectDispatchFactory::GetInstance)
    );
