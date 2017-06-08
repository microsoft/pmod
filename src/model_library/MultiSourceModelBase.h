/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MultiSourceModelBase.h
****/

#pragma once

#include <vector>
#include <algorithm>

#include <pmod/eventhandler_util.h>
#include "BaseModelSource.h"

//------------------------------------------------------------------------------
// Class:   TMultiSourceModelContainer
//
// Purpose: A base template class to mantain a list of multiple 'Sources' Models
//
//------------------------------------------------------------------------------
template <
    typename TSource,
    typename TEventHandler,
    typename TEventArgs,
    typename TListenerBase
    >
class TMultiSourceModelContainer
{
protected:
    typedef TSource _TSource;
    typedef std::vector< foundation::ComPtr<TSource> > VectorSourceType;

#ifdef DEBUG
    virtual ~TMultiSourceModelContainer()
    {
        // Make sure the delegste is cleaned before destring.
        foundation_assert(m_spMultiSourceDelegate==nullptr);
    }
#endif

    HRESULT AddSourceModel(TSource *pSource,bool attach)
    {
        foundation_assert(pSource);
        m_Sources.push_back(pSource);
        if(attach)
        {
            IFR_ASSERT(EnsureDelegate());
            IFR_ASSERT(TListenerBase::AttachEventHandler(pSource, m_spMultiSourceDelegate));
        }
        return S_OK;
    }

    HRESULT RemoveSourceModel(TSource *pSource,bool detach)
    {
        foundation_assert(pSource);
        typename VectorSourceType::iterator iter = std::find(
                    m_Sources.begin(),
                    m_Sources.end(),
                    pSource);
        if(iter != m_Sources.end())
        {
            if(detach)
            {
                foundation_assert(m_spMultiSourceDelegate != nullptr);
                IFR_ASSERT(TListenerBase::DetachEventHandler(pSource, m_spMultiSourceDelegate));
            }
            m_Sources.erase(iter);
        }
        return S_OK;
    }

    HRESULT AttachAllSources()
    {
        IFR_ASSERT(EnsureDelegate());
        VectorSourceType sources(m_Sources);

        for (typename VectorSourceType::const_iterator iter = sources.begin();
                        iter != sources.end(); 
                        ++iter)
        {
            IFR_ASSERT(TListenerBase::AttachEventHandler((*iter), m_spMultiSourceDelegate));
        }
        return S_OK;
    }

    HRESULT DetachAllSources()
    {
        if (this->m_spMultiSourceDelegate == nullptr)
        {
            // we were not before attached
            return S_FALSE;
        }
        VectorSourceType sources(m_Sources);
        for (typename VectorSourceType::const_iterator iter = sources.begin();
                        iter != sources.end(); 
                        ++iter)
        {
            IFR_ASSERT(TListenerBase::DetachEventHandler((*iter), m_spMultiSourceDelegate));
        }
        
        // After removing all referenced to the handler we are going to clean it
        IFR_ASSERT(DisposeInternal());
        return S_OK;
    }

    HRESULT RemoveAllSources(bool detach)
    {
        if(detach)
        {
            IFR_ASSERT(DetachAllSources());
        }
        m_Sources.clear();
        return S_OK;
    }

    HRESULT DisposeInternal()
    {
        foundation_assert(m_spMultiSourceDelegate != nullptr);
        // Dispose delegate to prevent already fired and panding events to get here
        IFR_ASSERT(foundation::library::DisposeIf(m_spMultiSourceDelegate->CastToUnknown()));

        // clean up the reference when is done. 
        m_spMultiSourceDelegate.Release();

        return S_OK;
    }

    virtual HRESULT on_source_changed_internal(TSource *pSender, TEventArgs *pEventArgs) = 0;
    virtual foundation::IUnknown *GetMultiSourceDelegateController() = 0;
    
protected:

    const VectorSourceType& GetSources() const
    {
        return m_Sources;
    }
private:
    HRESULT EnsureDelegate()
    {
        // Create a delegate for the receiver, if we haven't already
        if (!m_spMultiSourceDelegate)
        {
            IFR_ASSERT(TDelegate::CreateDelegateInstance(
                this,
                &TMultiSourceModelContainerType::OnSourceChanged,
                GetMultiSourceDelegateController(),
                m_spMultiSourceDelegate.ReleaseAndGetAddressOf()));
        }
        return S_OK;
    }

    HRESULT OnSourceChanged(foundation::IUnknown *pSender, TEventArgs *pEventArgs)
    {
        TSource *pSource = nullptr;
        IFR_ASSERT(foundation::QueryWeakReference(pSender, &pSource));

        return on_source_changed_internal(pSource,pEventArgs);
    }
private:
    typedef TMultiSourceModelContainer<TSource,TEventHandler,TEventArgs,TListenerBase> TMultiSourceModelContainerType;

    typedef foundation::EventHandlerDelegateBaseEx<
        TMultiSourceModelContainerType,
        TEventHandler,
        TEventArgs
        > TDelegate;

    VectorSourceType           m_Sources;
    foundation::ComPtr<typename TDelegate::ComObject>  m_spMultiSourceDelegate;
};

// Specialized types
typedef  TMultiSourceModelContainer
                <
                    pmod::IObservableObject,
                    pmod::IPropertyChangedEventHandler,
                    pmod::IPropertyChangedEventArgs,
                    pmod::ObservableObjectListenerBase
                > CMultiSourcePropertyModelContainer;

typedef  TMultiSourceModelContainer
                <
                    
                    pmod::IObservableCollection,
                    pmod::INotifyCollectionChangedEventHandler,
                    pmod::INotifyCollectionChangedEventArgs,
                    pmod::ObservableCollectionListenerBase
                > CMultiSourceCollectionModelContainer;

typedef  TMultiSourceModelContainer
                <
                    pmod::IBindingBase,
                    pmod::IBindingValueChangedEventHandler,
                    pmod::IBindingValueChangedEventArgs,
                    pmod::PropertyBindingListener
                > CMultiSourceBindingContainer;

//------------------------------------------------------------------------------
// Class:   CMultiSourceModelBase
//
// Purpose: A base template class to mantain a list of multiple 'Sources' Models
//
//------------------------------------------------------------------------------
template <
    typename TMultiSourceModelContainer
    >
class CMultiSourceModelBase :
    public _BaseModelSource,
    protected TMultiSourceModelContainer
{
protected:
    CMultiSourceModelBase()
    {
    }
protected:
    typedef typename TMultiSourceModelContainer::_TSource TSource;

    virtual HRESULT AttachToSourceInternal(TSource *pSource)
    {
        IFR_ASSERT(this->AddSourceModel(pSource, IsAttached()));
        return S_OK;
    }

    virtual HRESULT DetachFromSourceInternal(TSource *pSource)
    {
        IFR_ASSERT(this->RemoveSourceModel(pSource, IsAttached()));
        return S_OK;
    }

    // Overrides from TMultiSourceModelContainer
    foundation::IUnknown *GetMultiSourceDelegateController() override
    {
        return static_cast<_IBaseModelSource *>(this);
    }

    // Overrides from _BaseModelSource
    HRESULT AttachToSource() override
    {
        IFR_ASSERT(this->AttachAllSources());
        IFR_ASSERT(this->_BaseModelSource::AttachToSource());
        return S_OK;
    }

    HRESULT DetachFromSource() override
    {
        IFR_ASSERT(this->DetachAllSources());
        IFR_ASSERT(this->_BaseModelSource::DetachFromSource());
        return S_OK;
    }
};


