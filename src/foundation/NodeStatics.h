/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NodeStatics.h
****/

#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/node_statics_util.h>

//------------------------------------------------------------------------------
// Class:   CNodeStatics
//
// Purpose: Node statics class
//
//------------------------------------------------------------------------------

class CNodeStatics :
    public foundation::ctl::ComInspectableBase,
    public foundation::INodeStatics
{
public:
    static foundation::INodeStatics *GetInstance();
    STDMETHOD(GetObjectParent)(
        _In_ foundation::IObject *pObject,
        _COM_Outptr_result_maybenull_ foundation::IObject **ppParent) override;

    STDMETHOD(GetObjectAncestor)(
        _In_ foundation::IObject *pObject,
        REFIID iidAncestorType,
        _COM_Outptr_result_maybenull_ foundation::IObject **ppAncestor) override;
    STDMETHOD(GetObjectRoot)(
        _In_ foundation::IObject *pObject,
        foundation::IObject **ppRoot) override;

    static HRESULT GetAncestorInternal(
        _In_ foundation::IObject *pObject,
        REFIID iidAncestorType,
        _COM_Outptr_result_maybenull_ foundation::IObject **ppAncestor) ;

    static HRESULT GetObjectParentInternal(
        _In_ foundation::IObject *pObject,
        _COM_Outptr_result_maybenull_ foundation::IObject **ppParent);

    static HRESULT GetRootInternal(
        _In_ foundation::IObject *pObject,
        foundation::IObject **ppRoot);
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::INodeStatics::GetIID())
        {
            *ppInterface = static_cast<foundation::INodeStatics *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

private:
    static foundation::_IObjectNode *GetObjectNode(_In_ foundation::IObject *pObject)
    {
        return foundation::CastTo<foundation::_IObjectNode>(pObject);
    }
};



