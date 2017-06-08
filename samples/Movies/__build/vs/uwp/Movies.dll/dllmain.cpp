/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dllmain.cpp
****/

#include <winrt/com_activation.h>
#include <pmod/library/library_util.h>
#include <foundation/library/logger_macros.h>
#include "Movies.h"

namespace Movies {
    HRESULT RegisterModernTypeAdapters();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" void RegisterMoviesIf();

class CClassStatics :
    public ctlwinrt::AbstractActivationFactory,
    public ABI::Movies::IClassStatics
{
public:
    HRESULT STDMETHODCALLTYPE Register()
    {
        RegisterMoviesIf();
        // register the winrt adapters for this model
        Movies::RegisterModernTypeAdapters();
        return S_OK;
    }

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if(iid == __uuidof(ABI::Movies::IClassStatics))
        {
            *ppInterface = static_cast<ABI::Movies::IClassStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};

STDAPI DllGetActivationFactory(_In_ HSTRING hstrAcid, _Deref_out_ IActivationFactory** factory)
{
    HRESULT hr = S_OK;

    LPCWSTR szClassID = ::WindowsGetStringRawBuffer(hstrAcid, nullptr);

    if(wcscmp(szClassID, L"Movies.Class") == 0)
    {
        CClassStatics *pClassStatics;
        foundation::ctl::inspectable::CreateInstance(&pClassStatics);
        * factory = pClassStatics;
        return S_OK;
    }
 
    return CLASS_E_CLASSNOTAVAILABLE;
}

