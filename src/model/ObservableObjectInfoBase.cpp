/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectInfoBase.cpp
****/

#include "pch.h"
#include "ObservableObjectInfoBase.h"

using namespace pmod;
using namespace foundation_lib;

void _CObservableObjectInfoBase::AddEventInfoInternal(pmod::IEventInfo *pEventInfo)
{
    foundation_assert(pEventInfo);
    m_events.push_back(pEventInfo);
}

HRESULT _CObservableObjectInfoBase::AddDeclaredEvents(EventInfoVector::iterator iter, EventInfoVector& events)
{
    events.insert(iter, m_events.begin(), m_events.end());
    return S_OK;
}

HRESULT _CObservableObjectInfoBase::GetEventsInternal(UINT32 eventsFlags,UINT32* size,pmod::IEventInfo ***ppEvents)
{
    IFCPTR_ASSERT(ppEvents);

    EventInfoVector events;

    if(IsOptionEnabled(eventsFlags, foundation::InstanceFlags::Inherited))
    {
        _CObservableObjectInfoBase *pBaseType = GetBaseTypeAs<_CObservableObjectInfoBase>();
        while(pBaseType)
        {
            IFR_ASSERT(pBaseType->AddDeclaredEvents(events.begin(),events));
            pBaseType = pBaseType->GetBaseTypeAs<_CObservableObjectInfoBase>();
        }
    }
    if (IsOptionEnabled(eventsFlags, foundation::InstanceFlags::Declared))
    {
        IFR_ASSERT(AddDeclaredEvents(events.end(),events));
    }
    ContainerRefCountToArray(events, size, ppEvents);
    return S_OK;
}
 
HRESULT _CObservableObjectInfoBase::GetEventByIdInternal(UINT32 eventId,pmod::IEventInfo **ppEventInfo)
{
    foundation_assert(ppEventInfo);

    // attempt to lookup
    for(EventInfoVector::const_iterator iter = m_events.begin();
        iter != m_events.end();
        ++iter)
    {
        UINT32 _eventId;
        (*iter)->GetId(&_eventId);
        if(_eventId == eventId)
        {
            // Note: no add ref count
            *ppEventInfo = *iter;
            return S_OK;
        }
    }
    return S_FALSE;
}

