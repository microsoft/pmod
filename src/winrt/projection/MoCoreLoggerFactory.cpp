/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreLoggerFactory.cpp
****/

#include "pch.h"

#include "MoCoreLoggerFactory.h"

#include <pmod/library/library_util.h>
#include <pmod/errors.h>

STDMETHODIMP MoLoggerFactory::CreateInstance(
    UINT32 rootCategoryTypeId,
    LoggerOptions options,
    ILogger **instance)
{

    foundation::library::LoggerCreateParameters param(rootCategoryTypeId);
    param.m_options = (foundation::LoggerOptions)options;

    foundation::InspectablePtr spInner;
    HRESULT hr = foundation::logger_util::GetLoggerFactory()->
        CreateLogger(&param,nullptr,spInner.GetAddressOf());
    IFHR(hr);

    IFHR(foundation::QueryInterface(spInner,instance));
    return S_OK;
}
