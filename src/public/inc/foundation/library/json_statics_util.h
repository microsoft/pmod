/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:json_statics_util.h
****/
#pragma once

#include "interfaces/json_statics.h"

#include "activation_factory.h"
#include <foundation/boxing_util.h>
#include <foundation/errors.h>


#define FOUNDATION_E_JSON_PARSER    MAKE_HRESULT(SEVERITY_ERROR, 0xf6,  0x600)


namespace foundation
{
    namespace json_util
    {
        static inline library::IJsonStatics *GetJsonStatics()
        {
            return library::GetFoundationLibraryFactory<library::IJsonStatics>(U("foundation.JsonStatics"));
        }

        inline static HRESULT Parse(
            _In_ const char *json_str,
            _Out_opt_ IDictionary **ppJsonError,
            _COM_Outptr_ foundation::IInspectable **ppInspectable)
        {
            return GetJsonStatics()->Parse(json_str, ppJsonError,ppInspectable);
        }

        template <class T>
        static HRESULT Parse_t(
            _In_ const char *json_str,
            _Out_opt_ IDictionary **ppJsonError,
            T& value)
        {
            InspectablePtr jsonObjectPtr;
            _IFR_(Parse(json_str, ppJsonError, jsonObjectPtr.GetAddressOf()));
            return GetValue(jsonObjectPtr, value);
        }


        __unused_attribute__
        static HRESULT ToJson(
            _In_ foundation::IInspectable *pInspectable,
            _Out_ HSTRING *json)
        {
            return GetJsonStatics()->ToJson(pInspectable, json);
        }

        template <class T>
        static HRESULT FromJson(
            _In_ HSTRING json,
            _Out_ T& value)
        {
            InspectablePtr valuePtr;
            IFR_ASSERT(GetJsonStatics()->FromJson(
                json,
                valuePtr.GetAddressOf()));
            IFR_ASSERT(foundation::GetValue(valuePtr, value));
            return S_OK;
        }

        template <class T>
        static HRESULT ParseFromFile(
            _In_ const char *file_path,
            _Out_ T& value)
        {
            InspectablePtr valuePtr;
            _IFR_(GetJsonStatics()->ParseFromFile(
                file_path,
                nullptr,
                valuePtr.GetAddressOf()));
            IFR_ASSERT(foundation::GetValue(valuePtr, value));
            return S_OK;
        }

        __unused_attribute__
        static foundation::IStreamProvider *GetStreamProvider()
        {
            ComPtr<foundation::IStreamProvider> stream_provider_ptr;
            GetJsonStatics()->GetStreamProvider(stream_provider_ptr.GetAddressOf());
            foundation_assert(stream_provider_ptr != nullptr);
            return stream_provider_ptr;
        }

    }
}
