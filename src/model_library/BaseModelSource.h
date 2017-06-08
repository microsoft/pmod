/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseModelSource.h
****/
#pragma once

#include <foundation/assert.h>
#include <memory>

#include <foundation/weak_reference.h>
#include <foundation/delegate_library.h>
#include <foundation/errors.h>
#include <foundation/type_info_statics_util.h>
#include <foundation/library/disposable_util.h>
#include <foundation/library/logger_macros.h>

#include <pmod/errors.h>

//------------------------------------------------------------------------------
// Class:   _BaseModelSource Template
//
// Purpose: Define a template class to store and attach to Source(s) Model(s)
//
//------------------------------------------------------------------------------
BEGIN_DECLARE_INTERFACE( _IBaseModelSource,foundation::IUnknown,_VOID_MACRO)
   // Called when the Class is being attached to the Source
    virtual HRESULT AttachToSource() = 0;
    // Called when the Class is being detached from the Source
    virtual HRESULT DetachFromSource() = 0;

    virtual bool IsAttached() = 0;
    virtual HRESULT EnsureAddSourceHandler() = 0;
    virtual HRESULT EnsureRemoveSourceHandler() = 0;
    virtual bool NeedAttachToEventSource() = 0;
    virtual bool NeedDetachFromEventSource() = 0;
END_DECLARE_INTERFACE()

class _BaseModelSource:
    public _IBaseModelSource
{
public:
protected:
    _BaseModelSource();
    virtual ~_BaseModelSource() {}

    // Called when an attachment/detachment from Source happen
    virtual void SetIsAttached(bool isAttached);

    // Called when attach Enabled Flags is changed
    virtual void OnIsAttachEnabledChanged(bool isAttachEnabled);

    // Called when Attached Needed has changed on this object
    virtual void OnIsAttachedNeededChanged(bool isAttachedNeeded);

    virtual bool IsAttachEnabled() { return m_isAttachEnabled; }
    bool IsForceAttached() { return m_forceAttached; }
    bool IsAttachedNeeded() { return m_isAttachedNeed; }

    // Force the Attachment to the Source Model
    HRESULT SetForceAttachedInternal(bool newValue);

    // Enable/Disable the Attachment to the Source Model
    HRESULT SetIsAttachEnabledInternal(bool newValue);

    void DetachIf();

    // Interface _IBaseModelSource
    HRESULT AttachToSource() override;
    HRESULT DetachFromSource() override;

public:
    // Interface _IBaseModelSource
    bool IsAttached() override { return m_isAttached; }

    // Evaluate if Source Attachment is necessary
    HRESULT EnsureAddSourceHandler() override;

    // Evaluate if we can remove the Attachment from Source 
    HRESULT EnsureRemoveSourceHandler() override;

private:
    HRESULT DetachFromSourceInternal();
    void EvaluateIsAttachedNeeded(bool newValue);

private:
    bool m_isAttached:1;          // return true if we are attached to the source object
    bool m_forceAttached:1;       // return true if we 'enforce' this object to be attached
    bool m_isAttachedNeed:1;      // return true only if an attach is needed
    bool m_isAttachEnabled:1;     // return true if attach is enabled for this 'Source' based object
};

//------------------------------------------------------------------------------
// Class:   SingleBaseModelSourceBaseRefHolder Template
//
// Purpose: Define a template class to store and attach a 'Source' Model
//
//------------------------------------------------------------------------------

