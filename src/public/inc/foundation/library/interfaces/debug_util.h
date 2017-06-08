/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:debug_util.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/interfaces/dictionary.h>

namespace foundation
{
    namespace library
    {
        const UINT32 _OptionTrackTypeInfoLeaks = 0x01;
        const UINT32 _OptionTrackCountObjects = 0x02;
        const UINT32 _OptionIsAssertDisabled = 0x04;

        enum class TrackingInfoType
        {
            None = 0x00,
            AddRef = 0x01,
            Release = 0x02,
            FinalRelease = 0x04,
            All = 0xff,
        };

        // IDebugUtil Interface
        BEGIN_DECLARE_INTERFACE(IDebugUtil, foundation::IUnknown, FOUNDATION_API)
        
        STDMETHOD(SetOptionsFlags)(UINT32 flags) = 0;
        STDMETHOD(GetOptionsFlags)(UINT32 *flags) = 0;

        STDMETHOD(SetIsTrackEnabled)(bool value) = 0;
        STDMETHOD(DumpTrackedObjects)() = 0;
        STDMETHOD(ClearTrackedObjects)() = 0;
        STDMETHOD(CountTrackedObjects)(UINT32 *count) = 0;

        STDMETHOD(SetTrackRefCountTypeFlags)(const IID& iidType, ULONG uniqueId, TrackingInfoType flags) = 0;
        STDMETHOD(GetTrackRefCountTypeFlags)(const IID& iidType, ULONG uniqueId, TrackingInfoType *flags) = 0;

        // perf counter API's
        STDMETHOD(AddPerfCounter)(UINT32 perfId,UINT64 time) = 0;
        STDMETHOD(GetPerfCounter)(UINT32 perfId,UINT32 *pCounter,UINT64* pTime) = 0;
        STDMETHOD(ClearPerfCounter)(UINT32 perfId) = 0;
        STDMETHOD(ClearAllPerfCounters)() = 0;
        STDMETHOD(GetPerfCountersInfo)(foundation::IDictionary **ppPerfInfo) = 0;

        END_DECLARE_INTERFACE()

    }
}

