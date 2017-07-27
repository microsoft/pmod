/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_class.h
****/
#pragma once

#include "interfaces/async_operation_factory.h"
#include "interfaces/async_operation_class.h"

#include <foundation/async_operation_ptr.h>
#include <foundation/activation_factory.h>
#include <foundation/delegate_library.h>
#include <foundation/pv_util.h>
#include <foundation/type_info_statics_util.h>

#include "class_options.h"
#include "type_info_factory_util.h"

namespace foundation
{

    namespace library
    {
        inline library::IAsyncOperationFactory *GetAsyncOperationFactory()
        {
            return GetFoundationLibraryFactory<library::IAsyncOperationFactory>(U("foundation.AsyncOperation"));
        }

        template <class T>
        HRESULT CreateAsyncOperationClass(
            const IID& iidType,
            AsyncOperationOptions options,
            _COM_Outptr_ T **ppT)
        {
            IFCPTR_ASSERT(ppT);

            InspectablePtr spInstance;
            library::async_operation_create_parameters ctor_params;

            ctor_params.m_iid_type = iidType;
            ctor_params.m_options = (AsyncOperationOptions)(static_cast<UINT32>(options) | library::FoundationClassOptions_IsFreeThreadApartment);
            IFR_ASSERT(GetAsyncOperationFactory()->CreateAsyncOperation(
                &ctor_params,
                nullptr,
                spInstance.GetAddressOf()));
            return foundation::QueryInterface(spInstance, ppT);
        }

        template <class T>
        HRESULT CreateAsyncOperationClass(
            const IID& iidType,
            T **ppT)
        {
            return CreateAsyncOperationClass(
                iidType,
                AsyncOperationOptions::None,
                ppT);
        }

        template <class T>
        HRESULT SetCompleted(
            _In_ foundation::library::IAsyncOperationClass *pAsyncOperationClass,
            _In_ HRESULT hr,
            _In_ T result)
        {
            return pAsyncOperationClass->SetCompleted(hr, foundation::pv_util::CreateValue(result));
        }

        template <class T>
        HRESULT CreateAsyncOperationClass(
            _In_ foundation::IPropertyTypeInfo *pResultTypeInfo,
            _In_  AsyncOperationOptions    options,
            T **ppT)
        {
            IID iidType;
            IFR_ASSERT(GetAsyncOperationTypeInfo(pResultTypeInfo, iidType));
            return CreateAsyncOperationClass(
                iidType,
                options,
                ppT);
        }

        template <class T>
        HRESULT CreateAsyncOperationClass(
            _In_ const PropertyTypeInfo *p_result_type_info,
            _In_ AsyncOperationOptions    options,
            T **ppT)
        {
            ComPtr<IPropertyTypeInfo> result_type_info_ptr;
            IFR_ASSERT(library::CreatePropertyTypeInfo(
                p_result_type_info,
                result_type_info_ptr.GetAddressOf()));

            return CreateAsyncOperationClass(
                result_type_info_ptr,
                options,
                ppT);
        }

    }

}

