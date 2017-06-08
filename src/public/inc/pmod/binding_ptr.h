/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:binding_ptr.h
****/
#pragma once

#include <foundation/object_ptr.h>
#include <pmod/interfaces/binding.h>
#include "source_model_ptr.h"

namespace pmod
{

template <class T, class TPublicInterface = IBindingBase>
class BindingBasePtrWrapper : public foundation::_PtrWrapperBase<T, TPublicInterface>
{
public:
    template <class TValue>
    HRESULT GetValue(TValue *value)
    {
        foundation::InspectablePtr spValue;
        IFR_ASSERT(this->GetClassInterface()->GetValue(spValue.GetAddressOf()));
        _IFR_(foundation::pv_util::GetValue(spValue, value));
        return S_OK;
    }
    template <class TValue>
    HRESULT GetValue(TValue& value)
    {
        foundation::InspectablePtr spValue;
        IFR_ASSERT(this->GetClassInterface()->GetValue(spValue.GetAddressOf()));
        _IFR_(foundation::GetValue(spValue, value));
        return S_OK;
    }
};

template <class T = IBindingBase>
class _BindingBasePtr :
    public foundation::ComPtr<T>,
    public BindingBasePtrWrapper<_BindingBasePtr<T>>,
    public SourceModelBasePtrWrapper<_BindingBasePtr<T>, ISourceModelBase>
{
public:
    typedef foundation::ComPtr<T> _Base_Type;

    _BindingBasePtr() throw()
    {
    }

    _BindingBasePtr(_Inout_ const _BindingBasePtr<T>& lp) throw() :
        _Base_Type(lp)
    {
    }

    _BindingBasePtr(_In_ T* lp) throw() :
        _Base_Type(lp)
    {
    }

    T *GetInterface() const
    {
        return this->Get();
    }

    ISourceModelBase *GetSourceInterface()
    {
        ISourceModelBase *pSourceModelBase = nullptr;
        foundation::QueryWeakReference(this->get(), &pSourceModelBase);
        foundation_assert(pSourceModelBase != nullptr);
        return pSourceModelBase;
    }
};

typedef _BindingBasePtr<> BindingBasePtr;

}

