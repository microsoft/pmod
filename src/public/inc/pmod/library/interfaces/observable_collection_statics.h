/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection_statics.h
****/
#pragma once

#include <pmod/interfaces/observable_collection.h>
#include "../pmod_lib_api.h"

namespace pmod {
    namespace library {
        /////////////////////////////////// Collection Model Statics //////////////////////////////////////////////
        BEGIN_DECLARE_INTERFACE(IObservableCollectionStatics, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(LookupItem)(
                IObservableCollection *pCollection,
                foundation::IInspectable *pDefaultPropertyValue,
                foundation::IInspectable **ppItem) = 0;
        END_DECLARE_INTERFACE()
    }

}


