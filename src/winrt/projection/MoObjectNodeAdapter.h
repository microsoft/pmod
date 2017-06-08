/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObjectNodeAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

#include "MoDispatcherAdapter.h"

// Public Headers
#include <foundation/library/adapter_base.h>
#include <foundation/com_ptr.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoObjectNodeAdapter
//
// Purpose: Implementation of IObjectNode Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoObjectNodeAdapter :
    public foundation::AdapterBase
        <
        foundation::_IObjectNode,
        IObjectNode,
        &__uuidof(IObjectNode)
        >
{
protected:
    HRESULT STDMETHODCALLTYPE get_Parent(IObject **ppModel) override
    {
        foundation::ComPtr<foundation::IObject> spParent;
        IFR(m_pInner->GetParent(spParent.GetAddressOf()));
        if(spParent != nullptr)
        {
            return foundation::QueryInterface(spParent,ppModel);
        }
        else
        {
            *ppModel = nullptr;
            return S_OK;
        }
    }


    HRESULT STDMETHODCALLTYPE GetChilds( 
        UINT32 *length,
        IObject ***value) override
    {
        foundation::ArrayRefCountWrapper<foundation::IObject> childs;
        IFR(m_pInner->GetChilds(childs.GetSizeAddressOf(),childs.GetBufferAddressOf()));

        std::vector< foundation::ComPtr<IObject> > moChilds;
        for(UINT32 index = 0;index < childs.GetSize();++index)
        {
            foundation::ComPtr<IObject> spMoModel;
            IFR(foundation::QueryInterface(childs[index],spMoModel.GetAddressOf()));
            moChilds.push_back(spMoModel);
        }
        ContainerRefCountToArray(moChilds,length,value);
        return S_OK;
    }
};


