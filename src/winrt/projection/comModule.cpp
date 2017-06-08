/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:comModule.cpp
****/
#include "pch.h"

#include "comModule.h"
using namespace ctlwinrt;

ULONG ctlwinrt::g_nObjects = 0;

IActivationFactory* module::GetActivationFactory(HSTRING hClassID)
{
    HRESULT hr = S_OK;
    activation_factory_entry *pEntry = __creationMap;
    IActivationFactory *pFound = nullptr;

#if defined(HSTRINGS_LEAK_DETECTION) && defined(DBG)
    struct HSTRING_HEAP_OBJ
    {
        HSTRING_HEAP_OBJ():m_hString(nullptr){};    
        HSTRING m_hString;
    } * pHeapObj = new HSTRING_HEAP_OBJ;
    pHeapObj->m_hString = hClassID;

    hClassID = reinterpret_cast<HSTRING>(pHeapObj);
#endif

	LPCWSTR szClassID = ::WindowsGetStringRawBuffer(hClassID, nullptr);

    while (pEntry->m_szClassID != nullptr)
    {
        if (!wcscmp(pEntry->m_szClassID, szClassID))
        {
            pFound = pEntry->GetFactory();
            break;
        }

        pEntry++;
    }

    if (pFound)
    {
        pFound->AddRef();
    }
   
#if defined(HSTRINGS_LEAK_DETECTION) && defined(DBG)
    delete pHeapObj;
#endif
    return pFound;
}

void module::ClearFactoryCache()
{
    HRESULT hr = S_OK;
    activation_factory_entry *pEntry = __creationMap;
    
    while (pEntry->m_szClassID != nullptr)
    {
        pEntry->DestroyFactory();
        pEntry++;
    }
}

void ctlwinrt::IncrementObjectCount()
{
    g_nObjects++;
}

void ctlwinrt::DecrementObjectCount()
{
    g_nObjects--;
}