template <
class TSourceType,      // the Source Type class
class TEventSource,     // the Source Event Source Type
class TEventHandler,    // the Source Event Handler Type
class TEventArgs        // the Source Event Argument Type
>
class SingleBaseModelSourceBaseRefHolder :
    public _BaseModelSource,
    public foundation::IReferenceHolder<TSourceType>
{
protected:
    typedef foundation::EventHandlerDelegateBase
    <
    SingleBaseModelSourceBaseRefHolder<TSourceType, TEventSource, TEventHandler, TEventArgs>,
        TEventHandler,
        TEventArgs
    > _EventHandlerDelegate;

    SingleBaseModelSourceBaseRefHolder():
        m_pEventHandlerDelegate(nullptr)
    {
    }

    virtual ~SingleBaseModelSourceBaseRefHolder()
    {
        // the dtor will Release and Cleanup the created delegate if needed
        if(m_pEventHandlerDelegate != nullptr)
        {
            foundation::library::DisposeIf(m_pEventHandlerDelegate);
            RELEASE_INTERFACE(m_pEventHandlerDelegate);
        }
    }

    virtual HRESULT GetSourceInternal(TSourceType **source)
    {
        foundation_assert(source);
        IFCPTR_ASSERT(source);
        return this->ResolveReference(*source);
    }

    virtual HRESULT SetSourceInternal(TSourceType *source)
    {
        return SetSourceInternal(source,true);
    }

    HRESULT SetSourceInternal(TSourceType *source, bool isVerifyType)
    {
        DetachIf();
        if(source != nullptr && isVerifyType)
        {
            foundation::ComPtr<foundation::IObjectTypeInfo> spSourceModelTypeInfo;
            IFR_ASSERT(GetSourceModelTypeInfo(spSourceModelTypeInfo.GetAddressOf()));
            if(spSourceModelTypeInfo != nullptr)
            {
                IID iidSourceType;
                IFR_ASSERT(spSourceModelTypeInfo->GetType(&iidSourceType));
                bool result;
                IFR_ASSERT(foundation::IsInstanceOfType(source, iidSourceType, result));
                if(!result)
                {
                    foundation_assert(false);
                    return foundation::E_WRONG_TYPE;
                }
            }
        }
        IFR_ASSERT(this->Attach(source));
        if(source)
        {
            return EnsureAddSourceHandler();
        }
        return S_OK;
    }

    virtual HRESULT GetSourceModelTypeInfo(foundation::IObjectTypeInfo **ppModelTypeInfo)
    {
        foundation_assert(ppModelTypeInfo);
        *ppModelTypeInfo = nullptr;
        return S_OK;
    }

    virtual HRESULT CreateSourceEventHandlerDelegate(
        bool isFreeThreaded,
        TEventHandler **ppEventHandlerDelegate)
    {
        _EventHandlerDelegate *pEventHandlerDelegate;
        // Create the Delegate
        IFR_ASSERT(CreateDelegate(this, &pEventHandlerDelegate));
        *ppEventHandlerDelegate = static_cast<_EventHandlerDelegate *>(pEventHandlerDelegate);
        return S_OK;
    }

    virtual HRESULT EnableCallback(
        bool enable,
        foundation::IUnknown *pController)
    {
        return SetCallback<_EventHandlerDelegate>(
            this->m_pEventHandlerDelegate,
            enable ? this:nullptr,
            pController);
    }

    HRESULT _InitializeSourceEventHandler(bool isFreeThreaded)
    {
        // Create the Delegate
        IFR_ASSERT(CreateSourceEventHandlerDelegate(isFreeThreaded,&m_pEventHandlerDelegate));
        return S_OK;
    }

    template <
        typename _EventHandlerDelegateType,
        typename _EventHandler
    >
    static HRESULT SetCallback(
        _EventHandler *pEventHandler,
        typename _EventHandlerDelegateType::_TSink_type *pSink,
        foundation::IUnknown *pController)
    {
        foundation_assert(pEventHandler != nullptr);
        _EventHandlerDelegateType *pEventHandlerDelegate = static_cast<_EventHandlerDelegateType *>(pEventHandler);
        pEventHandlerDelegate->SetCallback(pSink, pController);
        return S_OK;
    }

    template <
        typename _EventHandlerDelegateType,
        typename _EventHandler
    >
    static HRESULT EnableWeakRefCallbackType(
        _EventHandler *pEventHandler,
        bool enable)
    {
        foundation_assert(pEventHandler != nullptr);
        _EventHandlerDelegateType *pEventHandlerDelegate = static_cast<_EventHandlerDelegateType *>(pEventHandler);
        pEventHandlerDelegate->SetCallbackEnabled(enable);
        return S_OK;
    }

protected:

    // Define Pure Virtual methods required
    virtual HRESULT InvokeInternal(foundation::IUnknown* pSender,TEventArgs* pArgs) = 0;

protected:

    typedef foundation::ctl::ComObject<_EventHandlerDelegate>  ComEventHandlerDelegate;

    friend class 
        foundation::EventHandlerDelegateBase
        <
        SingleBaseModelSourceBaseRefHolder<TSourceType, TEventSource, TEventHandler, TEventArgs>,
            TEventHandler,
            TEventArgs
        >;
protected:
    TEventHandler *m_pEventHandlerDelegate;
};

