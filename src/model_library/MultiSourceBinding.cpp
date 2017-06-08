/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MultiSourceBinding.cpp
****/

#include "pch.h"
#include "MultiSourceBinding.h"
#include "CreateFoundationHelper.h"
#include <foundation/pv_util.h>
#include <foundation/array_wrapper.h>
#include <foundation/library/com_ptr_singleton.h>

// {12F95CF5-F681-49F3-83F4-95AFA80A9EA2}
const GUID pmod::library::IID_IMultiSourceBindingClassFactory =
{ 0x12f95cf5, 0xf681, 0x49f3,{ 0x83, 0xf4, 0x95, 0xaf, 0xa8, 0xa, 0x9e, 0xa2 } };

using namespace pmod;
using namespace foundation::library;
using namespace pmod::library;

HRESULT CMultiSourceBinding::on_source_changed_internal(IBindingBase *pSender, IBindingValueChangedEventArgs *pEventArgs)
{
    UNREFERENCED_PARAMETER(pSender);
    UNREFERENCED_PARAMETER(pEventArgs);

    // invalidate our cached binding value
    InvalidateCachedBindingValue();

    IFR(InternalFireBindingValueChanged());
    return S_OK;
}

void CMultiSourceBinding::ForceRemoveAttachedSources()
{
    if(IsAttached())
    {
        // this is typicaly invoked from FinalRelease and we don't want to invoke the Delegate callback
        IGNOREHR(DetachAllSources());
    }
}

HRESULT CMultiSourceBinding::AttachToSourceInternal(IBindingBase *pSource)
{
    IFR(CMultiSourceModelBase<CMultiSourceBindingContainer>::AttachToSourceInternal(pSource));
    if(IsAttached())
    {
        IFR(InternalFireBindingValueChanged());
    }
    return S_OK;
}

HRESULT CMultiSourceBinding::DetachFromSourceInternal(IBindingBase *pSource)
{
    IFR(CMultiSourceModelBase<CMultiSourceBindingContainer>::DetachFromSourceInternal(pSource));
    if(IsAttached())
    {
        IFR(InternalFireBindingValueChanged());
    }
    return S_OK;
}

HRESULT CMultiSourceBinding::AddSourceInternal(IBindingBase *pSource)
{
    return AttachToSourceInternal(pSource);
}

HRESULT CMultiSourceBinding::RemoveSourceInternal(IBindingBase *pSource)
{
    return DetachFromSourceInternal(pSource);
}

HRESULT CMultiSourceBinding::GetSourcesInternal(UINT32 *size,pmod::IBindingBase ***ppModel)
{
    return this->GetBindingSources(size,ppModel);
}

HRESULT CMultiSourceBinding::RemoveAllSourcesInternal() 
{
    return CMultiSourceModelBase<CMultiSourceBindingContainer>::RemoveAllSources(true);
}

HRESULT CMultiSourceBinding::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    if(iid == IMultiSourceModel::GetIID())
    {
        *ppInterface = static_cast<IMultiSourceModel *>(this);
    }
    else
    {
        return CBindingBase<_MultiSourceBindingBaseT>::QueryInterfaceImpl(iid,ppInterface);
    }
    return S_OK;
}

void CMultiSourceBinding::FinalRelease()
{
    CBindingBase<_MultiSourceBindingBaseT>::FinalRelease();
}

STDMETHODIMP CMultiSourceBinding::GetBindingSources(_Out_ UINT32 *size,_Outptr_ IBindingBase ***ppBindings)
{
    ContainerRefCountToArray(CMultiSourceModelBase<CMultiSourceBindingContainer>::GetSources(),size,ppBindings);
    return S_OK;
}

HRESULT CMultiSourceBinding::_Initialize(const MultiSourcePropertyBindingCreateParameters *modelImpl)
{
    IFR_ASSERT(_MultiSourceBindingBaseT::_Initialize(modelImpl));
    return S_OK;
}

HRESULT  CMultiSourceBinding::CreateInstance(
	_In_ const MultiSourcePropertyBindingCreateParameters *pParameters,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CMultiSourceBinding>(pParameters,pOuter,ppNewInstance);
}

HRESULT CMultiSourceBinding::RecalcMultisourceBindingValue(foundation::IInspectable **ppValue)
{
    m_IsValueValid = true;
    m_IsValueSet = true;

    _Inspectable_ptr_vector_type bindingValues;

    for (std::vector<foundation::ComPtr<IBindingBase> >::const_iterator iter = 
                    CMultiSourceModelBase<CMultiSourceBindingContainer>::GetSources().begin();
                    iter != CMultiSourceModelBase<CMultiSourceBindingContainer>::GetSources().end(); 
                    ++iter)
    {
        foundation::ComPtr<IBindingBasePrivate> spBindingBasePrivate;
        IFR(foundation::QueryInterface(*iter,spBindingBasePrivate.GetAddressOf()));
        if(!spBindingBasePrivate->IsValueValid())
        {
            m_IsValueValid = false;
            return S_FALSE;
        }
        foundation::InspectablePtr bindingValue;
        IFR((*iter)->GetValue(bindingValue.GetAddressOf()));

        if (bindingValue != nullptr && 
            foundation::pv_util::IsValueEmpty(bindingValue))
        {
            m_IsValueSet = false;
            return S_FALSE;
        }
        bindingValues.push_back(bindingValue);
    }
    // TODO: for now we enforce a Delegate to resolve the combined binding values
    foundation_assert(m_pDelegate);
    if(m_pDelegate)
    {
        foundation::InspectableArrayWrapper bindingValuesArray;
        ContainerToArray(bindingValues,bindingValuesArray.GetSizeAddressOf(),bindingValuesArray.GetBufferAddressOf());

        IFR(m_pDelegate->OnBindingValuesChanged(
            bindingValuesArray.GetSize(),
            (foundation::IInspectable **)bindingValuesArray.GetBuffer(),
            ppValue));
    }

    return S_OK;
}

STDMETHODIMP CMultiSourceBinding::GetValue(foundation::IInspectable **ppValue)
{
    IFCPTR(ppValue);

    if (IsAttached() && IsCachedBindingValue())
    {
        return GetCachedBindingValue(ppValue);
    }
    IFR(RecalcMultisourceBindingValue(ppValue));
    if (IsAttached())
    {
        SetCachedBindingValue(*ppValue);
    }
    return S_OK;
}

HRESULT CMultiSourceBinding::AttachToSource()
{
    return _CMultiSourceBindingBase_Type::AttachToSource();
}

HRESULT CMultiSourceBinding::DetachFromSource()
{
    InvalidateCachedBindingValue();
    return _CMultiSourceBindingBase_Type::DetachFromSource();
}

#if PMOD_LIB_LIBRARY_STATIC_LIB
EXTERN_C void _ModelLibraryInstanceInitializerStub();
#endif

class CMultiSourceBindingClassFactory :
    public foundation::ctl::Implements<IMultiSourceBindingClassFactory, &IID_IMultiSourceBindingClassFactory>
{
public:
    static IMultiSourceBindingClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CMultiSourceBindingClassFactory, IMultiSourceBindingClassFactory>();
        }
    }

    STDMETHODIMP CreateMultiSourceBindingInstance(
        _In_ const MultiSourcePropertyBindingCreateParameters *pMultiSourceBindingParam,
        _In_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CMultiSourceBinding::CreateInstance(pMultiSourceBindingParam, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.MultiSourceBindingClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CMultiSourceBindingClassFactory::GetInstance)
);
