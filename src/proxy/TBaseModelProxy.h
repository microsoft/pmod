/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TBaseModelProxy.h
****/
#pragma once

#include "TBaseObjectProxy.h"
#include <foundation/interfaces/node.h>
#include <foundation/node_statics_util.h>

//------------------------------------------------------------------------------
// Class:   TBaseModelProxy Template
//
// Purpose: Define a base class for all the Model based proxy we will support
//
//------------------------------------------------------------------------------
template
<
    class Model,
    class TEventSource,
    class TEventHandler,
    class TEventArgs,
    class TProxyModelDelegate,
    class TProxyEventSourceImpl = TNotifyEventSourceImpl<TEventSource, TEventHandler, foundation::IUnknown, TEventArgs, BaseObjectProxy, foundation::library::ICriticalSection>,
    class TSourceModel = Model
>
class TBaseModelProxy :
    public TBaseObjectProxy<Model, TEventSource, TEventHandler, TEventArgs, TProxyModelDelegate, TProxyEventSourceImpl, TSourceModel>,
    public foundation::_IObjectNode
{
private:
    typedef TBaseObjectProxy<Model, TEventSource, TEventHandler, TEventArgs, TProxyModelDelegate, TProxyEventSourceImpl, TSourceModel> _Base_Type;
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::_IObjectNode::GetIID())
        {
            *ppInterface = static_cast<foundation::_IObjectNode *>(this);
        }       
        else
        {
            return _Base_Type::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // _IObjectNode Interface
    STDMETHOD(GetParent)(foundation::IObject **ppModel) override
    {
        IFCPTR(ppModel);
        IFR(_Base_Type::VerifyModelSource());

        //*ppModel = nullptr;
        //foundation::ComPtr<foundation::IObject> spModel;
        //IFR(foundation::library_util::GetObjectParent(this->m_spObjectSource, spModel.GetAddressOf()));
        //if(spModel != nullptr)
        //{
        //    // proxy the parent first
        //    foundation::ComPtr<foundation::IObject> spProxyParent;
        //    IFR_ASSERT(_Base_Type::ToProxyObjectInternal(spModel, spProxyParent.GetAddressOf()));
        //    return foundation::QueryInterface(spProxyParent, ppModel);
        //}
        return S_OK;
    }

    STDMETHOD (GetChilds)(UINT32 *size, foundation::IObject ***ppChilds) override
    {
        IFCPTR(size);
        IFCPTR(ppChilds);
        IFR(_Base_Type::VerifyModelSource());


        //foundation::ComPtr<foundation::_IObjectNode> spSourceObjectNode;
        //foundation::QueryInterface(this->m_spObjectSource, spSourceObjectNode.GetAddressOf());

        //std::vector< foundation::ComPtr<foundation::IObject> > proxyChilds;
        //if (spSourceObjectNode != nullptr)
        //{
        //    foundation::ArrayRefCountWrapper<foundation::IObject> childs;
        //    IFR(spSourceObjectNode->GetChilds(childs.GetSizeAddressOf(), childs.GetBufferAddressOf()));

        //    proxyChilds.resize(childs.size());

        //    for (UINT32 index = 0; index < childs.size(); ++index)
        //    {
        //        foundation::ComPtr<foundation::IObject> spProxyObject;
        //        IFR(_Base_Type::ToProxyObjectInternal(childs[index], spProxyObject.GetAddressOf()));

        //        IFR_ASSERT(foundation::QueryInterface(spProxyObject, proxyChilds[index].GetAddressOf()));
        //    }
        //}
        //ContainerRefCountToArray(proxyChilds,size,ppChilds);
        return S_OK;
    }
};
