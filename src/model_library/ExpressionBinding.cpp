/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ExpressionBinding.cpp
****/

#include "pch.h"
#include "ExpressionBinding.h"
#include "FactoryBindingConverters.h"

#include <pmod/library/library_util.h>
#include <pmod/library/interfaces/expression_binding_class.h>
#include <foundation/library/string_util.h>
#include <foundation/library/logger_macros.h>
#include <foundation/library/com_ptr_singleton.h>

// {D07D369F-BC81-4850-A6F5-C1121A23F2AD}
const GUID pmod::library::IID_IExpressionBindingClassFactory =
{ 0xd07d369f, 0xbc81, 0x4850,{ 0xa6, 0xf5, 0xc1, 0x12, 0x1a, 0x23, 0xf2, 0xad } };

using namespace pmod;
using namespace foundation;
using namespace foundation::library;
using namespace pmod::library;

const string_t _ExpressionBindingInfo::GetBindingPath() const
{
    if (_bindingPathPosStart == -1)
    {
        return _referenceToken;
    }
    return string_t(_referenceToken).substr(
        _bindingPathPosStart,
        _bindingPathPosEnd - _bindingPathPosStart );
}

HRESULT CExpressionBindingInfo::_Initialize(IExpression *pExpression)
{
    this->m_spExpression = pExpression;

    foundation::HStringArrayWrapper referenceTokens;
    IFR_ASSERT(pExpression->GetReferenceTokens(referenceTokens.GetSizeAddressOf(), referenceTokens.GetBufferAddressOf()));

    for (UINT32 index = 0; index < referenceTokens.GetSize(); ++index)
    {
        LPCSTR_t referenceToken = _pal_GetStringRawBuffer(referenceTokens[index], nullptr);

        _ExpressionBindingInfo expressionBindingInfo;
        expressionBindingInfo._bindingPathPosStart = -1;
        expressionBindingInfo._bindingPathPosEnd = -1;
        expressionBindingInfo._referenceToken = referenceToken;

        if (referenceToken[0] == U('$'))
        {
            string_t referenceToken_str = referenceToken;

            size_t pos_start = referenceToken_str.find_first_of(U('('));
            size_t pos_end = referenceToken_str.find_last_of(U(')'));
            if (pos_start == string_t::npos || pos_end == string_t::npos)
            {
                return E_UNEXPECTED;
            }
            string_t converter_name = referenceToken_str.substr(1, pos_start - 1);
            IFR(CFactoryBindingConverters::GetBindingConverter(
                converter_name.c_str(),
                expressionBindingInfo._spBindingConverter.GetAddressOf()));

            std::vector<string_t> converter_params =
                foundation::library::string_util::split_t(
                referenceToken_str.substr(pos_start + 1, pos_end - pos_start - 1),
                U(','));
            foundation_assert(converter_params.size() > 0);
            expressionBindingInfo._bindingPathPosStart = pos_start + 1;
            expressionBindingInfo._bindingPathPosEnd = pos_start + 1 + converter_params[0].size();

            if (converter_params.size() > 1)
            {
                // first parameter is the binding path
                converter_params.erase(converter_params.begin());

                // we have some parameters to pass into the binding converter
                if (converter_params.size() == 1)
                {
                    // just one parameter
                    expressionBindingInfo._spBindingConverterParameter =
                        pv_util::CreateValue(converter_params[0].c_str());
                }
                else
                {
                    std::vector<foundation::InspectablePtr> parameters;
                    for (std::vector<string_t>::const_iterator iter = converter_params.begin();
                        iter != converter_params.end();
                        ++iter)
                    {
                        parameters.push_back(pv_util::CreateValue((*iter).c_str()));
                    }
                    expressionBindingInfo._spBindingConverterParameter =
                        pv_util::CreateValue(
                        (UINT32)parameters.size(),
                        (foundation::IInspectable ** *)(parameters.front().GetAddressOfPtr()));
                }
            }

        }
        m_BindingFactoryInfos.push_back(expressionBindingInfo);
    }
    return S_OK;
}

HRESULT CExpressionBindingInfo::CreateInstance(
    _In_ IExpression *pBindingExpression,
    _COM_Outptr_ CExpressionBindingInfo **ppExpressionBindingInfo)
{
    foundation_assert(pBindingExpression);
    foundation_assert(ppExpressionBindingInfo);

    foundation::ctl::ComObject<CExpressionBindingInfo>::CreateInstance(ppExpressionBindingInfo);
    (*ppExpressionBindingInfo)->_Initialize(pBindingExpression);
    return S_OK;
}

