/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Mailbox.h
****/
#pragma once

// SDK includes
#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>
#include <foundation/library/mailbox_library.h>

class CMailboxFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IMailboxFactory
{
public:
    static foundation::library::IMailboxFactory *GetInstance();

    STDMETHOD(CreateMailbox)(
        _In_ const foundation::library::mailbox_create_parameters *parameters,
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance)override;
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IMailboxFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IMailboxFactory *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

};
