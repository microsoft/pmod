/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:command_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <pmod/interfaces/command.h>
#include <foundation/library/interfaces/delegate_base.h>
#include <foundation/library/interfaces/disposable.h>
#include <foundation/library/type_info_create_parameter.h>

#include "object_node_class.h"
#include "../pmod_lib_api.h"


namespace pmod
{
    namespace library
    {
        // IExecuteDelegate Interface
        BEGIN_DECLARE_INTERFACE(IExecuteDelegate, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(_In_opt_ foundation::IInspectable *parameter, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppResult) = 0;
        END_DECLARE_INTERFACE()

        // ICanExecuteDelegate Interface
        BEGIN_DECLARE_INTERFACE(ICanExecuteDelegate, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(_In_opt_ foundation::IInspectable *parameter, _Out_ bool *pValue) = 0;
        END_DECLARE_INTERFACE()

        // ICommandClass Interface
        BEGIN_DECLARE_INTERFACE(ICommandClass, IObjectNodeClass, PMOD_LIB_API)
            // Property 'ExecuteDelegate'
            STDMETHOD(GetExecuteDelegate)(_COM_Outptr_result_maybenull_ IExecuteDelegate **ppExecuteDelegate) = 0;
            STDMETHOD(SetExecuteDelegate)(_In_opt_ IExecuteDelegate *pExecuteDelegate) = 0;
            // Property 'CanExecuteDelegate'
            STDMETHOD(GetCanExecuteDelegate)(_COM_Outptr_result_maybenull_ ICanExecuteDelegate **ppCanExecuteDelegate) = 0;
            STDMETHOD(SetCanExecuteDelegate)(_In_opt_ ICanExecuteDelegate *pCanExecuteDelegate) = 0;

            STDMETHOD(SetCommandState)(_In_ UINT32 commandState, _In_ bool fireCanExecute) = 0;
            STDMETHOD(FireCanExecuteChanged)() = 0;
        END_DECLARE_INTERFACE()

        // Delegate Interface for a Command Model Impl
        BEGIN_DECLARE_INTERFACE(ICommandDelegate, foundation::library::IDelegateBase<ICanExecuteChangedEventHandler>, PMOD_LIB_API)
            STDMETHOD(OnCanExecute)(_In_opt_ foundation::IInspectable *parameter, _Out_ bool *pValue) = 0;
            STDMETHOD(OnExecute)(_In_opt_ foundation::IInspectable *parameter, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppResult) = 0;
        END_DECLARE_INTERFACE()

        enum class CommandOptions
        {
            None = 0x00,
            NoReturnAsyncOnVoidResult = 0x100
        };

        struct CommandCreateParameters :
            public foundation::library::_TypeInfoCreateParameterBase
            <
            ICommandDelegate,
            CommandOptions
            >
        {
            int                     m_command_id;
        };

        BEGIN_DECLARE_INTERFACE(ICommandClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateCommandInstance)(
                _In_ const CommandCreateParameters *pCreateParams,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()

    }
}

