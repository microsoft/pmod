/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationTracker.h
****/
#pragma once

#include "FoundationBaseTracker.h"
#include "DebugUtil.h"

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
//------------------------------------------------------------------------------
// Class:   CFoundationTracker
//
// Purpose: Implementation of Base Tracker Manager
//
//------------------------------------------------------------------------------
class CFoundationTracker:
    protected FoundationTracker,
    public _FoundationTrackerInstance<CFoundationTracker>
{
public:
    static FoundationTracker& _GetInstance()
    {
        static CFoundationTracker _FoundationTracker;
        return _FoundationTracker;
    }
private:
    CFoundationTracker() :
        FoundationTracker(U("CFoundationTracker"))
    {
    }
    ~CFoundationTracker()
    {
        SetIsTrackEnabled(false);
    }
protected:
    bool is_trace_count_enabled() override
    {
        return (GetDebugUtilOptionsFlagsInternal() & foundation::library::_OptionTrackCountObjects) != 0;
    }
};

EXTERN_C FOUNDATION_API IFoundationTracker *STDAPICALLTYPE GetFoundationTracker();
EXTERN_C FOUNDATION_API bool STDAPICALLTYPE IsFoundationTrackerEnabled();

template <bool isTrackingEnabled>
class _FoundationTrackerBase:
    public IFoundationTrackerInstance
{
protected:
    _FoundationTrackerBase()
    {
        if (isTrackingEnabled && IsFoundationTrackerEnabled())
        {
            GetFoundationTracker()->AddInstance(this);
        }
    }
    ~_FoundationTrackerBase()
    {
        if (isTrackingEnabled && IsFoundationTrackerEnabled())
        {
            GetFoundationTracker()->RemoveInstance(this);
        }
    }

protected:
    void DumpObject() override
    {
    }
};

#endif

