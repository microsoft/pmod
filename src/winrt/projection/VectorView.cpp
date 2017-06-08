/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:VectorView.cpp
****/

#pragma once
#include "pch.h"

#include "VectorView.h"

#define _DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

_DEFINE_GUID(VectorGuid, 0xb32bdca4, 0x5e52, 0x5b27, 0xbc, 0x5d, 0xd6, 0x6a, 0x1a, 0x26, 0x8c, 0x2a);
_DEFINE_GUID(VectorViewGuid, 0xa6487363, 0xb074, 0x5c60, 0xab, 0x16, 0x86, 0x6d, 0xce, 0x4e, 0xe5, 0x4d);
_DEFINE_GUID(IteratorGuid, 0x44a94f2d, 0x04f8, 0x5091, 0xb3, 0x36, 0xbe, 0x78, 0x92, 0xdd, 0x10, 0xbe);
_DEFINE_GUID(IterableGuid, 0x092b849b, 0x60b1, 0x52be, 0xa4, 0x4a, 0x6f, 0xe8, 0xe9, 0x33, 0xcb, 0xe4);
_DEFINE_GUID(ObservableVectorGuid, 0x7B81C56A, 0x0985, 0x518D, 0xBA, 0xA9, 0x0D, 0xA9, 0xAE, 0x00, 0x9F, 0x65);

IFACEMETHODIMP 
utils::View<IInspectable*>::IndexOf(
    _In_opt_ IInspectable* value, 
    __out UINT *index, 
    __out boolean *found)
{
    UNREFERENCED_PARAMETER(value);

    HRESULT hr = S_OK;
    std::list<IInspectable*>::iterator it;
    UINT nPosition = 0;

    for (it = m_list.begin(); it != m_list.end(); ++nPosition, ++it)
    {
        BOOLEAN areEqual = FALSE;
        // TODO: add an implementation for Equal
        //IFCL(PropertyValue::AreEqual(value, *it, &areEqual));
        if (areEqual)
        {
            *index = nPosition;
            *found = TRUE;
            goto Cleanup;
        }
    }

    *index = 0;
    *found = FALSE;
    IFCL(S_FALSE);

Cleanup:
    RRETURN(hr);
}
