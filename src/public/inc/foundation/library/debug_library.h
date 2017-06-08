/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:debug_library.h
****/
#pragma once

#if defined(DEBUG)

#include "interfaces/debug_util.h"
#include <foundation/macros.h>
#include <foundation/hstring_wrapper.h>
#include <foundation/interfaces/object.h>
#include <foundation/foundation_factory.h>
#include <pal/library/trace_util.h>

namespace foundation
{
    namespace debug_util
    {
        inline library::IDebugUtil *GetDebugUtil()
        {
            return GetFoundationFactory<library::IDebugUtil>(U("foundation.DebugUtil"));
        }

        inline static HRESULT SetOptionsFlags(UINT32 flags)
        {
            return GetDebugUtil()->SetOptionsFlags(flags);
        }

        inline static HRESULT GetOptionsFlags(UINT32& flags)
        {
            return GetDebugUtil()->GetOptionsFlags(&flags);
        }

        inline static HRESULT SetOptionsFlag(UINT32 flags)
        {
            UINT32 all_flags;
            GetOptionsFlags(all_flags);
            all_flags |= flags;
            return SetOptionsFlags(all_flags);
        }

        inline static HRESULT RemoveOptionsFlag(UINT32 flags)
        {
            UINT32 all_flags;
            GetOptionsFlags(all_flags);
            all_flags &= ~flags;
            return SetOptionsFlags(all_flags);
        }

        inline static bool IsOptionsFlag(UINT32 flags)
        {
            UINT32 all_flags = 0;
            GetOptionsFlags(all_flags);
            return (all_flags & flags) != 0;
        }

        inline static HRESULT SetIsTrackEnabled(bool value)
        {
            return GetDebugUtil()->SetIsTrackEnabled(value);
        }
            
        inline static HRESULT DumpTrackedObjects()
        {
            return GetDebugUtil()->DumpTrackedObjects();
        }

        inline static HRESULT ClearTrackedObjects()
        {
            return GetDebugUtil()->ClearTrackedObjects();
        }

        inline static UINT32 CountTrackedObjects()
        {
            UINT32 count = 0;
            GetDebugUtil()->CountTrackedObjects(&count);
            return count;
        }

        inline static HRESULT AddPerfCounter(UINT32 perfId, UINT64 time)
        {
            return GetDebugUtil()->AddPerfCounter(perfId, time);
        }

        inline static HRESULT GetPerfCounter(UINT32 perfId, UINT32& counter, UINT64& time)
        {
            return GetDebugUtil()->GetPerfCounter(perfId, &counter, &time);
        }

        inline static HRESULT ClearPerfCounter(UINT32 perfId)
        {
            return GetDebugUtil()->ClearPerfCounter(perfId);
        }
        inline static HRESULT ClearAllPerfCounters()
        {
            return GetDebugUtil()->ClearAllPerfCounters();
        }
        inline static HRESULT GetPerfCountersInfo(IDictionary **ppPerfInfo)
        {
            return GetDebugUtil()->GetPerfCountersInfo(ppPerfInfo);
        }

        inline static HRESULT SetTrackRefCountFlags(
            const IID& iidType, ULONG uniqueId, library::TrackingInfoType flags)
        {
            return GetDebugUtil()->SetTrackRefCountTypeFlags(iidType,uniqueId,flags);
        }

        inline static HRESULT GetTrackRefCountFlags(
            const IID& iidType, ULONG uniqueId, library::TrackingInfoType& flags)
        {
            return GetDebugUtil()->GetTrackRefCountTypeFlags(iidType, uniqueId, &flags);
        }

        inline static bool IsTrackRefCountFlags(
            const IID& iidType, ULONG uniqueId, library::TrackingInfoType flags_options)
        {
            library::TrackingInfoType flags;
            IGNOREHR(GetTrackRefCountFlags(iidType, uniqueId, flags));

            return (static_cast<int>(flags) & static_cast<int>(flags_options)) != 0;
        }

        inline static bool IsAssertDisabled()
        {
            return IsOptionsFlag(library::_OptionIsAssertDisabled);
        }

        __unused_attribute__
        static void TrackRefCount(
            IObjectTypeInfo *pModelTypeInfo,
            UINT32 flags,
            library::TrackingInfoType tracking_flags_options,
            LPCSTR_t message,
            ULONG uniqueId,
            _pal_AtomicIntegerType cRef)
        {
            if (pModelTypeInfo)
            {
                IID iidType = GUID_NULL;
                pModelTypeInfo->GetType(&iidType);
                if (
                    (flags != 0 && IsOptionsFlag(flags) ) ||
                    IsTrackRefCountFlags(iidType, uniqueId, tracking_flags_options))
                {
                    HStringPtr typeName;
                    pModelTypeInfo->GetName(typeName.GetAddressOf());
                    pal::library::TraceMessage(U("%s name:%s unique_id:%d ref_count=%d\n"),
                        message,
                        typeName.GetRawBuffer(),
                        uniqueId,
                        cRef);
                }
            }
        }
    }
}

#endif
