/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreProxyFactory.cpp
****/
#include "pch.h"

#include "MoCoreProxyFactory.h"
#include "MoObservableObjectProxy.h"
#include "MoCommandProxy.h"
#include "MoObservableCollectionProxy.h"
#include "MoAsyncOperationProxy.h"
#include "MoEnumValueProxy.h"
#include "MoDictionaryProxy.h"
#include <pmod/library/library_util.h>

using namespace ABI::Microsoft::PropertyModel;

// Private export
EXTERN_C __declspec(dllexport) HRESULT  STDAPICALLTYPE ToObjectProxy(
        IInspectable *pMoObject,
        foundation::IObject **ppFoundationObject)
{
    return CMoCoreProxyFactory::ToObjectProxy(pMoObject, ppFoundationObject);
}

HRESULT CMoCoreProxyFactory::ToTypeInfoProxy(
        IObjectTypeInfo *pMoModelTypeInfo,
        foundation::IObjectTypeInfo **ppCoreModelTypeInfo)
{
    foundation_assert(pMoModelTypeInfo);

    IID iidType;
    IFR(pMoModelTypeInfo->get_IIdType(&iidType));

    // Note: the implementation of the Proxy require the Type to be registered in the existing running process
    return foundation::GetTypeInfoStatics()->GetTypeInfo(iidType,ppCoreModelTypeInfo);
}

HRESULT CMoCoreProxyFactory::ToPropertyDataProxy(
	ABI::Microsoft::PropertyModel::IObjectDispatch *pMoPropertyData,
	foundation::IObjectDispatch **ppPropertyData)
{
	return CMoPropertyDataProxy::CreateInstance<CMoPropertyDataProxy>(pMoPropertyData, ppPropertyData);
}
HRESULT CMoCoreProxyFactory::ToObservableObjectProxy(
        IObservableObject *pMoPropertyModel,
        pmod::IObservableObject **ppCorePropertyModel)
{
    return CMoObservableObjectProxy::CreateInstance<CMoObservableObjectProxy>(pMoPropertyModel,ppCorePropertyModel);
}

HRESULT CMoCoreProxyFactory::ToCommandProxy(
        ICommand *pMoCommandModel,
        pmod::ICommand **ppCoreCommandModel)
{
    return CMoCommandModelProxy::CreateInstance<CMoCommandModelProxy>(pMoCommandModel,ppCoreCommandModel);
}

HRESULT CMoCoreProxyFactory::ToObservableCollectionProxy(
        ABI::Microsoft::PropertyModel::IObservableCollection *pMoCollectionModel,
        pmod::IObservableList **ppCoreCollectionModel)
{
    return CMoObservableCollectionProxy::CreateInstance<CMoObservableCollectionProxy>(pMoCollectionModel,ppCoreCollectionModel);
}

HRESULT CMoCoreProxyFactory::ToAsyncOperationProxy(
        ABI::Microsoft::PropertyModel::IAsyncOperation *pMoAsyncOperation,
        foundation::IAsyncOperation **ppCoreAsyncOperation)
{
    return CMoAsyncOperationProxy::CreateInstance<CMoAsyncOperationProxy>(
        pMoAsyncOperation, ppCoreAsyncOperation);
}

HRESULT CMoCoreProxyFactory::ToObjectProxy(
        IInspectable *pMoObject,
        foundation::IObject **ppFoundationObject)
{
    foundation_assert(ppFoundationObject);
    if (pMoObject == nullptr)
    {
        *ppFoundationObject = nullptr;
        return S_OK;
    }
    return ToObjectProxyInternal(pMoObject, ppFoundationObject);
}

