/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenericAdapterFactory.h
****/
#pragma once

#include <foundation/library/foundation_library.h>
#include <foundation/library/util_stl.h>
#include <foundation/ctl/com_library.h>
#include "FoundationUtilHelper.h"
#include "CriticalSection.h"

#include <map>

//------------------------------------------------------------------------------
// Class:   CGenericAdapterFactory
//
// Purpose: A generic Model Adapter Factory
//
//------------------------------------------------------------------------------

class CGenericAdapterFactory:
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IInterfaceAdapterFactory,
    public foundation::library::IGenericAdapterFactory
{
public:
    static HRESULT CreateInstance(
        _COM_Outptr_ foundation::library::IGenericAdapterFactory **ppModelAdapterFactory)
    {
        foundation::ctl::ComInspectableObject<CGenericAdapterFactory> *pNewInstance;

        IFR_ASSERT(foundation::ctl::ComInspectableObject<CGenericAdapterFactory>::CreateInstance(&pNewInstance));
        *ppModelAdapterFactory = static_cast<foundation::library::IGenericAdapterFactory *>(pNewInstance);
        return S_OK;
    }

protected:
    CGenericAdapterFactory();
    // Override from ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;
    // Interface IGenericAdapterFactory
    STDMETHOD(AddAdapterEntry)(
        _In_ IID riid_outer,
        _In_ IID iidTargetType,
        _In_ IID iidSourceType,
        _In_ foundation::library::CreateAdapterInstanceCallback creatorCallback);

    // Interface foundation::library::IInterfaceAdapterFactory
    STDMETHOD(TryCreateInterfaceAdapter)(
            _In_ REFIID riid_outer,
            _In_ REFIID riid,
            _In_ foundation::IInspectable *pOuter,
            _In_ foundation::IInspectable *pInner,
            _Outptr_ foundation::IInspectable **ppInner);

private:
    typedef std::pair<IID, foundation::library::CreateAdapterInstanceCallback>    _CallbackEntry;
    typedef std::map<IID, _CallbackEntry, foundation::library::GuidLessThan> _IIdTargetTypeEntry;
    typedef std::map<IID, _IIdTargetTypeEntry, foundation::library::GuidLessThan> _OuterEntryMap;

    bool Lookup(
        REFIID riid_outer, 
        REFIID riid,
        _CallbackEntry& callbackEntryType);

    foundation_lib::CAutoDeleteCriticalSection          m_critsec;
    _OuterEntryMap m_OuterEntries;
};
