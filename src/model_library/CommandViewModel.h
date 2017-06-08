/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandViewModel.h
****/

#pragma once

#include "Command.h"
#include "BaseModelSource.h"

typedef SingleSourceViewModelBase
    <
        CCommand,
        pmod::library::ICommandViewModelDelegate,
        pmod::ICommand,
        pmod::ICanExecuteChangedEventSource,
        pmod::ICanExecuteChangedEventHandler,
        pmod::ICanExecuteChangedEventArgs
    > _CommandViewModelBaseType;

class CCommandViewModel :
    public _CommandViewModelBaseType,
    public pmod::library::ICommandViewModelClass
{
public:
    HRESULT _Initialize(const pmod::library::CommandViewModelCreateParameters *modelImpl);

    static HRESULT CreateInstance(
        _In_ const pmod::library::CommandViewModelCreateParameters *pCommandViewModelParam,
        _In_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);


protected:
    HRESULT InvokeInternal(foundation::IUnknown* pSender,pmod::ICanExecuteChangedEventArgs* pArgs) override;

    HRESULT GetSourceEventSource(pmod::ICanExecuteChangedEventSource **pEventSource) override
    {
        return m_spObjectSource->GetCanExecuteChangedEventSource(pEventSource);
    }

    HRESULT AttachToSource() override;

    // ICommandViewModelClass
    STDMETHOD (SetCommandLabel)(_In_ HSTRING label);

protected:
    CCommandViewModel()
    {
    }
    virtual ~CCommandViewModel()
    {
    }

    // Overrides from EventSourceBase
    foundation::IUnknown *GetControllerDelegate() override
    {
        return this->AsInspectable();
    }

    // Override from ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::library::ICommandViewModelClass::GetIID())
        {
            *ppInterface = static_cast<pmod::library::ICommandViewModelClass *>(this);
        }
        else
        {
            return _CommandViewModelBaseType::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};



