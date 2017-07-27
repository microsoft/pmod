/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationTracker.cpp
****/

#include "pch.h"
#include "FoundationTracker.h"

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

bool _FoundationTrackerInstance<CFoundationTracker>::_isTrackEnabled = true;
template<> void _FoundationTrackerInstance<CFoundationTracker>::SetIsTrackEnabled(bool value)
{
    _isTrackEnabled = value;
}

template<> bool _FoundationTrackerInstance<CFoundationTracker>::IsTrackEnabled()
{
    return _isTrackEnabled;
}


EXTERN_C FOUNDATION_API IFoundationTracker *STDAPICALLTYPE GetFoundationTracker()
{
    return &CFoundationTracker::_GetInstance();
}

EXTERN_C FOUNDATION_API bool STDAPICALLTYPE IsFoundationTrackerEnabled()
{
    return CFoundationTracker::IsTrackEnabled();
}

#endif
