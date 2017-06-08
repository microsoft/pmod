/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionFactory.h
****/

#pragma once

#include <pmod/library/model_library.h>

#include "MoModelFactory.h"
#include "MoBaseDelegateAdapter.h"

#include "Microsoft.PropertyModel.Library.h"

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoObservableCollectionDelegateAdapter
//
// Purpose: Implementation of IObservableObjectDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoObservableCollectionDelegateAdapter :
    public CMoDelegeteBaseAdapter
    <
        pmod::library::IObservableCollectionDelegate,
        pmod::INotifyCollectionChangedEventHandler
    >
{
protected:
     // IObservableCollectionDelegate Implementation
    STDMETHOD( OnCreateItemsContainer )(_COM_Outptr_ pmod::library::IItemsContainer **ppValue);
    STDMETHOD( OnCollectionChanged )(_In_ pmod::INotifyCollectionChangedEventArgs *pEventArgs);
};
//------------------------------------------------------------------------------
// Class:   MoCollectionModelFactory
//
// Purpose: Factory class for a PropertyModel
//
//------------------------------------------------------------------------------
class MoCollectionModelFactory :
        public MoModelFactoryStaticsBase
        <
            IObservableCollectionFactory,
            IObservableCollectionStatics
        >
{
public:
    // ICollectionModelFactory
    IFACEMETHOD(CreateInstance)(
        IInspectable* pOuter,
        IInspectable** ppInner,
        IObservableCollection** ppInstance);
    IFACEMETHOD(CreateInstanceWithOptions)(
         ObservableCollectionOptions modelOptions,
        IInspectable* pOuter,
        IInspectable** ppInner,
        IObservableCollection** ppInstance);
    IFACEMETHOD(CreateInstanceWithTypeAndOptions)(
        GUID iidType,
        ObservableCollectionOptions modelOptions,
        IInspectable* pOuter,
        IInspectable** ppInner,
        IObservableCollection** ppInstance);

    // IObservableCollectionStatics
    IFACEMETHOD(get_StringType)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection_String;
        return S_OK;
    }
                        
    IFACEMETHOD(get_Int16Type)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection_Int16;
        return S_OK;
    }
                        
    IFACEMETHOD(get_UInt16Type)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection_UInt16;
        return S_OK;
    }
                        
    IFACEMETHOD(get_Int32Type)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection_Int32;
        return S_OK;
    }
                        
    IFACEMETHOD(get_UInt32Type)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection_UInt32;
        return S_OK;
    }
                        
    IFACEMETHOD(get_BooleanType)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection_Boolean;
        return S_OK;
    }
                        
    IFACEMETHOD(get_DateTimeType)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection_DateTime;
        return S_OK;
    }
                        
    IFACEMETHOD(get_ByteType)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection_UInt8;
        return S_OK;
    }

    IFACEMETHOD(get_ObjectType)(GUID *value)
    {
        *value = pmod::IID_IObservableCollection;
        return S_OK;
    }
};
