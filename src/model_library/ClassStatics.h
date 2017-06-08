/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ClassStatics.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <pmod/library/interfaces/class_statics.h>
#include <foundation/library/base_adapter_host.h>

class CClassStatics :
    public foundation::library::_DefaultAdapterHost
    <
        pmod::library::IClassStatics,
        foundation::ctl::ImplementsInspectable<
            pmod::library::IClassStatics,
            &pmod::library::IID_IClassStatics
        >
    >
{
 public:
     static CClassStatics *GetInstanceInternal()
     {
         return static_cast<CClassStatics *>(GetInstance());
     }

     static pmod::library::IClassStatics *GetInstance();
     static HRESULT GetOrCreateInstance(pmod::library::IClassStatics **ppPropertyModelInstance);

     //Interface IClassStatics

    // Event Dispatcher property
    STDMETHOD(GetEventDispatcher)(_In_ foundation::library::IDispatcher **ppEventDispatcher) override;
    STDMETHOD(SetEventDispatcher)(_Outptr_ foundation::library::IDispatcher *pEventDispatcher)  override;

    // Working Dispatcher property
    STDMETHOD(GetWorkingDispatcher)(_In_ foundation::library::IDispatcher **ppWorkingDispatcher)  override;
    STDMETHOD(SetWorkingDispatcher)(_Outptr_ foundation::library::IDispatcher *pWorkingDispatcher)  override;

    // For Internal access
    foundation::library::IDispatcher *GetEventDispatcher() { return _eventDispatcher; }
    foundation::library::IDispatcher *GetWorkingDispatcher() { return _workingDispatcher; }
private:
    foundation::ComPtr<foundation::library::IDispatcher> _eventDispatcher;
    foundation::ComPtr<foundation::library::IDispatcher> _workingDispatcher;
};

