/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dictionary_class.h
****/
#pragma once

#include "interfaces/dictionary_factory.h"
#include <foundation/foundation_factory.h>


namespace foundation
{
    namespace library
    {
        inline library::IDictionaryFactory *GetDictionaryFactory()
        {
            return GetFoundationFactory<library::IDictionaryFactory>(U("foundation.Dictionary"));
        }

        inline HRESULT CreateDictionaryClass(IDictionary **ppDictionary)
        {
            return GetDictionaryFactory()->CreateDictionary(ppDictionary);
        }

        inline HRESULT CreateDictionaryPair(
            HSTRING key, 
            _In_opt_ IInspectable *pValue,
            IDictionaryPair **ppDictionaryPair)
        {
            return GetDictionaryFactory()->CreateDictionaryPair(key, pValue,ppDictionaryPair);
        }
    }

}

