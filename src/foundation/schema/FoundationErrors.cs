/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationErrors.cs
****/
using System;

namespace foundation
{
    public enum Errors
    {
        S_OK_              = 0,
        S_FALSE_ = 1,
        E_UNEXPECTED_ = 0x8000FFFFL,
        E_NOTIMPL_ = 0x80004001L,
        E_OUTOFMEMORY_ = 0x8007000EL,
        E_INVALIDARG_ = 0x80070057L,
        E_NOINTERFACE_ = 0x80004002L,
        E_POINTER_ = 0x80004003L,
        E_HANDLE_ = 0x80070006L,
        E_ABORT_ = 0x80004004L,
        E_FAIL_ = 0x80004005L,
        E_ACCESSDENIED_ = 0x80070005L,
        E_BOUNDS_ = 0x8000000BL,

        E_KEY_NOT_FOUND            = 0x80f60100L,
        E_ARRAY_EXPECTED           = 0x80f60101L,

        E_WRONG_TYPE                      = 0x80f60102L,
        E_INVALID_GUID                    = 0x80f60103L,

        E_INVALID_ENUM_VALUE               = 0x80f60108L,
        E_INVALID_ENUM_NAME                = 0x80f60109L,
        E_INVALID_ENUM_TYPE_INFO           = 0x80f6010AL,
        E_OBJECT_ENTRY_NOTFOUND            = 0x80f6010BL,

// For Type Info
        E_TYPE_INFO_NOT_REGISTERED         = 0x80f60300L,
        E_TYPE_INFO_ALREADY_REGISTERED     = 0x80f60301L,
        E_TYPE_INFO_ENUM_NOT_REGISTERED    = 0x80f60302L,

// Async operation error types
        E_ASYNC_OPERATION_CANCELED         = 0x80f60305L,
        E_ASYNC_OPERATION_TIMEOUT          = 0x80f60306L,

// Event Handler related Errors
        E_EVENT_HANDLER_EXIST                = 0x80f60200L,
        E_EVENT_HANDLER_NOT_FOUND            = 0x80f60201L,

// property value error types
        E_PV_E_WRONG_TYPE               = 0x80f80100L,
        E_PV_E_CAST_FAILED              = 0x80f80101L,

// property data error types
        E_PROPERTY_NOT_FOUND = 0x80f60310L,
        E_PROPERTY_NOT_DEFINED = 0x80f60311L,
        E_PROPERTY_READ_ONLY = 0x80f60312L,
        E_PROPERTY_EXIST = 0x80f60313L,
        E_METHOD_NOT_DEFINED = 0x80f60314L,

        // some Windows specific to match the C# exception
        E_TYPEMISMATCH = 0x80028CA0,
        RPC_E_SERVER_NOT_AVAILABLE = 0x800706ba,

    }

}

