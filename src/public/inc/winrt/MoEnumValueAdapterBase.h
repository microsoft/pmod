/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEnumValueAdapterBase.h
****/

#pragma once

#include <foundation/library/adapter_base.h>

//------------------------------------------------------------------------------
// Class:   TMoObservableObjectAdapterBase
//
// Purpose: Define a template base class for all the model adapter generated classes
//
//------------------------------------------------------------------------------
template <class TInterface,class TEnum,const IID* piid = &__uuidof(TInterface)>
class TMoEnumValueAdapterBase :
    public foundation::AdapterBase
        <
        foundation::IEnumValue,
        TInterface,
        piid
        >
{
protected:
    HRESULT STDMETHODCALLTYPE get_Value(_Out_ TEnum *value) override
    {
        UINT32 enumValue;
        this->m_pInner->GetValue(&enumValue);
        *value = (TEnum)enumValue;
        return S_OK;
    }


};
