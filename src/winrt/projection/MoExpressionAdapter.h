/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoExpressionAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

#include "MoCallbackAdapterBase.h"

// Public Headers
#include <foundation/library/adapter_base.h>
#include <foundation/hstring_wrapper.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoExpressionAdapter
//
// Purpose: Implementation of IExpression Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoExpressionAdapter :
    public foundation::AdapterBase
    <
        foundation::IExpression,
        IExpression,
        &__uuidof(IExpression)
    >
{
private:
    class CResolveTokenCallbackAdapter:
        public TMoCallbackAdapterBase
        <
            IResolveTokenCallback,
            foundation::IResolveTokenDelegate,
            &foundation::IID_IResolveTokenDelegate
        >
    {
    protected:

        HRESULT STDMETHODCALLTYPE Invoke( 
            LPCTSTR referenceToken,
            ABI::Windows::Foundation::IPropertyValue **ppValue)
        {
            return this->GetMoDelegate()->Invoke( foundation::HStringPtr(referenceToken),ppValue);
        }
    };

public:
    HRESULT STDMETHODCALLTYPE GetReferenceTokens( 
        UINT32 *size,
        HSTRING **referenceToken) override
    {
        return this->m_pInner->GetReferenceTokens(size,referenceToken);
    }

    HRESULT STDMETHODCALLTYPE Evaluate( 
        IResolveTokenCallback *pCallback,
        ABI::Windows::Foundation::IPropertyValue **ppValue) override
    {
        IFCPTR(pCallback);
        IFCPTR(ppValue);

        foundation::ComPtr<foundation::IResolveTokenDelegate> spResolveTokenDelegate;

        IFR(CResolveTokenCallbackAdapter::CreateInstance<CResolveTokenCallbackAdapter>(pCallback,spResolveTokenDelegate.GetAddressOf()));
        HRESULT hr = this->m_pInner->Evaluate(
            spResolveTokenDelegate,
            ppValue);
        return hr;
    }

};