//------------------------------------------------------------------------------
// Class:   SingleBaseModelSource Template
//
// Purpose: A SingleBaseModelSourceBaseRefHolder that use a 'Strong' Reference to 
// the Source Model
//
//------------------------------------------------------------------------------
template 
    <
    class TSourceType,      // the Source Type class
    class TEventSource,		// the Source Event Source Type
    class TEventHandler,	// the Source Event Handler Type
    class TEventArgs		// the Source Event Argument Type
    >
class SingleBaseModelSource :
    public SingleBaseModelSourceBaseRefHolder
    <
    TSourceType,
        TEventSource,
        TEventHandler,
        TEventArgs
    >
{
protected:
    // Implements ReferenceHolder<TModel>
    HRESULT ResolveReference(_Outptr_ TSourceType*& pReference) const override
    {
        pReference = m_spObjectSource;
        if(pReference)
        {
            pReference->AddRef();
        }
        return S_OK;
    }
    HRESULT Attach(_In_ foundation::IUnknown *pUnknown) override
    {
        m_spObjectSource.Release();
        if(pUnknown)
        {
            return foundation::QueryInterface(pUnknown,m_spObjectSource.GetAddressOf());
        }
        return S_OK;
    }
    bool IsReferenceAttached() const override
    {
        return m_spObjectSource != nullptr;
    }
protected:
    foundation::ComPtr<TSourceType> m_spObjectSource;  // A Strong Reference Ptr
};

//------------------------------------------------------------------------------
// Class:   _ReferenceHolderContainer Template
//
// Purpose: A reference holder class that can use weak/strong reference to a type
//
//------------------------------------------------------------------------------
template <class T>
class _ReferenceHolderContainer
{
protected:

    virtual ~_ReferenceHolderContainer()
    {
        m_spReferenceHolder.reset();
    }

    // Define if this class will use a Weak or Strong reference holder
    void SetIsWeakReference(bool isWeakReference)
    {
        if(isWeakReference)
        {
            m_spReferenceHolder.reset(new foundation::WeakReferenceHolder<T>());
        }
        else
        {
            m_spReferenceHolder.reset(new foundation::StrongReferenceHolder<T>());
        }
    }

    HRESULT ResolveReferenceInternal(_Outptr_ T*& pReference) const
    {
        foundation_assert(m_spReferenceHolder.get());
        IFCPTR(m_spReferenceHolder.get());
        return m_spReferenceHolder->ResolveReference(pReference);
    }
    HRESULT AttachInternal(_In_ foundation::IUnknown *pUnknown)
    {
        foundation_assert(m_spReferenceHolder.get());
        IFCPTR(m_spReferenceHolder.get());
        return m_spReferenceHolder->Attach(pUnknown);
    }

    bool IsReferenceAttachedInternal() const
    {
        foundation_assert(m_spReferenceHolder.get());
        IFCPTREx(m_spReferenceHolder.get(),false);
        return m_spReferenceHolder->IsReferenceAttached();
    }
protected:
    HRESULT ResolveRefHolder(_Outptr_ foundation::ComPtr<T>& spStrongRef,_In_ bool ignoreNullReference=false)
    {
        foundation_assert(m_spReferenceHolder.get());
        IFCPTR(m_spReferenceHolder.get());
        IFR_ASSERT(foundation::ResolveReferenceHolderPtr(*m_spReferenceHolder.get(), ignoreNullReference, spStrongRef));
        return S_OK;
    }

protected:
    std::auto_ptr< foundation::ReferenceHolderBase<T> > m_spReferenceHolder;
};

