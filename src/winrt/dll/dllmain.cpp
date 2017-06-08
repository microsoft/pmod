/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dllmain.cpp
****/

#include "dllmain.h"
#include <foundation/library/logger_macros.h>
#include "..\projection\comModule.h"

#ifdef DEBUG
#include <pal/library/trace_util.h>
#endif

EXTERN_C  HRESULT  STDAPICALLTYPE RegisterMoAdapterFactory();

extern UINT32 GetObjectBaseCount();

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

#ifdef DEBUG
    pal::library::TraceMessage(U("pmod_uwp.DllMain dwReason:%d\n"), dwReason);
#endif

    switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
        break;
	case DLL_PROCESS_DETACH:
        ctlwinrt::__module.ClearFactoryCache();
		break;
	}
	return TRUE;

}

// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow(void)
{
    // TODO: figure the object base count
    //return GetObjectBaseCount() == 0 ? S_OK : S_FALSE;
    return S_FALSE;
}

STDAPI DllGetActivationFactory(_In_ HSTRING hstrAcid, _Deref_out_ IActivationFactory** factory)
{
    HRESULT hr = S_OK;

#ifdef DEBUG
    pal::library::TraceMessage(U("DllGetActivationFactory activationId:%s\n"), _pal_GetStringRawBuffer(hstrAcid,nullptr));
#endif

    IFR(RegisterMoAdapterFactory());
    
    *factory = ctlwinrt::__module.GetActivationFactory(hstrAcid);
    if (*factory == nullptr)
    {
        IFHR(CLASS_E_CLASSNOTAVAILABLE);
    }

    return hr;
}
