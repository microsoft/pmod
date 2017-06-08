/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MultiSourceCollectionViewModel.cpp
****/

#include "pch.h"

#include "MultiSourceCollectionViewModel.h"
#include "CreateFoundationHelper.h"

using namespace pmod;
using namespace foundation::library;
using namespace pmod::library;

HRESULT CMultiSourceCollectionInternal::on_source_changed_internal(IObservableCollection *pSender,INotifyCollectionChangedEventArgs *pEventArgs)
{
    IFR(HandleCollectionChanged(pSender,pEventArgs));
    if(m_spMultiSourceCollectionViewModelDelegate)
    {
        IFR(m_spMultiSourceCollectionViewModelDelegate->OnSourceCollectionChanged(pSender,pEventArgs));
    }
    return S_OK;
}

void CMultiSourceCollectionInternal::ForceRemoveAttachedSources()
{
    if(IsAttached())
    {
        // this is typicaly invoked from FinalRelease and we don't want to invoke the Delegate callback
        IGNOREHR(DetachAllSources());
    }
}

HRESULT CMultiSourceCollectionInternal::AttachToSource()
{
    IFR(_MultiSourceCollectionInternalBaseType::AttachToSource());
    // Note:
    // We don't need to explictiy notify since the attached Multi Source Collection View
    // has to be on his own AttachToSource call and will Refresh the Items anyway
    // If we pass 'true' a double notification will happen since the 'Reset' action will be triggered
    IFR(RefreshSourceItems(false));
    return S_OK;
}

HRESULT CMultiSourceCollectionInternal::DetachFromSource()
{
    IFR(_MultiSourceCollectionInternalBaseType::DetachFromSource());
    IFR(RemoveAllInternal());
    return S_OK;
}

HRESULT CMultiSourceCollectionInternal::AttachToSourceInternal(IObservableCollection *pSource)
{
    IFR(_MultiSourceCollectionInternalBaseType::AttachToSourceInternal(pSource));
    if(IsAttached())
    {
        IFR(RefreshSourceItems());
    }
    return S_OK;
}

HRESULT CMultiSourceCollectionInternal::DetachFromSourceInternal(IObservableCollection *pSource)
{
    IFR(_MultiSourceCollectionInternalBaseType::DetachFromSourceInternal(pSource));
    if(IsAttached())
    {
        IFR(RefreshSourceItems());
    }
    return S_OK;
}


HRESULT CMultiSourceCollectionInternal::RefreshSourceItems(bool fireNotify)
{
    _FoundationBaseLock csl(*this);

    IFR_ASSERT(EnsureItemsContainer());

    _Inspectable_ptr_vector_type oldItems,newItems;

    this->CopyContainerItems(oldItems);
    this->GetItemsContainer()->Clear();

    for (std::vector<foundation::ComPtr<IObservableCollection> >::const_iterator iter = GetSources().begin(); 
                    iter != GetSources().end(); 
                    ++iter)
    {
        IFR(AddSourceItems((*iter)));
    }

    this->CopyContainerItems(newItems);
    if(fireNotify)
    {
        IFR(FireResetNotifyCollectionChanged(oldItems,newItems));
    }
    return S_OK;
}

HRESULT CMultiSourceCollectionInternal::AddSourceItems(IObservableCollection *pSource)
{
    foundation::InspectableArrayWrapper items;
    IFR(pSource->GetItems(items.GetSizeAddressOf(),items.GetBufferAddressOf()));

    UINT32 nCount = items.GetSize();
    for (UINT32 i = 0; i < nCount; i++)
    {
        this->GetItemsContainer()->Append(items[i]);
    }
    return S_OK;
}


HRESULT CMultiSourceCollectionInternal::OnCollectionItemAdd(
            _In_ IObservableCollection* pSender,
            _In_ foundation::IInspectable *item,
            _In_ UINT32 nNewStartIndex,
            _In_ bool fItemMove)
{
    UNREFERENCED_PARAMETER(pSender);
    UNREFERENCED_PARAMETER(nNewStartIndex);

    VectorSourceType::const_iterator findIter = 
            std::find(
                GetSources().begin(),
                GetSources().end(),
                foundation::ComPtr<IObservableCollection>(pSender)
                );
    foundation_assert(findIter != GetSources().end());
    if(findIter != GetSources().end())
    {
        int offsetStartIndex = 0;
        for(VectorSourceType::const_iterator iter = GetSources().begin();
            iter != findIter;
            ++iter)
        {
            UINT32 count;
            IFR((*iter)->GetCount(&count));
            offsetStartIndex += count;
        }
        // now we will insert in the 'aggregated' collection using the offset
        IFR(this->InsertItemInternal(nNewStartIndex + offsetStartIndex,item));
    }
    return S_OK;
}

HRESULT CMultiSourceCollectionInternal::OnCollectionItemRemove(
            _In_ IObservableCollection* pSender,
            _In_ foundation::IInspectable *item,
            _In_ UINT32 nOldStartIndex,
            _In_ bool fItemMove) 
{
    UNREFERENCED_PARAMETER(pSender);
    UNREFERENCED_PARAMETER(nOldStartIndex);
    UNREFERENCED_PARAMETER(fItemMove);

    IFR(this->RemoveItemInternal(item,true));
    return S_OK;
}

HRESULT CMultiSourceCollectionInternal::OnCollectionReset(
        _In_ IObservableCollection* pSender,
        _In_ const foundation::InspectableArrayWrapper& oldItems,
        _In_ const foundation::InspectableArrayWrapper& newItems)
{
    UNREFERENCED_PARAMETER(pSender);
    UNREFERENCED_PARAMETER(oldItems);
    UNREFERENCED_PARAMETER(newItems);
    IFR(RefreshSourceItems());
    return S_OK;
}

