/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_load_library.cpp
****/

#include <pal/pal_load_library.h>
#include <foundation/com_interfaces.h>

#include <foundation/library/string_util.h>

static void _Get_module_name_path(
    _In_ LPCSTR_t moduleNamePath,
    _In_ LPCSTR_t prefix,
    _In_ LPCSTR_t extension,
    foundation::string_t& libraryPath)
{
    foundation::string_t moduleName;

    libraryPath = moduleNamePath;
    size_t pos = libraryPath.find_last_of(U('/'));
    if (pos != foundation::string_t::npos)
    {
        ++pos;
        moduleName = libraryPath.substr(pos);
        libraryPath.erase(pos);
    }
    else
    {
        moduleName = moduleNamePath;
        libraryPath.clear();
    }
    libraryPath += prefix;
    libraryPath += moduleName;
    libraryPath += extension;
}


#if defined(_WINDOWS)

#include <Windows.h>

EXTERN_C PAL_API _pal_module_t _pal_LoadLibrary(_In_ LPCSTR_t moduleName)
{
    foundation::string_t moduleNamePath;
    _Get_module_name_path(moduleName, L"", L".dll", moduleNamePath);
#ifdef _WINDOWS_RUNTIME
    return LoadPackagedLibrary(moduleNamePath.c_str(), 0);
#else
    return LoadLibrary(moduleNamePath.c_str());
#endif
}

EXTERN_C PAL_API HRESULT _pal_FreeLibrary(_In_ _pal_module_t hModule)
{
    return FreeLibrary(hModule) ? S_OK : E_FAIL;
}

#else

#include <dlfcn.h>

typedef void * _pal_module_t;

EXTERN_C PAL_API _pal_module_t _pal_LoadLibrary(_In_ LPCSTR_t moduleName)
{
    foundation::string_t moduleNamePath;
    _Get_module_name_path(moduleName, "lib",
#if defined(__APPLE__) && defined(TARGET_OS_MAC)
        ".dylib"
#else
        ".so"
#endif
        , moduleNamePath);
    return dlopen(moduleNamePath.c_str(), RTLD_NOW);
}

EXTERN_C PAL_API HRESULT _pal_FreeLibrary(_In_ _pal_module_t hModule)
{
    return dlclose(hModule) == 0 ? S_OK : E_FAIL;
}

#endif
