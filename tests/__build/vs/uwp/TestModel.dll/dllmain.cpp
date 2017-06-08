/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dllmain.cpp
****/

#include <winrt/com_activation.h>
#include "TestModelTypeInfo.g.h"
#include "TestModel.h"

namespace TestModel {
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

class CClassStatics :
    public ctlwinrt::AbstractActivationFactory,
    public ABI::TestModel::IClassStatics
{
public:
    HRESULT STDMETHODCALLTYPE Register()
    {
		TestModel::RegisterTypeInfos();
		TestModel::RegisterModernTypeAdapters();
		return S_OK;
    }

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if(iid == __uuidof(ABI::TestModel::IClassStatics))
        {
            *ppInterface = static_cast<ABI::TestModel::IClassStatics *>(this);
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

    if(wcscmp(szClassID, L"TestModel.Class") == 0)
    {
		CClassStatics *pClassStatics;
		foundation::ctl::inspectable::CreateInstance(&pClassStatics);
        * factory = pClassStatics;
        return S_OK;
    }
 
    return CLASS_E_CLASSNOTAVAILABLE;
}

