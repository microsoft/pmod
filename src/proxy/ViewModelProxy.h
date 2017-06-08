/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ViewModelProxy.h
****/

#pragma once

#include "PropertyModelProxy.h"
#include <pmod/core_util.h>

#include "TypeInfoStatics.h"

BEGIN_DECLARE_INTERFACE( IPropertyViewModelProxy,IPropertyModelProxy,_VOID_MACRO)
END_DECLARE_INTERFACE()

//------------------------------------------------------------------------------
// Class:   CPropertyViewModelProxy
//
// Purpose: Specialized proxy to support ViewModel runtime replacement
//
//------------------------------------------------------------------------------
class CPropertyViewModelProxy:
    public CPropertyModelProxy,
    public ITypeInfoInternalSupport
{
public:
    HRESULT _Initialize(
        _In_ UINT32 proxyUniqueId,
        _In_ pmod::IPropertyModel *pPropertyModel,
        _In_ pmod::proxy::library::IBaseProxyObjectDelegateBase *pProxyModelDelegateBase,
        _In_ CProxyObjectFactory *pPropertyModelFactory,
        _In_ bool isFreeThreadedEnabled)
    {
        IFR(CPropertyModelProxy::_Initialize(proxyUniqueId, pPropertyModel, pProxyModelDelegateBase, pPropertyModelFactory, isFreeThreadedEnabled));
        return S_OK;
    }

protected:
    CPropertyViewModelProxy():
        m_pInnerSourceModel(nullptr)
    {
    }

    void FinalRelease() override
    {
        ReleaseInterface(m_pInnerSourceModel);
        CPropertyModelProxy::FinalRelease();
    }

#if defined(_WINDOWS_RUNTIME)
    INSPECTABLE_CLASS(U("Microsoft.PropertyModel.Proxy.PropertyViewModelProxy"), foundation::TrustLevel::BaseTrust);
#endif

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if(iid == pmod::ISourceModel::GetIID())
        {
            if(m_pInnerSourceModel == nullptr)
            {
                pmod::ISourceModel *pSourceModelProxy = nullptr;
                IFR(foundation::QueryWeakReference(m_spModelProxy,&pSourceModelProxy));
                IFR(CSourceModelImpl::CreateInstance(
                        this->AsInspectable(),
                        pSourceModelProxy,
                        &m_pInnerSourceModel));
            }
            return foundation::QueryWeakReference(m_pInnerSourceModel,iid,ppInterface);
        }
        else if(iid == IPropertyViewModelProxy::GetIID())
        {
            // TODO: how we can support a 'derived' IPropertyViewModelProxy
            *ppInterface = static_cast<IPropertyModelProxy *>(this);
        }
        else if (iid == ITypeInfoInternalSupport::GetIID())
        {
            *ppInterface = static_cast<ITypeInfoInternalSupport *>(this);
        }
        else
        {
            return CPropertyModelProxy::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }

    // IObject Interface
    STDMETHOD(GetUniqueId)(UINT32 *uniqueId)
    {
        if(m_spModelProxy != nullptr)
        {
            return m_spModelProxy->GetUniqueId(uniqueId);
        }
        *uniqueId = 0;
        return S_FALSE;
    }

    STDMETHOD(GetProcessId)(UINT32 *processId)
    {
        if(m_spModelProxy != nullptr)
        {
            return m_spModelProxy->GetProcessId(processId);
        }
        *processId = 0;
        return S_FALSE;
    }

	// _IObjectNode Interface
	STDMETHOD(GetParent)(foundation::IObject **ppModel) override
	{
		if (m_spModelProxy != nullptr)
		{
			foundation::ComPtr<foundation::_IObjectNode> spModelProxyAsObjectNode;
			IFR_ASSERT(foundation::QueryWeakReference(m_spModelProxy,spModelProxyAsObjectNode.GetAddressOf()));
			return spModelProxyAsObjectNode->GetParent(ppModel);
		}
		*ppModel = nullptr;
		return S_FALSE;
	}

    // IPropertyModel Interface
    STDMETHOD(GetProperty)(_In_ UINT32 propertyId,_COM_Outptr_ IInspectable **ppValue) override
    {
        if(propertyId == pmod::Property_Parent && m_spModelProxy != nullptr)
        {
            // for Parent object we want to return the Model Proxy parent
			foundation::ComPtr<foundation::IObject> spParent;
			IFR(foundation::library_util::GetObjectParent(this->m_spModelProxy, spParent.GetAddressOf()));

            *ppValue = spParent;
            if(*ppValue)
            {
                (*ppValue)->AddRef();
            }
            return S_OK;
        }
        return this->GetPropertyValue(propertyId,ppValue);
    }
    // ITypeInfoInternalSupport Interface
    STDMETHOD(IsInstanceOfType)(REFIID iidInstanceType, foundation::InstanceTypeOptions options, bool *pResult)
    {
        if (m_spModelProxy != nullptr &&
            IsOptionEnabled(options,foundation::InstanceTypeOptions::AllowInheritance))
        {
            IFR_ASSERT(foundation::IsInstanceOfType(
                m_spModelProxy,
                iidInstanceType,
                pResult));
        }
        return S_OK;
    }

    // BaseObjectProxy Overrides
    STDMETHOD(Dispose)()  override
    {
        IFR(CPropertyModelProxy::Dispose());
        if(m_spModelProxy != nullptr )
        {
            IBaseObjectProxyInternal *pModelProxyInternal = nullptr;
            foundation::QueryWeakReference(m_spModelProxy,&pModelProxyInternal);
            foundation_assert(pModelProxyInternal);
            IFR(pModelProxyInternal->Dispose());
        }
        return S_OK;
    }

    HRESULT SetSourceConnectStateInternal(bool newValue) override
    {
        // chain the call into the 'Source' proxy
        if(m_spModelProxy != nullptr)
        {
            IFR(GetSourceModelProxyInternal()->SetSourceConnectState(newValue));
        }
        return S_OK;
    }

    STDMETHOD(DisconnectFromSource)(foundation::IInspectable **ppSourceInfo) override
    {
        // chain the call into the 'Source' proxy
        if(m_spModelProxy != nullptr)
        {
            IFR(GetSourceModelProxyInternal()->DisconnectFromSource(ppSourceInfo));
        }
        return S_OK;
    }

    STDMETHOD(ConnectToSource)(foundation::IInspectable *pSourceInfo, _SyncSourceInfo_Vector_Type& syncSourceInfo) override
    {
        // chain the call into the 'Source' proxy
        if(m_spModelProxy != nullptr)
        {
            IFR(GetSourceModelProxyInternal()->ConnectToSource(pSourceInfo, syncSourceInfo));
        }
        return S_OK;
    }
    STDMETHOD(ResyncSource)(foundation::IInspectable *pSource, _SyncSourceInfo_Vector_Type& syncSourceInfo) override
    {
        // chain the call into the 'Source' proxy
        if (m_spModelProxy != nullptr)
        {
            IFR(GetSourceModelProxyInternal()->ResyncSource(pSource, syncSourceInfo));
        }
        return S_OK;
    }
    STDMETHOD(FireResyncEvent)(foundation::IUnknown *pEventArg) override
    {
        // chain the call into the 'Source' proxy
        if (m_spModelProxy != nullptr)
        {
            IFR(GetSourceModelProxyInternal()->FireResyncEvent(pEventArg));
        }
        return S_OK;
    }

    HRESULT on_source_attached_internal(pmod::IPropertyModel *pSource) override
    {
        IFR(CPropertyModelProxy::on_source_attached_internal(pSource));
        m_spModelProxy.Release();
        if(pSource != nullptr)
        {
            foundation::ComPtr<foundation::IObject> spSource;
            IFR(pmod::core_util::try_get_source(pSource, spSource.GetAddressOf()));
            IFR(foundation::QueryInterface(spSource, m_spModelProxy.GetAddressOf()));
        }
        return S_OK;
    }
    // CPropertyModelProxy Overrides
    HRESULT ToPropertyProxyValue(_In_ UINT32 propertyId,_Inout_ foundation::InspectablePtr& value) override
    {
        IInspectable *pValue = value;
        IFR(CPropertyModelProxy::ToPropertyProxyValue(propertyId,value));

        if(pValue && 
            // If proxy was swapped for this Var type
            (pValue != value.get()) &&
            // and also a view model property was retrieved
            propertyId >= pmod::PropertyModel_ViewModel_Start)
        {
            // we just have access a property on the runtime view model
            // and also a Proxy was created for this
            // Note: to avoid the QI we will assume the IUnknown is an IObjectNode

            // Flag as a ViewModel Proxy
            IBaseObjectProxyInternal *pBaseObjectProxyInternal;
            IFR_ASSERT(foundation::QueryWeakReference(value, &pBaseObjectProxyInternal));

            BaseObjectProxy *pBaseObjectProxy = BaseObjectProxy::AsBaseProxyClass(pBaseObjectProxyInternal);
            pBaseObjectProxy->SetIsViewModelProxy(true);
        }
        return S_OK;
    }

private:
    IBaseObjectProxyInternal *GetSourceModelProxyInternal()
    {
        foundation_assert(m_spModelProxy != nullptr);
        IBaseObjectProxyInternal *pSourceModelProxyInternal = nullptr;
        HRESULT hr = foundation::QueryWeakReference(m_spModelProxy,&pSourceModelProxyInternal);
        foundation_assert(SUCCEEDED(hr));
        return pSourceModelProxyInternal;
    }
private:
    // Class CSourceModelImpl
    // Implement a 'Private' ISourceModel to be exposed when QI the container object 
    class CSourceModelImpl:
        public foundation::ctl::ComInspectableBase, 
        public pmod::ISourceModel
    {
    public:
        static HRESULT CreateInstance(
                _In_ IInspectable       *pOuter,
                _In_ pmod::ISourceModel   *pSourceModelProxy,
                _COM_Outptr_ IInspectable      **ppUnknown)
        {
            foundation_assert(pOuter);

            foundation::ctl::AggregableComInspectableObject<CSourceModelImpl> *pNewInstance;
            IFR(foundation::ctl::AggregableComInspectableObject<CSourceModelImpl>::CreateInstance(pOuter, &pNewInstance));
            IFR(pNewInstance->Initialize(pSourceModelProxy));
            *ppUnknown = reinterpret_cast<IInspectable*>(static_cast<foundation::ctl::INonDelegatingInspectable *>(pNewInstance));
            return S_OK;
        }
    
    protected:
        CSourceModelImpl():
             m_pSourceModelProxy(nullptr)
        {
        }

        HRESULT Initialize(_In_ pmod::ISourceModel   *pSourceModelProxy)
        {
            m_pSourceModelProxy = pSourceModelProxy;
            return S_OK;
        }
        
        HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
        {
            if(iid == pmod::ISourceModel::GetIID())
            {
                *ppInterface = static_cast<pmod::ISourceModel *>(this);
            }
            else
            {
                return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid,ppInterface);
            }
            return S_OK;
        }
       // ISourceModel Interface
        STDMETHOD (GetSource)(foundation::IObject **ppModel) override
        {
            return m_pSourceModelProxy->GetSource(ppModel);
        }
        STDMETHOD(SetSource)(foundation::IObject *pModel) override
        {
            UNREFERENCED_PARAMETER(pModel);
            return E_NOTIMPL;
        }
        STDMETHOD(GetIsAttached)(bool *value) override
        {
             UNREFERENCED_PARAMETER(value);
             return E_NOTIMPL;
        }
        STDMETHOD(GetForceAttached)(bool *value) override
        {
             UNREFERENCED_PARAMETER(value);
             return E_NOTIMPL;
        }        
        STDMETHOD (SetForceAttached)(bool newValue) override
        {
            UNREFERENCED_PARAMETER(newValue);
            return E_NOTIMPL;
        }
        STDMETHOD(GetAttachEnabled)(bool *value) override
        {
             UNREFERENCED_PARAMETER(value);
             return E_NOTIMPL;
        }        
        STDMETHOD (SetAttachEnabled)(bool newValue) override
        {
            UNREFERENCED_PARAMETER(newValue);
            return E_NOTIMPL;
        }
    private:
        ISourceModel *m_pSourceModelProxy;
    };
 private:
     IInspectable *m_pInnerSourceModel;
    foundation::ComPtr<foundation::IObject> m_spModelProxy;
};
