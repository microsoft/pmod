/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DebugUtil.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/interfaces/debug_util.h>
#include <foundation/library/util_stl.h>
#include "CriticalSection.h"

#include <map>
EXTERN_C FOUNDATION_API UINT32 GetDebugUtilOptionsFlagsInternal();

class CDebugUtil :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IDebugUtil
{
 public:
     static foundation::library::IDebugUtil *GetInstance();
     static inline UINT32 GetDebugUtilOptionsFlagsInternal() { return _options; }

     //Interface IDebugUtil
     STDMETHOD(SetOptionsFlags)(UINT32 flags)  override;
     STDMETHOD(GetOptionsFlags)(UINT32 *flags)  override;

     STDMETHOD(SetIsTrackEnabled)(bool value)  override;
     STDMETHOD(DumpTrackedObjects)()  override;
     STDMETHOD(ClearTrackedObjects)()  override;
     STDMETHOD(CountTrackedObjects)(UINT32 *count)  override;

     STDMETHOD(SetTrackRefCountTypeFlags)(const IID& iidType, ULONG uniqueId, foundation::library::TrackingInfoType flags)  override;
     STDMETHOD(GetTrackRefCountTypeFlags)(const IID& iidType, ULONG uniqueId, foundation::library::TrackingInfoType *flags)  override;

     STDMETHOD(AddPerfCounter)(UINT32 perfId, UINT64 time) override;
     STDMETHOD(GetPerfCounter)(UINT32 perfId, UINT32 *pCounter, UINT64* pTime) override;
     STDMETHOD(ClearPerfCounter)(UINT32 perfId) override;
     STDMETHOD(ClearAllPerfCounters)() override;
     STDMETHOD(GetPerfCountersInfo)(foundation::IDictionary **ppPerfInfo) override;
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        HRESULT hr = S_OK;
        if (iid == foundation::library::IDebugUtil::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IDebugUtil *>(this);
        }
        else
        {
            hr = foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return hr;
    }
private:
    typedef std::map<ULONG, foundation::library::TrackingInfoType> UniqueIdInfoTypeMap;
    typedef std::map<GUID, UniqueIdInfoTypeMap, foundation::library::GuidLessThan> TrackRefCountTypeMap;

    typedef std::map<UINT32, std::pair<UINT32,UINT64>> PerfCounterTypeMap;

    TrackRefCountTypeMap _trackRefCountTypeMap;
    PerfCounterTypeMap _perfCounters;
    foundation_lib::CAutoCriticalSection _criticalSection;

    static UINT32 _options;
};


