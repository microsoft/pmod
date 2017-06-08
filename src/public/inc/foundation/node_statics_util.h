/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:node_statics_util.h
****/
#pragma once

#include <foundation/macros.h>
#include <foundation/array_wrapper.h>
#include "interfaces/node_statics.h"
#include <foundation/activation_factory.h>
#include <foundation/com_ptr.h>

namespace foundation {

    inline INodeStatics *GetNodeStatics()
    {
        return GetActivationFactoryWithCast<INodeStatics>(U("foundation.NodeStatics"));
    }

    template <class T>
    static HRESULT GetObjectParent(
        _In_ IObject* pObjectNode,
        _COM_Outptr_result_maybenull_ T** ppValue)
    {
        foundation::ComPtr<IObject> parent_ptr;
        IFR_ASSERT(GetNodeStatics()->GetObjectParent(pObjectNode, parent_ptr.GetAddressOf()));
        if (parent_ptr != nullptr)
        {
            return foundation::QueryInterface(parent_ptr, ppValue);
        }

        *ppValue = nullptr;
        return S_OK;
    }

    template <class T>
    static HRESULT GetObjectChilds(
        _In_ IObject* pObject,
        _Out_ UINT32 *pSize,
        _COM_Outptr_result_maybenull_ T*** ppChilds)
    {
        IFCPTR_ASSERT(pObject);
        IFCPTR_ASSERT(ppChilds);

        _IObjectNode  *pObjectNode = foundation::CastTo<foundation::_IObjectNode>(pObject);
        if (pObjectNode == nullptr)
        {
            *ppChilds = nullptr;
            return S_FALSE;
        }
        ArrayRefCountWrapper<foundation::IObject> childs;
        IFR_ASSERT(pObjectNode->GetChilds(childs.GetSizeAddressOf(), childs.GetBufferAddressOf()));

        std::vector<foundation::ComPtr<T>> childs_with_type;
        childs_with_type.resize(childs.GetSize());
        for (UINT32 index = 0; index < childs.GetSize();++index)
        {
            IFR_ASSERT(foundation::QueryInterfaceIf(
                childs[index], 
                childs_with_type[index].GetAddressOf()));
        }
        return RefCountCopyTo(
            childs_with_type,
            pSize,
            ppChilds);
    }

    template <class T>
    static HRESULT GetObjectAncestor(
        _In_ IObject *pObject,
        const IID& ancestorType,
        _COM_Outptr_result_maybenull_ T **ppAncestor)
    {
        ComPtr<IObject> ancestor_ptr;
        IFR_ASSERT(GetNodeStatics()->GetObjectAncestor(pObject, ancestorType, ancestor_ptr.GetAddressOf()));
        return QueryInterfaceIf(ancestor_ptr, ppAncestor);
    }

    template <class T>
    static HRESULT GetObjectAncestor(
        _In_ IObject *pObject,
        _COM_Outptr_result_maybenull_ T **ppAncestor)
    {
        return GetObjectAncestor(pObject, T::GetIID(), ppAncestor);
    }

    template <class T>
    static HRESULT GetObjectRoot(
        _In_ IObject *pObject,
        _COM_Outptr_result_maybenull_ T **ppRoot)
    {
        ComPtr<IObject> root_ptr;
        IFR_ASSERT(GetNodeStatics()->GetObjectRoot(pObject, root_ptr.GetAddressOf()));
        return QueryInterfaceIf(root_ptr, ppRoot);
    }

    template <class T>
    static HRESULT QueryObjectService(
        _In_ IObject *pObject,
        REFIID guidService,
        _COM_Outptr_ T** ppService)
    {
        IFCPTR_ASSERT(pObject);
        IFCPTR_ASSERT(ppService);
        *ppService = nullptr;

        while (pObject != nullptr)
        {
            foundation::ComPtr<foundation::IServiceProvider> service_provider_ptr;
            if (SUCCEEDED(foundation::QueryInterface(pObject, service_provider_ptr.GetAddressOf()))
                && SUCCEEDED(service_provider_ptr->QueryService(guidService, T::GetIID(), (void **)ppService)))
            {
                return S_OK;
            }
            foundation::ComPtr<IObject> parent_ptr;
            IFR_ASSERT(GetObjectParent(pObject,parent_ptr.GetAddressOf()));
            pObject = parent_ptr;
        }
        *ppService = nullptr;
        return E_NOTIMPL;
    }

}

