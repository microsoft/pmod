/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoLogParameterFactory.cpp
****/

#include "pch.h"

#include "MoLogParameterFactory.h"

#include <pmod/library/library_util.h>
#include <pmod/errors.h>
#include <foundation/library/logger_util.h>
#include "MoEnumValueInterop.h"

STDMETHODIMP MoLogParameterFactory::CreateInstance( 
    IInspectable *pProperty,
    IInspectable *pValue,
    ILogParameter **instance)
{
    foundation::ComPtr<foundation::IEnumValue> spPropertyEnumValue;
    IFR_ASSERT(ObjectToEnumValue(pProperty, spPropertyEnumValue.GetAddressOf()));

    foundation::ComPtr<foundation::ILogParameterPair> spLogParameterPair;
    IFR_ASSERT(foundation::logger_util::GetLoggerFactory()->CreateLogParameter(
        spPropertyEnumValue,
        pValue,
        spLogParameterPair.GetAddressOf()));

    IFR_ASSERT(foundation::QueryInterface(spLogParameterPair,instance));
    return S_OK;
}
