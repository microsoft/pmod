/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionStatics.cpp
****/

#include "pch.h"

#include "ObservableCollectionStatics.h"

#include <foundation/library/com_ptr_singleton.h>
#include <foundation/library/logger_macros.h>
#include <pmod/interfaces/observable_object.h>

using namespace pmod;
using namespace pmod::library;

// {C9D1B5C5-4711-498B-8156-78956941B403}
const GUID pmod::library::IID_IObservableCollectionStatics =
{ 0xc9d1b5c5, 0x4711, 0x498b,{ 0x81, 0x56, 0x78, 0x95, 0x69, 0x41, 0xb4, 0x3 } };

IObservableCollectionStatics *CObservableCollectionStatics::GetInstance()
{
	return foundation::library::_GetFactoryInstance<CObservableCollectionStatics, IObservableCollectionStatics>();
}


CObservableCollectionStatics::~CObservableCollectionStatics()
{
}

HRESULT CObservableCollectionStatics::LookupItemInternal(
    pmod::IObservableCollection *pObservableCollection,
    foundation::IInspectable *pDefaultPropertyValue,
    foundation::IInspectable **ppItem)
{
    foundation_assert(pObservableCollection != nullptr);

    foundation::ComPtr<foundation::IIterator_impl<foundation::IInspectable *>> spIterator;
    IFR_ASSERT(pObservableCollection->First(spIterator.GetAddressOf()));

    bool hasCurrent;
    spIterator->GetHasCurrent(&hasCurrent);
    while (hasCurrent)
    {
        foundation::InspectablePtr spNextItem;
        spIterator->GetCurrent(spNextItem.GetAddressOf());

        if (spNextItem != nullptr)
        {
            // TODO: we may want to look in the TypeInfo for the
            // default property, for now we expect be 1
            const UINT32 _default_property_id = 1;

            foundation::ComPtr<IObservableObject> spPropertyModel;
            IFR(foundation::QueryInterface(
                spNextItem,
                spPropertyModel.GetAddressOf()));

            foundation::InspectablePtr spDefaultValue;
            IFR(spPropertyModel->GetProperty(
                _default_property_id,
                spDefaultValue.GetAddressOf()));

            if (foundation::pv_util::CompareInspectableValues(
                spDefaultValue,
                pDefaultPropertyValue
                ) == 0)
            {
                // we found an item that statisfy our lookup
                *ppItem = spNextItem.Detach();
                return S_OK;
            }
        }
        spIterator->MoveNext(&hasCurrent);
    }
    // if we arrive here we could not complete the lookup
    return S_OK;
}

STDMETHODIMP CObservableCollectionStatics::LookupItem(
    pmod::IObservableCollection *pCollection,
    foundation::IInspectable *pDefaultPropertyValue,
    foundation::IInspectable **ppItem)
{
    return LookupItemInternal(pCollection, pDefaultPropertyValue, ppItem);
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.ObservableCollectionStatics"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObservableCollectionStatics::GetInstance)
    );
