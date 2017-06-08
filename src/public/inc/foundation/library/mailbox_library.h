/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:mailbox_library.h
****/
#pragma once

#include <foundation/macros.h>
#include <foundation/com_ptr.h>
#include <foundation/activation_factory.h>
#include "interfaces/mailbox.h"
#include "foundation_lib_api.h"

namespace foundation
{
    enum class MailboxOptions
    {
        None = 0x00,
    };

    namespace library
    {

        struct mailbox_create_parameters
        {
            mailbox_create_parameters() :
                _slot_size(0),
                _slot_count(0)
            {}

            UINT32 _slot_size;
            UINT32 _slot_count;
        };

        //------------------------------------------------------------------------------
        // Interface:   IMailboxFactory
        //
        // Purpose: An Interface to create a mailbox instances
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IMailboxFactory, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(CreateMailbox)(
                _In_ const mailbox_create_parameters *parameters,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

namespace foundation
{
    namespace library
    {
        inline library::IMailboxFactory *get_mailbox_factory()
        {
            return GetFoundationLibraryFactory<library::IMailboxFactory>(U("foundation.Mailbox"));
        }

        template <class T>
        HRESULT create_mailbox_class(
            _In_ const library::mailbox_create_parameters *parameters,
            _COM_Outptr_ T **ppT)
        {
            InspectablePtr spInstance;

            IFR_ASSERT(get_mailbox_factory()->CreateMailbox(
                parameters,
                nullptr,
                spInstance.GetAddressOf()));
            return foundation::QueryInterface(spInstance, ppT);
        }

        template <class T>
        HRESULT create_mailbox_class(UINT32 slot_size, UINT32 slot_count, _COM_Outptr_ T **ppT)
        {
            library::mailbox_create_parameters ctor_params;
            ctor_params._slot_size = slot_size;
            ctor_params._slot_count = slot_count;

            return create_mailbox_class(&ctor_params, ppT);
        }
    }
}

