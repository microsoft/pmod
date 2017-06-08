/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectViewModel.cpp
****/

#include "pch.h"

#include "ObjectViewModel.h"
#include "CreateFoundationHelper.h"

#include <foundation/pv_util.h>

// {46DA8892-B402-43B6-97EC-72108973A480}
const GUID pmod::library::IID_IViewModelClassFactory =
{ 0x46da8892, 0xb402, 0x43b6,{ 0x97, 0xec, 0x72, 0x10, 0x89, 0x73, 0xa4, 0x80 } };

using namespace pmod;
using namespace pmod::library;
using namespace foundation::library;

HRESULT  CObjectViewModel::CreateInstance(
    _In_ const ViewModelCreateParameters *pCreateParameters,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CObjectViewModel>(pCreateParameters, pOuter, ppNewInstance);
}

HRESULT CObjectViewModel::_Initialize(const ViewModelCreateParameters *modelImpl)
{
    IFR_ASSERT(CObservableObject::_Initialize(modelImpl));

    foundation_assert(modelImpl->m_p_viewmodel_delegate);

    m_spViewModelSourceDelegate = modelImpl->m_p_viewmodel_delegate;
    IFR_ASSERT(_SingleSourcePropertyViewModelType::_InitializeSourceEventHandler(this->IsFreeThreadApartment()));

    // Initialize Weak/Strong Reference option
    SetIsWeakReference(IsOptionEnabled(modelImpl->m_view_model_options,ViewModelOptions::UseSourceWeakReference));

    if(modelImpl->m_p_source)
    {
        IFR_ASSERT(SetSourceInternal(modelImpl->m_p_source));
    }

    return S_OK;
}

HRESULT CObjectViewModel::SetSourceInternal(IObservableObject *pSource)
{
    IFR(_SingleSourcePropertyViewModelType::SetSourceInternal(pSource,!IsUseQuirksMode()));
    if(m_spViewModelSourceDelegate && this->IsDelegateAvailable())
    {
        IFR(m_spViewModelSourceDelegate->OnSourceChanged(pSource));
    }
    // Fire Notify changes on the intrinsic property
    _FoundationBaseLock csl(*this);
    IFR(this->FirePropertyChangedEvent(
        csl,
        Property_Source,
        nullptr,
        pSource));

    return S_OK;
}

HRESULT CObjectViewModel::GetSourceModelTypeInfo(foundation::IObjectTypeInfo **ppModelTypeInfo)
{
    foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
    IFR_ASSERT(GetTypeInfo(spTypeInfo.GetAddressOf()));
    if (spTypeInfo != nullptr)
    {
        IViewModelInfo *pPropertyViewModelInfo = nullptr;
        if (SUCCEEDED(foundation::QueryWeakReference(spTypeInfo, &pPropertyViewModelInfo)))
        {
            foundation::ComPtr<IObservableObjectInfo> spSourceModelTypeInfo;
            IFR(pPropertyViewModelInfo->GetSourceType(spSourceModelTypeInfo.GetAddressOf()));
            *ppModelTypeInfo = spSourceModelTypeInfo.Detach();
         }
    }
    return S_OK;
}

bool CObjectViewModel::IsSourceProperty(_In_ IObservableObject* pSource,_In_ UINT32 propertyId)
{
    UNREFERENCED_PARAMETER(pSource);
    // TODO: 
    // For now just check the range of the property id if it is a ViewModel property
    // This limit ViewModel instances to never 'Source' another ViewModel 
    // Future improvement may detect the metadata of the 'Source'
    return !IsViewModelProperty(propertyId);
}