HRESULT CMoCoreProxyFactory::ToObjectProxyInternal(
    IInspectable *pInspectable,
    foundation::IObject **ppFoundationObject)
{
    foundation_assert(pInspectable);

	foundation::ComPtr<IObjectDispatch> spMoPropertyData;
	foundation::ComPtr<IObservableObject> spMoPropertyModel;
    foundation::ComPtr<ICommand> spMoCommandModel;
    foundation::ComPtr<IObservableCollection> spMoCollectionModel;
    foundation::ComPtr<IAsyncOperation> spMoAsyncOperation;
	foundation::ComPtr<IObjectNode> spMoObjectNode;

    foundation::ComPtr<IEnumValue> spMoEnumValue;
    if (SUCCEEDED(foundation::QueryInterface(pInspectable, spMoEnumValue.GetAddressOf())))
    {
        // Note: we will retrive the registered EnumValue from the foundation library
        // instead of using the still supported CMoEnumValueProxy
        foundation::ComPtr<ABI::Microsoft::PropertyModel::IObject> spAsObject;
        IFR_ASSERT(foundation::QueryInterface(spMoEnumValue, spAsObject.GetAddressOf()));

        foundation::ComPtr<ABI::Microsoft::PropertyModel::IObjectTypeInfo> spObjectTypeInfo;
        IFR_ASSERT(spAsObject->get_ObjectTypeInfo(spObjectTypeInfo.GetAddressOf()));

        UINT32 enumTypeId;
        IFR_ASSERT(spObjectTypeInfo->get_TypeId(&enumTypeId));
        UINT32 enumValue;
        IFR_ASSERT(spMoEnumValue->get_Value(&enumValue));

        IFR_ASSERT(foundation::GetEnumValue(enumTypeId, enumValue, reinterpret_cast<foundation::IEnumValue **>(ppFoundationObject)));
    }
    else if (SUCCEEDED(foundation::QueryInterface(pInspectable, spMoCommandModel.GetAddressOf())))
    {
        IFR_ASSERT(ToCommandProxy(spMoCommandModel, (pmod::ICommand **)ppFoundationObject));
    }
    else if(SUCCEEDED(foundation::QueryInterface(pInspectable,spMoCollectionModel.GetAddressOf())))
    {
        IFR_ASSERT(ToObservableCollectionProxy(spMoCollectionModel, (pmod::IObservableList **)ppFoundationObject));
    }
    else if (SUCCEEDED(foundation::QueryInterface(pInspectable, spMoAsyncOperation.GetAddressOf())))
    {
        IFR_ASSERT(ToAsyncOperationProxy(spMoAsyncOperation, (foundation::IAsyncOperation **)ppFoundationObject));
    }
    else if(SUCCEEDED(foundation::QueryInterface(pInspectable,spMoPropertyModel.GetAddressOf())))
    {
        IFR_ASSERT(ToObservableObjectProxy(spMoPropertyModel, (pmod::IObservableObject **)ppFoundationObject));
    }
	else if (SUCCEEDED(foundation::QueryInterface(pInspectable, spMoPropertyData.GetAddressOf())))
	{
		IFR_ASSERT(ToPropertyDataProxy(spMoPropertyData, (foundation::IObjectDispatch **)ppFoundationObject));
	}
	else
    {
        return E_NOINTERFACE;
    }
    return S_OK;
}

HRESULT CMoCoreProxyFactory::ToProxy(
    IInspectable *pInspectable,
    IInspectable **ppInspectable)
{
    return ToObjectProxyInternal(true,pInspectable,ppInspectable);
}

HRESULT CMoCoreProxyFactory::ToProxyIf(
    IInspectable *pInspectable,
    IInspectable **ppInspectable)
{
    return ToObjectProxyInternal(false, pInspectable, ppInspectable);
}

