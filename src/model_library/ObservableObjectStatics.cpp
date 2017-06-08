/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectStatics.cpp
****/

#include "pch.h"

#include "ObservableObjectStatics.h"
#include <foundation/library/logger_macros.h>
#include <pmod/errors.h>
#include "PropertyInfo.h"
#include "PropertyBinding.h"
#include "ObjectNode.h"


using namespace pmod;
using namespace pmod::library;

// {B1091E86-7F21-47DF-9219-9DDEB13B8AAD}
const GUID pmod::library::IID_IObservableObjectStatics =
{ 0xb1091e86, 0x7f21, 0x47df,{ 0x92, 0x19, 0x9d, 0xde, 0xb1, 0x3b, 0x8a, 0xad } };

inline bool IsModelClassType(_In_ foundation::IObject *pObject)
{
    foundation_assert(pObject);
    return foundation::IsTypeOf<pmod::library::IObjectNodeClass>(pObject);
}

IObservableObjectStatics *CObservableObjectStatics::GetInstance()
{
    return foundation::library::_GetFactoryInstance<CObservableObjectStatics, IObservableObjectStatics>();
}

HRESULT CObservableObjectStatics::GetOrCreateInstance(IObservableObjectStatics **ppPropertyModelStatics)
{
    IFCPTR(ppPropertyModelStatics);
    *ppPropertyModelStatics = GetInstance();
    (*ppPropertyModelStatics)->AddRef();
    return S_OK;
}

CObservableObjectStatics::~CObservableObjectStatics()
{
}

STDMETHODIMP CObservableObjectStatics::GetSource(foundation::IUnknown *pUnk, foundation::IObject **ppSource)
{
    return GetSourceInternal(pUnk,ppSource);
}

STDMETHODIMP CObservableObjectStatics::GetPropertyValue( 
            IObservableObject *pObservableObject,
            foundation::IInspectable *path,
            foundation::IInspectable **ppValue)
{
    return GetPropertyValueInternal(pObservableObject,path,ppValue);
}

STDMETHODIMP CObservableObjectStatics::SetPropertyValue( 
            pmod::IObservableObject *pObservableObject,
            foundation::IInspectable *path,
            foundation::IInspectable *pValue)
{
    IFCPTR_ASSERT(pObservableObject);
    IFCPTR_ASSERT(path);
    IFCPTR_ASSERT(pValue);

    return SetPropertyValueInternal(pObservableObject,path,pValue);
}

STDMETHODIMP CObservableObjectStatics::GetSourceOfType(
            foundation::IUnknown *pUnk,
            REFIID iidInstanceType,
            foundation::InstanceTypeOptions options,
            bool isModelClassType,
            foundation::IObject **ppSource)
{
    return GetSourceOfTypeInternal(pUnk,iidInstanceType,options,isModelClassType,ppSource);
}

HRESULT CObservableObjectStatics::GetSourceInternal(foundation::IUnknown *pUnk,foundation::IObject **ppSource)
{
    foundation_assert(pUnk);
    foundation_assert(ppSource);

    HRESULT hr;

    foundation::ComPtr<ISourceModel> spSourceModel;
    if (spSourceModel != nullptr || SUCCEEDED(hr = pUnk->QueryInterface(ISourceModel::GetIID(), (void **)&spSourceModel)))
    {
        foundation::ComPtr<foundation::IObject> spSource;
        IFR_ASSERT(spSourceModel->GetSource(spSource.GetAddressOf()));
        hr = spSource.CopyTo(ppSource);
    }
    else
    {
        *ppSource = nullptr;
    }
    return hr;
}


HRESULT CObservableObjectStatics::GetSourceOfTypeInternal(
    foundation::IUnknown *pUnk,
    REFIID iidInstanceType,
    foundation::InstanceTypeOptions options,
    bool isModelClassType,
    foundation::IObject **ppSource)
{
    foundation_assert(pUnk);
    foundation_assert(ppSource);

    foundation::ComPtr<foundation::IObject>     spSource;
    foundation::ComPtr<foundation::IUnknown>   spSourceModelUnk(pUnk);

    while(spSourceModelUnk != nullptr)
    {
        spSource.Release();
        // will use Private API to avoid the current 'hack' when View Model Proxies
        // use the 'Source'
        _IFR_(GetSourceInternal(spSourceModelUnk,spSource.GetAddressOf()));
        if(spSource == nullptr)
        {
            // found null source
            return E_POINTER;
        }
        bool result;
        IFR_ASSERT(foundation::IsInstanceOfTypeWithOptions(spSource, iidInstanceType, options, &result));
        if(result && (!isModelClassType || IsModelClassType(spSource)))
        {
            break;
        }
        // continue with the next ISourceModel if possible
        spSourceModelUnk = spSource;
    }
    return spSource.CopyTo(ppSource);
}



HRESULT CObservableObjectStatics::GetPropertyValueInternal(
    _In_ IObservableObject *pObservableObject,
    _In_ foundation::IInspectable *path,
    _COM_Outptr_ foundation::IInspectable **ppValue
    )
{
    foundation_assert(pObservableObject);
    foundation_assert(ppValue);


    IFR(CPropertyBinding::GetValue(pObservableObject, path, ppValue));
    return S_OK;
}

HRESULT CObservableObjectStatics::SetPropertyValueInternal(pmod::IObservableObject *pObservableObject,foundation::IInspectable *path,foundation::IInspectable *pValue)
{
    foundation_assert(pObservableObject);
    foundation_assert(pValue);

    UINT32 propertyId;
    foundation::ComPtr<IObservableObject> spResolvedReference;
    IFR(CPropertyBinding::ResolvePath(pObservableObject,path,&propertyId,spResolvedReference.GetAddressOf()));

    if(spResolvedReference != nullptr)
    {
        IFR(spResolvedReference->SetProperty(propertyId,pValue));
    }
    else
    {
        // TODO: unresolved reference
    }
    return S_OK;
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.ObservableObjectStatics"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObservableObjectStatics::GetInstance)
    );