HRESULT CObjectViewModel::OnSourcePropertyChanged(
        IObservableObject* pSource,
        UINT32 propertyId,
        foundation::InspectablePtr& oldValue,
        foundation::InspectablePtr& newValue)
{
    if(!IsSourceProperty(pSource,propertyId))
    {
        // in theory we should only receive 'Source' notifications from 'Model' properties (versus 'ViewModel' properties).
        // we found many scenarios for proxy runtime replacement where this is not true
        //  A ViewModel that is 'runtime' replaced will fire its own properties on ViewModel being created 
        // on top of the ViewModel proxy
        return S_FALSE;
    }

    IFR(on_source_property_changed_internal(propertyId,oldValue,newValue));

    if(IsInheritProperties())
    {
        _FoundationBaseLock csl(*this);
        IFR(FirePropertyChangedEvent(
            csl,
            pSource, 
            propertyId, 
            oldValue, 
            newValue));
    }
    return S_OK;
}

HRESULT CObjectViewModel::on_source_property_changed_internal(
        UINT32 propertyId,
        foundation::InspectablePtr& oldValue,
        foundation::InspectablePtr& newValue)
{
    if(m_spViewModelSourceDelegate)
    {
        IFR(m_spViewModelSourceDelegate->OnSourcePropertyChanged(propertyId,oldValue,newValue));
    }
    return S_OK;
}

HRESULT CObjectViewModel::GetSourceEventSource(IPropertyChangedEventSource **pEventSource)
{
    foundation::ComPtr<IObservableObject> spSource;
    IFR(foundation::ResolveReferenceHolderPtr(*m_spReferenceHolder.get(), false, spSource));

    return spSource->GetPropertyChangedEventSource(pEventSource);
}

bool CObjectViewModel::NeedAttachToEventSource()
{
    return IsReferenceAttached() && HasEventHandlers();
}

bool CObjectViewModel::NeedDetachFromEventSource()
{
    return !HasEventHandlers();
}

STDMETHODIMP CObjectViewModel::_GetParent(foundation::library::_INode_impl *pParent, foundation::IObject **ppParent)
{
    // If Parent is nullptr and we use 'inherited' we wil try the 'Source' for calculating the 'Parent'
    if(pParent == nullptr && IsInheritProperties() && IsReferenceAttached())
    {
        foundation::ComPtr<IObservableObject> spSource;
        IFR_ASSERT(ResolveRefHolder(spSource));

		foundation::_IObjectNode *pSourceObjectNode;
		foundation::QueryWeakReference(spSource, &pSourceObjectNode);

		if (pSourceObjectNode != nullptr)
		{
			foundation::ComPtr<foundation::IObject> spSourceObjectNodeParent;
			pSourceObjectNode->GetParent(spSourceObjectNodeParent.GetAddressOf());

			if (spSourceObjectNodeParent != nullptr &&
				spSourceObjectNodeParent != static_cast<foundation::IObject *>(this))
			{
				// Route Parent to our Source
				return spSourceObjectNodeParent.CopyTo(ppParent);
			}
		}
    }
    return CObjectViewModelBase_Type::_GetParent(pParent, ppParent);
}

STDMETHODIMP CObjectViewModel::GetProperty(_In_ UINT32 propertyId,_COM_Outptr_ foundation::IInspectable **ppValue)
{
    IFCPTR(ppValue);

    if(propertyId == Property_Source)
    {
        foundation::ComPtr<IObservableObject> spSource;
        if(IsReferenceAttached())
        {
            IFR(ResolveRefHolder(spSource));
        }
        *ppValue = spSource;
        if(*ppValue)
        {
            (*ppValue)->AddRef();
        }
        return S_OK;
    }
    else if(propertyId == Property_Parent)
    {
        foundation::ComPtr<foundation::IObject> spParent;
        IFR_ASSERT(foundation::GetObjectParent(this, spParent.GetAddressOf()));
        *ppValue = spParent;
        if(*ppValue)
        {
            (*ppValue)->AddRef();
        }
        return S_OK;
    }
    else if(propertyId >= PropertyModel_ViewModel_Start)
    {
        // it's a View Property
        return CObservableObject::GetProperty(propertyId,ppValue);
    }
    else if(IsInheritProperties())
    {
        if(IsReferenceAttached())
        {
            // it's a 'Source Property
            foundation::ComPtr<IObservableObject> spSource;
            IFR(ResolveRefHolder(spSource));
            return spSource->GetProperty(propertyId,ppValue);
        }
        else
        {
            // The 'Source' is null or not available
            return foundation::pv_util::CreateEmptyValue(ppValue);
        }
    }
    else
    {
        // TODO: provide a better HRESULT value (E_PROPERTY_NOT_FOUND)
        return E_FAIL;
    }
}

