/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseModelSource.cpp
****/
#include "pch.h"
#include "BaseModelSource.h"
#include <foundation/library/logger_macros.h>

// {D0C18A80-6953-471E-9629-8C1476EB9EDE}
EXTERN_C const GUID IID__IBaseModelSource =
{ 0xd0c18a80, 0x6953, 0x471e,{ 0x96, 0x29, 0x8c, 0x14, 0x76, 0xeb, 0x9e, 0xde } };

_BaseModelSource::_BaseModelSource():
            m_isAttached(false),
            m_forceAttached(false),
            m_isAttachedNeed(false),
            m_isAttachEnabled(true)
{
}

HRESULT _BaseModelSource::AttachToSource()
{
    return S_OK;
}

HRESULT _BaseModelSource::DetachFromSource()
{
    return S_OK;
}

HRESULT _BaseModelSource::DetachFromSourceInternal()
{
    foundation_assert(IsAttached());
    SetIsAttached(false);
    return DetachFromSource();
}

void _BaseModelSource::SetIsAttached(bool isAttached)
{
    m_isAttached = isAttached;
}

HRESULT _BaseModelSource::SetForceAttachedInternal(bool newValue)
{
    if(newValue != m_forceAttached)
    {
        m_forceAttached = newValue;
        if(newValue)
        {
            return EnsureAddSourceHandler();
        }
        else
        {
            return EnsureRemoveSourceHandler();
        }
    }
    return S_OK;
}

HRESULT _BaseModelSource::SetIsAttachEnabledInternal(bool newValue)
{
    if(m_isAttachEnabled != newValue)
    {
        m_isAttachEnabled = newValue;
        if(newValue)
        {
            // Ensure we attach if handlers are detected
            IFR(EnsureAddSourceHandler());
        }
        else
        {
            if(IsAttached())
            {
                IFR(DetachFromSourceInternal());
            }
        }
        // Inform base classes that our attach Enabled State has changed
        OnIsAttachEnabledChanged(newValue);
    }
    return S_OK;
}

void _BaseModelSource::OnIsAttachEnabledChanged(bool isAttachEnabled)
{
    UNREFERENCED_PARAMETER(isAttachEnabled);
}

void _BaseModelSource::OnIsAttachedNeededChanged(bool isAttachedNeeded)
{
    UNREFERENCED_PARAMETER(isAttachedNeeded);
}

void _BaseModelSource::DetachIf()
{
    if(m_isAttached)
    {
        DetachFromSourceInternal();
    }
}

HRESULT _BaseModelSource::EnsureAddSourceHandler()
{
    EvaluateIsAttachedNeeded(NeedAttachToEventSource());
    if(IsAttachEnabled() && !IsAttached() && (IsForceAttached() || IsAttachedNeeded()))
    {
        SetIsAttached(true);
        HRESULT hr = AttachToSource();
        // TODO: should we ignore errors during the attachment?
        if(FAILED(hr))
        {
            LOG_FAILURE(AttachToSource,hr);
            //// since for now we are silently failing we will Assert in DEBUG to alert about this condition
            foundation_assert(false);
            // Note: this will cause the IsAttached State to be 'false' again and so multiple
            // OnSource attach will be called
            SetIsAttached(false);
        }
    }
    return S_OK;
}

// Evaluate if we can remove the Attachment from Source 
HRESULT _BaseModelSource::EnsureRemoveSourceHandler()
{
    EvaluateIsAttachedNeeded(!NeedDetachFromEventSource());
    if(m_isAttached && (!IsForceAttached() && !IsAttachedNeeded()) )
    {
        return DetachFromSourceInternal();
    }
    return S_OK;
}

void _BaseModelSource::EvaluateIsAttachedNeeded(bool newValue)
{
    if(newValue != m_isAttachedNeed)
    {
        m_isAttachedNeed = newValue;
        OnIsAttachedNeededChanged(newValue);
    }
}
