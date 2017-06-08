/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeAdapterFactory.h
****/
#pragma once

#include <foundation/library/foundation_library.h>
#include <foundation/library/util_stl.h>

#include <foundation/ctl/com_library.h>
#include "FoundationUtilHelper.h"
#include "CriticalSection.h"

#include <map>

//------------------------------------------------------------------------------
// Class:   CTypeAdapterFactory
//
// Purpose: Implement foundation::library::IInterfaceAdapterFactory for an IModel Type
//
//------------------------------------------------------------------------------

class CTypeAdapterFactory:
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IInterfaceAdapterFactory,
    public foundation::library::ITypeAdapterFactory
{
public:
    static HRESULT CreateInstance(
        _In_opt_ const foundation::library::_TypeAdapterEntry iidTypes[],
        _COM_Outptr_ foundation::library::IInterfaceAdapterFactory **ppModelAdapterFactory)
    {
        foundation::ctl::ComInspectableObject<CTypeAdapterFactory> *pNewInstance = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CTypeAdapterFactory>::CreateInstance(&pNewInstance));
        if(iidTypes != nullptr)
        {
            IFR_ASSERT(pNewInstance->AddTypeEntries(iidTypes));
        }
	    *ppModelAdapterFactory = static_cast<foundation::library::IInterfaceAdapterFactory *>(pNewInstance);
        return S_OK;
    }

    static HRESULT CreateInstance(
        _Outptr_ foundation::library::ITypeAdapterFactory **ppModelTypeAdapterFactory)
    {
        foundation::ctl::ComInspectableObject<CTypeAdapterFactory> *pNewInstance;

        IFR_ASSERT(foundation::ctl::ComInspectableObject<CTypeAdapterFactory>::CreateInstance(&pNewInstance));
        *ppModelTypeAdapterFactory = static_cast<foundation::library::ITypeAdapterFactory *>(pNewInstance);
        return S_OK;
    }

protected:
    CTypeAdapterFactory();

    // Override from ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;
    // Interface IModelTypeAdapterFactory
    STDMETHOD(AddTypeEntries)(const foundation::library::_TypeAdapterEntry iidTypes[]);
    STDMETHOD(AddTypeEntry)(_In_ IID iidInterface,_In_ IID iidType,_In_ foundation::library::CreateAdapterInstanceCallback creator);

    // Interface foundation::library::IInterfaceAdapterFactory
    STDMETHOD(TryCreateInterfaceAdapter)(
            _In_ REFIID riid_outer,
            _In_ REFIID riid,
            _In_ foundation::IInspectable *pOuter,
            _In_ foundation::IInspectable *pInner,
            _Outptr_ foundation::IInspectable **ppInner);
private:

    void AddTypeEntriesInternal(const foundation::library::_TypeAdapterEntry entries[]);
    void AddTypeEntryInternal(_In_ IID iidInterface,_In_ IID iidType,_In_ foundation::library::CreateAdapterInstanceCallback creator);
private:
    typedef std::pair<IID,foundation::library::CreateAdapterInstanceCallback>    _InterfaceEntryType;

    bool Lookup(_In_ REFIID riid,_InterfaceEntryType& interfaceEntryType);

    typedef std::map<IID, _InterfaceEntryType, foundation::library::GuidLessThan>  _GuidCreatorMap;
    typedef std::vector<std::pair<IID,_InterfaceEntryType> >                _GuidCreatorVector;

    foundation_lib::CAutoDeleteCriticalSection          m_critsec;
    _GuidCreatorMap      m_iidTypesMap;
   _GuidCreatorVector   m_iidTypesVector;
};

