/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Mailbox.cpp
****/
#include "pch.h"

#include "Mailbox.h"

#include <pal/pal_signal.h>
#include <pal/pal_semaphore.h>
#include <pal/pal_critical_section.h>
#include <pal/pal_hstring.h>
#include <foundation/library/activation_factory.h>

#include <foundation/assert.h>
#include <foundation/macros.h>

#include "FoundationUtilHelper.h"
#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;
using namespace foundation::library;

// {22F1B9EA-527B-4399-BEFA-8A0EEE198D73}
const GUID foundation::library::IID_IMailboxFactory =
{ 0x22f1b9ea, 0x527b, 0x4399,{ 0xbe, 0xfa, 0x8a, 0xe, 0xee, 0x19, 0x8d, 0x73 } };

class CMailbox :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IMailbox
{
public:
    static HRESULT CreateInstance(
        _In_ const foundation::library::mailbox_create_parameters *parameters,
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

    HRESULT _Initialize(
        _In_ const foundation::library::mailbox_create_parameters *parameters);
protected:
    CMailbox();

    // ComBase overrides
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IMailbox::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IMailbox *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    void FinalRelease() override;

    // Interface IMailbox
    STDMETHOD(GetCount)(UINT32 *pValue)
    {
        *pValue = GetMessages();
        return S_OK;
    }

    STDMETHOD(Put)(void *Data, UINT32 waitTime)
    {
        return PutSlot(Data, waitTime) ? S_OK : S_FALSE;
    }

    STDMETHOD(Get)(void *Data, UINT32 waitTime)
    {
        return GetSlot(Data, waitTime) ? S_OK : S_FALSE;
    }

private:
    UINT32 GetMessages();

    bool GetSlot(LPVOID data, UINT32 wait_time);
    bool PutSlot(LPVOID data, UINT32 wait_time);
private:
    _PAL_CRITICAL_SECTION _put_cs;
    _PAL_CRITICAL_SECTION _get_cs;
    _PAL_SEMAPHORE          _put_sema;
    _PAL_SEMAPHORE          _get_sema;

    UINT8 * _slot_buffer;
    UINT32  _next_in;
    UINT32  _next_out;
    UINT32  _slot_size;
    UINT32  _slots;

};

HRESULT CMailbox::CreateInstance(
    _In_ const foundation::library::mailbox_create_parameters *parameters,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return foundation::ctl::inspectable::CreateInstanceWithInitialize<CMailbox>(pOuter, ppNewInstance, parameters);
}

CMailbox::CMailbox():
    _slot_buffer(nullptr),
    _next_in(0),
    _next_out(0)
{
    this->_next_in = 0;
    this->_next_out = 0;
}

HRESULT CMailbox::_Initialize(
    _In_ const foundation::library::mailbox_create_parameters *parameters)
{
    this->_slots = parameters->_slot_count;
    this->_slot_size = parameters->_slot_size;

    _pal_CreateSemaphore(&_put_sema, this->_slots, this->_slots);
    _pal_CreateSemaphore(&_get_sema, 0, this->_slots);
    _pal_InitializeCriticalSection(&_put_cs, true);
    _pal_InitializeCriticalSection(&_get_cs, true);
    this->_slot_buffer = static_cast<UINT8 *>(_pal_MemAlloc(this->_slot_size*this->_slots));

    return S_OK;
}

void CMailbox::FinalRelease()
{
    _pal_DeleteSemaphore(&this->_put_sema);
    _pal_DeleteSemaphore(&this->_get_sema);
    _pal_DeleteCriticalSection(&this->_put_cs);
    _pal_DeleteCriticalSection(&this->_get_cs);
    _pal_MemFree(this->_slot_buffer);

    foundation::ctl::ComInspectableBase::FinalRelease();
}

UINT32 CMailbox::GetMessages()
{
    UINT32 messages;
    _pal_EnterCriticalSection(&this->_get_cs);
    _pal_EnterCriticalSection(&this->_put_cs);

    if (_next_in >= _next_out)
    {
        messages = _next_in - _next_out;
    }
    else
        messages = _slots - (_next_out - _next_in);
    _pal_LeaveCriticalSection(&this->_get_cs);
    _pal_LeaveCriticalSection(&this->_put_cs);

    return messages;
}

bool CMailbox::GetSlot(LPVOID data, UINT32 wait_time)
{
    bool bRet;
    if (wait_time == (UINT32)-1)
    {
        _pal_WaitSemaphore(&_get_sema);
        bRet = true;
    }
    else if (wait_time == 0)
    {
        bRet = _pal_WaitCondSemaphore(&_get_sema);
    }
    else
    {
        bRet = _pal_WaitTimedSemaphore(&_get_sema, wait_time);
    }

    if (bRet)
    {
        _pal_EnterCriticalSection(&this->_get_cs);

        if (data)
        {
            void *item = _slot_buffer + (_next_out*_slot_size);
            memcpy(data, item, _slot_size);
        }

        _next_out++;
        _next_out %= _slots;
        _pal_LeaveCriticalSection(&this->_get_cs);

        ULONG prev_count;
        _pal_ReleaseSemaphore(&_put_sema, 1, &prev_count);
    }
    return bRet;
}

bool CMailbox::PutSlot(LPVOID data, UINT32 wait_time)
{
    bool bRet;
    if (wait_time == (UINT32)-1)
    {
        _pal_WaitSemaphore(&_put_sema);
        bRet = true;
    }
    else if (wait_time == 0)
    {
        bRet = _pal_WaitCondSemaphore(&_put_sema);
    }
    else
    {
        bRet = _pal_WaitTimedSemaphore(&_put_sema, wait_time);
    }

    if (bRet)
    {
        _pal_EnterCriticalSection(&this->_put_cs);

        void *item = _slot_buffer + _next_in*_slot_size;
        memcpy(item, data, _slot_size);

        _next_in++;
        _next_in %= _slots;

        _pal_LeaveCriticalSection(&this->_put_cs);

        ULONG prev_count;
        _pal_ReleaseSemaphore(&_get_sema, 1, &prev_count);
    }
    return bRet;
}

foundation::library::IMailboxFactory *CMailboxFactory::GetInstance()
{
    return _GetFactoryInstance<CMailboxFactory, foundation::library::IMailboxFactory>();
}

STDMETHODIMP CMailboxFactory::CreateMailbox(
    _In_ const foundation::library::mailbox_create_parameters *parameters,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CMailbox::CreateInstance(parameters, pOuter, ppNewInstance);
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.Mailbox"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CMailboxFactory::GetInstance)
    );
