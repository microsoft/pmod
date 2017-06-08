/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ViewModelBase.h
****/

#pragma once

template <class TModelBase,class TSourceBase,class TEventHandler>
class _CViewModelSourceBase :
    public TSourceBase,
    public TModelBase
{

protected:
    _CViewModelSourceBase()
    {
    }
    virtual ~_CViewModelSourceBase()
    {

    }
    // Overrides from _BaseModelSource
    bool NeedAttachToEventSource() override
    {
        return this->IsObserved();
    }

    bool NeedDetachFromEventSource() override
    {
        return !this->IsObserved();
    }

    // Override from ComBase
    void OnFinalRelease() override
    {
        // Enforce detach from Sources
        ForceRemoveAttachedSources();
        TModelBase::OnFinalRelease();
    }

    // Override from _FoundationBase
    void OnAddHandlerInternal() override
    {
        TModelBase::OnAddHandlerInternal();
        this->EnsureAddSourceHandler();
    }

    void OnRemoveHandlerInternal() override
    {
        TModelBase::OnRemoveHandlerInternal();
        this->EnsureRemoveSourceHandler();
    }

    virtual void ForceRemoveAttachedSources() = 0;

};

template <class TModelBase,class TSourceBase,class TEventHandler,class TDelegateViewModel>
class CViewModelSourceBase :
    public _CViewModelSourceBase<TModelBase,TSourceBase,TEventHandler>
{
protected:
    HRESULT AttachToSource() override
    {
        IFR_ASSERT(TSourceBase::AttachToSource());
        if(m_spViewModelSourceDelegate && this->IsDelegateAvailable())
        {
            IFR_ASSERT(m_spViewModelSourceDelegate->OnSourceAttached());
        }
        return S_OK;
    }

    HRESULT DetachFromSource() override
    {
        IFR_ASSERT(TSourceBase::DetachFromSource());
        // TODO:
        // Review if it is correct to invoke a delegate when a FinalRelease (using 'IsFinalRelease')
        // is being executed
        if(m_spViewModelSourceDelegate 
            && this->IsDelegateAvailable())
        {
            IFR_ASSERT(m_spViewModelSourceDelegate->OnSourceDetached());
        }
        return S_OK;
    }
protected:
    foundation::ComPtr<TDelegateViewModel> m_spViewModelSourceDelegate;
};



