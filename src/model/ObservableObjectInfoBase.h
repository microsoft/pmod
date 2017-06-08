/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectInfoBase.h
****/

#pragma once

#include "ObjectDispatchInfoBase.h"
#include <pmod/errors.h>

typedef std::vector<foundation::ComPtr<pmod::IEventInfo> > EventInfoVector;

class _CObservableObjectInfoBase : public _CObjectDispatchInfoBase
{
public:
    const EventInfoVector& GetEvents() { return m_events; }

    // QI for an Interface in my host
    template <class Q>
    Q *As()
    {
        Q *pQ = nullptr;
        HRESULT hr = foundation::QueryWeakReference(this,&pQ);
        foundation_assert(SUCCEEDED(hr));
        return pQ;
    }

protected:
    _CObservableObjectInfoBase()
    {
    }

    HRESULT GetEventsInternal(UINT32 eventsFlags,UINT32* size,pmod::IEventInfo ***ppEvents);
 
    void AddEventInfoInternal(pmod::IEventInfo *pEventInfo);
    
    HRESULT GetEventByIdInternal(UINT32 eventId,pmod::IEventInfo **ppEventInfo);

private:
    HRESULT AddDeclaredEvents(EventInfoVector::iterator iter,EventInfoVector& events);

private:
    EventInfoVector                 m_events;
};

template <class T,class TBASE,class _TypeInfo,class TBaseType=T>
class TObservableObjectInfoBase :
    public TObjectDispatchInfoBase<T, TBASE, _TypeInfo, TBaseType>
{
protected:
	TObservableObjectInfoBase()
    {
    }
 
    // IObservableObjectInfo Interface
    STDMETHOD(GetEvents)(UINT32 eventsFlags,UINT32* size,pmod::IEventInfo ***ppEvents)
    {
        return this->GetEventsInternal(eventsFlags,size,ppEvents);
    }
};

//------------------------------------------------------------------------------
// Class:   TViewModelInfoBase
//
// Purpose: A template implementation for IViewModelInfo
//
//------------------------------------------------------------------------------
template <class TBASE>
class TViewModelInfoBase :
    public TBASE
{
protected:
    TViewModelInfoBase():
      m_IIDTypeSource(foundation_GUID_NULL)
    {
    }
    // Override ComInspectableBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::IObservableObjectInfo::GetIID())
        {
            *ppInterface = static_cast<pmod::IObservableObjectInfo *>(this);
        }
        else
        {
            return TBASE::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
public:
    // Interface IViewModelInfo
    STDMETHOD (GetIsInherited)( 
            bool *value) override
    {
        *value = this->m_pTypeInfo->m_inherit_properties;
        return S_OK;
    }
protected:
    IID             m_IIDTypeSource;
};
