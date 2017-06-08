/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectViewModel.h
****/

#pragma once

#include "ObservableObject.h"
#include "ObjectViewModelBase.h"
#include "BaseModelSource.h"
#include "BatchPropertyChangedHelper.h"
#include "TypeInfoStatics.h"
// from model API
#include "../model/ObservableObjectInfo.h"

typedef     
    SingleSourceModelBaseRefHolder
    <
        pmod::IObservableObject,
        pmod::IPropertyChangedEventSource,
        pmod::IPropertyChangedEventHandler,
        pmod::IPropertyChangedEventArgs
    > _SingleSourcePropertyViewModelType;

typedef 
CObjectViewModelBase
<
    _SingleSourcePropertyViewModelType,
    pmod::library::IViewModelDelegate,
    CObservableObject
> CObjectViewModelBase_Type;

class CObjectViewModel :
    public CObjectViewModelBase_Type,
    public pmod::library::IViewModelClass,
    public ITypeInfoInternalSupport
{
 public:
     HRESULT _Initialize(const pmod::library::ViewModelCreateParameters *modelImpl);

    static HRESULT CreateInstance(
        _In_ const pmod::library::ViewModelCreateParameters *pModelImpl,
        _In_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

protected:
    CObjectViewModel():
        m_pEventModelSourceDelegate(nullptr)
    {
    }

    virtual ~CObjectViewModel()
    {
        foundation::ctl::ReleaseInterface(m_pEventModelSourceDelegate);
    }

    // Override from CPropertyViewModelBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::library::IViewModelClass::GetIID())
        {
            *ppInterface = static_cast<pmod::library::IViewModelClass *>(this);
        }
        else if (iid == pmod::ISourceModel::GetIID())
        {
            *ppInterface = static_cast<pmod::ISourceModel *>(this);
        }
        else if (iid == pmod::ISourceModelBase::GetIID())
        {
            *ppInterface = static_cast<pmod::ISourceModelBase *>(this);
        }
        else if (iid == ITypeInfoInternalSupport::GetIID())
        {
            *ppInterface = static_cast<ITypeInfoInternalSupport *>(this);
        }
        else
        {
            return CObservableObject::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
    //// IObjectNodeClass Interface Override
	STDMETHOD(_GetParent)(foundation::library::_INode_impl *pParent, foundation::IObject **ppParent) override;

    // IObservableObject Interface Overrides
    STDMETHOD( SetProperty )(_In_ UINT32 propertyId,_In_ foundation::IInspectable *value);
    STDMETHOD( GetProperty )(_In_ UINT32 propertyId,_COM_Outptr_ foundation::IInspectable **pValue);
    STDMETHOD( InvokeMethod ) (_In_ UINT32 methodId,_In_ UINT32 size,foundation::IInspectable **parameters,_COM_Outptr_ foundation::IInspectable **ppResult);

    //// IDictionary Interface Overrides
    //STDMETHOD(HasKey)(HSTRING propertyName, bool* found) override;

    // ITypeInfoInternalSupport Interface
    STDMETHOD(IsInstanceOfType)(REFIID iidInstanceType, foundation::InstanceTypeOptions options, bool *pResult) override;

    // Overrides from EventSourceBase
    foundation::IUnknown *GetControllerDelegate() override
    {
        return this->AsInspectable();
    }    
    HRESULT CreateSourceEventHandlerDelegate(
        bool isFreeThreaded, 
        pmod::IPropertyChangedEventHandler **ppEventHandlerDelegate) override;

    // Overrides from BaseModelSource
    HRESULT InvokeInternal(foundation::IUnknown* pSender,pmod::IPropertyChangedEventArgs* pArgs) override
    {
        return OnSourcePropertyChangedHandler(pSender,pArgs);
    }
    HRESULT AttachToSource() override;
    HRESULT DetachFromSource() override;

    HRESULT SetSourceInternal(pmod::IObservableObject *pSource) override;
    HRESULT GetSourceEventSource(pmod::IPropertyChangedEventSource **pEventSource) override;
    bool NeedAttachToEventSource() override;
    bool NeedDetachFromEventSource() override;
    HRESULT GetSourceModelTypeInfo(foundation::IObjectTypeInfo **ppModelTypeInfo) override;

    // Pure Overrides
    HRESULT OnSourcePropertyChanged(
        pmod::IObservableObject* pSource,
        UINT32 propertyId,
        foundation::InspectablePtr& oldValue,
        foundation::InspectablePtr& newValue) override;

    HRESULT on_source_property_changed_internal(
        UINT32 propertyId,
        foundation::InspectablePtr& oldValue,
        foundation::InspectablePtr& newValue);

    void ForceRemoveAttachedSources() override
    {
        // don't notify the Delegate since we are on the Final Release
        _SingleSourcePropertyViewModelType::SetSourceInternal(nullptr);
    }

    // View Model instances only care about View Model Properties
    bool IsPropertyIdContained(UINT32 propertyId) override
    {
        return IsViewModelProperty(propertyId);
    }

    bool IsInheritProperties()
    {
        foundation_assert(this->m_pTypeInfo);
        CPropertyViewModelInfoClass *pPropertyViewModelInfoClass = static_cast<CPropertyViewModelInfoClass *>(this->m_pTypeInfo);
        bool isInherited = false;
        pPropertyViewModelInfoClass->GetIsInherited(&isInherited);
        return isInherited;
    }

    typedef BatchPropertyChangedEventHandlerDelegateBase<CObjectViewModel,_EventHandlerDelegate>
    PropertyChangedEventHandlerDelegate;

    HRESULT OnSourceEventModel(foundation::IUnknown *pSender, pmod::IEventModelEventArgs *pEventArgs);
    typedef pmod::EventModelConnection::_DelegateFactory<CObjectViewModel> CEventModelDelegateFactory;

    bool IsSourceProperty(_In_ pmod::IObservableObject* pSource,_In_ UINT32 propertyId);
public:
    // Support for IBatchPropertyChangedEventHandler Delegate
    HRESULT InvokeInternal(foundation::IUnknown* pSender,pmod::IBatchPropertyChangedEventArgs* pArgs);

private:
    // attach/detach from Event Model Source
    HRESULT AttachToEventModelSource();
    HRESULT DetachFromEventModelSource();

    static bool IsViewModelProperty(_In_ UINT32 propertyId)
    {
        return propertyId >= pmod::PropertyModel_ViewModel_Start && propertyId < foundation::Property_ResevervedMask;
    }


    typedef SingleSourceModelBase
        <
            pmod::IObservableObject,
            pmod::IPropertyChangedEventSource,
            pmod::IPropertyChangedEventHandler,
            pmod::IPropertyChangedEventArgs
        > _SourceModelBase;
private:
    CEventModelDelegateFactory::_type_eventhandler_delegate                   *m_pEventModelSourceDelegate;
};


