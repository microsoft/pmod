/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoClassStatics.cpp
****/

#include "pch.h"

#include "MoPropertyModelInstanceStatics.h"
#include <pmod/library/library_util.h>
#include "MoDispatcherAdapter.h"
#include <foundation/library/logger_util.h>

STDMETHODIMP MoPropertyModelInstanceStatics::get_ObjectFactory(IObjectFactory **value)
{
    return foundation::QueryInterface(
        foundation::library::GetObjectFactory(),
        value);
}
STDMETHODIMP MoPropertyModelInstanceStatics::get_Logger(ILogger **value)
{
    return foundation::QueryInterface(
        foundation::logger_util::GetFoundationLogger(),
        value);
}

STDMETHODIMP MoPropertyModelInstanceStatics::get_EventDispatcher(IDispatcher **value)
{
    foundation::ComPtr<foundation::library::IDispatcher> spCoreDispatcher;
    IFR_ASSERT(pmod::library::get_propertymodel_instance()->GetEventDispatcher(spCoreDispatcher.GetAddressOf()));
    if(spCoreDispatcher != nullptr)
    {
        IFR_ASSERT(MoDispatcherAdapter::CreateInstance(spCoreDispatcher,value));
    }
    return S_OK;
}

STDMETHODIMP MoPropertyModelInstanceStatics::put_EventDispatcher(IDispatcher *value)
{
    foundation::ComPtr<foundation::library::IDispatcher> spCoreDispatcher;
    if(value != nullptr)
    {
        IFR_ASSERT(CDispatcherAdapter::CreateInstance(value,spCoreDispatcher.GetAddressOf()));
    }
    IFR_ASSERT(pmod::library::get_propertymodel_instance()->SetEventDispatcher(spCoreDispatcher));
    return S_OK;
}

STDMETHODIMP MoPropertyModelInstanceStatics::get_WorkingDispatcher(IDispatcher **value)
{
    foundation::ComPtr<foundation::library::IDispatcher> spCoreDispatcher;
    IFR_ASSERT(pmod::library::get_propertymodel_instance()->GetWorkingDispatcher(spCoreDispatcher.GetAddressOf()));
    if(spCoreDispatcher != nullptr)
    {
        IFR_ASSERT(MoDispatcherAdapter::CreateInstance(spCoreDispatcher,value));
    }
    return S_OK;
}

STDMETHODIMP MoPropertyModelInstanceStatics::put_WorkingDispatcher(IDispatcher *value)
{
    foundation::ComPtr<foundation::library::IDispatcher> spCoreDispatcher;
    if(value != nullptr)
    {
        IFR_ASSERT(CDispatcherAdapter::CreateInstance(value,spCoreDispatcher.GetAddressOf()));
    }
    IFR_ASSERT(pmod::library::get_propertymodel_instance()->SetWorkingDispatcher(spCoreDispatcher));
    return S_OK;
}