STDMETHODIMP CObjectViewModel::SetProperty(_In_ UINT32 propertyId,_In_ foundation::IInspectable *value)
{
    if(IsViewModelProperty(propertyId))
    {
        // it's a View Property
        return CObservableObject::SetProperty(propertyId,value);
    }
    else if(IsInheritProperties())
    {
        if(IsReferenceAttached())
        {
            // it's a 'Source Property
            foundation::ComPtr<IObservableObject> spSource;
            IFR(ResolveRefHolder(spSource));
            // it's a 'Source Property
            return spSource->SetProperty(propertyId,value);
        }
        else
        {
            // The 'Source' is null or not available
            // TODO: a more descriptive error number
            return E_UNEXPECTED;
        }
    }
    else
    {
        // TODO: provide a better HRESULT value (E_PROPERTY_NOT_FOUND)
        return E_FAIL;
    }
}

STDMETHODIMP CObjectViewModel::InvokeMethod(_In_ UINT32 methodId,_In_ UINT32 size,foundation::IInspectable **parameters,_COM_Outptr_ foundation::IInspectable **ppResult)
{
    if(methodId >= PropertyModel_ViewModel_Start)
    {
        return CObservableObject::InvokeMethod(methodId,size,parameters,ppResult);
    }
    else if(IsInheritProperties())
    {
        foundation::ComPtr<IObservableObject> spSource;
        IFR(ResolveRefHolder(spSource));
        // it's a 'Source Property
        return spSource->InvokeMethod(methodId,size,parameters,ppResult);
    }
    else
    {
        // TODO: provide a better HRESULT value (E_METHOD_NOT_FOUND)
        return E_FAIL;
    }
}

STDMETHODIMP CObjectViewModel::IsInstanceOfType(
    REFIID iidInstanceType, foundation::InstanceTypeOptions options, bool *pResult)
{
    if (IsOptionEnabled(options,foundation::InstanceTypeOptions::AllowInheritance) &&
        IsInheritProperties())
    {
        foundation::ComPtr<IObservableObject> spSource;
        IFR(ResolveRefHolder(spSource));
        IFR_ASSERT(foundation::IsInstanceOfType(spSource, iidInstanceType, *pResult));
    }
    return S_OK;
}

HRESULT CObjectViewModel::CreateSourceEventHandlerDelegate(
    bool isFreeThreaded, 
    pmod::IPropertyChangedEventHandler **ppEventHandlerDelegate)
{
    PropertyChangedEventHandlerDelegate *pPropertyChangedEventHandlerDelegate = nullptr;
    IFR(foundation::CreateDelegate(this,&pPropertyChangedEventHandlerDelegate));
    *ppEventHandlerDelegate = pPropertyChangedEventHandlerDelegate;
    return S_OK;
}

HRESULT CObjectViewModel::InvokeInternal(foundation::IUnknown* pSender,IBatchPropertyChangedEventArgs* pArgs)
{
    UNREFERENCED_PARAMETER(pSender);

    struct _ForEach : public CPropertyChangedEventArgsFunc {
    _ForEach(
        _In_ CObjectViewModel *pThis):
        m_pThis(pThis)
    {
    }

    HRESULT operator() (_In_ IPropertyChangedEventArgs *pPropertyChangedEventArgs)
    {
        UINT32 propertyId;
        pPropertyChangedEventArgs->GetProperty(&propertyId);
        foundation::InspectablePtr oldValue,newValue;
        pPropertyChangedEventArgs->GetOldValue(oldValue.GetAddressOf());
        pPropertyChangedEventArgs->GetNewValue(newValue.GetAddressOf());

        IFR(m_pThis->on_source_property_changed_internal(propertyId,oldValue,newValue));
        return S_OK;
    }
    private:
        CObjectViewModel                     *m_pThis;
    };
    // Iterate on 'on_source_property_changed_internal'
    _ForEach iter(this);
    IFR(ForEachPropertyChangedEventArgs(pArgs,&iter));

    if(IsInheritProperties())
    {
        _FoundationBaseLock csl(*this);
        IFR(FirePropertyChangedEvent(
            csl,
            pArgs));
    }
    return S_OK;
}