HRESULT CExpressionBindingInfo::Evaluate(
    IResolveTokenDelegate *pResolveTokenDelegate,
    _Outptr_ foundation::IInspectable **ppValue)
{
    foundation_assert(m_spExpression != nullptr);

    foundation::ComPtr<foundation::IPropertyValue> spResult;
    IFR(m_spExpression->Evaluate(pResolveTokenDelegate, spResult.GetAddressOf()));
    *ppValue = spResult.Detach();
    return S_OK;
}

HRESULT CExpressionBinding::CreateInstance(
    _In_ IObservableObject *pSource,
    _In_ IExpression *pBindingExpression,
    _In_ PropertyBindingOptions bindingOptions,
    _COM_Outptr_ IBindingBase **ppNewInstance)
{
    foundation::ComPtr<CExpressionBindingInfo> spExpressionBindingInfo;
    IFR_ASSERT(CExpressionBindingInfo::CreateInstance(pBindingExpression, spExpressionBindingInfo.GetAddressOf()));

    return CreateInstance(pSource, spExpressionBindingInfo, bindingOptions, ppNewInstance);
}

HRESULT CExpressionBinding::CreateInstance(
                    _In_ IObservableObject *pSource,
                    _In_ CExpressionBindingInfo *pExpressionBindingInfo,
                    _In_ PropertyBindingOptions bindingOptions,
                    _COM_Outptr_ IBindingBase **ppNewInstance)
{
    foundation_assert(pExpressionBindingInfo);
    IFCPTR(pExpressionBindingInfo);

    CExpressionBinding *pExpressionBinding = nullptr;
    IFR_ASSERT(foundation::library::CreateClassWithInner(
        foundation_GUID_NULL,
        MultiSourceBindingOptions::None,
        &pExpressionBinding));
    pExpressionBinding->SetIsWeakReference(IsOptionEnabled(bindingOptions,PropertyBindingOptions::UseSourceWeakReference));
    pExpressionBinding->m_bindingOptions = bindingOptions;

    pExpressionBinding->SetExpressionBindingInfo(pExpressionBindingInfo);
    if(pSource)
    {
        IFR(pExpressionBinding->SetSourceInternal(pSource));
    }

    *ppNewInstance = pExpressionBinding->GetInterface();
    return S_OK;
}

STDMETHODIMP CExpressionBinding::GetSource(foundation::IObject **ppModel)
{
    IFCPTR(ppModel);

    foundation::ComPtr<IObservableObject> spModel;
    IFR(this->ResolveReferenceInternal(*spModel.GetAddressOf()));
    *ppModel = spModel.Detach();
    return S_OK;
}

STDMETHODIMP CExpressionBinding::SetSource(foundation::IObject *pModel)
{
    foundation::ComPtr<IObservableObject> spSource;
    if(pModel)
    {
        IFR_ASSERT(foundation::QueryInterface(pModel, spSource.GetAddressOf()));
    }
    return this->SetSourceInternal(spSource);
}

HRESULT CExpressionBinding::SetExpressionBindingInfo(CExpressionBindingInfo *pExpressionBindingInfo)
{
    m_spExpressionBindingInfo = pExpressionBindingInfo;
    for (CExpressionBindingInfo::_BindingInfoFactoryVector_Type::const_iterator
        iter = pExpressionBindingInfo->GetBindingFactoryInfos().begin();
        iter != pExpressionBindingInfo->GetBindingFactoryInfos().end();
    ++iter)
    {
        string_t bindingPath = (*iter).GetBindingPath();

        foundation::ComPtr<IPropertyBinding> spPropertyBinding;
        IFR_ASSERT(pmod::library::CreateBinding(
            nullptr,
            bindingPath.c_str(),
            m_bindingOptions,
            spPropertyBinding.GetAddressOf()));
        spPropertyBinding->SetConverter((*iter).GetBindingConverter());
        spPropertyBinding->SetConverterParameter((*iter).GetBindingConverterParameter());

        IFR_ASSERT(this->AddSource(spPropertyBinding));

        this->m_ReferenceTokenResolverMap[(*iter).GetReferenceToken()] = spPropertyBinding;
    }
    return S_OK;
}

