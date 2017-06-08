/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationLibraryInstance.cpp
****/
#include "pch.h"

#include "FoundationLibraryInstance.h"

#include "AsyncOperationClassInfoCache.h"
#include "EnumTypeClassInfoCache.h"
#include "ObjectDispatchClassInfoCache.h"
#include "EnumerableClassInfoCache.h"

#include <foundation/library/dispatcher_callback_adapter.h>

extern HRESULT STDMETHODCALLTYPE register_foundation_errors_type_info(void);

//------------------------------------------------------------------------------
// Class:   _FoundationLibraryInstanceInitializer
//
// Purpose: Static Initializer for the Foundation Library
//
//------------------------------------------------------------------------------
struct _FoundationLibraryInstanceInitializer
{
    _FoundationLibraryInstanceInitializer();
    static _FoundationLibraryInstanceInitializer& GetInstance();

    void _Stub()
    {}
#ifdef _WINDOWS
    bool                                        _IsWindowsRuntimeAvailable;
#endif

    CTypeInfoCacheProvider<CAsyncOperationClassInfoCache, foundation::TypeInfo_AsyncOperation> _AsyncOperationClassInfoCacheProvider;
    CTypeInfoCacheProvider<CEnumTypeClassInfoCache, foundation::TypeInfo_EnumType> _EnumTypeClassInfoCacheProvider;
    CTypeInfoCacheProvider<CObjectDispatchClassInfoCache, foundation::TypeInfo_PropertyData> _PropertyDataInfoClassInfoCacheProvider;
    CTypeInfoCacheProvider<CEnumerableClassInfoCache, foundation::TypeInfo_Enumerable> _EnumerableClassInfoCacheProvider;
};

_FoundationLibraryInstanceInitializer::_FoundationLibraryInstanceInitializer()
{
#ifdef _WINDOWS
    #if defined(__windows2Efoundation_h__)
    _IsWindowsRuntimeAvailable = true;
    #else
    _IsWindowsRuntimeAvailable = LoadLibraryW(L"api-ms-win-core-winrt-string-l1-1-0.dll") != nullptr;
    #endif
#endif

    // register the 'core' logger type info
    foundation::ComPtr<foundation::library::IDelegateDispatchable> spCallback;
    foundation::DispatcherCallbackAdapter::CreateInstance(&register_foundation_errors_type_info, spCallback.GetAddressOf());

    CEnumTypeClassInfoCache::register_enum_factory_type_info(spCallback);

}

_FoundationLibraryInstanceInitializer& _FoundationLibraryInstanceInitializer::GetInstance()
{
    static _FoundationLibraryInstanceInitializer _foundationInstanceInitializer;
    return _foundationInstanceInitializer;
}

void CFoundationLibraryInstance::_Initialize()
{
    _FoundationLibraryInstanceInitializer::GetInstance()._Stub();
}

#ifdef _WINDOWS
bool IsWindowsRuntimeAvailable()
{
	return _FoundationLibraryInstanceInitializer::GetInstance()._IsWindowsRuntimeAvailable;
}
#endif
