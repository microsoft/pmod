/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationTracker.cpp
****/

#include "pch.h"
#include "FoundationTracker.h"

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

template<> bool _FoundationTrackerInstance<CFoundationTracker>::_isTrackEnabled = true;


EXTERN_C FOUNDATION_API IFoundationTracker *STDAPICALLTYPE GetFoundationTracker()
{
    return &CFoundationTracker::_GetInstance();
}

EXTERN_C FOUNDATION_API bool STDAPICALLTYPE IsFoundationTrackerEnabled()
{
    return CFoundationTracker::IsTrackEnabled();
}

#endif
