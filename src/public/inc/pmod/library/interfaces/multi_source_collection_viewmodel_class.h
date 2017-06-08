/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:multi_source_collection_viewmodel_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <pmod/interfaces/observable_collection.h>

#include "source_model_delegate.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        BEGIN_DECLARE_INTERFACE(IMultiSourceCollectionViewModelDelegate, ISourceBaseModelDelegateBase<INotifyCollectionChangedEventHandler>, PMOD_LIB_API)
            STDMETHOD(OnSourceCollectionChanged)(_In_ IObservableCollection *pSource, _In_ INotifyCollectionChangedEventArgs* pArgs) = 0;
        END_DECLARE_INTERFACE()

       struct MultiSourceCollectionViewModelCreateParameters :
            _MultiSourceViewModelCreateParametersBase
            <
            ObservableCollectionCreateParameters,
            ICollectionViewModelDelegate,
            CollectionViewModelOptions,
            IObservableCollection
            >
        {
            MultiSourceCollectionViewModelCreateParameters() :
                m_p_multi_source_CollectionViewModelDelegate(nullptr)
            {
            }

            IMultiSourceCollectionViewModelDelegate *m_p_multi_source_CollectionViewModelDelegate;
        };

        BEGIN_DECLARE_INTERFACE(IMultiSourceCollectionViewModelClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateMultiSourceCollectionViewModelInstance)(
                _In_ const MultiSourceCollectionViewModelCreateParameters *pModelImpl,
                _In_opt_ foundation::IInspectable *pOuter,
                _Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