HRESULT CExpressionBinding::SetSourceInternal(_In_ pmod::IObservableObject *pSource)
{
    IFR_ASSERT(this->AttachInternal(pSource));
    if(pSource != nullptr)
    {
        foundation::ArrayRefCountWrapper<foundation::IObject> sources;
        IFR_ASSERT(this->_pSourceModel->GetSources(sources.GetSizeAddressOf(), sources.GetBufferAddressOf()));
        for (UINT32 index = 0; index < sources.GetSize(); ++index)
        {
            foundation::ComPtr<ISourceModel> spSourceModel;
            IFR_ASSERT(foundation::QueryInterface(sources[index], spSourceModel.GetAddressOf()));
            IFR_ASSERT(spSourceModel->SetSource(pSource));
        }
    }
    return S_OK;
}

HRESULT CExpressionBinding::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    HRESULT hr = S_OK;
    if (iid == ISourceModel::GetIID())
    {
        *ppInterface = static_cast<ISourceModel *>(this);
    }
    else if (iid == IMultiSourceModel::GetIID())
    {
        hr = E_NOINTERFACE;
    }
    else
    {
        hr = pmod::library::_MultiSourceBindingClass::QueryInterfaceImpl(iid,ppInterface);
    }
    return hr;
}

STDMETHODIMP CExpressionBinding::Invoke(LPCSTR_t referenceToken, foundation::IPropertyValue ** ppValue)
{
    HRESULT hr;

    _ReferenceTokenReolverMapType::iterator iter = m_ReferenceTokenResolverMap.find(referenceToken);
    if(iter != m_ReferenceTokenResolverMap.end())
    {
        foundation::InspectablePtr spValue;
        IFHR((*iter).second->GetValue(spValue.GetAddressOf()));
        if(spValue != nullptr)
        {
            // the API require an IPropertyValue interface
            hr = pv_util::CastToPropertyValue(spValue, ppValue);
            if (FAILED(hr))
            {
                // the binding value may be returning a IObservableObject interface and so
                // we can ask for the default property
                foundation::ComPtr<IObservableObject> propert_model_ptr;
                if (SUCCEEDED(foundation::QueryInterface(spValue, propert_model_ptr.GetAddressOf())))
                {
                    // TODO: if TypeInfo support default property in the schema we request here
                    const UINT32 default_property_id = 1;

                    IFHR(propert_model_ptr->GetProperty(
                        default_property_id,
                        spValue.ReleaseAndGetAddressOf()));

                    IFHR(pv_util::CastToPropertyValue(spValue, ppValue));
                }
            }
        }
        else
        {
            IFR_ASSERT(pv_util::CreateEmptyValue((foundation::IInspectable **)ppValue));
        }
    }
    else
    {
        foundation_assert(false);
        // TODO: better error here
        return E_UNEXPECTED;
    }
    return S_OK;
}

HRESULT CExpressionBinding::OnBindingValuesChanged(
        _In_ UINT32 size,
        foundation::IInspectable **pBindingValues,
        _Outptr_ foundation::IInspectable **ppValue)
{
    foundation_assert(m_spExpressionBindingInfo != nullptr);
    UNREFERENCED_PARAMETER(size);
    UNREFERENCED_PARAMETER(pBindingValues);

    return m_spExpressionBindingInfo->Evaluate(static_cast<IResolveTokenDelegate *>(this), ppValue);
}


#if PMOD_LIB_LIBRARY_STATIC_LIB
EXTERN_C void _ModelLibraryInstanceInitializerStub();
#endif

class CExpressionBindingClassFactory :
    public foundation::ctl::Implements<IExpressionBindingClassFactory, &IID_IExpressionBindingClassFactory>
{
public:
    static IExpressionBindingClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CExpressionBindingClassFactory, IExpressionBindingClassFactory>();
        }
    }

    STDMETHODIMP CreateExpressionBindingInstance(
        _In_ IObservableObject *pSource,
        _In_ foundation::IExpression *pBindingExpression,
        _In_ PropertyBindingOptions bindingOptions,
        _Outptr_ IBindingBase **ppNewInstance) override
    {
        return CExpressionBinding::CreateInstance(pSource, pBindingExpression, bindingOptions, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.ExpressionBindingClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CExpressionBindingClassFactory::GetInstance)
);
