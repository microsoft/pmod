/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventInfo.cpp
****/

#include "pch.h"

#include "EventInfo.h"
#include "TypeInfoFactory.h"

HRESULT CEventInfo::Initialize(
    _In_ const pmod::EventInfo *pEventInfo,
    _In_opt_ const GUID         *pEventArgsType)
{
    foundation_assert(pEventInfo);

    this->_eventId = pEventInfo->m_eventId;
    CreateStringHelper(pEventInfo->m_eventName, _eventName.GetAddressOf());

    if (pEventArgsType != nullptr)
    {
        // TODO: find a way to make this more reasonable to code
        // the idea is to make a copy of the 'PropertyTypeInfo'
        // and to define 'DeleteVarTypeInfo' -> true
        IFR_ASSERT(CPropertyTypeInfo::CreateInstance(
            new foundation::PropertyTypeInfo(
                pEventInfo->m_pEventArgsTypeInfo->m_property_type,nullptr),
            pEventArgsType, 
            m_spEventArgsTypeInfo.GetAddressOf()));
        static_cast<CPropertyTypeInfo *>(m_spEventArgsTypeInfo.Get())->SetDeleteVarTypeInfo(true);
    }
    else
    {
        IFR_ASSERT(CPropertyTypeInfoFactoryCache::CreatePropertyTypeInfoInternal(
            pEventInfo->m_pEventArgsTypeInfo,
            m_spEventArgsTypeInfo.GetAddressOf()));
    }
    return S_OK;
}

HRESULT CEventInfo::CreateInstance(
    _In_ const pmod::EventInfo *pEventInfo,
    _In_opt_ const GUID             *pEventArgsType,
    _Outptr_ pmod::IEventInfo **ppEventInfo)
{
    foundation_assert(pEventInfo);

    HRESULT hr;

    CEventInfo *pEventInfoClass;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CEventInfo>::CreateInstance(&pEventInfoClass));
    IFR_ASSERT(hr = (pEventInfoClass)->Initialize(pEventInfo, pEventArgsType));
    *ppEventInfo = pEventInfoClass;

    return S_OK;
}
