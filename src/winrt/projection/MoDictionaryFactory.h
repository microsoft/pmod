/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoDictionaryFactory.h
****/

#pragma once

#include <foundation/library/library_util.h>
#include <winrt/com_activation.h>
//------------------------------------------------------------------------------
// Class:   MoDictionaryFactory
//
// Purpose: Factory class for a PropertyModel
//
//------------------------------------------------------------------------------
class MoDictionaryFactory :
    public ctlwinrt::AbstractActivationFactory
{
public:
    IFACEMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable **instance) override
    {
        foundation::ComPtr<foundation::IDictionary>    spCoreDictionary;
        IFR_ASSERT(foundation::library::CreateDictionaryClass(spCoreDictionary.ReleaseAndGetAddressOf()));
        *instance = spCoreDictionary.Detach();
        return S_OK;
    }

};
