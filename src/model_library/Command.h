/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Command.h
****/
#pragma once

#include <pmod/library/model_library.h>
#include "ObjectNode.h"

class CCommandLogFactory : public _ObjectBaseLogFactory<>
{
public:
    static const CLogCategory*GetLogCategory();
};

typedef ObjectNode
<
	pmod::ICommand,
	pmod::ICommandInfo,
	pmod::library::ICommandClass,
	pmod::library::ICommandDelegate,
	pmod::library::CommandCreateParameters,
	pmod::ICanExecuteChangedEventSource,
	pmod::ICanExecuteChangedEventHandler,
	foundation::IUnknown,
	pmod::ICanExecuteChangedEventArgs,
	CCommandLogFactory
>
_CommandBase_Type;

class CCommand :
    public _CommandBase_Type
{
public:
    static HRESULT CreateInstance(
        _In_ const pmod::library::CommandCreateParameters *pCommandModelParam,
    _In_ foundation::IInspectable *pOuter,
    _Outptr_ foundation::IInspectable **ppNewInstance);

    HRESULT _Initialize(const pmod::library::CommandCreateParameters *pCommandModelParam);

protected:

#if defined(_WINDOWS_RUNTIME)
    INSPECTABLE_CLASS(U("Microsoft.PropertyModel.Library.CommandModel"), foundation::TrustLevel::BaseTrust);
#endif

    CCommand() :
             m_CommandId(0),
             m_CommandState((int)pmod::CommandStateUnknown),
             m_AllowReturnAsyncOnVoidResult(false)
    {
    }

    #if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override;

    #endif

    // ICommand
    STDMETHOD (GetState)(UINT32* pVal);
    STDMETHOD (CanExecute)(_In_ foundation::IInspectable *parameter,_Outptr_ bool *pValue);
    STDMETHOD (Execute)(_In_ foundation::IInspectable *parameter,foundation::IInspectable **ppResult);
    STDMETHOD (GetCanExecuteChangedEventSource)(_Outptr_ pmod::ICanExecuteChangedEventSource** ppEventSource);

    // Override foundation::ctl::ComBase

    // ICommandClass

    // Property 'ExecuteDelegate'
    STDMETHOD(GetExecuteDelegate)(_COM_Outptr_ pmod::library::IExecuteDelegate **ppExecuteDelegate);
    STDMETHOD(SetExecuteDelegate)(_In_ pmod::library::IExecuteDelegate *pExecuteDelegate);
    // Property 'CanExecuteDelegate'
    STDMETHOD(GetCanExecuteDelegate)(_COM_Outptr_ pmod::library::ICanExecuteDelegate **ppCanExecuteDelegate);
    STDMETHOD(SetCanExecuteDelegate)(_In_ pmod::library::ICanExecuteDelegate *pCanExecuteDelegate);

    STDMETHOD (SetCommandState)(_In_ UINT32 commandState,_In_ bool fireCanExecute);
    STDMETHOD (FireCanExecuteChanged)();

    // ComBase Override
    void OnFinalRelease() override;

protected:
    HRESULT InternalFireCanExecuteChanged(UINT32 oldState,UINT32 newState);
    HRESULT InternalFireCanExecuteChanged(_In_ pmod::ICanExecuteChangedEventArgs* pEventArgs);
    
    /// logging methods 
    void LogExecute(_In_ UINT32 uniqueId, _In_ foundation::IInspectable *parameter);
    void LogCanExecute(_In_ UINT32 uniqueId, _In_ foundation::IInspectable *parameter, _In_ bool result);

private:

    HRESULT VerifyCommandParameterTypeInfo(_Inout_ foundation::IInspectable **ppParameter);
    HRESULT VerifyCommandResultTypeInfo(_Inout_ foundation::IInspectable **ppResult);
    HRESULT on_execute_internal(
        _In_ bool isDispatched,
        _In_ bool isAsync,
        _In_ foundation::IInspectable *parameter,
        foundation::IInspectable **ppResult);

private:
    UINT32 m_CommandId;
    UINT32 m_CommandState;
    bool   m_AllowReturnAsyncOnVoidResult : 1;

    foundation::ComPtr<pmod::library::IExecuteDelegate>         m_ExecuteDelegate;
    foundation::ComPtr<pmod::library::ICanExecuteDelegate>      m_CanExecuteDelegate;
};


