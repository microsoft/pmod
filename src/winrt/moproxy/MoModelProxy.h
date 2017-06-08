/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelProxy.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

#include "MoAdapterProxyInterface.h"
#include "FreeThreadedMarshalerBase.h"

#include <foundation/ctl/com_library.h>
#include <foundation/interfaces/object_serializer.h>
#include <pmod/interfaces.h>
#include <foundation/com_ptr.h>
#include "MoCoreProxyFactory.h"
#include <foundation/library/base_adapter_host.h>

#include <vector>
#include <map>

template <class TAdapterInterface, class TSourceInterface,class TBASE = foundation::ctl::ComInspectableBase>
class TFreeThreadedAdapterProxyInterface :
    public _TFreeThreadedMarshalerComBase < TAdapterProxyInterface<TAdapterInterface, TSourceInterface, TBASE> >
{
};

template <class TCoreInterface, class TMoInterface, class THostInterface = TCoreInterface>
class TMoObjectProxy :
    public foundation::library::_DefaultAdapterHost
    <
        THostInterface,
        TMoAdapterProxyInterfaceWithBase
        <
            TCoreInterface,
            foundation::IObject,
            TMoInterface,
            ABI::Microsoft::PropertyModel::IObject
        >
    >,
    public foundation::library::IEventSourceClass,
    public foundation::IObjectSerializer,
	public foundation::_IObjectNode
{
protected:
    // Core IObject  interface
    STDMETHOD(GetUniqueId)(UINT32 *uniqueId)
    {
        foundation_assert(m_pMoBaseInterface);
        return m_pMoBaseInterface->get_UniqueId(uniqueId);
    }
    STDMETHOD(GetProcessId)(UINT32 *processId)
    {
        foundation_assert(m_pMoBaseInterface);
        return m_pMoBaseInterface->get_ProcessId(processId);
    }

    STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppModelTypeInfo)
    {
        foundation_assert(m_pMoBaseInterface);
        foundation::ComPtr<ABI::Microsoft::PropertyModel::IObjectTypeInfo> spMoModelTypeInfo;
        IFR(m_pMoBaseInterface->get_ObjectTypeInfo(spMoModelTypeInfo.GetAddressOf()));
        if (spMoModelTypeInfo != nullptr)
        {
            return CMoCoreProxyFactory::ToTypeInfoProxy(spMoModelTypeInfo, ppModelTypeInfo);
        }
        return S_OK;
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IEventSourceClass::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IEventSourceClass *>(this);
        }
        else if (iid == foundation::IObjectSerializer::GetIID())
        {
            *ppInterface = static_cast<foundation::IObjectSerializer *>(this);
        }
		else if (iid == foundation::_IObjectNode::GetIID())
		{
			*ppInterface = static_cast<foundation::_IObjectNode *>(this);
		}
		else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // Interface foundation::library::IEventSourceClass
    STDMETHOD(AddEventHandler)(
        _In_ GUID eventSourceType,
        _In_opt_ foundation::IUnknown *pEventHandler,
        _In_opt_ foundation::IInspectable *pOptions,
        _COM_Outptr_ foundation::IInspectable **ppResult) override
    {
        foundation::ComPtr<ABI::Microsoft::PropertyModel::IEventSourceClass> spMoEventSourceClass;
        IFR_ASSERT(foundation::QueryInterface(m_pMoBaseInterface, spMoEventSourceClass.GetAddressOf()));

        foundation::ComPtr<ABI::Microsoft::PropertyModel::IDispatchable> spMoDispatchable;
        if (pEventHandler != nullptr)
        {
            _IFR_(this->GetMoEventHandler(eventSourceType, pEventHandler, spMoDispatchable.GetAddressOf()));
        }
        
        foundation::InspectablePtr spResult;
        IFR_ASSERT(spMoEventSourceClass->AddEventHandler(
            eventSourceType, 
            spMoDispatchable, 
            pOptions, 
            spResult.GetAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spResult, ppResult));

        if (pEventHandler != nullptr)
        {
            _moEventHandlerMap[pEventHandler] = spMoDispatchable;
        }
        return S_OK;
    }

    STDMETHOD(RemoveEventHandler)(
        _In_ GUID eventSourceType,
        _In_ foundation::IUnknown *pEventHandler) override
    {
        _MoEventHandler_Type::const_iterator iter = _moEventHandlerMap.find(pEventHandler);
        if (iter != _moEventHandlerMap.end())
        {
            foundation::ComPtr<ABI::Microsoft::PropertyModel::IEventSourceClass> spMoEventSourceClass;
            IFR_ASSERT(foundation::QueryInterface(m_pMoBaseInterface, spMoEventSourceClass.GetAddressOf()));
            IFR(spMoEventSourceClass->RemoveEventHandler(eventSourceType, (*iter).second));
            _moEventHandlerMap.erase(iter);
        }
        return S_OK;
    }

    virtual HRESULT GetMoEventHandler(
        _In_ GUID eventSourceType,
        foundation::IUnknown *pEventHandler,
        ABI::Microsoft::PropertyModel::IDispatchable **ppMoDispatchable)
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }

    // Interface IObjectSerializer
    STDMETHOD(GetContent)(HSTRING serializerActivationId, IInspectable *pContentOptions, IInspectable **ppSerializedContent) override
    {
        PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_Object_GetContent_Id)

        foundation::ComPtr<ABI::Microsoft::PropertyModel::IObjectSerializer> spMoObjectSerializer;
        IFR_ASSERT(foundation::QueryInterface(m_pMoBaseInterface, spMoObjectSerializer.GetAddressOf()));
        return spMoObjectSerializer->GetContent(serializerActivationId, pContentOptions, ppSerializedContent);
    }

    STDMETHOD(SetContent)(HSTRING serializerActivationId, IInspectable *pSerializedContent) override
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }
	// IObjectNode  interface
	STDMETHOD(GetParent)(foundation::IObject **ppModel)
	{
		foundation::ComPtr<ABI::Microsoft::PropertyModel::IObjectNode> spMoObjectNode;
		foundation::QueryInterface(m_pMoBaseInterface, spMoObjectNode.GetAddressOf());
		if (spMoObjectNode == nullptr)
		{
			*ppModel = nullptr;
			return S_OK;
		}

		foundation::ComPtr<ABI::Microsoft::PropertyModel::IObject> spMoModel;
		IFR(spMoObjectNode->get_Parent(spMoModel.GetAddressOf()));
		IFR_ASSERT(CMoCoreProxyFactory::ToObjectProxy(spMoModel, (foundation::IObject **)ppModel));
		return S_OK;
	}

	STDMETHOD(GetChilds)(UINT32 *size, foundation::IObject ***ppChilds)
	{
		foundation::ComPtr<ABI::Microsoft::PropertyModel::IObjectNode> spMoObjectNode;
		foundation::QueryInterface(m_pMoBaseInterface, spMoObjectNode.GetAddressOf());
		if (spMoObjectNode == nullptr)
		{
			*size = 0;
			*ppChilds = nullptr;
			return S_OK;
		}

		foundation::ArrayRefCountWrapper<ABI::Microsoft::PropertyModel::IObject> childsArrayWrapper;
		IFR(spMoObjectNode->GetChilds(
			childsArrayWrapper.GetSizeAddressOf(), childsArrayWrapper.GetBufferAddressOf()));

		std::vector< foundation::ComPtr<foundation::IObject> > proxyChilds;
		for (UINT32 index = 0; index < childsArrayWrapper.GetSize(); ++index)
		{
			foundation::ComPtr<foundation::IObject> spProxyModel;
			IFR(CMoCoreProxyFactory::ToObjectProxy(
				childsArrayWrapper[index],
				(foundation::IObject **)spProxyModel.GetAddressOf()));
			proxyChilds.push_back(spProxyModel);
		}
		ContainerRefCountToArray(proxyChilds, size, ppChilds);
		return S_OK;
	}
private:
    typedef std::map<
        foundation::ComPtr<foundation::IUnknown>, 
        foundation::ComPtr<ABI::Microsoft::PropertyModel::IDispatchable>
    > _MoEventHandler_Type;

    _MoEventHandler_Type _moEventHandlerMap;
};
