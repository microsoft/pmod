/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:VectorChangedEventArgs.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include "windows.foundation.collections.h"

//------------------------------------------------------------------------------
// Class:   CVectorChangedEventArgs
//
// Purpose: An implementation for the IVectorChangedEventArgs
//
//------------------------------------------------------------------------------
class CVectorChangedEventArgs:
    public foundation::ctl::ComInspectableBase,
    public ABI::Windows::Foundation::Collections::IVectorChangedEventArgs
{
public:
    static HRESULT CreateInstance(
        ABI::Windows::Foundation::Collections::CollectionChange action,
        unsigned index,
        ABI::Windows::Foundation::Collections::IVectorChangedEventArgs **ppVectorChangedEventArgs)
    {
        CVectorChangedEventArgs *pVectorChangedEventArgs;
        IFR(foundation::ctl::ComInspectableObject<CVectorChangedEventArgs>::CreateInstance(&pVectorChangedEventArgs));
        IFR(pVectorChangedEventArgs->Initialize(action,index));
        *ppVectorChangedEventArgs = pVectorChangedEventArgs;
        return S_OK;
    }

    // Properties.
    IFACEMETHODIMP 
        get_CollectionChange(__out ABI::Windows::Foundation::Collections::CollectionChange* value)
    {
        IFCPTR(value);
        *value = m_action;
        return S_OK;
    }

    IFACEMETHODIMP 
        get_Index(__out unsigned* value)
    {
        IFCPTR(value);
        *value = m_index;
        return S_OK;
    }

protected:
    CVectorChangedEventArgs()
        :m_index(0),
        m_action(ABI::Windows::Foundation::Collections::CollectionChange::CollectionChange_Reset)
    {
    }
public:
    BEGIN_IID_MAP
        UUIDOF_INTERFACE_ENTRY(ABI::Windows::Foundation::Collections::IVectorChangedEventArgs)
    END_IID_MAP(foundation::ctl::ComInspectableBase)
    
protected:
    __override HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
    {
        if (iid == __uuidof(ABI::Windows::Foundation::Collections::IVectorChangedEventArgs))
        {
            *ppInterface = static_cast<ABI::Windows::Foundation::Collections::IVectorChangedEventArgs *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
private:
    HRESULT Initialize(
        ABI::Windows::Foundation::Collections::CollectionChange action,
        unsigned index)
    {
        m_action = action;
        m_index = index;
        return S_OK;
    }
private:
    // Private properties.
    ABI::Windows::Foundation::Collections::CollectionChange m_action;
    unsigned m_index;
};