HRESULT CMoCoreProxyFactory::ToObjectProxyInternal(
    bool force,
    IInspectable *pInspectable,
    IInspectable **ppInspectable)
{
    foundation_assert(ppInspectable);
    if(pInspectable == nullptr)
    {
        *ppInspectable = nullptr;
        return S_OK;
    }

    foundation::ComPtr<foundation::IObject> spObjectCoreProxy;
    if(!force && 
        SUCCEEDED(foundation::QueryInterface(pInspectable, spObjectCoreProxy.GetAddressOf()))
    )
    {
        *ppInspectable = pInspectable;
        pInspectable->AddRef();
        return S_OK;
    }

    foundation::ComPtr<IDictionary> spMoDictionary;
    if (SUCCEEDED(ToObjectProxyInternal(pInspectable, spObjectCoreProxy.GetAddressOf())))
    {
        *ppInspectable = spObjectCoreProxy.Detach();
    }
    else if (SUCCEEDED(foundation::QueryInterface(pInspectable, spMoDictionary.GetAddressOf())))
    {
        return CMoDictionaryProxy::CreateInstance<CMoDictionaryProxy>(
            spMoDictionary, 
            (foundation::IDictionary **)ppInspectable);
    }
    else
    {
        foundation::PropertyType propertyType;
        IGNOREHR(foundation::pv_util::GetType(pInspectable, &propertyType));

        if (propertyType == foundation::PropertyType_InspectableArray)
        {
            foundation::InspectableArrayWrapper objectArrayWrapper;
            IFR_ASSERT(foundation::pv_util::GetInspectableArray(
                pInspectable,
                objectArrayWrapper.GetSizeAddressOf(),
                objectArrayWrapper.GetBufferAddressOf()));

            foundation::InspectableArrayWrapper proxyObjectArrayWrapper;
            IFR_ASSERT(ToProxyArrayInternal(
                force,
                objectArrayWrapper.GetSize(),
                objectArrayWrapper.GetRawBuffer(),
                proxyObjectArrayWrapper.GetSizeAddressOf(),
                proxyObjectArrayWrapper.GetBufferAddressOf()));

            IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
                proxyObjectArrayWrapper.GetSize(),
                proxyObjectArrayWrapper.GetRawBuffer(),
                ppInspectable));
        }
        else
        {
            // pass trough (could IPropertyValue or another unsupported IInspectable
            *ppInspectable = pInspectable;
            pInspectable->AddRef();
        }
    }
    return S_OK;
}

HRESULT CMoCoreProxyFactory::ToProxyArray(
    _In_ UINT32 size, IInspectable **pInspectableArray,
    _Out_ UINT32* pSize, IInspectable ***ppInspectableArray)
{
    return ToProxyArrayInternal(
        true,
        size, pInspectableArray,
        pSize, ppInspectableArray);
}

HRESULT CMoCoreProxyFactory::ToProxyArrayIf(
    _In_ UINT32 size, IInspectable **pInspectableArray,
    _Out_ UINT32* pSize, IInspectable ***ppInspectableArray)
{
    return ToProxyArrayInternal(
        false,
        size, pInspectableArray,
        pSize, ppInspectableArray);
}
HRESULT CMoCoreProxyFactory::ToProxyArrayInternal(
    bool force,
    _In_ UINT32 size, IInspectable **pInspectableArray,
    _Out_ UINT32* pSize,IInspectable ***ppInspectableArray)
{
    std::vector< foundation::InspectablePtr > coreProxies;
    coreProxies.resize(size);
    for(UINT32 index = 0;index < size;++index)
    {
        IFR_ASSERT(ToObjectProxyInternal(force,pInspectableArray[index], coreProxies[index].GetAddressOf()));
    }
    ContainerRefCountToArray(coreProxies,pSize,ppInspectableArray);
    return S_OK;
}

HRESULT CMoCoreProxyFactory::FromProxyArray(
                _In_ UINT32 size,IInspectable **pInspectableArray,
                _Out_ UINT32* pSize,IInspectable ***ppInspectableArray)
{
    std::vector< foundation::InspectablePtr > values;
    values.resize(size);
    for(UINT32 index = 0;index < size;++index)
    {
        IFR_ASSERT(FromProxy(pInspectableArray[index],values[index].GetAddressOf()));
    }
    ContainerRefCountToArray(values,pSize,ppInspectableArray);
    return S_OK;
}

HRESULT CMoCoreProxyFactory::FromProxy(IInspectable *pInspectable,IInspectable **ppInspectable)
{
    foundation_assert(ppInspectable);
    if(pInspectable == nullptr)
    {
        *ppInspectable = nullptr;
        return S_OK;
    }

    foundation::ComPtr<IAdapterProxyInterface> spAdapterProxyInterface;
    if(SUCCEEDED(foundation::QueryInterface(pInspectable,spAdapterProxyInterface.GetAddressOf())))
    {
        IFR_ASSERT(spAdapterProxyInterface->GetSourceInterface((IUnknown **)ppInspectable));
    }
    else
    {
        *ppInspectable = pInspectable;
        pInspectable->AddRef();
    }
    return S_OK;
}
