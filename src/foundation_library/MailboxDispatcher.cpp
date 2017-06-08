/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MailboxDispatcher.cpp
****/
#include "pch.h"

#include "MailboxDispatcher.h"

#include <pal/pal_thread.h>
#include <foundation/library/async_operation_class.h>
#include <foundation/library/dispatcher_util.h>
#include <foundation/library/mailbox_library.h>
#include <foundation/library/activation_factory.h>

#include "FoundationUtilHelper.h"
#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;

// {7E8E4ED1-E1A9-4C90-9EDE-058F5E20B48C}
const GUID library::IID_IMailboxDispatcherFactory =
{ 0x7e8e4ed1, 0xe1a9, 0x4c90,{ 0x9e, 0xde, 0x5, 0x8f, 0x5e, 0x20, 0xb4, 0x8c } };

#define S_QUIT                                ((HRESULT)2L)


struct _task_mailbox_data{
    _task_mailbox_data() :
        _pDelegateDispatchable(nullptr),
        _pAsyncOperationClass(nullptr)
    {}

    library::IDelegateDispatchable* _pDelegateDispatchable;
    library::IAsyncOperationClass* _pAsyncOperationClass;
};

HRESULT CMailboxDispatcher::CreateInstance(
    _In_ const foundation::library::mailbox_dispatcher_create_parameters *parameters,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return ctl::inspectable::CreateInstanceWithInitialize<CMailboxDispatcher>(pOuter, ppNewInstance, parameters);
}

CMailboxDispatcher::CMailboxDispatcher() :
    _wait_idle(0),
    _queue_size(0),
    _dispatcher_thread_id(0),
    _p_task_mailbox(nullptr)
{}

bool CMailboxDispatcher::HasThreadAccess()
{
    return _dispatcher_thread_id == _pal_CurrentThreadId();
}

STDMETHODIMP CMailboxDispatcher::HasThreadAccess(_Out_ bool *pValue)
{
    if (_p_task_mailbox == nullptr)
    {
        *pValue = false;
        //foundation_assert(false);
        return E_UNEXPECTED;
    }

    *pValue = HasThreadAccess();
    return S_OK;
}

STDMETHODIMP CMailboxDispatcher::RunAsync(
    _In_ foundation::library::IDelegateDispatchable* pDelegateDispatchable,
    foundation::IAsyncOperation **ppAsyncAction)
{
    if (_p_task_mailbox == nullptr)
    {
        //foundation_assert(false);
        return E_UNEXPECTED;
    }

    _task_mailbox_data mb_data;
    mb_data._pDelegateDispatchable = pDelegateDispatchable;

    if (ppAsyncAction != nullptr)
    {
        IFR_ASSERT(foundation::library::CreateAsyncOperationClass(
            foundation_GUID_NULL,
            ppAsyncAction));
        foundation::QueryInterface(*ppAsyncAction, &mb_data._pAsyncOperationClass);
    }
    pDelegateDispatchable->AddRef();
    _p_task_mailbox->Put(&mb_data, (UINT32)-1);

    return S_OK;
}

STDMETHODIMP CMailboxDispatcher::Dispose()
{
    if (_p_task_mailbox != nullptr)
    {
		StopDispatcherTask();
    }
    return S_OK;
}

void CMailboxDispatcher::FinalRelease()
{
    Dispose();
    foundation::ctl::ComInspectableBase::FinalRelease();
}

void *STDAPICALLTYPE CMailboxDispatcher::_DispatcherTaskStub(void *p)
{
    reinterpret_cast<CMailboxDispatcher *>(p)->DispatcherTask();
    return nullptr;
}

HRESULT CMailboxDispatcher::_Initialize(_In_ const foundation::library::mailbox_dispatcher_create_parameters *parameters)
{
    this->_wait_idle = parameters->_queue_wait;
    this->_queue_size = parameters->_queue_size;

    this->_mailbox_delegate_ptr = parameters->m_p_delegate;

    if (parameters->_auto_start)
    {
        IFR_ASSERT(StartDispatcherTask());
    }
    return S_OK;
}

HRESULT CMailboxDispatcher::StartDispatcherTask()
{
    foundation_assert(_p_task_mailbox == nullptr);

    IFR_ASSERT(library::create_mailbox_class(
        sizeof(_task_mailbox_data), // t size
        this->_queue_size,    // slot count
        &_p_task_mailbox));

    _pal_CreateThread(&_dispatcher_thread, &_DispatcherTaskStub, this);
    return S_OK;
}

