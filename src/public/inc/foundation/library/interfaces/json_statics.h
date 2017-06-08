/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:json_statics.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/interfaces/dictionary.h>
#include <foundation/interfaces/stream_provider.h>

#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        // IJsonUtil Interface util
        struct IJsonUtil : foundation::IUnknown
        {
            // parse a UTF-8 input json string and return the parsed object
            virtual HRESULT STDMETHODCALLTYPE Parse(
                _In_ const char *json_str,
                _COM_Outptr_opt_result_maybenull_ IDictionary **ppJsonError,
                _COM_Outptr_ IInspectable **ppInspectable) = 0;
            // parse a file reference path and return the parsed object
            virtual HRESULT STDMETHODCALLTYPE ParseFromFile(
                _In_ const char *file_path,
                _COM_Outptr_opt_result_maybenull_ IDictionary **ppJsonError,
                _COM_Outptr_ IInspectable **ppInspectable) = 0;
        };

        // IJsonStatics Interface util
        BEGIN_DECLARE_INTERFACE(IJsonStatics, IJsonUtil, FOUNDATION_LIB_API)
            // To/From Json for pInspectable
            virtual HRESULT STDMETHODCALLTYPE ToJson(
                _In_ foundation::IInspectable *pInspectable,
                _Out_ HSTRING *json) = 0;
        virtual HRESULT STDMETHODCALLTYPE FromJson(
            _In_ HSTRING json,
            _COM_Outptr_ foundation::IInspectable **ppInspectable) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetStreamProvider(
            _COM_Outptr_ foundation::IStreamProvider **ppStreamProvider) = 0;
        END_DECLARE_INTERFACE()
    }
}
