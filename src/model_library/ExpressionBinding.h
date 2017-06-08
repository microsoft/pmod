/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ExpressionBinding.h
****/
#pragma once

#include <pmod/library/mtl.h>
#include "BaseModelSource.h"
#include <map>

class _ExpressionBindingInfo
{
public:
    friend class CExpressionBindingInfo;

    LPCSTR_t GetReferenceToken() const { return _referenceToken; }
    const foundation::string_t GetBindingPath() const;
    pmod::IBindingConverter *GetBindingConverter() const
    {
        return _spBindingConverter;
    }
    foundation::IInspectable *GetBindingConverterParameter() const
    {
        return _spBindingConverterParameter;
    }
private:
    LPCSTR_t    _referenceToken;
    foundation::ComPtr<pmod::IBindingConverter>  _spBindingConverter;
    foundation::InspectablePtr                            _spBindingConverterParameter;
    size_t      _bindingPathPosStart;
    size_t      _bindingPathPosEnd;
};

class CExpressionBindingInfo : public foundation::ctl::ComBase
{
public:
    typedef std::vector<_ExpressionBindingInfo> _BindingInfoFactoryVector_Type;

    static HRESULT CreateInstance(
        _In_ foundation::IExpression *pBindingExpression,
        _COM_Outptr_ CExpressionBindingInfo **ppExpressionBindingInfo);

    HRESULT Evaluate(
        foundation::IResolveTokenDelegate *pResolveTokenDelegate,
        _Outptr_ foundation::IInspectable **ppValue);

    const _BindingInfoFactoryVector_Type& GetBindingFactoryInfos() const
    {
        return m_BindingFactoryInfos;
    }
private:
    HRESULT _Initialize(foundation::IExpression *pExpression);

private:
    _BindingInfoFactoryVector_Type                      m_BindingFactoryInfos;
    foundation::ComPtr<foundation::IExpression>    m_spExpression;
};

class CExpressionBinding : 
    public pmod::library::_MultiSourceBindingClass,
    public pmod::ISourceModel,
    protected foundation::IResolveTokenDelegate,
    protected _ReferenceHolderContainer<pmod::IObservableObject>
{
public:
    static HRESULT CreateInstance(
        _In_ pmod::IObservableObject *pSource,
        _In_ CExpressionBindingInfo *pExpressionBindingInfo,
        _In_ pmod::library::PropertyBindingOptions bindingOptions,
        _COM_Outptr_ pmod::IBindingBase **ppNewInstance);

    static HRESULT CreateInstance(
        _In_ pmod::IObservableObject *pSource,
        _In_ foundation::IExpression *pBindingExpression,
        _In_ pmod::library::PropertyBindingOptions bindingOptions,
        _COM_Outptr_ pmod::IBindingBase **ppNewInstance);

    CExpressionBindingInfo *GetExpressionBindingInfo() { return m_spExpressionBindingInfo;  }

public:
    template <class T>
    static void _DeleteInstance(T *pT)
    {
        delete pT;
    }

protected:
    CExpressionBinding():
        m_bindingOptions(pmod::library::PropertyBindingOptions::None)
    {
    }

    virtual ~CExpressionBinding()
    {
    }
    // Interface ISourceModelBase
    STDMETHOD( GetIsAttached )(bool* value)
    {
        UNREFERENCED_PARAMETER(value);
        return E_NOTIMPL;
    }

    STDMETHOD( GetForceAttached )(bool* value)
    {
        UNREFERENCED_PARAMETER(value);
        return E_NOTIMPL;
    }

    STDMETHOD( SetForceAttached )(bool newValue)
    {
        UNREFERENCED_PARAMETER(newValue);
        return E_NOTIMPL;
    }

    STDMETHOD( GetAttachEnabled )(bool* value)
    {
        UNREFERENCED_PARAMETER(value);
        return E_NOTIMPL;
    }

    STDMETHOD( SetAttachEnabled )(bool newValue)
    {
        UNREFERENCED_PARAMETER(newValue);
        return E_NOTIMPL;
    }
    // Interface ISourceModel
    STDMETHOD(GetSource)(foundation::IObject **ppModel);
    STDMETHOD(SetSource)(foundation::IObject *pModel);

   // Override foundation::ctl::ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;

    // Override 
    STDMETHOD(OnBindingValuesChanged)(_In_ UINT32 size, foundation::IInspectable **pBindingValues,_Outptr_ foundation::IInspectable **ppValue) override;
    
    STDMETHOD(Invoke)(LPCSTR_t referenceToken, foundation::IPropertyValue ** ppValue) override;

private:
    HRESULT SetSourceInternal(_In_ pmod::IObservableObject *pSource);
    HRESULT SetExpressionBindingInfo(CExpressionBindingInfo *pExpressionBindingInfo);

    typedef std::map<LPCSTR_t, pmod::IBindingBase *, foundation::library::StringLessThan> _ReferenceTokenReolverMapType;

    foundation::ComPtr<CExpressionBindingInfo>        m_spExpressionBindingInfo;
    _ReferenceTokenReolverMapType                   m_ReferenceTokenResolverMap;
    pmod::library::PropertyBindingOptions       m_bindingOptions;
};
