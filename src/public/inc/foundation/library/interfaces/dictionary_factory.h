/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dictionary_factory.h
****/
#pragma once

#include <foundation/interfaces/dictionary.h>

namespace foundation
{
    namespace library
    {
        //------------------------------------------------------------------------------
        // Interface:   IDictionaryFactory
        //
        // Purpose: An Interface to create IDictionary instances
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IDictionaryFactory, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(CreateDictionary)(_COM_Outptr_ IDictionary **ppNewInstance) = 0;
            STDMETHOD(CreateDictionaryPair)(
                HSTRING key,
                _In_opt_ IInspectable *pValue,
                _COM_Outptr_ IDictionaryPair **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

