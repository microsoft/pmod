/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NamespaceStatics.h
****/

#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/interfaces/namespace_statics.h>
#include <map>

//------------------------------------------------------------------------------
// Class:   CNamespaceStatics
//
// Purpose: Namespace manager class
//
//------------------------------------------------------------------------------
#include <map>

#include <foundation/hstring_wrapper.h>
#include <foundation/library/util_stl.h>
#include "FoundationUtilHelper.h"

class CNamespaceStatics :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::INamespaceStatics
{
public:
    static foundation::library::INamespaceStatics *GetInstance();

    STDMETHOD(RegisterNamespace)(
        _In_ HSTRING foundationNamespace,
        _In_ HSTRING runtimeNamespace) override;

    STDMETHOD(LookupRuntimeNamespace)(
        _In_ HSTRING foundationNamespace,
        _In_ HSTRING *runtimeNamespace) override;

    STDMETHOD(LookupFoundationNamespace)(
        _In_ HSTRING runtimeNamespace,
        _Out_ UINT32 *size,
        _Out_ HSTRING **ppFoundationNamespaces) override;
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        HRESULT hr = S_OK;
        if (iid == foundation::library::INamespaceStatics::GetIID())
        {
            *ppInterface = static_cast<foundation::library::INamespaceStatics *>(this);
        }
        else
        {
            hr = foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return hr;
    }

private:
    typedef std::map<foundation::HStringPtr, foundation::HStringPtr, foundation::library::HStringPtrLessThan> _Namespace_Map_Type;

    _Namespace_Map_Type _registeredNamespaces;
};



