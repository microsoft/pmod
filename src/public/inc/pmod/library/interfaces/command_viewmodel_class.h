/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:command_viewmodel_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <pmod/interfaces/command.h>
#include <foundation/library/interfaces/delegate_base.h>

#include "source_model_delegate.h"
#include "view_model_parameters.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // Delegate Interface for a Command Model View Impl
        typedef ISingleSourceModelDelegateBase<ICanExecuteChangedEventHandler, ICommand> ICommandViewModelDelegateBase;
        BEGIN_DECLARE_INTERFACE(ICommandViewModelDelegate, ICommandViewModelDelegateBase, PMOD_LIB_API)
            STDMETHOD(OnSourceCommandStateChanged)(_In_ int oldState, _In_ int newState) = 0;
        END_DECLARE_INTERFACE()

        // ICommandViewModelClass Interface
        BEGIN_DECLARE_INTERFACE(ICommandViewModelClass, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(SetCommandLabel)(_In_ HSTRING label) = 0;
        END_DECLARE_INTERFACE()

        enum class CommandViewModelOptions
        {
            None = 0x00
        };

        struct CommandViewModelCreateParameters : public
            _SingleSourceViewModelCreateParametersBase
            <
            CommandCreateParameters,
            ICommandViewModelDelegate,
            CommandViewModelOptions,
            ICommand
            >
        {
            LPCSTR_t                        m_command_label;           // The Command Label
        };

        BEGIN_DECLARE_INTERFACE(ICommandViewModelClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateCommandViewModelInstance)(
                _In_ const CommandViewModelCreateParameters *pViewModelImpl,
                _In_opt_ foundation::IInspectable *pOuter,
                _Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

