/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeAdapterFactory.cpp
****/
#include "pch.h"

#include "TypeAdapterFactory.h"
#include "TypeInfoStatics.h"

CTypeAdapterFactory::CTypeAdapterFactory()
{
    m_critsec.Init();
}

void CTypeAdapterFactory::AddTypeEntriesInternal(
        const foundation::library::_TypeAdapterEntry entries[])
{
    const foundation::library::_TypeAdapterEntry *pEntry = entries;
    while(pEntry->_creatorCallback)
    {
        AddTypeEntryInternal(pEntry->_iidInterface,pEntry->_iidType,pEntry->_creatorCallback);
        ++pEntry;
    }
}

void CTypeAdapterFactory::AddTypeEntryInternal(
    _In_ IID iidInterface,
    _In_ IID iidType,
    _In_ foundation::library::CreateAdapterInstanceCallback creator)
{
    _InterfaceEntryType interfaceEntryType(iidType,creator);

    if(m_iidTypesMap.find(iidInterface) == m_iidTypesMap.end())
    {
        m_iidTypesMap.insert(std::make_pair(iidInterface,interfaceEntryType));
    }
    else
    {
        // we have a duplicate so use the vector
        m_iidTypesVector.push_back(std::make_pair(iidInterface,interfaceEntryType));
    }
}

HRESULT CTypeAdapterFactory::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    if (iid == foundation::library::IInterfaceAdapterFactory::GetIID())
    {
        *ppInterface = static_cast<foundation::library::IInterfaceAdapterFactory *>(this);
        return S_OK;
    }
    else if (iid == foundation::library::ITypeAdapterFactory::GetIID())
    {
        *ppInterface = static_cast<foundation::library::ITypeAdapterFactory *>(this);
        return S_OK;
    }
    return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid,ppInterface);
}
// Interface IModelTypeAdapterFactory
STDMETHODIMP CTypeAdapterFactory::AddTypeEntries(const foundation::library::_TypeAdapterEntry iidTypes[])
{
    foundation_lib::_CriticalSectionLock csl(m_critsec);

    AddTypeEntriesInternal(iidTypes);
    return S_OK;
}

STDMETHODIMP CTypeAdapterFactory::AddTypeEntry(_In_ IID iidInterface,_In_ IID iidType,_In_ foundation::library::CreateAdapterInstanceCallback creator)
{
    foundation_lib::_CriticalSectionLock csl(m_critsec);

    AddTypeEntryInternal(iidInterface, iidType, creator);
    return S_OK;
}

STDMETHODIMP CTypeAdapterFactory::TryCreateInterfaceAdapter(
    _In_ REFIID riid_outer,
    _In_ REFIID riid,
    _In_ foundation::IInspectable *pOuter,
    _In_ foundation::IInspectable *pInner,
    _Outptr_ foundation::IInspectable **ppInner)
{
    _InterfaceEntryType interfaceEntryType;
    foundation::IObject *pObject = nullptr;

    if (Lookup(riid, interfaceEntryType) && 
        SUCCEEDED(foundation::QueryWeakReference(pOuter, &pObject)))
    {
        bool result;
        IFR_ASSERT(CTypeInfoStatics::IsTypeOfInternal(pObject, interfaceEntryType.first, &result));
        if(result)
        {
            IFR_ASSERT(interfaceEntryType.second(pOuter, pInner, ppInner));
            return S_OK;
        }
    }
    return E_NOINTERFACE;
}
bool CTypeAdapterFactory::Lookup(_In_ REFIID riid,_InterfaceEntryType& interfaceEntryType)
{
    foundation_lib::_CriticalSectionLock csl(m_critsec);

    _GuidCreatorMap::iterator iter = m_iidTypesMap.find(riid);
    if(iter != m_iidTypesMap.end())
    {
        interfaceEntryType = (*iter).second;
        return true;
    }
    // try the Vector in case our map does not have it
    for(_GuidCreatorVector::const_iterator iter = m_iidTypesVector.begin();
        iter != m_iidTypesVector.begin();
        ++iter)
    {
        if(IsEqualGUID(riid,(*iter).first))
        {
            interfaceEntryType = (*iter).second;
            return true;
        }
    }
    return false;
}

