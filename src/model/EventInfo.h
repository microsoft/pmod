/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventInfo.h
****/

#pragma once

#include <pmod/library/model_library.h>
#include "PropertyTypeInfo.h"

#include <foundation/hstring_wrapper.h>

//------------------------------------------------------------------------------
// Class:   CEventInfo
//
// Purpose: A IEventInfo implementation based on const struct EventInfo
//
//------------------------------------------------------------------------------
class CEventInfo :
    public foundation::library::_DefaultAdapterHost<pmod::IEventInfo, foundation::ctl::ComInspectableBase>,
    public pmod::IEventInfo
{
public:
    static HRESULT CreateInstance(
        _In_ const pmod::EventInfo *pEventInfo,
        _COM_Outptr_ pmod::IEventInfo **ppEventInfo)
    {
        return CreateInstance(pEventInfo, nullptr, ppEventInfo);
    }

    static HRESULT CreateInstance(
        _In_ const pmod::EventInfo *pEventInfo,
        _In_opt_ const GUID             *pEventArgsType,
        _Outptr_ pmod::IEventInfo **ppEventInfo);

protected:
    // Override ComInspectableBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::IEventInfo::GetIID())
        {
            *ppInterface = static_cast<pmod::IEventInfo *>(this);
        }
        else
        {
            return foundation::library::_DefaultAdapterHost<pmod::IEventInfo, foundation::ctl::ComInspectableBase>::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    STDMETHOD (GetId)(UINT32 *eventId) override
    {
        IFCPTR_ASSERT(eventId);
        *eventId = _eventId;
        return S_OK;
    }

    STDMETHOD (GetEventName)(HSTRING* eventName) override
    {
        IFCPTR_ASSERT(eventName);
        return _pal_DuplicateString(_eventName,eventName);
    }

    STDMETHOD (GetEventArgsTypeInfo)(foundation::IPropertyTypeInfo **ppVarTypeInfo) override
    {
        IFCPTR_ASSERT(ppVarTypeInfo);
        return m_spEventArgsTypeInfo.CopyTo(ppVarTypeInfo);
    }

private:
    HRESULT Initialize(
        const pmod::EventInfo *pEventInfo)
    {
        return Initialize(pEventInfo, nullptr);
    }
    HRESULT Initialize(
        _In_ const pmod::EventInfo *pEventInfo,
        _In_opt_ const GUID             *pEventArgsType);

private:
    UINT32              _eventId;
    foundation::HStringPtr  _eventName;
    foundation::ComPtr<foundation::IPropertyTypeInfo> m_spEventArgsTypeInfo;

};

