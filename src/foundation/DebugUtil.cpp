/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DebugUtil.cpp
****/
#include "pch.h"

#if defined(DEBUG)


#include "DebugUtil.h"
#include "FoundationTracker.h"
#include <foundation/pv_util.h>
#include <foundation/type_info_statics_util.h>
#include <foundation/library/dictionary_class.h>

#include <foundation/library/com_ptr_singleton.h>
#include "ActivationFactory.h"

using namespace foundation;
const ULONG _allUniqueId = (ULONG)-1;

// {7C151F88-FA07-4C98-93E6-7E53339D3F3B}
const GUID foundation::library::IID_IDebugUtil =
{ 0x7c151f88, 0xfa07, 0x4c98, { 0x93, 0xe6, 0x7e, 0x53, 0x33, 0x9d, 0x3f, 0x3b } };

UINT32 CDebugUtil::_options = library::_OptionTrackTypeInfoLeaks;

EXTERN_C FOUNDATION_API UINT32 GetDebugUtilOptionsFlagsInternal()
{
    return CDebugUtil::GetDebugUtilOptionsFlagsInternal();
}

library::IDebugUtil *CDebugUtil::GetInstance()
{
    return library::_GetFactoryInstance<CDebugUtil, library::IDebugUtil>();
}

STDMETHODIMP CDebugUtil::SetOptionsFlags(UINT32 flags)
{
    _options = flags;
    return S_OK;
}

STDMETHODIMP CDebugUtil::GetOptionsFlags(UINT32 *flags)
{
    IFCPTR_ASSERT(flags);
    *flags = _options;
    return S_OK;
}

STDMETHODIMP CDebugUtil::SetIsTrackEnabled(bool value)
{
    CFoundationTracker::SetIsTrackEnabled(value);
    return S_OK;
}

STDMETHODIMP CDebugUtil::DumpTrackedObjects()
{
    CFoundationTracker::DumpTrackedObjects();
    return S_OK;
}

STDMETHODIMP CDebugUtil::ClearTrackedObjects()
{
    CFoundationTracker::ClearTrackedObjects();
    return S_OK;
}

STDMETHODIMP CDebugUtil::CountTrackedObjects(UINT32 *count)
{
    IFCPTR_ASSERT(count);
    *count = (UINT32)CFoundationTracker::CountTrackedObjects();
    return S_OK;
}

STDMETHODIMP CDebugUtil::SetTrackRefCountTypeFlags(const IID& iidType, ULONG uniqueId, foundation::library::TrackingInfoType flags)
{
    TrackRefCountTypeMap::iterator iter = _trackRefCountTypeMap.find(iidType);
	if (flags != library::TrackingInfoType::None)
    {
        if (iter == _trackRefCountTypeMap.end())
        {
            UniqueIdInfoTypeMap firstItem;
            firstItem[uniqueId] = flags;
            _trackRefCountTypeMap[iidType] = firstItem;
        }
        else
        {
            if (uniqueId == _allUniqueId)
            {
                (*iter).second.clear();
            }
            (*iter).second[uniqueId] = flags;
        }
    }
    else
    {
        if (iter != _trackRefCountTypeMap.end())
        {
            _trackRefCountTypeMap.erase(iidType);
        }
    }
    return S_OK;
}

STDMETHODIMP CDebugUtil::GetTrackRefCountTypeFlags(
    const IID& iidType, 
    ULONG uniqueId, 
    library::TrackingInfoType *flags)
{
    IFCPTR_ASSERT(flags);

    TrackRefCountTypeMap::iterator iter = _trackRefCountTypeMap.find(iidType);
    if (iter != _trackRefCountTypeMap.end())
    {
        UniqueIdInfoTypeMap::iterator iter2 = (*iter).second.find(uniqueId);
        if (iter2 != (*iter).second.end())
        {
            *flags = (*iter2).second;
            return S_OK;
        }
        // fallback into All Unique Id's
        iter2 = (*iter).second.find(_allUniqueId);
        if (iter2 != (*iter).second.end())
        {
            *flags = (*iter2).second;
            return S_OK;
        }
    }
    *flags = library::TrackingInfoType::None;
    return S_OK;
}

