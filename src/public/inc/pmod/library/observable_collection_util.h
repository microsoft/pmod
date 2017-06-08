/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection_util.h
****/
#pragma once

#include "model_template_library.h"

namespace pmod {
    namespace library {

        template <
            class CollectionModelT,
            class ContainerT
            >
        static HRESULT create_collection_with_values(
            _In_ const ContainerT& values,
            _COM_Outptr_ CollectionModelT** ppNewInstance,
            bool fIsReadOnly = true
            )
        {
            library::ObservableCollectionCreateParameters params;
            params.m_options = (ObservableCollectionOptions)(fIsReadOnly ? ObservableCollectionOptions::IsReadOnly : ObservableCollectionOptions::None);
            params.m_iid_type = CollectionModelT::GetIID();
            params.m_initial_values_ptr = foundation::CreateValue(values);

            return CreateObservableCollection(params, ppNewInstance);
        }
    }
}
