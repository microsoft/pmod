/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:command_class_wrapper.h
****/
#pragma once

#include "object_node_class_wrapper.h"
#include "interfaces/command_class.h"

namespace pmod
{
    namespace library {

        template <class T>
        class _CommandClassWrapper :
            public _ObjectNodeClassWrapper<T, ICommandClass>
        {
        public:
            HRESULT RaiseCanExecuteChanged(UINT32 commandState)
            {
                return this->GetClassInterfaceInternal()->SetCommandState(commandState, true);
            }

            HRESULT RaiseCanExecuteChanged(bool newValue)
            {
                return this->GetClassInterfaceInternal()->SetCommandState(newValue ? CommandStateEnabled : CommandStateDisabled, true);
            }

            HRESULT SetExecuteDelegate(_In_ IExecuteDelegate *pExecuteDelegate)
            {
                return this->GetClassInterfaceInternal()->SetExecuteDelegate(pExecuteDelegate);
            }

            HRESULT GetExecuteDelegate(_Outptr_ IExecuteDelegate **pExecuteDelegate)
            {
                return this->GetClassInterfaceInternal()->GetExecuteDelegate(pExecuteDelegate);
            }
        };
    }
}
