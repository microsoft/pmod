/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ActivationFactory.h
****/
#pragma once
#include <map>

#include <foundation/com_interfaces.h>
#include <foundation/library/util_stl.h>
#include <foundation/library/activation_factory.h>

//------------------------------------------------------------------------------
// Class:   CActivationFactory
//
// Purpose: Activation Factory entry point
//
//------------------------------------------------------------------------------

struct _ActivationFactoryEntry
{
    LPCSTR_t _activationId;
    foundation::library::GetActivationFactoryInstanceFunc _pGetActivationFactoryInstanceFunc;
};

class CActivationFactory
{
public:
    static foundation::IUnknown *GetActivationFactoryInstance(LPCSTR_t activationId);
    static HRESULT RegisterActivationFactory(LPCSTR_t activationId, foundation::library::GetActivationFactoryInstanceFunc pGetActivationFactoryInstance);
    static HRESULT RegisterActivationFactoryEntries(_ActivationFactoryEntry *pEntries);
private:
    static CActivationFactory& GetInstance();
    foundation::IUnknown *GetActivationFactoryInstanceInternal(LPCSTR_t activationId);
private:
    typedef std::map<LPCSTR_t, foundation::library::GetActivationFactoryInstanceFunc, foundation::library::StringLessThan> _ActivationFactoryInstance_Map_Type;

    _ActivationFactoryInstance_Map_Type _registeredFactories;
};

class _RegisterActivationFactory
{
public:

    _RegisterActivationFactory(LPCSTR_t activationId, foundation::library::GetActivationFactoryInstanceFunc pGetActivationFactoryInstance)
    {
        CActivationFactory::RegisterActivationFactory(activationId, pGetActivationFactoryInstance);
    }
    _RegisterActivationFactory(_ActivationFactoryEntry *pEntries)
    {
        CActivationFactory::RegisterActivationFactoryEntries(pEntries);
    }
};
