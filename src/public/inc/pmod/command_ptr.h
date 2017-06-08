/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:command_ptr.h
****/
#pragma once

#include "object_node_ptr.h"
#include <foundation/pv_util.h>
#include <pmod/interfaces.h>

namespace pmod
{
    template <class T, class TPublicInterface>
    class _CommandPtrWrapper : public _ObjectNodePtrWrapper<T, TPublicInterface>
    {
    public:
        UINT32 GetState()
        {
            UINT32 state;
            IGNOREHR(this->GetClassInterface()->GetState(&state));
            return state;
        }

        HRESULT Execute(
            typename TPublicInterface::_TParameter_Type parameter, 
            typename TPublicInterface::_TResult_Type* result)
        {
            return this->GetClassInterface()->Execute(parameter, result);
        }

        HRESULT CanExecute(
            typename TPublicInterface::_TParameter_Type parameter,
            bool* result)
        {
            return this->GetClassInterface()->CanExecute(parameter, result);
        }

        HRESULT Execute(
            typename TPublicInterface::_TResult_Type* result)
        {
            return this->GetClassInterface()->Execute(result);
        }

        HRESULT CanExecute(
            bool* result)
        {
            return this->GetClassInterface()->CanExecute(result);
        }

        HRESULT Execute(
            typename TPublicInterface::_TParameter_Type parameter)
        {
            return this->GetClassInterface()->Execute(parameter);
        }
        HRESULT Execute()
        {
            return this->GetClassInterface()->Execute();
        }
    };

template <class T>
class _CommandPtr : 
    public foundation::_ObjectPtr
    <
        _CommandPtrWrapper<_CommandPtr<T>, T>,
        T
    >
{
public:
    typedef foundation::_ObjectPtr
        <
        _CommandPtrWrapper<_CommandPtr<T>, T>,
        T
        > _Base_Type;

    _CommandPtr() throw()
    {
    }

    _CommandPtr(_Inout_ const _CommandPtr<T>& other) throw() :
        _Base_Type(other)
    {
    }

    _CommandPtr(_Inout_ const foundation::ComPtr<T>& lp) throw() :
        _Base_Type(lp.get())
    {
    }

    _CommandPtr(_In_ T* lp) throw() :
        _Base_Type(lp)
    {
    }
};

typedef _CommandPtr<ICommand> CommandPtr;
typedef _CommandPtr<ISimpleCommand> SimpleCommandPtr;

typedef _CommandPtr<IAsyncCommand> AsyncCommandPtr;
typedef _CommandPtr<ISimpleAsyncCommand> SimpleAsyncCommandPtr;

}