//------------------------------------------------------------------------------
// Class:   _SingleBaseModelSourceRefHolder Template
//
// Purpose: A SingleBaseModelSourceBaseRefHolder that use a Reference Holder to
// the Source Model
//
//------------------------------------------------------------------------------
template 
    <
    class TSourceType,       // the Source Model class
    class TEventSource,     // the Source Event Source Type
    class TEventHandler,    // the Source Event Handler Type
    class TEventArgs	    // the Source Event Argument Type
    >
class _SingleBaseModelSourceRefHolder :
    public SingleBaseModelSourceBaseRefHolder
    <
        TSourceType,
        TEventSource,
        TEventHandler,
        TEventArgs
    >,
    protected _ReferenceHolderContainer<TSourceType>
{
protected:
    typedef _SingleBaseModelSourceRefHolder<TSourceType, TEventSource, TEventHandler, TEventArgs> _SingleBaseModelSourceRefHolderT;
    _SingleBaseModelSourceRefHolder()
    {
    }

    // Implements ReferenceHolder<TModel>
    HRESULT ResolveReference(_Outptr_ TSourceType*& pReference) const override
    {
        return this->ResolveReferenceInternal(pReference);
    }
    HRESULT Attach(_In_ foundation::IUnknown *pUnknown) override
    {
        return this->AttachInternal(pUnknown);
    }

    bool IsReferenceAttached() const override
    {
        return this->IsReferenceAttachedInternal();
    }
};

//------------------------------------------------------------------------------
// Class:   EventSourceBase Template
//
// Purpose: A Event Source template class
//
//------------------------------------------------------------------------------

