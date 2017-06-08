/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MultiSourceBinding.h
****/
#pragma once

#include "FoundationBase.h"

#include "MultiSourceModelBase.h"
#include "ViewModelBase.h"

#include "BindingBase.h"
#include "BaseModelSource.h"


typedef _FoundationBase
    <
    pmod::IMultiSourceBinding,
    pmod::library::IMultiSourceBindingClass,
    pmod::library::IMultiSourceBindingDelegate,
    pmod::library::MultiSourcePropertyBindingCreateParameters,
    pmod::IBindingValueChangedEventSource,
    pmod::IBindingValueChangedEventHandler,
    foundation::IUnknown,
    pmod::IBindingValueChangedEventArgs,
    true
    >
    _MultiSourceBindingBaseT;

//------------------------------------------------------------------------------
// Class:   CMultiSourceBinding
//
// Purpose: A multi-source binding support class
//
//------------------------------------------------------------------------------
typedef MultiSourceModelImpl
<
    _CViewModelSourceBase
    <
        CBindingBase<_MultiSourceBindingBaseT>,
        CMultiSourceModelBase<CMultiSourceBindingContainer>,
        pmod::IBindingValueChangedEventHandler
    >,
    pmod::IBindingBase
> _CMultiSourceBindingBase_Type;

class CMultiSourceBinding :
    public _CMultiSourceBindingBase_Type
{ 
public:
    HRESULT _Initialize(const pmod::library::MultiSourcePropertyBindingCreateParameters *modelImpl);

    static HRESULT CreateInstance(
        _In_ const pmod::library::MultiSourcePropertyBindingCreateParameters *pModelImpl,
        _In_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

protected:
    CMultiSourceBinding():
        m_IsValueSet(false),
        m_IsValueValid(false)
    {
    }

    // Override from ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;
    void FinalRelease() override;

    // Overrides from CMultiSourceModelBase
    HRESULT on_source_changed_internal(pmod::IBindingBase *pSender,pmod::IBindingValueChangedEventArgs *pEventArgs) override;

    // IBindingBase Interface
    STDMETHOD( GetValue )(foundation::IInspectable **ppValue);

    // IMultiSourceBindingClass Interface
    STDMETHOD(GetBindingSources)(_Out_ UINT32 *size, _Outptr_ pmod::IBindingBase ***ppBindings) override;

    // Overrides from _BaseModelSource
    HRESULT AttachToSource() override;
    HRESULT DetachFromSource() override;


    void ForceRemoveAttachedSources() override;
    HRESULT AttachToSourceInternal(pmod::IBindingBase *pSource) override;
    HRESULT DetachFromSourceInternal(pmod::IBindingBase *pSource) override;

    // Overrides from MultiSourceModelImpl
    HRESULT AddSourceInternal(pmod::IBindingBase *pSource) override;
    HRESULT RemoveSourceInternal(pmod::IBindingBase *pSource) override;
    HRESULT GetSourcesInternal(UINT32 *size,pmod::IBindingBase ***ppModel) override;
    HRESULT RemoveAllSourcesInternal() override;

    bool IsValueSet() override
    {
        return m_IsValueSet;
    }
    bool IsValueValid() override
    {
        return m_IsValueValid;
    }

    #if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override
    {
        _traceMessage(U("MultiSourceBinding this:%04x"),this);
    }
    #endif
private:
    HRESULT RecalcMultisourceBindingValue(foundation::IInspectable **ppValue);
private:
    bool        m_IsValueSet;
    bool        m_IsValueValid;
};

