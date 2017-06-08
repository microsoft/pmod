/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MailboxDispatcher.h
****/
#pragma once

// SDK includes
#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>
#include <foundation/library/interfaces/dispatcher.h>
#include <foundation/library/interfaces/disposable.h>
#include <foundation/library/interfaces/component.h>
#include <foundation/library/interfaces/mailbox.h>

#include <pal/pal_thread.h>
#include <pal/pal_signal.h>
#include <foundation/library/mailbox_dispatcher.h>

class CMailboxDispatcher :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IDispatcher,
    public foundation::library::IDisposable,
    public foundation::library::IComponent
{
public:
    static HRESULT CreateInstance(
        _In_ const foundation::library::mailbox_dispatcher_create_parameters *parameters,
        _In_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

    HRESULT _Initialize(
        _In_ const foundation::library::mailbox_dispatcher_create_parameters *parameters);
protected:
    CMailboxDispatcher();

    // Interface IDispatcher
    STDMETHOD(HasThreadAccess)(_Out_ bool *pValue)override;

    STDMETHOD(RunAsync)(
        _In_ foundation::library::IDelegateDispatchable* _pDelegateDispatchable,
        foundation::IAsyncOperation **ppAsyncAction) override;

    // Interface IDisposable
    STDMETHOD(Dispose)() override;

    // Interface IComponent
    STDMETHOD(Start)() override;
    STDMETHOD(Stop)() override;

    // ComBase overrides
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;

protected:
    void FinalRelease() override;

private:

    static HRESULT _DispatcherTask(
        UINT32 wait_idle,
        const foundation::ComPtr<foundation::library::IMailbox> mailbox_ptr,
        const foundation::ComPtr<foundation::library::IMailboxDispatcherDelegate> mailbox_delegate_ptr
        );
    HRESULT DispatcherTask();
    HRESULT StartDispatcherTask();
    HRESULT StopDispatcherTask();
    bool HasThreadAccess();
    void ReleaseTaskMailbox();
    void FlushTaskMailbox();
    static void *STDAPICALLTYPE _DispatcherTaskStub(void *p);
private:
    UINT32 _wait_idle;
    UINT32 _queue_size;
    foundation::library::IMailbox *_p_task_mailbox;
    foundation::ComPtr<foundation::library::IMailboxDispatcherDelegate> _mailbox_delegate_ptr;
    _pal_ThreadHandle _dispatcher_thread;
    UINT64      _dispatcher_thread_id;
};

class CMailboxDispatcherFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IMailboxDispatcherFactory
{
public:
    static foundation::library::IMailboxDispatcherFactory *GetInstance();

    STDMETHOD(CreateMailboxDispatacher)(
        _In_ const foundation::library::mailbox_dispatcher_create_parameters *parameters,
        _In_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance);
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IMailboxDispatcherFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IMailboxDispatcherFactory *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};
