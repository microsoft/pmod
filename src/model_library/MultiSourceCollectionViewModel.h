/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MultiSourceCollectionViewModel.h
****/
#pragma once

#include "MultiSourceModelBase.h"
#include "ViewModelBase.h"
#include "ObservableCollection.h"
#include "CollectionViewModel.h"

class CMultiSourceCollectionViewModel;
//------------------------------------------------------------------------------
// Class:   CMultiSourceCollectionInternal
//
// Purpose: A simple multi-source collection view model
//
//------------------------------------------------------------------------------
typedef    
    _CViewModelSourceBase
    <
        CObservableCollection,
        CMultiSourceModelBase<CMultiSourceCollectionModelContainer>,
        pmod::INotifyCollectionChangedEventHandler
    > _MultiSourceCollectionInternalBaseType;

class CMultiSourceCollectionInternal :
    public _MultiSourceCollectionInternalBaseType,
    private pmod::NotifyCollectionChangedHelper
{
public:
    void SetMultiSourceCollectionViewModelDelegate(pmod::library::IMultiSourceCollectionViewModelDelegate *m_p_multi_source_CollectionViewModelDelegate)
    {
        m_spMultiSourceCollectionViewModelDelegate = m_p_multi_source_CollectionViewModelDelegate;
    }
protected:
    // Overrides from CMultiSourceModelBase
    HRESULT on_source_changed_internal(pmod::IObservableCollection *pSender, pmod::INotifyCollectionChangedEventArgs *pEventArgs) override;

    void ForceRemoveAttachedSources() override;
    HRESULT AttachToSource() override;
    HRESULT DetachFromSource() override;
	HRESULT AttachToSourceInternal(pmod::IObservableCollection *pSource) override;
	HRESULT DetachFromSourceInternal(pmod::IObservableCollection *pSource) override;

    friend class CMultiSourceCollectionViewModel;
private:
    // Override NotifyCollectionChangedHelper
    HRESULT OnCollectionItemAdd(
            _In_ pmod::IObservableCollection* pSender,
            _In_ foundation::IInspectable *item,
            _In_ UINT32 nNewStartIndex,
            _In_ bool fItemMove) override;

    HRESULT OnCollectionItemRemove(
            _In_ pmod::IObservableCollection* pSender,
            _In_ foundation::IInspectable *item,
            _In_ UINT32 nOldStartIndex,
            _In_ bool fItemMove) override;

    HRESULT OnCollectionReset(
        _In_ pmod::IObservableCollection* pSender,
        _In_ const foundation::InspectableArrayWrapper& oldItems,
        _In_ const foundation::InspectableArrayWrapper& newItems) override;

    HRESULT OnCollectionItemReplaced(
        _In_ pmod::IObservableCollection* pSender,
        _In_ foundation::IInspectable *oldItem,
        _In_ foundation::IInspectable *newItem,
        _In_ UINT32 nStartIndex) override;


    HRESULT AddSourceItems(pmod::IObservableCollection *pSource);

    HRESULT RefreshSourceItems(bool fireNotify = true);
private:
    foundation::ComPtr<pmod::library::IMultiSourceCollectionViewModelDelegate> m_spMultiSourceCollectionViewModelDelegate;
};

//------------------------------------------------------------------------------
// Class:   CMultiSourceCollectionViewModel
//
// Purpose: A multi-source collection view model
//
//------------------------------------------------------------------------------
typedef MultiSourceModelImpl
    <
        CCollectionViewModel,
        pmod::IObservableCollection
    > _MultiSourceCollectionViewModelBaseType;

class CMultiSourceCollectionViewModel :
    public _MultiSourceCollectionViewModelBaseType
{
 public:
     HRESULT _Initialize(const pmod::library::MultiSourceCollectionViewModelCreateParameters *modelImpl);

    static HRESULT CreateInstance(
        _In_ const pmod::library::MultiSourceCollectionViewModelCreateParameters *pModelImpl,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance);

protected:
    CMultiSourceCollectionViewModel()
    {
    }

    // Override from ComBase
    void OnFinalRelease() override;
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;

    // Override from MultiSourceModelImpl
    HRESULT AddSourceInternal(pmod::IObservableCollection *pModel) override;
    HRESULT RemoveSourceInternal(pmod::IObservableCollection *pModel) override;
    HRESULT GetSourcesInternal(UINT32 *size,pmod::IObservableCollection ***ppModel) override;
    HRESULT RemoveAllSourcesInternal() override;

private:
    foundation::ComPtr<foundation::ctl::ComInspectableObject<CMultiSourceCollectionInternal> > m_spMultiSourceCollectionInternal;
};
