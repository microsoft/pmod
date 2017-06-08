/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandViewModelAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoCommandViewModelAdapter
//
// Purpose: Implementation of ICommandViewModel Interface
//
//------------------------------------------------------------------------------
class CMoCommandViewModelAdapter :
    public foundation::AdapterBase
        <
        pmod::library::ICommandViewModelClass,
        ICommandViewModel,
        &__uuidof(ICommandViewModel)
        >
{
public:
};

//------------------------------------------------------------------------------
// Class:   CMoCommandViewModelProtectedAdapter
//
// Purpose: Implementation of ICollectionViewModelProtected Interface
//
//------------------------------------------------------------------------------
class CMoCommandViewModelProtectedAdapter :
    public foundation::AdapterBase
        <
        pmod::library::ICommandViewModelClass,
        ICommandViewModelProtected,
        &__uuidof(ICommandViewModelProtected)
        >
{
public:
      HRESULT STDMETHODCALLTYPE SetCommandLabel(HSTRING commandLabel)
      {
          return m_pInner->SetCommandLabel(commandLabel);
      }
};

//------------------------------------------------------------------------------
// Class:   CMoCommandViewModelOverridesAdapter
//
// Purpose: Implementation of ICommandViewModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoCommandViewModelOverridesAdapter :
    public foundation::AdapterBase
        <
        pmod::library::ICommandClass,
        ICommandViewModelOverrides,
        &__uuidof(ICommandViewModelOverrides)
        >
{
public:
    HRESULT STDMETHODCALLTYPE OnSourceCommandStateChanged(int oldState,int newState) override
    {
        UNREFERENCED_PARAMETER(oldState);
        return m_pInner->SetCommandState(newState,true);
    }
};
