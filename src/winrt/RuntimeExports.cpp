/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:RuntimeExports.cpp
****/
#include <activation.h>
#include <roapi.h>
#include "winString.h"
#include <foundation/hstring_wrapper.h>
#include "windows.foundation.h"


#if DEBUG
#include <foundation/library/debug_library.h>

const UINT32 _OptionForcePropertyValueStaticsInternal = 0x1000000;

EXTERN_C HRESULT CreatePropertyValueStaticsInternal(ABI::Windows::Foundation::IPropertyValueStatics **ppPropertyValueStatics);
#endif

EXTERN_C FOUNDATION_API HRESULT  STDAPICALLTYPE GetPropertyValueStatics(
    _COM_Outptr_ ABI::Windows::Foundation::IPropertyValueStatics **ppPropertyValueStatics)
{
#if DEBUG
    if (foundation::debug_util::IsOptionsFlag(_OptionForcePropertyValueStaticsInternal))
    {
        return CreatePropertyValueStaticsInternal(ppPropertyValueStatics);
    }
#endif
    return Windows::Foundation::GetActivationFactory(
        foundation::HStringRef(RuntimeClass_Windows_Foundation_PropertyValue),
        ppPropertyValueStatics);

}

