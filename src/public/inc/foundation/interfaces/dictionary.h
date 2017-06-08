/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dictionary.h
****/
#pragma once

#include "key_value_pair.h"
#include "map.h"

namespace foundation {

    enum class NotifyCollectionModelChangedAction
    {
        ItemAdded = 1,
        ItemRemoved = 2,
        ItemReplaced = 3,
        Reset = 4
    };

    /////////////////////////////////// Dictionary Map //////////////////////////////////////////////
    BEGIN_DECLARE_IID(IDictionaryPair, FOUNDATION_API);
    typedef IKeyValuePair<HSTRING, IInspectable *, IID_IDictionaryPair> IDictionaryPair;

    BEGIN_DECLARE_IID(IDictionary, FOUNDATION_API);
    typedef IMap<HSTRING, IInspectable *, IDictionaryPair, IID_IDictionary> IDictionary;

}

