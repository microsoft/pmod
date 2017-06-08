/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:command.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <foundation/interfaces/async_operation.h>
#include <foundation/foundation_macros.h>

namespace pmod {

    enum CommmandModelState
    {
        CommandStateUnknown = 0,    // The command state is unknown so you would need to ask CanExecute to get the enable/disable 
        CommandStateEnabled = 1,    // command is always enabled no matter which parameter you could pass
        CommandStateDisabled = 2,    // Command is always disable no matter which parameter you pass
        CommandNotAvailable = 3    // The Command is not available
    };
    BEGIN_DECLARE_INTERFACE(ICommandInfo, foundation::IObjectTypeInfo, PMOD_API)
        STDMETHOD(GetParameterTypeInfo)(_COM_Outptr_ foundation::IPropertyTypeInfo **ppVarTypeInfo) = 0;
        STDMETHOD(GetResultTypeInfo)(_COM_Outptr_ foundation::IPropertyTypeInfo **ppVarTypeInfo) = 0;
        STDMETHOD(GetIsAsync)(_Out_ bool *value) = 0;
    END_DECLARE_INTERFACE()

    /////////////////////////////////// Command Model //////////////////////////////////////////////
    BEGIN_DECLARE_INTERFACE(ICanExecuteChangedEventArgs, foundation::IInspectable, PMOD_API)
        STDMETHOD(GetOldState)(_Out_ UINT32* pState) = 0;
        STDMETHOD(GetNewState)(_Out_ UINT32* pState) = 0;
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(ICanExecuteChanged, PMOD_API)

    struct ICommandBase : public foundation::IObject
    {
        typedef foundation::IInspectable*  _TParameter_Type;
        typedef foundation::IInspectable*  _TResult_Type;
        
        // Properties
        STDMETHOD(GetState)(_Out_ UINT32* pVal) = 0;
        // Event methods.
        STDMETHOD(GetCanExecuteChangedEventSource)(_COM_Outptr_ ICanExecuteChangedEventSource** ppEventSource) = 0;
    };
    template <class TParameter, class TResult, const IID& iidType>
    struct ICommand_impl : public ICommandBase
    {
        static const IID& GetIID() { return iidType; }

        typedef TParameter  _TParameter_Type;
        typedef TResult     _TResult_Type;

        // Methods.
        STDMETHOD(CanExecute)(TParameter parameter, _Out_ bool *pCanExecute) = 0;
        STDMETHOD(Execute)(TParameter parameter, TResult *pResult) = 0;
    };
    BEGIN_DECLARE_IID(ICommand, PMOD_API);
    typedef ICommand_impl<foundation::IInspectable *, foundation::IInspectable *, IID_ICommand> ICommand;

    BEGIN_DECLARE_IID(IAsyncCommand, PMOD_API);
    typedef ICommand_impl<foundation::IInspectable *, foundation::IAsyncOperation *, IID_IAsyncCommand> IAsyncCommand;

    BEGIN_DECLARE_INTERFACE(ISimpleCommand, ICommandBase, PMOD_API)
        // Methods.
        STDMETHOD(CanExecute)(_Out_ bool *pCanExecute) = 0;
        STDMETHOD(Execute)() = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(ISimpleAsyncCommand, ICommandBase, PMOD_API)
        typedef foundation::IAsyncOperationBase*  _TResult_Type;
        // Methods.
        STDMETHOD(CanExecute)(_Out_ bool *pCanExecute) = 0;
        STDMETHOD(Execute)(_COM_Outptr_ foundation::IAsyncOperationBase **ppResult) = 0;
    END_DECLARE_INTERFACE()

    template <class TParameter, const IID& iidType>
    struct ICommandParameter_impl : public ICommandBase
    {
        static const IID& GetIID() { return iidType; }

        typedef TParameter  _TParameter_Type;

        // Methods.
        STDMETHOD(CanExecute)(TParameter parameter, bool *pCanExecute) = 0;
        STDMETHOD(Execute)(TParameter parameter) = 0;
    };

    template <class TResult, const IID& iidType>
    struct ICommandResult_impl : public ICommandBase
    {
        static const IID& GetIID() { return iidType; }
        typedef TResult     _TResult_Type;

        // Methods.
        STDMETHOD(CanExecute)(bool *pCanExecute) = 0;
        STDMETHOD(Execute)(TResult *pResult) = 0;
    };

	template <const IID& iidType>
	struct ISimpleCommand_impl : public ISimpleCommand
	{
		static const IID& GetIID() { return iidType; }
	};
}
