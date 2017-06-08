/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationClassAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.Library.h"
#include <foundation/library/logger_macros.h>

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;


//------------------------------------------------------------------------------
// Class:   CMoAsyncOperationClassAdapter
//
// Purpose: Implementation of IAsyncOperationModelClass Interface
//
//------------------------------------------------------------------------------
class CMoAsyncOperationClassAdapter :
    public foundation::AdapterBase<
        foundation::library::IAsyncOperationClass,
        IAsyncOperationClass,
        &__uuidof(IAsyncOperationClass)
        >
{
public:
    // Interface IAsyncOperationModelClass
    HRESULT STDMETHODCALLTYPE SetCompleted(UINT32 resultCode,IInspectable *result)
    {
        IFR(m_pInner->SetCompleted(resultCode, result));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE FireProgress(IInspectable *pProgressValue)
    {
        IFR(m_pInner->FireProgress(pProgressValue));
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CMoAsyncOperationModelProtectedAdapter
//
// Purpose: Implementation of IPropertyResultModelProtected Interface
//
//------------------------------------------------------------------------------
class CMoAsyncOperationModelProtectedAdapter :
    public foundation::AdapterBase<
    foundation::library::IAsyncOperationClass,
    IAsyncOperationProtected,
    &__uuidof(IAsyncOperationProtected)
    >
{
public:
};

//------------------------------------------------------------------------------
// Class:  CMoAsyncOperationModelOverridesAdapter
//
// Purpose: Implementation of IPropertyResultModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoAsyncOperationModelOverridesAdapter :
    public foundation::AdapterBase
    <
        foundation::library::IAsyncOperationClass,
        IAsyncOperationOverrides,
        &__uuidof(IAsyncOperationOverrides)
    >
{
public:
    // Interface IPropertyModelOverrides
    HRESULT STDMETHODCALLTYPE OnCancel() override
    {
        return S_OK;
    }
};

