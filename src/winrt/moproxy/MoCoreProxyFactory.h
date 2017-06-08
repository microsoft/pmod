/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreProxyFactory.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include <pmod/interfaces.h>

#include <foundation/library/perf_counter_util.h>
#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#endif

class CMoCoreProxyFactory
{
public:
    static HRESULT ToObjectProxy(
        IInspectable *pMoModel,
        foundation::IObject **ppFoundationObject);

	static HRESULT ToPropertyDataProxy(
		ABI::Microsoft::PropertyModel::IObjectDispatch *pMoPropertyModel,
		foundation::IObjectDispatch **ppPropertyData);

	static HRESULT ToObservableObjectProxy(
        ABI::Microsoft::PropertyModel::IObservableObject *pMoPropertyModel,
        pmod::IObservableObject **ppCorePropertyModel);

    static HRESULT ToCommandProxy(
        ABI::Microsoft::PropertyModel::ICommand *pMoCommandModel,
        pmod::ICommand **ppCoreCommandModel);

    static HRESULT ToObservableCollectionProxy(
        ABI::Microsoft::PropertyModel::IObservableCollection *pMoCollectionModel,
        pmod::IObservableList **ppCoreCollectionModel);

    static HRESULT ToAsyncOperationProxy(
        ABI::Microsoft::PropertyModel::IAsyncOperation *pMoAsyncOperation,
        foundation::IAsyncOperation **ppFoundationAsyncOperation);

    static HRESULT ToTypeInfoProxy(
        ABI::Microsoft::PropertyModel::IObjectTypeInfo *pMoModelTypeInfo,
        foundation::IObjectTypeInfo **ppCoreModelTypeInfo);

    static HRESULT ToProxy(IInspectable *pInspectable,IInspectable **ppInspectable);
    static HRESULT FromProxy(IInspectable *pInspectable,IInspectable **ppInspectable);


    static HRESULT ToProxyArray(
                _In_ UINT32 size,IInspectable **pInspectableArray,
                _Out_ UINT32* pSize,IInspectable ***ppInspectableArray);
    static HRESULT FromProxyArray(
                _In_ UINT32 size,IInspectable **pInspectableArray,
                _Out_ UINT32* pSize,IInspectable ***ppInspectableArray);

    static HRESULT ToProxyIf(IInspectable *pInspectable, IInspectable **ppInspectable);
    static HRESULT ToProxyArrayIf(
        _In_ UINT32 size, IInspectable **pInspectableArray,
        _Out_ UINT32* pSize, IInspectable ***ppInspectableArray);

private:
    static HRESULT ToProxyArrayInternal(
        bool force,
        _In_ UINT32 size, IInspectable **pInspectableArray,
        _Out_ UINT32* pSize, IInspectable ***ppInspectableArray);
    static HRESULT ToObjectProxyInternal(bool force, IInspectable *pInspectable, IInspectable **ppInspectable);
    static HRESULT ToObjectProxyInternal(IInspectable *pInspectable, foundation::IObject **ppFoundationObject);
};
