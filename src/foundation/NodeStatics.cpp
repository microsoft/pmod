/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NodeStatics.cpp
****/

#include "pch.h"

#include "NodeStatics.h"
#include <foundation/type_info_statics_util.h>
#include <foundation/library/activation_factory.h>

#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;

// {7956BB87-46D2-438B-8B73-904E34BC2B84}
const GUID foundation::IID_INodeStatics =
{ 0x7956bb87, 0x46d2, 0x438b,{ 0x8b, 0x73, 0x90, 0x4e, 0x34, 0xbc, 0x2b, 0x84 } };

INodeStatics *CNodeStatics::GetInstance()
{
    return library::_GetFactoryInstance<CNodeStatics, INodeStatics>();
}

STDMETHODIMP CNodeStatics::GetObjectParent(
    _In_ IObject *pObject,
    _COM_Outptr_result_maybenull_ IObject **ppParent)
{
    IFCPTR_ASSERT(pObject);
    IFCPTR_ASSERT(ppParent);

    return GetObjectParentInternal(pObject, ppParent);
}

STDMETHODIMP CNodeStatics::GetObjectAncestor(
    _In_ IObject *pObject,
    REFIID iidAncestorType,
    _COM_Outptr_result_maybenull_ IObject **ppAncestor)
{
    IFCPTR_ASSERT(pObject);
    IFCPTR_ASSERT(ppAncestor);

    return GetAncestorInternal(pObject, iidAncestorType, ppAncestor);
}

STDMETHODIMP CNodeStatics::GetObjectRoot(
    _In_ IObject *pObject,
    IObject **ppRoot)
{
    IFCPTR_ASSERT(pObject);
    IFCPTR_ASSERT(ppRoot);

    return GetRootInternal(pObject, ppRoot);
}

HRESULT CNodeStatics::GetObjectParentInternal(
    _In_ foundation::IObject *pObject,
    _COM_Outptr_result_maybenull_ foundation::IObject **ppParent)
{
    foundation_assert(pObject);
    IFCPTR_COM_OUT_PTR_ASSERT(ppParent);

    foundation::_IObjectNode *pObjectNode = GetObjectNode(pObject);
    if (pObjectNode != nullptr)
    {
        return pObjectNode->GetParent(ppParent);
    }
    *ppParent = nullptr;
    return S_OK;
}

HRESULT CNodeStatics::GetAncestorInternal(
    _In_ IObject *pObject,
    REFIID iidAncestorType,
    _COM_Outptr_result_maybenull_ IObject **ppAncestor)
{
    foundation_assert(pObject);
    foundation_assert(ppAncestor);

    *ppAncestor = nullptr;

    foundation::ComPtr<IObject> spNextParent;
    IFR_ASSERT(GetObjectParentInternal(pObject,spNextParent.GetAddressOf()));
    while (spNextParent)
    {
        bool result = FALSE;
        IFR_ASSERT(IsInstanceOfType(spNextParent, iidAncestorType, result));

        if (result)
        {
            // ancestor found
            *ppAncestor = spNextParent.Detach();
            break;
        }
        // next Parent to found
        IObject *pTemp = spNextParent.Get();
        IFR_ASSERT(GetObjectParentInternal(pTemp, spNextParent.ReleaseAndGetAddressOf()));
    }
    return S_OK;
}

HRESULT CNodeStatics::GetRootInternal(
    _In_ IObject *pObject,
    IObject **ppRoot)
{
    foundation_assert(pObject);
    IFCPTR_COM_OUT_PTR_ASSERT(ppRoot);

    // the final calculated Root
    foundation::ComPtr<IObject> spRoot;

    foundation::ComPtr<IObject> spNextParent;
    IFR_ASSERT(GetObjectParentInternal(pObject, spNextParent.GetAddressOf()));
    if (spNextParent == nullptr)
    {
        // Root is first passed Model
        spRoot = pObject;
    }
    else
    {
        while (spNextParent)
        {
            spRoot = spNextParent;
            // next Parent to found
            foundation::ComPtr<IObject> spParent;
            IFR_ASSERT(GetObjectParentInternal(spNextParent, spParent.GetAddressOf()));
            if (spParent == nullptr)
            {
                break;
            }
            spNextParent = spParent;
        }
    }
    return spRoot.CopyTo(ppRoot);
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.NodeStatics"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CNodeStatics::GetInstance)
    );
