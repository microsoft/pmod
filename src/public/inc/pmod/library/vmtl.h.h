/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:vmtl.h.h
****/
#pragma once

#include "mtl.h"


#include "interfaces/property_viewmodel_class.h"
#include "interfaces/collection_viewmodel_class.h"

namespace pmod {
    namespace library {


        class _ViewModelDelegate :
            public _SingleSourceModelDelegateBase<IViewModelDelegate, IPropertyChangedEventHandler, IObservableObject>
        {
            STDMETHOD(OnSourcePropertyChanged)(_In_ UINT32 propertyId, _In_ foundation::IInspectable *oldValue, _In_ foundation::IInspectable *newValue)
            {
                UNREFERENCED_PARAMETER(propertyId);
                UNREFERENCED_PARAMETER(oldValue);
                UNREFERENCED_PARAMETER(newValue);
                return S_OK;
            }
            STDMETHOD(OnSourceEventModel)(_In_ UINT32 eventId, _In_ foundation::IInspectable *pEventArgs)
            {
                UNREFERENCED_PARAMETER(eventId);
                UNREFERENCED_PARAMETER(pEventArgs);
                return S_OK;
            }
        };

        class _CollectionViewModelDelegate :
            public _SingleSourceModelDelegateBase<ICollectionViewModelDelegate, INotifyCollectionChangedEventHandler, IObservableCollection>
        {
            STDMETHOD(OnSourceCollectionChanged)(_In_ INotifyCollectionChangedEventArgs *pEventArgs)
            {
                UNREFERENCED_PARAMETER(pEventArgs);
                return S_OK;
            }

            STDMETHOD(OnSourceItemPropertyChanged)(
                _In_ IObservableObject *item,
                _In_ UINT32 propertyId,
                _In_opt_ foundation::IInspectable *oldValue,
                _In_opt_ foundation::IInspectable *newValue,
                bool *pNeedRefresh)
            {
                UNREFERENCED_PARAMETER(item);
                UNREFERENCED_PARAMETER(propertyId);
                UNREFERENCED_PARAMETER(oldValue);
                UNREFERENCED_PARAMETER(newValue);

                *pNeedRefresh = true;
                return S_OK;
            }

            STDMETHOD(FilterItem)(_In_ foundation::IInspectable * item, bool *pValue)
            {
                UNREFERENCED_PARAMETER(item);
                *pValue = true;
                return S_FALSE;
            }

            STDMETHOD(CompareItem)(_In_ foundation::IInspectable * item1, _In_ foundation::IInspectable * item2, int *pResult)
            {
                UNREFERENCED_PARAMETER(item1);
                UNREFERENCED_PARAMETER(item2);
                *pResult = 0;
                return S_FALSE;
            }

            STDMETHOD(OnItemSourceAttached)(_In_ IObservableObject *item)
            {
                UNREFERENCED_PARAMETER(item);
                return S_OK;
            }

            STDMETHOD(OnItemSourceDetached)(_In_ IObservableObject *item)
            {
                UNREFERENCED_PARAMETER(item);
                return S_OK;
            }
            STDMETHOD(CreateModelAdapter)(_In_ foundation::IInspectable *item, ISourceModel **ppSourceModel)
            {
                return S_FALSE;
            }
        };


    }
}
