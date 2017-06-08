/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BindingBase.h
****/
#pragma once

#include <pmod/library/model_library.h>
#include <foundation/library/logger_macros.h>

#include "BindingValueChangedEventArgs.h"
#include <foundation/com_ptr.h>

#include <foundation/library/perf_counter_util.h>
#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#endif

BEGIN_DECLARE_INTERFACE( IBindingBasePrivate,foundation::IUnknown,_VOID_MACRO)
    virtual bool IsValueSet() = 0;
    virtual bool IsValueValid() = 0;
END_DECLARE_INTERFACE()

#ifdef _MSC_VER
#pragma warning( disable : 4503 )
#endif
//------------------------------------------------------------------------------
// Class:   CBindingBase
//
// Purpose: Implementation of IBindingBase Interface
//
//------------------------------------------------------------------------------

template <class TBASE> 
class CBindingBase :
    public TBASE,
    protected IBindingBasePrivate,
    protected foundation::IObject
{
protected:
    CBindingBase() :
        m_bCachedBindingValue(false)
    {
    }

    virtual ~CBindingBase()
    {
    }
    // IObject Interface => E_NOTIMPL
    // Note: we want a simple E_NOTIMPL interface since we
    // would like to be QI'ed as an IObjectNode for multisource scenarios
    STDMETHOD(GetUniqueId)(UINT32 *uniqueId)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(GetProcessId)(UINT32 *processId)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue)
    {
        return E_NOTIMPL;
    }

    // IBindingBase Interface
    STDMETHOD( GetConverter )(pmod::IBindingConverter **pValue)
    {
        return m_spConverter.CopyTo(pValue);
    }

    STDMETHOD( SetConverter )(pmod::IBindingConverter *value)
    {
        m_spConverter = value;
        return S_OK;
    }

    STDMETHOD(GetConverterParameter)(foundation::IInspectable **pValue)
    {
        return m_spConverterParameter.CopyTo(pValue);
    }

    STDMETHOD(SetConverterParameter)(foundation::IInspectable *value)
    {
        m_spConverterParameter = value;
        return S_OK;
    }

    STDMETHOD( GetBindingValueChangedEventSource )(_Outptr_ pmod::IBindingValueChangedEventSource** ppEventSource) override
    {
        IFR_ASSERT(this->EnsureEventSourceModel());

        *ppEventSource = this->GetEventSourceImpl();
        (*ppEventSource)->AddRef();
        return S_OK;
    }

    // Override foundation::ctl::ComInspectableBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if(iid == pmod::IBindingBase::GetIID())
        {
            *ppInterface = static_cast<pmod::IBindingBase *>(this);
        }
        else if(iid == IBindingBasePrivate::GetIID())
        {
            *ppInterface = static_cast<IBindingBasePrivate *>(this);
        }
        else if (iid == foundation::IObject::GetIID())
        {
            *ppInterface = static_cast<foundation::IObject *>(this);
        }
        else
        {
            return TBASE::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    HRESULT InternalFireBindingValueChanged()
    {
        if (this->IsObserved())
        {
            PERF_CONTEXT(pmod::Core_PerfId_BindingBase_FireValueChanged_Id)
            
            foundation::ComPtr<foundation::ctl::ComInspectableObject<CBindingValueChangedEventArgs> > spBindingValueChangedEventArgs;
            IFR_ASSERT(foundation::ctl::ComInspectableObject<CBindingValueChangedEventArgs>::CreateInstance(spBindingValueChangedEventArgs.GetAddressOf()));

            IFR(this->FireWithCallback(
                &CBindingBase<TBASE>::PerformBindingValueChanged,
                static_cast<pmod::IBindingValueChangedEventArgs *>(spBindingValueChangedEventArgs.Get())));
        }
        return S_OK;
    }

    HRESULT ConvertValue(_Inout_ foundation::InspectablePtr& value)
    {
        if(m_spConverter)
        {
            foundation::InspectablePtr originalValue(value);
            value.Release();
            // proceed to convert
            IFR(m_spConverter->Convert(
                originalValue,
                m_spConverterParameter,
                value.GetAddressOf()));
        }
        return S_OK;
    }

    // caching binding value methods
    void InvalidateCachedBindingValue()
    {
        m_bCachedBindingValue = false;
        m_BindingValue.Release();
    }

    void SetCachedBindingValue(foundation::IInspectable *pValue)
    {
        m_bCachedBindingValue = true;
        m_BindingValue = pValue;
    }

    bool IsCachedBindingValue() { return m_bCachedBindingValue; }
    HRESULT GetCachedBindingValue(foundation::IInspectable **ppValue)
    {
        return m_BindingValue.CopyTo(ppValue);
    }

private:
    HRESULT PerformBindingValueChanged(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		pmod::IBindingValueChangedEventArgs *pEventArgs)
    {
        IFR(this->FireEvent(pEventHandlerContainer,pEventArgs));
	    return S_OK;
    }
private:
    foundation::ComPtr<pmod::IBindingConverter>  m_spConverter;
    foundation::InspectablePtr                            m_spConverterParameter;
    bool                                m_bCachedBindingValue : 1;
    foundation::InspectablePtr        m_BindingValue;
};