template <
class TBASE,                // Base Class derived Type
class TEventSource          // the Source Event Source Type
>
class EventSourceBase :
    public TBASE
{
protected:
    EventSourceBase()
    {
    }

    // Define Pure Virtual methods required
    virtual HRESULT GetSourceEventSource(TEventSource **pEventSource) = 0;
    virtual foundation::IUnknown *GetControllerDelegate() = 0;

    virtual HRESULT AttachToEventSource()
    {
        foundation::ComPtr<TEventSource> spEventSource;
        IFR_ASSERT(this->GetSourceEventSource(spEventSource.GetAddressOf()));
        // Enforce the Event Source is not nullptr
        IFCEXPECT_ASSERT(spEventSource.Get());
        IFR_ASSERT(spEventSource->AddHandler(this->m_pEventHandlerDelegate));
        return S_OK;
    }

    virtual HRESULT DetachFromEventSource()
    {
        foundation::ComPtr<TEventSource> spEventSource;
        IFR_ASSERT(this->GetSourceEventSource(spEventSource.GetAddressOf()));
        IFR_ASSERT(spEventSource->RemoveHandler(this->m_pEventHandlerDelegate));
        return S_OK;
    }

    // Implement attach for a single source
    virtual HRESULT AttachToSource()
    {
        // Ensure we setup the callback and 'Controller'
        this->EnableCallback(true, this->GetControllerDelegate());
        foundation_assert(this->IsReferenceAttached());

        return AttachToEventSource();
    }

    // Implement detach for a single source
    virtual HRESULT DetachFromSource()
    {
        // Cleanup Callback
        this->EnableCallback(false,nullptr);

        // when we detach we check if the Reference is attached
        // This will allow Weak reference to be gone when we remove the EventHandler
        if(this->IsReferenceAttached())
        {
            return DetachFromEventSource();
        }
        // indicate the Reference was not attached
        return S_FALSE;
    }
};
//------------------------------------------------------------------------------
// Class:   SingleBaseModelSourceAndEventSource Template
//
// Purpose: Specialization of the 'SingleBaseModelSource' template class
//
//------------------------------------------------------------------------------
template <
class TSourceType,      // the Source Model class
class TEventSource,		// the Source Event Source Type
class TEventHandler,	// the Source Event Handler Type
class TEventArgs		// the Source Event Argument Type
>
class SingleBaseModelSourceAndEventSource :
    public EventSourceBase
        <
        SingleBaseModelSource<TSourceType, TEventSource, TEventHandler, TEventArgs>,
            TEventSource
        >
{
};
//------------------------------------------------------------------------------
// Class:   SingleBaseModelSourceRefHolder Template
//
// Purpose: Specialization of the 'SingleBaseModelSourceRefHolder' template class
//
//------------------------------------------------------------------------------
template <
class TModel,			// the Source Model class
class TEventSource,		// the Source Event Source Type
class TEventHandler,	// the Source Event Handler Type
class TEventArgs		// the Source Event Argument Type
>
class SingleBaseModelSourceRefHolder :
    public EventSourceBase
        <
            _SingleBaseModelSourceRefHolder<TModel,TEventSource,TEventHandler,TEventArgs>,
            TEventSource
        >
{
};
//------------------------------------------------------------------------------
// Class:   BaseSourceModelImpl Template
//
// Purpose: Define a template class that Implements the ISourceModelBase pattern
//
//------------------------------------------------------------------------------
template <class TBase,class TModel,class TSourceModel>
class BaseSourceModelImpl :
    public TBase,
    public TSourceModel
{

protected:
    virtual ~BaseSourceModelImpl()
    {
    }

    // ISourceModelBase Interface Overrides
    STDMETHOD( GetIsAttached )(bool* value)
    {
        * value = this->IsAttached() ;
        return S_OK;
    }

    STDMETHOD( GetForceAttached )(bool* value)
    {
        * value = this->IsForceAttached();
        return S_OK;
    }

    STDMETHOD( SetForceAttached )(bool newValue)
    {
        return this->SetForceAttachedInternal(newValue ? true:false);
    }

    STDMETHOD( GetAttachEnabled )(bool* value)
    {
        * value = this->IsAttachEnabled();
        return S_OK;
    }

    STDMETHOD( SetAttachEnabled )(bool newValue)
    {
        return this->SetIsAttachEnabledInternal(newValue);
    }
};
//------------------------------------------------------------------------------
// Class:   SingleSourceModelImpl Template
//
// Purpose: Define a template class that Implements the ISourceModel pattern
//
//------------------------------------------------------------------------------
template <class TBase,class TSourceType>
class SingleSourceModelImpl :
    public BaseSourceModelImpl<TBase, TSourceType, pmod::ISourceModel>
{
protected:
    // ISourceModel Interface Overrides
    STDMETHOD( GetSource )(foundation::IObject **ppModel)
    {
        foundation::ComPtr<TSourceType> spSource;
        IFR_ASSERT(this->GetSourceInternal(spSource.GetAddressOf()));
        if (spSource)
        {
            *ppModel = spSource;
            (*ppModel)->AddRef();
        }
        else
        {
            *ppModel = nullptr;
        }
        return S_OK;
    }

    STDMETHOD(SetSource)(foundation::IObject *pModel)
    {
        foundation::ComPtr<TSourceType> spSource;
        if(pModel)
        {
            // need to QI for my expected type
            IFR_ASSERT(foundation::QueryInterface(pModel, spSource.GetAddressOf()));
        }
        return this->SetSourceInternal(spSource);
    }
};

