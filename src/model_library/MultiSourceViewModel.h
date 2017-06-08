/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MultiSourceViewModel.h
****/
#pragma once

#include "MultiSourceModelBase.h"
#include "ViewModelBase.h"
#include "ObservableObject.h"

//------------------------------------------------------------------------------
// Class:   CMultiSourceViewModel
//
// Purpose: A simple multi-source Property view model
//
//------------------------------------------------------------------------------
typedef MultiSourceModelImpl
    <
        CViewModelSourceBase
        <
            CObservableObject,
            CMultiSourceModelBase<CMultiSourcePropertyModelContainer>,
            pmod::IPropertyChangedEventHandler,
            pmod::library::IMultiSourceViewModelDelegate
        >,
        pmod::IObservableObject
    > _MultiSourcePropertyViewModelBaseType;

class CMultiSourceViewModel :
    public _MultiSourcePropertyViewModelBaseType
{
public:
 public:
     HRESULT _Initialize(const pmod::library::MultiSourceViewModelCreateParameters *modelImpl);

    static HRESULT CreateInstance(
        _In_ const pmod::library::MultiSourceViewModelCreateParameters *pModelImpl,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance);

protected:
    // Override from ComBase
    void OnFinalRelease() override;
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;

    // Override from MultiSourceModelImpl
    HRESULT AddSourceInternal(pmod::IObservableObject *pModel) override;
    HRESULT RemoveSourceInternal(pmod::IObservableObject *pModel) override;
    HRESULT GetSourcesInternal(UINT32 *size,pmod::IObservableObject ***ppModel) override;
    HRESULT RemoveAllSourcesInternal() override;

    // Override from CMultiSourceModelBase
    HRESULT on_source_changed_internal(pmod::IObservableObject *pSender, pmod::IPropertyChangedEventArgs *pEventArgs) override;

    void ForceRemoveAttachedSources() override;

};
