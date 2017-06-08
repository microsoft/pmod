/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationLibraryInterfaces.cpp
****/
#include "pch.h"

#include <foundation/library/interfaces/json_statics.h>
#include <foundation/library/interfaces/async_operation_class.h>
#include <foundation/library/interfaces/logger_class.h>
#include <foundation/library/interfaces/logger_factory.h>
#include <foundation/library/interfaces/dictionary_factory.h>
#include <foundation/library/interfaces/event_source_class.h>

#include <foundation/library/interfaces/object_dispatch_factory.h>
#include <foundation/library/interfaces/fast_properties_container.h>
#include <foundation/library/interfaces/persistant_storage_service.h>
#include <foundation/library/interfaces/component.h>
#include <foundation/library/interfaces/input_stream.h>
#include <foundation/library/interfaces/output_stream.h>
#include <foundation/library/interfaces/object_base_factory.h>

#include <foundation/library/mailbox_library.h>
#include <foundation/library/mailbox_dispatcher.h>



// {82BD56D2-19BE-47FF-B44C-766DF8CA862D}
const GUID foundation::library::IID_IMailbox =
{ 0x82bd56d2, 0x19be, 0x47ff,{ 0xb4, 0x4c, 0x76, 0x6d, 0xf8, 0xca, 0x86, 0x2d } };

// {96AA6FD7-1D75-4A88-B716-8335CF257036}
const GUID foundation::IID_IObjectBaseEventHandler =
{ 0x96aa6fd7, 0x1d75, 0x4a88,{ 0xb7, 0x16, 0x83, 0x35, 0xcf, 0x25, 0x70, 0x36 } };

// {684F09E3-2DDC-416C-BA95-7975F6AD19D3}
const GUID foundation::IID_IObjectBaseEventSource =
{ 0x684f09e3, 0x2ddc, 0x416c,{ 0xba, 0x95, 0x79, 0x75, 0xf6, 0xad, 0x19, 0xd3 } };


// {BFD5B0DD-622E-42CE-80AD-DC04977B094D}
const GUID foundation::library::IID_IAsyncOperationClass =
{ 0xbfd5b0dd, 0x622e, 0x42ce,{ 0x80, 0xad, 0xdc, 0x4, 0x97, 0x7b, 0x9, 0x4d } };


// {6753AD4D-311C-4555-B7C7-046BCD447FD2}
const GUID foundation::library::IID_ILoggerClass =
{ 0x6753ad4d, 0x311c, 0x4555,{ 0xb7, 0xc7, 0x4, 0x6b, 0xcd, 0x44, 0x7f, 0xd2 } };

// {6079B912-AAC5-498F-8797-CC48B3B1B27D}
const GUID foundation::library::IID_ILoggerFactory =
{ 0x6079b912, 0xaac5, 0x498f,{ 0x87, 0x97, 0xcc, 0x48, 0xb3, 0xb1, 0xb2, 0x7d } };

// {D45EA934-E0CB-41F3-9B00-9ADF7661A0AE}
const GUID foundation::library::IID_ITextLogListenerDelegate =
{ 0xd45ea934, 0xe0cb, 0x41f3,{ 0x9b, 0x0, 0x9a, 0xdf, 0x76, 0x61, 0xa0, 0xae } };

// {6CED4AF5-B5D7-402F-9C67-54528A003472}
const GUID foundation::library::IID_IBinaryLogListenerDelegate =
{ 0x6ced4af5, 0xb5d7, 0x402f,{ 0x9c, 0x67, 0x54, 0x52, 0x8a, 0x0, 0x34, 0x72 } };

// {D93FED73-B9CA-4A8A-AAD9-B18B183C5FD3}
const GUID foundation::library::IID_IObjectClass =
{ 0xd93fed73, 0xb9ca, 0x4a8a,{ 0xaa, 0xd9, 0xb1, 0x8b, 0x18, 0x3c, 0x5f, 0xd3 } };

// {17B4E2AB-10F4-4897-8966-2D747389FB9E}
const GUID foundation::library::IID_IAsyncOperationDelegate =
{ 0x17b4e2ab, 0x10f4, 0x4897,{ 0x89, 0x66, 0x2d, 0x74, 0x73, 0x89, 0xfb, 0x9e } };

// {D0E4E290-C7F7-4E41-BBFE-D06B8F122B37}
const GUID foundation::library::IID_IPersistantStorageService =
{ 0xd0e4e290, 0xc7f7, 0x4e41,{ 0xbb, 0xfe, 0xd0, 0x6b, 0x8f, 0x12, 0x2b, 0x37 } };

// {EBA64C2F-3B20-46A9-B1A8-322A8B0C5BFC}
const GUID foundation::library::IID_IMailboxDispatcherDelegate =
{ 0xeba64c2f, 0x3b20, 0x46a9,{ 0xb1, 0xa8, 0x32, 0x2a, 0x8b, 0xc, 0x5b, 0xfc } };

// {F3584802-9E47-43CD-A828-12C9E07999C6}
const GUID foundation::library::IID_IComponent =
{ 0xf3584802, 0x9e47, 0x43cd,{ 0xa8, 0x28, 0x12, 0xc9, 0xe0, 0x79, 0x99, 0xc6 } };

// {CEDBF473-46E1-46D8-84FE-0691592969B3}
const GUID foundation::library::IID_IInputStream =
{ 0xcedbf473, 0x46e1, 0x46d8,{ 0x84, 0xfe, 0x6, 0x91, 0x59, 0x29, 0x69, 0xb3 } };

// {9F9C2CAD-28B9-4EEE-84B4-D73AAD28554A}
const GUID foundation::library::IID_IOutputStream =
{ 0x9f9c2cad, 0x28b9, 0x4eee,{ 0x84, 0xb4, 0xd7, 0x3a, 0xad, 0x28, 0x55, 0x4a } };

// Note: this were copied from the Proxy GUID definition
 EXTERN_C FOUNDATION_LIB_API const GUID IID_IBaseModelProxyPrivate =
{ 0x75a64e27, 0x45c8, 0x4bed, { 0xa1, 0xbe, 0x29, 0x8c, 0xb8, 0xe, 0xa0, 0x91 } };

