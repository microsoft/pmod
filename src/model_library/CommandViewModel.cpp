/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandViewModel.cpp
****/

#include "pch.h"

#include "CommandViewModel.h"
#include "CreateFoundationHelper.h"
#include <foundation/hstring_wrapper.h>

// {C70D219F-DA12-439A-99F6-24261DD1CEBE}
const GUID pmod::library::IID_ICommandViewModelClassFactory =
{ 0xc70d219f, 0xda12, 0x439a,{ 0x99, 0xf6, 0x24, 0x26, 0x1d, 0xd1, 0xce, 0xbe } };

using namespace pmod;
using namespace pmod::library;
using namespace foundation;
using namespace foundation::library;

HRESULT CCommandViewModel::CreateInstance(
    _In_ const CommandViewModelCreateParameters *pCommandViewModelParam,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CCommandViewModel>(pCommandViewModelParam, pOuter, ppNewInstance);
}

HRESULT CCommandViewModel::_Initialize(const CommandViewModelCreateParameters *modelImpl)
{
    IFR(InitializeModelImpl(modelImpl));
    if(modelImpl->m_command_label)
    {
        IFR(SetCommandLabel(foundation::HStringRef(modelImpl->m_command_label)));
    }
    return S_OK;
}

HRESULT CCommandViewModel::InvokeInternal(foundation::IUnknown* pSender,ICanExecuteChangedEventArgs* pArgs)
{
    UNREFERENCED_PARAMETER(pSender);

    UINT32 oldState,newState;
    IFR(pArgs->GetOldState(&oldState));
    IFR(pArgs->GetNewState(&newState));

    if (this->m_pViewModelDelegate != nullptr)
    {
        return m_pViewModelDelegate->OnSourceCommandStateChanged(oldState, newState);
    }
    return S_OK;
}

HRESULT CCommandViewModel::SetCommandLabel(_In_ HSTRING label)
{
    foundation_assert(false);
    // TODO: fix this
    // IFR(SetModelProperty(CommandViewModel::Property_CommandLabel,label));
    return E_NOTIMPL;
}

HRESULT CCommandViewModel::AttachToSource()
{
    IFR(_CommandViewModelBaseType::AttachToSource());
    //
    // Initialize command State based on the value from Source when source is attached
    //

    foundation_assert(m_spObjectSource != nullptr);

    UINT32 sourceCommandState;
    IFR(m_spObjectSource->GetState(&sourceCommandState));
    IFR(SetCommandState(sourceCommandState,true));

    return S_OK;
}

class CCommandViewModelClassFactory :
    public foundation::ctl::Implements<ICommandViewModelClassFactory, &IID_ICommandViewModelClassFactory>
{
public:
    static ICommandViewModelClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CCommandViewModelClassFactory, ICommandViewModelClassFactory>();
        }
    }

    STDMETHOD(CreateCommandViewModelInstance)(
        _In_ const CommandViewModelCreateParameters *pViewModelImpl,
        _In_opt_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CCommandViewModel::CreateInstance(pViewModelImpl, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.CommandViewModelClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CCommandViewModelClassFactory::GetInstance)
);
