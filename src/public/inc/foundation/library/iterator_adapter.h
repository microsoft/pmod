/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:iterator_adapter.h
****/

#pragma once

#include <foundation/assert.h>

#include <foundation/ctl/com_library.h>
#include <foundation/interfaces/iterator.h>

namespace foundation
{
//------------------------------------------------------------------------------
// Class:   AdapterBase
//
// Purpose: Define a template base class for an Adapter class
//
//------------------------------------------------------------------------------
template <class TIterator>
class IteratorInspectableAdapter :
    public foundation::ctl::ComInspectableBase,
    public IIterator_Inspectable,
    public TIterator
{
public:
    HRESULT _Initialize(_In_ IIterator_Inspectable *pIterator)
    {
        foundation_assert(pIterator != nullptr);

        _pInspectableIterator = pIterator;
        _pInspectableIterator->AddRef();
        return S_OK;
    }

protected:
    IteratorInspectableAdapter():
        _pInspectableIterator(nullptr)
    {}

    // override ComInspectableBase

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == TIterator::GetIID())
        {
            *ppInterface = static_cast<TIterator *>(this);
        }
        else if (iid == IIterator_Inspectable::GetIID())
        {
            *ppInterface = static_cast<IIterator_Inspectable *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    void FinalRelease() override
    {
        foundation::ctl::ComInspectableBase::FinalRelease();
        RELEASE_INTERFACE(_pInspectableIterator);
    }

    STDMETHOD(GetCurrent)(foundation::IInspectable **current)
    {
        return _pInspectableIterator->GetCurrent(current);
    }

    STDMETHOD(GetCurrent)(typename TIterator::_Item_Type *current)
    {
        InspectablePtr current_ptr;
        _IFR_(_pInspectableIterator->GetCurrent(current_ptr.GetAddressOf()));
        if (current_ptr != nullptr)
        {
            return foundation::QueryInterface(current_ptr, current);
        }
        *current = nullptr;
        return S_OK;
    }

    STDMETHOD(GetHasCurrent)(_Out_ bool *hasCurrent)
    {
        return _pInspectableIterator->GetHasCurrent(hasCurrent);
    }
    STDMETHOD(MoveNext)(_Out_ bool *hasCurrent)
    {
        return _pInspectableIterator->MoveNext(hasCurrent);
    }
protected:
    IIterator_Inspectable          *_pInspectableIterator;
};

template <class TIterator>
static inline HRESULT CreateIteratorInspectableAdapter(
    _In_ IIterator_Inspectable *pIterator,
    _COM_Outptr_ TIterator **ppIterator
    )
{
    return ctl::inspectable::CreateInstanceWithInitialize<IteratorInspectableAdapter<TIterator>>(
        ppIterator,
        pIterator);
}

}