HRESULT CObjectViewModel::AttachToEventModelSource()
{
    foundation::ComPtr<IObservableObject> spSource;
    IFR(foundation::ResolveReferenceHolderPtr(*m_spReferenceHolder.get(), false, spSource));

    foundation_assert(spSource);
    if(m_pEventModelSourceDelegate == nullptr)
    {
        IFR_ASSERT(CEventModelDelegateFactory::CreateInstance(
            this,
            &CObjectViewModel::OnSourceEventModel,
            nullptr,
            &m_pEventModelSourceDelegate));
    }
    foundation_assert(m_pEventModelSourceDelegate);
    // Ensure we set the Controller
    m_pEventModelSourceDelegate->SetCallback(this,this->AsInspectable());
    IFR(EventModelListenerBase::AttachEventHandler(spSource,m_pEventModelSourceDelegate));
    return S_OK;
}

HRESULT CObjectViewModel::OnSourceEventModel(foundation::IUnknown *pSender, IEventModelEventArgs *pEventArgs)
{
    foundation_assert(pEventArgs);

    UINT32 eventId;
    foundation::InspectablePtr spEventArgs;
    
    IFR(pEventArgs->GetEvent(&eventId));
    IFR(pEventArgs->GetEventArgs(spEventArgs.ReleaseAndGetAddressOf()));

    if(m_spViewModelSourceDelegate)
    {
        IFR(m_spViewModelSourceDelegate->OnSourceEventModel(eventId,spEventArgs));
    }
    return S_OK;
}

HRESULT CObjectViewModel::DetachFromEventModelSource()
{
    foundation::ComPtr<IObservableObject> spSource;
    IFR(foundation::ResolveReferenceHolderPtr(*m_spReferenceHolder.get(), false, spSource));

    foundation_assert(spSource);
    foundation_assert(m_pEventModelSourceDelegate);
    m_pEventModelSourceDelegate->SetCallback(nullptr);
    IFR(EventModelListenerBase::DetachEventHandler(spSource,m_pEventModelSourceDelegate));
    return S_OK;
}

HRESULT CObjectViewModel::AttachToSource()
{
    IFR(CObjectViewModelBase_Type::AttachToSource());
    IFR(AttachToEventModelSource());
    return S_OK;
}

HRESULT CObjectViewModel::DetachFromSource()
{
    IFR(DetachFromEventModelSource());
    IFR(CObjectViewModelBase_Type::DetachFromSource());
    return S_OK;
}

//STDMETHODIMP CObjectViewModel::HasKey(HSTRING propertyName, bool* found)
//{
//    _IFR_(CObservableObject::HasKey(propertyName, found));
//    if (!(*found) && IsInheritProperties())
//    {
//        foundation::ComPtr<IObservableObject> spSource;
//        IFR_ASSERT(ResolveRefHolder(spSource));
//
//        foundation::ComPtr<foundation::IDictionary> dictionary_source_ptr;
//        IFR_ASSERT(foundation::QueryInterface(spSource, dictionary_source_ptr.GetAddressOf()));
//        return dictionary_source_ptr->HasKey(propertyName, found);
//    }
//    return S_OK;
//}

class CObjectViewModelClassFactory :
    public foundation::ctl::Implements<IViewModelClassFactory, &IID_IViewModelClassFactory>
{
public:
    static IViewModelClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CObjectViewModelClassFactory, IViewModelClassFactory>();
        }
    }

    STDMETHOD(CreateViewModelInstance)(
        _In_ const ViewModelCreateParameters *pViewModelImpl,
        _In_opt_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CObjectViewModel::CreateInstance(pViewModelImpl, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.ViewModelClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObjectViewModelClassFactory::GetInstance)
);
