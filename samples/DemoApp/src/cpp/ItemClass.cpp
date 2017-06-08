/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ItemClass.cpp
****/

#include "ItemClass.h"

using namespace pmod;

HRESULT CItemClass::_InitializeModel()
{
    IFR_ASSERT(DemoApp::_FastItemBase::_InitializeModel());
    return S_OK;
}
HRESULT CItemClass::_InitializeModel(HSTRING name)
{
    IFR_ASSERT(DemoApp::_FastItemBase::_InitializeModel());
    IFR_ASSERT(SetName(name));

    return S_OK;
}

HRESULT CItemClass::ExecuteChangeName(HSTRING parameter) 
{
    this->BeginModelChanges(library::BatchModelChangesOptions::None);
    IFR_ASSERT(SetName(parameter));
    this->EndModelChanges();
    return S_OK;
}

_pal_AtomicIntegerType CItemClass::AddRefImpl()
{
    return DemoApp::_FastItemBase::AddRefImpl();
}

_pal_AtomicIntegerType CItemClass::ReleaseImpl()
{
    return DemoApp::_FastItemBase::ReleaseImpl();
}

void CItemClass::FinalRelease()
{
    return DemoApp::_FastItemBase::FinalRelease();
}
