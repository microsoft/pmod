/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "MoBaseDelegateAdapter.h"

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoObservableObjectDelegateAdapter
//
// Purpose: Implementation of IObservableObjectDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoObservableObjectDelegateAdapter :
    public CMoDelegeteBaseAdapter<
        pmod::library::IObservableObjectDelegate,pmod::IPropertyChangedEventHandler>
{
public:
    static HRESULT CreateAsyncParameters(
        _In_ UINT32 size,
        _In_ IInspectable **parameters,
        _In_ IInspectable *pAsyncOperation,
        _Out_ std::vector<IInspectable *>& parameters_);

protected:
     // IObservableObjectDelegate Implementation
    STDMETHOD( OnCreatePropertiesContainer )(_In_ pmod::IObservableObjectInfo* pPropertyModelInfo,_In_ UINT32 size,_COM_Outptr_ foundation::library::IPropertiesContainer **ppValue) ;
    STDMETHOD( OnGetProperty )(
        _In_ UINT32 propertyId,
        _Inout_ IInspectable **ppValue);
    STDMETHOD(OnSetProperty)(
        _In_ UINT32 propertyId, 
        _In_ bool isPublic, 
        _In_ IInspectable *oldValue, 
        _In_ IInspectable *newValue);
    STDMETHOD( OnInvokeMethod ) (
            _In_ foundation::IMethodInfo *pMethodInfo,
            _In_ UINT32 methodId,
            _In_ UINT32 size,
            _In_ IInspectable **parameters,
            _Outptr_ IInspectable **ppValue);
};
//------------------------------------------------------------------------------
// Class:   MoObservableObjectFactory
//
// Purpose: Factory class for a PropertyModel
//
//------------------------------------------------------------------------------
class MoObservableObjectFactory :
        public MoModelFactoryStaticsBase
        <
            IObservableObjectFactory,
            IObservableObjectStatics
        >
{
public:
    // IObservableObjectFactory
    IFACEMETHOD(CreateInstance)(
        _In_ ObservableObjectOptions modelOptions,
        _In_opt_ IInspectable* pOuter,
        _COM_Outptr_ IInspectable** ppInner,
        _COM_Outptr_ IObservableObject** ppInstance) override;
    IFACEMETHOD(CreateInstanceWithType)(
        GUID iidType,
        _In_ ObservableObjectOptions modelOptions,
        _In_opt_ IInspectable* pOuter,
        _COM_Outptr_ IInspectable** ppInner,
        _COM_Outptr_ IObservableObject** ppInstance) override;
    // IObservableObjectStatics
    IFACEMETHOD(GetPropertyValue)( 
                IObservableObject *pObservableObject,
                IInspectable *pPath,
                IInspectable **ppValue) override;
    IFACEMETHOD(CreateBatchEventHandler)(
        IPropertyChangedEventHandler *pPropertyChangedEventHandler,
        IBatchPropertyChangedEventHandler *pBatchPropertyChangedEventHandler,
        IPropertyChangedEventHandler **ppPropertyChangedEventHandler) override;
};
