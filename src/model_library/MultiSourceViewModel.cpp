/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MultiSourceViewModel.cpp
****/

#include "pch.h"

#include "MultiSourceViewModel.h"
#include "CreateFoundationHelper.h"

// {F7CC5F9C-E895-447C-8D83-6F677DC232B5}
const GUID pmod::library::IID_IMultiSourceViewModelClassFactory =
{ 0xf7cc5f9c, 0xe895, 0x447c,{ 0x8d, 0x83, 0x6f, 0x67, 0x7d, 0xc2, 0x32, 0xb5 } };

using namespace pmod;
using namespace pmod::library;
using namespace foundation::library;

HRESULT CMultiSourceViewModel::on_source_changed_internal(IObservableObject *pSender, IPropertyChangedEventArgs *pEventArgs)
{
    if(m_spViewModelSourceDelegate)
    {
        UINT32 propertyId;
		IFR(pEventArgs->GetProperty(&propertyId));
        foundation::InspectablePtr oldValue,newValue;
        IFR(pEventArgs->GetOldValue(oldValue.GetAddressOf()));
		IFR(pEventArgs->GetNewValue(newValue.GetAddressOf()));

        IFR(m_spViewModelSourceDelegate->OnSourcePropertyChanged(
                pSender,
                propertyId,
                oldValue,
                newValue));
    }
    return S_OK;
}

HRESULT CMultiSourceViewModel::_Initialize(const MultiSourceViewModelCreateParameters *multiSourcePropertyViewParam)
{
    foundation_assert(multiSourcePropertyViewParam);

    ObservableObjectCreateParameters propertyModelParam;
    propertyModelParam.m_iid_type = multiSourcePropertyViewParam->m_iid_type;
    propertyModelParam.m_options = multiSourcePropertyViewParam->m_options;
    propertyModelParam.m_p_delegate = multiSourcePropertyViewParam->m_p_delegate;

    IFR(_MultiSourcePropertyViewModelBaseType::_Initialize(&propertyModelParam));

    m_spViewModelSourceDelegate = multiSourcePropertyViewParam->m_p_viewmodel_delegate;

    if(multiSourcePropertyViewParam->m_p_sources)
    {
        for(int i=0;i< multiSourcePropertyViewParam->m_size_sources;++i)
        {
            IFR(AddSourceInternal(multiSourcePropertyViewParam->m_p_sources[i]));
        }
    }
    return S_OK;
}

void CMultiSourceViewModel::OnFinalRelease()
{
    _MultiSourcePropertyViewModelBaseType::OnFinalRelease();
}

void CMultiSourceViewModel::ForceRemoveAttachedSources()
{
    // this is typicaly invoked from FinalRelease and we don't want to invoke the Delegate callback
    if(IsAttached())
    {
        IGNOREHR(DetachAllSources());
    }
}

HRESULT CMultiSourceViewModel::AddSourceInternal(::IObservableObject *pModel)
{
    return AttachToSourceInternal(pModel);
}

HRESULT CMultiSourceViewModel::RemoveSourceInternal(::IObservableObject *pModel)
{
    return DetachFromSourceInternal(pModel);
}

HRESULT CMultiSourceViewModel::GetSourcesInternal(UINT32 *size,pmod::IObservableObject ***ppModel)
{
    ContainerRefCountToArray(this->CMultiSourcePropertyModelContainer::GetSources(),size,ppModel);
    return S_OK;
}

HRESULT CMultiSourceViewModel::RemoveAllSourcesInternal()
{
    return this->CMultiSourcePropertyModelContainer::RemoveAllSources(true);
}

HRESULT CMultiSourceViewModel::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    if(iid == IMultiSourceModel::GetIID())
    {
        *ppInterface = static_cast<IMultiSourceModel *>(this);
    }
    else
    {
        return _MultiSourcePropertyViewModelBaseType::QueryInterfaceImpl(iid,ppInterface);
    }
    return S_OK;
}

HRESULT  CMultiSourceViewModel::CreateInstance(
    _In_ const MultiSourceViewModelCreateParameters *pParameters,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CMultiSourceViewModel>(pParameters,pOuter,ppNewInstance);
}

class CMultiSourceViewModelClassFactory :
    public foundation::ctl::Implements<IMultiSourceViewModelClassFactory, &IID_IViewModelClassFactory>
{
public:
    static IMultiSourceViewModelClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CMultiSourceViewModelClassFactory, IMultiSourceViewModelClassFactory>();
        }
    }

    STDMETHOD(CreateMultiSourceViewModelInstance)(
        _In_ const MultiSourceViewModelCreateParameters *pViewModelImpl,
        _In_opt_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CMultiSourceViewModel::CreateInstance(pViewModelImpl, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.MultiSourceViewModelClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CMultiSourceViewModelClassFactory::GetInstance)
);