HRESULT CMailboxDispatcher::StopDispatcherTask()
{
    foundation_assert(_p_task_mailbox != nullptr);

    auto quit_func = []()
    {
        return S_QUIT;
    };
    if (!HasThreadAccess())
    {
        // post a quit message into our dispatcher thread
        foundation::RunSync(
            static_cast<library::IDispatcher *>(this),
            quit_func);

        // Wait for our main dispatcher to finish
        _pal_JoinThread(&_dispatcher_thread);

		ReleaseTaskMailbox();
    }
    else
    {
		FlushTaskMailbox();
        foundation::RunAsync(
            static_cast<library::IDispatcher *>(this),
            quit_func,
            nullptr);
        RELEASE_INTERFACE(_p_task_mailbox);
    }
    this->_dispatcher_thread_id = 0;

    return S_OK;
}

void CMailboxDispatcher::FlushTaskMailbox()
{
    foundation_assert(_p_task_mailbox != nullptr);
    HRESULT hr;
    _task_mailbox_data mb_data;
    while ((hr = _p_task_mailbox->Get(&mb_data, 0)) == S_OK)
    {
        if (mb_data._pAsyncOperationClass != nullptr)
        {
            mb_data._pAsyncOperationClass->SetCompleted(E_ABORT, nullptr);
            mb_data._pAsyncOperationClass->Release();
        }
        mb_data._pDelegateDispatchable->Release();
    }
}

void CMailboxDispatcher::ReleaseTaskMailbox()
{
    // make sure any entry in our mailbox data is being finished
    if (_p_task_mailbox != nullptr)
    {
		FlushTaskMailbox();
        RELEASE_INTERFACE(_p_task_mailbox);
    }
}

STDMETHODIMP CMailboxDispatcher::Start()
{
    if (_p_task_mailbox == nullptr)
    {
        return StartDispatcherTask();
    }
    return S_FALSE;
}

STDMETHODIMP CMailboxDispatcher::Stop()
{
    if (_p_task_mailbox != nullptr)
    {
        return StopDispatcherTask();
    }
    return S_FALSE;
}

HRESULT CMailboxDispatcher::_DispatcherTask(
    UINT32 wait_idle,
    const foundation::ComPtr<foundation::library::IMailbox> mailbox_ptr,
    const foundation::ComPtr<foundation::library::IMailboxDispatcherDelegate> mailbox_delegate_ptr
    )
{
    HRESULT hr = S_OK;
    while (hr != S_QUIT)
    {
        _task_mailbox_data mb_data;
        bool has_dispatchable = mailbox_ptr->Get(&mb_data, wait_idle) == S_OK;
        if (has_dispatchable)
        {
            hr = mb_data._pDelegateDispatchable->Invoke();
            if (mb_data._pAsyncOperationClass != nullptr)
            {
                mb_data._pAsyncOperationClass->SetCompleted(hr, nullptr);
                mb_data._pAsyncOperationClass->Release();
            }
            mb_data._pDelegateDispatchable->Release();
        }
        if (mailbox_delegate_ptr != nullptr)
        {
            mailbox_delegate_ptr->OnIdle();
        }
    }
    return hr;
}

HRESULT CMailboxDispatcher::DispatcherTask()
{
    this->_dispatcher_thread_id = _pal_CurrentThreadId();

    return _DispatcherTask(
        this->_wait_idle,
        _p_task_mailbox,
        _mailbox_delegate_ptr);
}

HRESULT CMailboxDispatcher::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    if (iid == library::IDispatcher::GetIID())
    {
        *ppInterface = static_cast<library::IDispatcher *>(this);
    }
    else if (iid == library::IDisposable::GetIID())
    {
        *ppInterface = static_cast<library::IDisposable *>(this);
    }
    else if (iid == library::IComponent::GetIID())
    {
        *ppInterface = static_cast<library::IComponent *>(this);
    }
    else
    {
        return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
    }
    return S_OK;
}

foundation::library::IMailboxDispatcherFactory *CMailboxDispatcherFactory::GetInstance()
{
    return library::_GetFactoryInstance<CMailboxDispatcherFactory, foundation::library::IMailboxDispatcherFactory>();
}

STDMETHODIMP CMailboxDispatcherFactory::CreateMailboxDispatacher(
    _In_ const foundation::library::mailbox_dispatcher_create_parameters *parameters,
    _In_ foundation::IInspectable *pOuter,
    _Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CMailboxDispatcher::CreateInstance(parameters, pOuter, ppNewInstance);
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.MailboxDispatcher"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CMailboxDispatcherFactory::GetInstance)
    );
