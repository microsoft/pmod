/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeUtils.cpp
****/
#include "PInvokeUtils.h"
#include "pinvoke_api.h"

#include <foundation/com_ptr.h>
#include <foundation/pv_util.h>
#include <foundation/library/disposable_util.h>
#include <pal/pal_load_library.h>

using namespace foundation;

HRESULT EnsureLibraryPackage()
{
    static _pal_module_t hLibPackage = nullptr;
    if (hLibPackage == nullptr)
    {
        hLibPackage = _pal_LoadLibrary(U("pmod_lib"));
    }
    return hLibPackage != nullptr ? S_OK : E_NOTIMPL;
}

HRESULT ReturnString(HSTRING hstr, CHAR_t **values)
{
    UINT32 length = _pal_GetStringLen(hstr);
    UINT32 bufferSize = (length + 1) * sizeof(CHAR_t);
    *values = (CHAR_t *)_pal_MemAlloc(bufferSize);
    memcpy(*values, _pal_GetStringRawBuffer(hstr,nullptr), bufferSize);
    return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PInvokeUtils_LoadLibrary(
    LPCSTR_t moduleName)
{
    _pal_module_t hLibPackage = _pal_LoadLibrary(moduleName);
    return hLibPackage != nullptr ? S_OK : E_FAIL;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PInvokeUtils_Dispose(
    foundation::IUnknown *pUnk)
{
    return library::Dispose(pUnk);
}