//------------------------------------------------------------------------------
// Class:   MultiSourceModelImpl Template
//
// Purpose: Define a template class that Implements the IMultiSourceModel pattern
//
//------------------------------------------------------------------------------
template <class TBase,class TSourceType>
class MultiSourceModelImpl :
    public BaseSourceModelImpl<TBase, TSourceType, pmod::IMultiSourceModel>
{
protected:
    // IMultiSourceModel Interface Overrides
    STDMETHOD( AddSource )(foundation::IObject *pObject)
    {
        IFCPTR_ASSERT(pObject);
        foundation::ComPtr<TSourceType> spSource;
        IFR_ASSERT(foundation::QueryInterface(pObject, spSource.GetAddressOf()));
        IFR_ASSERT(AddSourceInternal(spSource));
        return S_OK;
    }

    STDMETHOD(RemoveSource)(foundation::IObject *pObject)
    {
        IFCPTR_ASSERT(pObject);
        foundation::ComPtr<TSourceType> spSource;
        IFR_ASSERT(foundation::QueryInterface(pObject, spSource.GetAddressOf()));
        IFR_ASSERT(RemoveSourceInternal(spSource));
        return S_OK;
    }
    STDMETHOD(GetSources)(UINT32 *size, foundation::IObject ***ppModel)
    {
        TSourceType **ppSourcesT = nullptr;
        IFR_ASSERT(this->GetSourcesInternal(size, &ppSourcesT));
        *ppModel = reinterpret_cast<foundation::IObject **>(ppSourcesT);
        return S_OK;
    }
    STDMETHOD (RemoveAllSources)()
    {
        return this->RemoveAllSourcesInternal();
    }
    
protected:
    virtual HRESULT AddSourceInternal(TSourceType *pModel) = 0;
    virtual HRESULT RemoveSourceInternal(TSourceType *pModel) = 0;
    virtual HRESULT GetSourcesInternal(UINT32 *size, TSourceType ***ppModel) = 0;
    virtual HRESULT RemoveAllSourcesInternal() = 0;

};
//------------------------------------------------------------------------------
// Class:   SingleSourceModelBase Template
//
// Purpose: ISourceModel implementation based on the 'SingleBaseModelSourceAndEventSource' class template
//
//------------------------------------------------------------------------------

template <
class TModel,			// the Source Model class
class TEventSource,		// the Source Event Source Type
class TEventHandler,	// the Source Event Handler Type
class TEventArgs		// the Source Event Argument Type
>
class SingleSourceModelBase :
    public SingleSourceModelImpl
    <
        SingleBaseModelSourceAndEventSource<TModel,TEventSource,TEventHandler,TEventArgs>, // TBase
        TModel      // The Model Type
    >
{
};
//------------------------------------------------------------------------------
// Class:   SingleSourceModelBaseRefHolder Template
//
// Purpose: ISourceModel implementation based on the 'SingleBaseModelSourceRefHolder' class template
//
//------------------------------------------------------------------------------

template <
class TModel,			// the Source Model class
class TEventSource,		// the Source Event Source Type
class TEventHandler,	// the Source Event Handler Type
class TEventArgs		// the Source Event Argument Type
>
class SingleSourceModelBaseRefHolder :
    public SingleSourceModelImpl
    <
        SingleBaseModelSourceRefHolder<TModel,TEventSource,TEventHandler,TEventArgs>, // TBase
        TModel      // The Model Type
    >
{
};
//------------------------------------------------------------------------------
// Class:   TSingleSourceModelBase Template
//
// Purpose: Base Template for Model Implementation that require a Source object
//
//------------------------------------------------------------------------------

