/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:adapter_factory.h
****/
#pragma once

#include "interface_adapter.h"

namespace foundation
{
    namespace library
    {
        //////////////////////  Adapter Support ////////////////////////////////
        typedef HRESULT(STDMETHODCALLTYPE *CreateAdapterInstanceCallback)(
            _In_ foundation::IInspectable *pOuter,              // The 'Outer' object
            _In_ foundation::IInspectable *pInner,              // The 'Inner' object
            _Outptr_ foundation::IInspectable **ppUnknown       // The new created Adapter
            );

        // IGenericAdapterFactory Interface
        BEGIN_DECLARE_INTERFACE(IGenericAdapterFactory, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(AddAdapterEntry)(
            _In_ IID riid_outer,
            _In_ IID iidTargetType,
            _In_ IID iidSourceType,
            _In_ CreateAdapterInstanceCallback creatorCallback) = 0;
        END_DECLARE_INTERFACE()

        struct _TypeAdapterEntry
        {
            IID                 _iidInterface;                     // the Interface being asked by the QI invocation
            IID                 _iidType;                          // the IID of type object that would support this 
            CreateAdapterInstanceCallback _creatorCallback;    // Model adapter creator callback
        };

        // ITypeAdapterFactory Interface
        BEGIN_DECLARE_INTERFACE(ITypeAdapterFactory, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(AddTypeEntries)(_In_ const _TypeAdapterEntry iidTypes[]) = 0;
        STDMETHOD(AddTypeEntry)(_In_ IID iidInterface, _In_ IID iidType, _In_ CreateAdapterInstanceCallback creatorCallback) = 0;
        END_DECLARE_INTERFACE()

        // IAdapterFactoryFactory Interface
        BEGIN_DECLARE_INTERFACE(IAdapterFactoryFactory, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(CreateTypeAdapterFactory)(
            _In_opt_ const foundation::library::_TypeAdapterEntry iidTypes[],
            _COM_Outptr_ IInterfaceAdapterFactory **ppModelAdapterFactory) = 0;

        STDMETHOD(CreateGenericAdapterFactory)(
            _COM_Outptr_ foundation::library::IGenericAdapterFactory **ppGenericAdapterFactory) = 0;
        END_DECLARE_INTERFACE()
    }
}

