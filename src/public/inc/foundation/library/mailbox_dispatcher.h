/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:mailbox_dispatcher.h
****/
#pragma once

#include <foundation/macros.h>
#include <foundation/com_ptr.h>
#include <pal/pal_api.h>
#include <foundation/activation_factory.h>
#include <foundation/delegate_library.h>

#include "interfaces/dispatcher.h"
#include "parameter_base.h"
#include "foundation_lib_api.h"

namespace foundation
{
    enum MailboxDispatcherOptions
    {
        None = 0x00,
    };

    namespace library
    {
        BEGIN_DECLARE_INTERFACE(IMailboxDispatcherDelegate, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(OnIdle)() = 0;
        END_DECLARE_INTERFACE()

        struct mailbox_dispatcher_create_parameters : public _CreateParameterBase<IMailboxDispatcherDelegate, MailboxDispatcherOptions>
        {
            mailbox_dispatcher_create_parameters():
                _auto_start(true),
                _queue_size(10),
                _queue_wait((UINT32)-1)
            {}

            bool _auto_start;
            UINT32 _queue_size;
            UINT32 _queue_wait;
        };

        //------------------------------------------------------------------------------
        // Interface:   IMailboxDispatcherFactory
        //
        // Purpose: An Interface to create a mailbox IDispatcher instances
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IMailboxDispatcherFactory, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(CreateMailboxDispatacher)(
                _In_ const mailbox_dispatcher_create_parameters *parameters,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

namespace foundation
{
    namespace library
    {
        inline library::IMailboxDispatcherFactory *get_mailbox_dispatcher_factory()
        {
            return GetFoundationLibraryFactory<library::IMailboxDispatcherFactory>(U("foundation.MailboxDispatcher"));
        }

        template <class T>
        HRESULT CreateMailboxDispatcherClass(
            _In_ const library::mailbox_dispatcher_create_parameters *parameters,
            T **ppT)
        {
            IFCPTR_ASSERT(parameters);

            InspectablePtr spInstance;

            IFR_ASSERT(get_mailbox_dispatcher_factory()->CreateMailboxDispatacher(
                parameters,
                nullptr,
                spInstance.GetAddressOf()));
            return foundation::QueryInterface(spInstance, ppT);
        }

        template <class T>
        HRESULT CreateMailboxDispatcherClass(
            UINT32 queue_size, 
            T **ppT)
        {
            library::mailbox_dispatcher_create_parameters ctor_params;
            ctor_params._queue_size = queue_size;
            return CreateMailboxDispatcherClass(&ctor_params, ppT);
        }

        template <class T>
        HRESULT CreateMailboxDispatcherClass(T **ppT)
        {
            library::mailbox_dispatcher_create_parameters ctor_params;
            return CreateMailboxDispatcherClass(&ctor_params, ppT);
        }

        template <class T>
        HRESULT CreateMailboxDispatcherClass(
            library::IMailboxDispatcherDelegate *pDelegate,
            T **ppT)
        {
            library::mailbox_dispatcher_create_parameters ctor_params;
            ctor_params.m_p_delegate = pDelegate;
            return CreateMailboxDispatcherClass(&ctor_params, ppT);
        }
        template <class TSink>
        class mailbox_dispatcher_delegate :
            public foundation::SinkDelegateBase <TSink, library::IMailboxDispatcherDelegate >
        {
        public:
            STDMETHOD(OnIdle)()
            {

                _DELEGATE_CALLBACK_PROLOGUE(TSink, library::IMailboxDispatcherDelegate);
                return this->m_Callback->on_idle_internal();
            }
        };
    }
}