HRESULT CMultiSourceCollectionInternal::OnCollectionItemReplaced(
        _In_ IObservableCollection* pSender,
        _In_ foundation::IInspectable *oldItem,
        _In_ foundation::IInspectable *newItem,
        _In_ UINT32 nStartIndex)
{
    UNREFERENCED_PARAMETER(pSender);
    UNREFERENCED_PARAMETER(oldItem);
    UNREFERENCED_PARAMETER(newItem);
    UNREFERENCED_PARAMETER(nStartIndex);

    foundation_assert(0);
    return E_NOTIMPL;
}

HRESULT CMultiSourceCollectionViewModel::_Initialize(const MultiSourceCollectionViewModelCreateParameters *multiSourceCollectionViewParam)
{
    foundation_assert(multiSourceCollectionViewParam);
    foundation::ComPtr<foundation::ctl::ComInspectableObject<CMultiSourceCollectionInternal> > spMultiSourceCollectionInternal;
    IFR(foundation::ctl::ComInspectableObject<CMultiSourceCollectionInternal>::CreateInstance(
        spMultiSourceCollectionInternal.GetAddressOf()));

    // Empty Collection param (without Delegate)
    ObservableCollectionCreateParametersBase internalCollectionParam;
    IFR(spMultiSourceCollectionInternal->_Initialize(&internalCollectionParam));

    collection_viewmodel_create_parameters viewModelCollectionParam;
    viewModelCollectionParam.m_iid_type = multiSourceCollectionViewParam->m_iid_type;
    viewModelCollectionParam.m_options = multiSourceCollectionViewParam->m_options;
    viewModelCollectionParam.m_p_delegate = multiSourceCollectionViewParam->m_p_delegate;
    viewModelCollectionParam.m_p_viewmodel_delegate = multiSourceCollectionViewParam->m_p_viewmodel_delegate;
    viewModelCollectionParam.m_view_model_options = multiSourceCollectionViewParam->m_view_model_options;
    viewModelCollectionParam.m_p_source = static_cast<IObservableList *>(spMultiSourceCollectionInternal);

    IFR(_MultiSourceCollectionViewModelBaseType::_Initialize(&viewModelCollectionParam));
    // Store Delegate
    spMultiSourceCollectionInternal->SetMultiSourceCollectionViewModelDelegate(
        multiSourceCollectionViewParam->m_p_multi_source_CollectionViewModelDelegate);
    // Store private Internal Collection
    // Note: this next sentence is more complex than needed but the compiler complain
    // when direct assignment
    m_spMultiSourceCollectionInternal.Attach(spMultiSourceCollectionInternal.Detach());
    return S_OK;
}

void CMultiSourceCollectionViewModel::OnFinalRelease()
{
    _MultiSourceCollectionViewModelBaseType::OnFinalRelease();
}

HRESULT CMultiSourceCollectionViewModel::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    if(iid == ISourceModel::GetIID())
    {
        return E_NOINTERFACE;
    }
    else if(iid == IMultiSourceModel::GetIID())
    {
        *ppInterface = static_cast<IMultiSourceModel *>(this);
    }
    else
    {
        return _MultiSourceCollectionViewModelBaseType::QueryInterfaceImpl(iid,ppInterface);
    }
    return S_OK;
}

HRESULT CMultiSourceCollectionViewModel::AddSourceInternal(IObservableCollection *pModel)
{
    foundation_assert(m_spMultiSourceCollectionInternal);
    return m_spMultiSourceCollectionInternal->AttachToSourceInternal(pModel);
}

HRESULT CMultiSourceCollectionViewModel::RemoveSourceInternal(IObservableCollection *pModel)
{
    foundation_assert(m_spMultiSourceCollectionInternal);
    return m_spMultiSourceCollectionInternal->DetachFromSourceInternal(pModel);
}

HRESULT CMultiSourceCollectionViewModel::GetSourcesInternal(UINT32 *size,pmod::IObservableCollection ***ppModel)
{
    foundation_assert(m_spMultiSourceCollectionInternal);
    ContainerRefCountToArray(m_spMultiSourceCollectionInternal->GetSources(),size,ppModel);
    return S_OK;
}

HRESULT CMultiSourceCollectionViewModel::RemoveAllSourcesInternal() 
{
    foundation_assert(m_spMultiSourceCollectionInternal);
    return m_spMultiSourceCollectionInternal->RemoveAll();
}

HRESULT  CMultiSourceCollectionViewModel::CreateInstance(
    _In_ const MultiSourceCollectionViewModelCreateParameters *pParameters,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CMultiSourceCollectionViewModel>(pParameters,pOuter,ppNewInstance);
}

class CMultiSourceCollectionViewModelClassFactory :
    public foundation::ctl::Implements<IMultiSourceCollectionViewModelClassFactory, &IID_ICollectionViewModelClassFactory>
{
public:
    static IMultiSourceCollectionViewModelClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CMultiSourceCollectionViewModelClassFactory, IMultiSourceCollectionViewModelClassFactory>();
        }
    }

    STDMETHOD(CreateMultiSourceCollectionViewModelInstance)(
        _In_ const MultiSourceCollectionViewModelCreateParameters *pViewModelFactoryParam,
        _In_opt_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CMultiSourceCollectionViewModel::CreateInstance(pViewModelFactoryParam, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.MultiSourceCollectionViewModelClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CMultiSourceCollectionViewModelClassFactory::GetInstance)
);