STDMETHODIMP CDebugUtil::AddPerfCounter(UINT32 perfId, UINT64 time)
{
    foundation_lib::_CriticalSectionLock lock(_criticalSection);
    PerfCounterTypeMap::iterator iter = _perfCounters.find(perfId);
    if (iter != _perfCounters.end())
    {
        (*iter).second.first++;
        (*iter).second.second += time;
    }
    else
    {
        _perfCounters[perfId] = std::make_pair(1, time);
    }
    return S_OK;
}

STDMETHODIMP CDebugUtil::GetPerfCounter(UINT32 perfId, UINT32 *pCounter, UINT64* pTime)
{
    IFCPTR_ASSERT(pCounter);
    IFCPTR_ASSERT(pTime);

    foundation_lib::_CriticalSectionLock lock(_criticalSection);
    PerfCounterTypeMap::iterator iter = _perfCounters.find(perfId);
    if (iter != _perfCounters.end())
    {
        *pCounter = (*iter).second.first++;
        *pTime = (*iter).second.second;
    }
    else
    {
        *pCounter = 0;
        *pTime = 0;
    }
    return S_OK;
}

STDMETHODIMP CDebugUtil::ClearPerfCounter(UINT32 perfId)
{
    foundation_lib::_CriticalSectionLock lock(_criticalSection);

    PerfCounterTypeMap::iterator iter = _perfCounters.find(perfId);
    if (iter != _perfCounters.end())
    {
        (*iter).second.first = 0;
        (*iter).second.second = 0;
    }
    return S_OK;
}

STDMETHODIMP CDebugUtil::ClearAllPerfCounters()
{
    foundation_lib::_CriticalSectionLock lock(_criticalSection);
    _perfCounters.clear();
    return S_OK;
}

STDMETHODIMP CDebugUtil::GetPerfCountersInfo(foundation::IDictionary **ppPerfInfo)
{
    foundation_lib::_CriticalSectionLock lock(_criticalSection);

    foundation::ComPtr<IDictionary> perf_info_ptr;
    IFR_ASSERT(library::CreateDictionaryClass(perf_info_ptr.GetAddressOf()));

    for (PerfCounterTypeMap::iterator iter = _perfCounters.begin();
        iter != _perfCounters.end();
        ++iter)
    {

        foundation::ComPtr<IEnumTypeInfo> enum_type_info_ptr;
        if (FAILED(GetEnumTypeInfo((*iter).first >> 16, enum_type_info_ptr.GetAddressOf())))
        {
            continue;
        }
        foundation::ComPtr<IEnumValue> enum_value_ptr;
        IFR_ASSERT(enum_type_info_ptr->GetEnumValue((*iter).first & 0xffff, enum_value_ptr.GetAddressOf()));

        foundation::ComPtr<IDictionary> perf_slot_ptr;
        IFR_ASSERT(library::CreateDictionaryClass(perf_slot_ptr.GetAddressOf()));

        bool replaced;
        perf_slot_ptr->Insert(HStringRef(U("id")), pv_util::CreateValue((*iter).first), &replaced);
        perf_slot_ptr->Insert(HStringRef(U("count")), pv_util::CreateValue((*iter).second.first), &replaced);

        FLOAT ms_time = ((FLOAT)(*iter).second.second) / 1000;
        perf_slot_ptr->Insert(HStringRef(U("time")), pv_util::CreateValue(ms_time), &replaced);
        // add to our info 

        HStringPtr type_info_name;
        enum_type_info_ptr->GetName(type_info_name.GetAddressOf());

        HStringPtr enum_value_str;
        enum_value_ptr->ToString(enum_value_str.GetAddressOf());

        string_t name = type_info_name.GetRawBuffer();
        name += U(".");
        name += enum_value_str.GetRawBuffer();

        perf_info_ptr->Insert(HStringRef(name.c_str()), perf_slot_ptr, &replaced);
    }
    return perf_info_ptr.CopyTo(ppPerfInfo);
}

static _RegisterActivationFactory _register(
    U("foundation.DebugUtil"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CDebugUtil::GetInstance)
    );


#endif
