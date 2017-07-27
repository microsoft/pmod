/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationBaseTracker.h
****/
#pragma once


#define TRACK_FOUNDATION_ENABLED

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

#include <foundation/pal.h>
#include <pal/pal_thread.h>

#include "CriticalSection.h"
#include <set>

using namespace foundation_lib;

// the Foundation Tracker Instance interface
struct IFoundationTrackerInstance
{
    virtual void DumpObject() = 0;
};

// the Foundation Tracker interface
struct IFoundationTracker
{
    virtual void AddInstance(IFoundationTrackerInstance *modelTracker) = 0;
    virtual void RemoveInstance(IFoundationTrackerInstance *modelTracker) = 0;
};

//------------------------------------------------------------------------------
// Class:   FoundationTracker
//
// Purpose: a generic tracking class based on the IFoundationTrackerInstance interface
//
//------------------------------------------------------------------------------
class FoundationTracker:
    public IFoundationTracker
{
public:
    FoundationTracker(LPCSTR_t id) :
        _track_id(id)
    {
        m_critsec.Init();
    }
    ~FoundationTracker()
    {
    }

    void AddInstance(IFoundationTrackerInstance *modelTracker) override
    {
        CFoundationLock<CCriticalSection> critSecLock(m_critsec);
        m_ModelTrackerSet.insert(modelTracker);
        TraceCountObjects(true);
    }

    void RemoveInstance(IFoundationTrackerInstance *modelTracker)  override
    {
        CFoundationLock<CCriticalSection> critSecLock(m_critsec);
        m_ModelTrackerSet.erase(modelTracker);
        TraceCountObjects(false);
    }

    void DumpTrackedInstance()
    {
        CFoundationLock<CCriticalSection> critSecLock(m_critsec);

        for (FoundationTracker_Set_Type::iterator iter = m_ModelTrackerSet.begin();
            iter != m_ModelTrackerSet.end();
            ++iter)
        {
            (*iter)->DumpObject();
            _pal_Trace(U("\n"));
        }
    }

    void ClearTrackedInstance()
    {
        CFoundationLock<CCriticalSection> critSecLock(m_critsec);
        m_ModelTrackerSet.clear();
    }

    size_t GetCountTrackedInstance()
    {
        CFoundationLock<CCriticalSection> critSecLock(m_critsec);
        return m_ModelTrackerSet.size();
    }
protected:
    virtual bool IsTraceCountEnabled() = 0;
private:
    void TraceCountObjects(bool add)
    {
        if (IsTraceCountEnabled())
        {
            _traceMessage(U("%s ProcessId=%ld %s Count=%d\n"),
                _track_id, 
                _pal_GetCurrentProcessId(),
                add ? U("(+)") : U("(-)"),
                m_ModelTrackerSet.size());
        }
    }
private:
    LPCSTR_t    _track_id;
    typedef std::set<IFoundationTrackerInstance *> FoundationTracker_Set_Type;
    FoundationTracker_Set_Type m_ModelTrackerSet;
    CAutoDeleteCriticalSection m_critsec;
};

//------------------------------------------------------------------------------
// Class:   _FoundationTrackerInstance
//
// Purpose: Static template for a FoundationTracker class
//
//------------------------------------------------------------------------------
template <typename T>
class _FoundationTrackerInstance
{
public:
    static void DumpTrackedObjects()
    {
        InternalGetInstance().DumpTrackedInstance();
    }
    static void ClearTrackedObjects()
    {
        InternalGetInstance().ClearTrackedInstance();
    }
    static size_t CountTrackedObjects()
    {
        return InternalGetInstance().GetCountTrackedInstance();
    }
    static void SetIsTrackEnabled(bool value);
    static bool IsTrackEnabled();
private:
    static bool _isTrackEnabled;
    inline static FoundationTracker& InternalGetInstance()
    {
        return T::_GetInstance();
    }
};

template<typename T> void _FoundationTrackerInstance<T>::SetIsTrackEnabled(bool value)
{
    _isTrackEnabled = value;
}

template<typename T> bool _FoundationTrackerInstance<T>::IsTrackEnabled()
{
    return _isTrackEnabled;
}

#endif