template <
class TBase,			 // the Base class
class TBaseModelSource,  // the Base Source Model Class
class TSourceDelegate,   // Source Model Delegate
class TModel,			 // the Source Model class
class TEventHandler	     // the Model Event Handler Type
>
class TSingleSourceModelBase :
    public TBaseModelSource,
    public TBase
{
public:
protected:
    ~TSingleSourceModelBase()
    {
    }

    // Override from ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::ISourceModel::GetIID())
        {
            *ppInterface = static_cast<pmod::ISourceModel *>(this);
        }
        else if (iid == pmod::ISourceModelBase::GetIID())
        {
            *ppInterface = static_cast<pmod::ISourceModelBase *>(this);
        }
        else
        {
            return TBaseModelSource::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // Overrides from BaseModelSource
    HRESULT SetSourceInternal(TModel *pSource) override
    {
        IFR_ASSERT(TBase::SetSourceInternal(pSource));
        if(GetSourceDelegate() && this->IsDelegateAvailable() )
        {
            IFR_ASSERT(GetSourceDelegate()->OnSourceChanged(pSource));
        }
        return S_OK;
    }

    bool NeedAttachToEventSource() override
    {
        return TBaseModelSource::HasEventHandlers();
    }

    bool NeedDetachFromEventSource() override
    {
        return !TBaseModelSource::HasEventHandlers();
    }
    
    bool IsAttachEnabled() override 
    { 
        return TBase::IsAttachEnabled() && this->IsReferenceAttached(); 
    }

    // Overrides from EventSourceBase
    foundation::IUnknown *GetControllerDelegate() //override
    {
        return foundation::ctl::CastToUnknown(this);
    }

    // Overrides from _FoundationBase
    void OnAddHandlerInternal() override
    {
        TBaseModelSource::OnAddHandlerInternal();
        this->EnsureAddSourceHandler();
    }

    void OnRemoveHandlerInternal() override
    {
        TBaseModelSource::OnRemoveHandlerInternal();
        this->EnsureRemoveSourceHandler();
    }

   HRESULT on_source_attached_internal()
    {
        if(GetSourceDelegate() && this->IsDelegateAvailable())
        {
            IFR_ASSERT(GetSourceDelegate()->OnSourceAttached());
        }

        return S_OK;
    }

    HRESULT on_source_detached_internal()
    {
        if(GetSourceDelegate() && this->IsDelegateAvailable())
        {
            IFR_ASSERT(GetSourceDelegate()->OnSourceDetached());
        }
        return S_OK;
    }
 
protected:
    virtual TSourceDelegate *GetSourceDelegate() = 0;
};

//------------------------------------------------------------------------------
// Class:   SourceViewModelBase Template
//
// Purpose: Template class to derive ViewModel implementation
//
//------------------------------------------------------------------------------

template <
class TBase,			// the Base class
class TViewModelDelegate, // View Model Delegate
class TSourceType,      // the Source Model class
class TEventSource,		// the Source Event Source Type
class TEventHandler,	// the Source Event Handler Type
class TEventArgs		// the Source Event Argument Type
>
class SingleSourceViewModelBase :
    public TSingleSourceModelBase
    <
    SingleSourceModelBase<TSourceType, TEventSource, TEventHandler, TEventArgs>,
        TBase,
        TViewModelDelegate,
        TSourceType,
        TEventHandler
    >
{
private:
    typedef SingleSourceModelBase<TSourceType, TEventSource, TEventHandler, TEventArgs> _single_source_model_base;
protected:
    SingleSourceViewModelBase():
        m_pViewModelDelegate(nullptr)
    {}

    // Overrides from TSingleSourceModelBase
    virtual TViewModelDelegate *GetSourceDelegate() { return m_pViewModelDelegate; }

    // Overrides from BaseModelSource
    HRESULT AttachToSource() override
    {
        IFR_ASSERT(_single_source_model_base::AttachToSource());
        IFR_ASSERT(this->on_source_attached_internal());

        return S_OK;
    }

    HRESULT DetachFromSource() override
    {
        IFR_ASSERT(_single_source_model_base::DetachFromSource());
        IFR_ASSERT(this->on_source_detached_internal());

        return S_OK;
    }

    // If the Base Class is _ComBase we want to enforce a detach to happen
    // Note: This template will fail if no 'DetachIf' method is being defined
    // A detach could happen at this moment if a View Model is forcing the attachment to happen
    void OnFinalRelease() override
    {
        if (!this->IsDelegateWeakRef())
        {
            RELEASE_INTERFACE(m_pViewModelDelegate);
        }
        // call protected member
    	this->DetachIf();
        TBase::OnFinalRelease();
    }

protected:
    template <class TViewModelParam>
    HRESULT InitializeModelImpl(TViewModelParam *viewModelParamImpl)
    {
        IFR_ASSERT(TBase::_Initialize(viewModelParamImpl));
        IFR_ASSERT(_single_source_model_base::_InitializeSourceEventHandler(this->IsFreeThreadApartment()));
        this->m_pViewModelDelegate = viewModelParamImpl->m_p_viewmodel_delegate;
        if (!this->IsDelegateWeakRef() && this->m_pViewModelDelegate != nullptr)
        {
            this->m_pViewModelDelegate->AddRef();
        }
        if(viewModelParamImpl->m_p_source)
        {
            IFR(this->SetSourceInternal(viewModelParamImpl->m_p_source));
        }
        return S_OK;
    }
protected:
    TViewModelDelegate *m_pViewModelDelegate;
};

