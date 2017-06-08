/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NamespaceStatics.cpp
****/

#include "pch.h"

#include "NamespaceStatics.h"

#include <foundation/library/com_ptr_singleton.h>
#include "ActivationFactory.h"

using namespace foundation;

// {C06A6171-4AA6-4966-8D11-3E0CA5B0F862}
const GUID library::IID_INamespaceStatics =
{ 0xc06a6171, 0x4aa6, 0x4966,{ 0x8d, 0x11, 0x3e, 0xc, 0xa5, 0xb0, 0xf8, 0x62 } };

library::INamespaceStatics *CNamespaceStatics::GetInstance()
{
	return library::_GetFactoryInstance<CNamespaceStatics, library::INamespaceStatics>();
}

STDMETHODIMP CNamespaceStatics::RegisterNamespace(
    _In_ HSTRING foundationNamespace,
    _In_ HSTRING runtimeNamespace)
{
    _registeredNamespaces[HStringPtr(foundationNamespace)] = HStringPtr(runtimeNamespace);
    return S_OK;
}

STDMETHODIMP CNamespaceStatics::LookupFoundationNamespace(
    _In_ HSTRING runtimeNamespace,
    _Out_ UINT32 *size,
    _Out_ HSTRING **ppFoundationNamespaces)
{
    IFCPTR_ASSERT(size);
    IFCPTR_ASSERT(ppFoundationNamespaces);

    std::vector<HStringPtr> matches;

    for (_Namespace_Map_Type::const_iterator iter = _registeredNamespaces.begin();
        iter != _registeredNamespaces.end();
        ++iter)
    {
        if (_pal_strcmp((*iter).second.GetRawBuffer(), _pal_GetStringRawBuffer(runtimeNamespace,nullptr)) == 0)
        {
            matches.push_back((*iter).first);
        }
    }
    *size = (UINT32)matches.size();

    *ppFoundationNamespaces = (HSTRING*)_pal_MemAlloc(*size * sizeof(HSTRING));
    HSTRING *pNames = *ppFoundationNamespaces;
    for (std::vector<HStringPtr>::const_iterator iter = matches.begin();
        iter != matches.end();
        ++iter)
    {
        _pal_DuplicateString(*iter, pNames);
        ++pNames;
    }

    return S_OK;
}

STDMETHODIMP CNamespaceStatics::LookupRuntimeNamespace(
    _In_ HSTRING foundationNamespace,
    _In_ HSTRING *runtimeNamespace)
{
    _Namespace_Map_Type::const_iterator iter = _registeredNamespaces.find(HStringPtr(foundationNamespace));
    if (iter != _registeredNamespaces.end())
    {
        return _pal_DuplicateString((*iter).second, runtimeNamespace);
    }
    // not found
    return _pal_DuplicateString(foundationNamespace,runtimeNamespace);
}

static _RegisterActivationFactory _namespaceStaticsRegister(
    U("foundation.NamespaceStatics"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CNamespaceStatics::GetInstance)
    );
