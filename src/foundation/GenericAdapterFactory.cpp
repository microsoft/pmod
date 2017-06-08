/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenericAdapterFactory.cpp
****/
#include "pch.h"

#include "GenericAdapterFactory.h"

using namespace foundation;

CGenericAdapterFactory::CGenericAdapterFactory()
{
    m_critsec.Init();
}

HRESULT CGenericAdapterFactory::QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
{
    if (iid == IInterfaceAdapterFactory::GetIID())
    {
        *ppInterface = static_cast<foundation::library::IInterfaceAdapterFactory *>(this);
        return S_OK;
    }
    else if (iid == library::IGenericAdapterFactory::GetIID())
    {
        *ppInterface = static_cast<library::IGenericAdapterFactory *>(this);
        return S_OK;
    }
    return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid,ppInterface);
}

// Interface IModelTypeAdapterFactory
STDMETHODIMP CGenericAdapterFactory::AddAdapterEntry(
        _In_ IID riid_outer,
        _In_ IID iidTargetType,
        _In_ IID iidSourceType,
        _In_ library::CreateAdapterInstanceCallback creatorCallback)
{
    foundation_lib::_CriticalSectionLock csl(m_critsec);

    _OuterEntryMap::iterator iter = m_OuterEntries.find(riid_outer);
    if(iter == m_OuterEntries.end())
    {
        _IIdTargetTypeEntry targetTypeEntry;
        targetTypeEntry[iidTargetType] = _CallbackEntry(iidSourceType,creatorCallback);
        m_OuterEntries[riid_outer] = targetTypeEntry;
    }
    else
    {
        (*iter).second[iidTargetType] = _CallbackEntry(iidSourceType,creatorCallback);
    }
    return S_OK;
}

STDMETHODIMP CGenericAdapterFactory::TryCreateInterfaceAdapter(
            _In_ REFIID riid_outer,
            _In_ REFIID riid,
            _In_ foundation::IInspectable *pOuter,
            _In_ foundation::IInspectable *pInner,
            _Outptr_ foundation::IInspectable **ppInner)
{
    _CallbackEntry callbackEntryType;
    if (Lookup(riid_outer, riid, callbackEntryType))
    {
        IID iidSourceType = callbackEntryType.first;
        foundation::IUnknown *pCastTo = nullptr;
        if(iidSourceType == riid_outer ||
            // QI the Inner first
            SUCCEEDED(foundation::QueryWeakReference(pInner,iidSourceType,&pCastTo)) ||
            // then QI the Outer
            SUCCEEDED(foundation::QueryWeakReference(pOuter, iidSourceType, &pCastTo))
            )
        {
            IFR_ASSERT(callbackEntryType.second(pOuter, pInner, ppInner));
            return S_OK;
        }
    }
    return E_NOINTERFACE;
}

bool CGenericAdapterFactory::Lookup(
    REFIID riid_outer,
    REFIID riid, 
    _CallbackEntry& callbackEntryType)
{
    foundation_lib::_CriticalSectionLock csl(m_critsec);

    _OuterEntryMap::const_iterator iter;
    if ((iter = m_OuterEntries.find(riid_outer)) != m_OuterEntries.end() ||
        (iter = m_OuterEntries.find(GUID_NULL)) != m_OuterEntries.end())
    {
        _IIdTargetTypeEntry::const_iterator iter2 = (*iter).second.find(riid);
        if (iter2 != (*iter).second.end())
        {
            callbackEntryType = (*iter2).second;
            return true;
        }
    }
    return false;

}


