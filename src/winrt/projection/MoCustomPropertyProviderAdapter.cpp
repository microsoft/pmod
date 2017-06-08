/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCustomPropertyProviderAdapter.cpp
****/

#include "pch.h"

#include "MoCustomPropertyProviderAdapter.h"
#include "MoObservableObjectAdapter.h"
#include <pmod/errors.h>

#include <string>
#include <roapi.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Xaml;

//------------------------------------------------------------------------------
// Class:   CCustomProperty
//
// Purpose: Define a 'ICustomProperty' implementation for Direct UI
//
//------------------------------------------------------------------------------
class CCustomProperty:
    public foundation::ctl::ComInspectableBase,
    public ABI::Windows::UI::Xaml::Data::ICustomProperty
{
public:
    static HRESULT CreateInstance(
    _In_ foundation::IPropertyInfo *pPropertyInfo,
    __out foundation::ctl::ComInspectableObject<CCustomProperty> **ppCustomProperty)
    {
        IFR(foundation::ctl::ComInspectableObject<CCustomProperty>::CreateInstance(ppCustomProperty));
        IFR((*ppCustomProperty)->Initialize(pPropertyInfo));

        return S_OK;
    }
protected:
    CCustomProperty()
    {
    }
public:
        // ICustomProperty 
    IFACEMETHODIMP get_Type(__out ABI::Windows::UI::Xaml::Interop::TypeName *value)
    {
        PropertyType type;
        IFR(m_PropertyInfo->GetPropertyType(&type));

        foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
        IFR(m_PropertyInfo->GetTypeInfo(spModelTypeInfo.GetAddressOf()));

        std::wstring typeName;
        switch(type)
        {
        case PropertyType_UInt8:
            typeName = L"UInt8";
            break;
        case PropertyType_UInt32:
            typeName = L"UInt32";
            break;
        case PropertyType_UInt64:
            typeName = L"UInt64";
            break;
        case PropertyType_Int32:
            typeName = L"Int32";
            break;
        case PropertyType_Int64:
            typeName = L"Int64";
            break;
        case PropertyType_Single:
            typeName = L"Single";
            break;
        case PropertyType_Double:
            typeName = L"Double";
            break;
        case PropertyType_DateTime:
            typeName = L"DateTime";
            break;
        case PropertyType_Char16:
            typeName = L"Char16";
            break;
        case PropertyType_Boolean:
            typeName = L"Boolean";
            break;
        case PropertyType_String:
            typeName = L"String";
            break;
        case PropertyType_Inspectable:
            typeName = L"Object";
            break;
    //            break;
        default:
            return E_NOTIMPL;
        }

        IFR(WindowsCreateString(typeName.data(),(UINT32)typeName.size(),&value->Name));
        value->Kind = ABI::Windows::UI::Xaml::Interop::TypeKind_Primitive;
        return S_OK;
    }

    IFACEMETHODIMP get_Name(__out HSTRING *name)
    {
        IFR(m_PropertyInfo->GetName(name));
        return S_OK;
    }

    IFACEMETHODIMP GetValue(_In_ IInspectable *obj, __out IInspectable **value)
    {
        foundation::ComPtr<pmod::IObservableObject> spPropertyModel;
        IFR(foundation::QueryInterface(obj,spPropertyModel.GetAddressOf()));

        UINT32 propertyId;
        IFR(m_PropertyInfo->GetId(&propertyId));

        IFR(spPropertyModel->GetProperty(propertyId,value));
        return S_OK;
    }

    IFACEMETHODIMP SetValue(_In_ IInspectable *obj, _In_ IInspectable *value)
    {
        foundation::ComPtr<pmod::IObservableObject> spPropertyModel;
        IFR(foundation::QueryInterface(obj,spPropertyModel.GetAddressOf()));

        UINT32 propertyId;
        IFR(m_PropertyInfo->GetId(&propertyId));

        IFR(spPropertyModel->SetProperty(propertyId,value));
        return S_OK;
    }

    IFACEMETHODIMP get_CanRead(__out boolean *value)
    {
        UINT32 flags;
        IFR(m_PropertyInfo->GetFlags(&flags));

        *value = (flags & (UINT32)foundation::PropertyFlagType::CanRead);
        return S_OK;
    }

    IFACEMETHODIMP get_CanWrite(__out boolean *value)
    {
        UINT32 flags;
        IFR(m_PropertyInfo->GetFlags(&flags));

        *value = (flags & (UINT32)foundation::PropertyFlagType::CanWrite);
        return S_OK;
    }

    IFACEMETHODIMP GetIndexedValue(_In_ IInspectable *obj, _In_ IInspectable *index, _COM_Outptr_ IInspectable **value)
    {
        UNREFERENCED_PARAMETER(obj);
        UNREFERENCED_PARAMETER(index);
        UNREFERENCED_PARAMETER(value);

        return E_NOTIMPL;
    }
 
    IFACEMETHODIMP SetIndexedValue(_In_ IInspectable *obj, _In_ IInspectable *value, _In_ IInspectable *index)
    {
        UNREFERENCED_PARAMETER(obj);
        UNREFERENCED_PARAMETER(index);
        UNREFERENCED_PARAMETER(value);

        return E_NOTIMPL;
    }

protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == __uuidof(ICustomProperty))
        {
            *ppInterface = static_cast<ICustomProperty *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
private:
    HRESULT Initialize(foundation::IPropertyInfo *pPropertyInfo)
    {
        m_PropertyInfo = pPropertyInfo;
        return S_OK;
    }
private:
    foundation::ComPtr<foundation::IPropertyInfo> m_PropertyInfo;
};

IFACEMETHODIMP CMoCustomPropertyProviderAdapter::GetCustomProperty(
    _In_ HSTRING name, 
    _COM_Outptr_ ABI::Windows::UI::Xaml::Data::ICustomProperty **property)
{
    _CustomPropertyMapType::iterator iter = m_CustomProperties.find(foundation::HStringPtr(name));

    if(iter != m_CustomProperties.end())
    {
        return iter->second.CopyTo(property);
    }
    else
    {
        foundation::ComPtr<pmod::IObservableObjectInfo> spPropertyModelInfo;
        IFR(foundation::GetObjectTypeInfo(m_pInner, spPropertyModelInfo.GetAddressOf()));
        
        foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
        HRESULT hr = spPropertyModelInfo->GetPropertyByName(name,spPropertyInfo.GetAddressOf());
        IFHR(hr);
        if(hr == S_OK)
        {
            foundation::ctl::ComInspectableObject<CCustomProperty> *pCustomProperty;
            IFR(CCustomProperty::CreateInstance(spPropertyInfo,&pCustomProperty));
            *property = pCustomProperty;
            m_CustomProperties[foundation::HStringPtr(name)] = pCustomProperty;
        }
        else if(hr == S_FALSE)
        {
            // we haven't found the property in our model
            // uncomment next line if you want to Assert when this thing happen
            //ASSERT(false);
            hr = foundation::E_PROPERTY_NOT_FOUND;
            IFHR(hr);
        }
        return hr;
    }
}

IFACEMETHODIMP CMoCustomPropertyProviderAdapter::GetIndexedProperty(
            _In_ HSTRING name, 
            _In_ ABI::Windows::UI::Xaml::Interop::TypeName typeName, 
            _COM_Outptr_ ABI::Windows::UI::Xaml::Data::ICustomProperty **property)
{
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(typeName);
    UNREFERENCED_PARAMETER(property);

    return E_NOTIMPL;
}


IFACEMETHODIMP CMoCustomPropertyProviderAdapter::GetStringRepresentation(__out HSTRING *stringRepresentation)
{ 
    UNREFERENCED_PARAMETER(stringRepresentation);

    return E_NOTIMPL;
}

IFACEMETHODIMP CMoCustomPropertyProviderAdapter::get_Type(__out ABI::Windows::UI::Xaml::Interop::TypeName *value)
{ 
    std::wstring typeName = L"Microsoft.PropertyModel.IObservableObject";
    IFR(WindowsCreateString(typeName.data(),(UINT32)typeName.size(),&value->Name));
    value->Kind = ABI::Windows::UI::Xaml::Interop::TypeKind_Metadata;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoCustomPropertyProviderAdapter::FireAdapterEvent(
	_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
	_In_ IUnknown* pEventArgs)
{
    foundation_assert(pEventArgs);

    IInspectable *pSender = static_cast<foundation::ctl::ComInspectableBase *>(this);

    foundation::ComPtr<pmod::IPropertyChangedEventArgs> spPropertyChangedEventArgs;
    foundation::ComPtr<pmod::IBatchPropertyChangedEventArgs> spBatchPropertyChangedEventArgs;
    if(SUCCEEDED(foundation::QueryInterface(pEventArgs,spBatchPropertyChangedEventArgs.GetAddressOf())))
    {
        this->FireEventWithInvoke<MoCustomPropertyChangedEventHandler>
            (pFireEventHandlerContainer,pSender,spBatchPropertyChangedEventArgs.Get());
    }
    else  if(SUCCEEDED(foundation::QueryInterface(pEventArgs,spPropertyChangedEventArgs.GetAddressOf())))
    {
        struct _TEventArgsAdapter
        {
            static HRESULT ToEventArgs(
                IInspectable *pSender,
                _In_ pmod::IPropertyChangedEventArgs *pPropertyChangedEventArgs,
                __out ABI::Windows::UI::Xaml::Data::IPropertyChangedEventArgs **ppEventArgs)
            {
                foundation::ctl::ComInspectableBase *pThis = static_cast<foundation::ctl::ComInspectableBase *>(pSender);
                return (static_cast<CMoCustomPropertyProviderAdapter *>(pThis))->ToEventArgs(pPropertyChangedEventArgs,ppEventArgs);
            }
        };
        this->FireEventWithAdapter<_TEventArgsAdapter>
            (pFireEventHandlerContainer, pSender,spPropertyChangedEventArgs.Get());

    }
    return S_OK;
}

HRESULT CMoCustomPropertyProviderAdapter::ToEventArgs(
    _In_ pmod::IPropertyChangedEventArgs *pPropertyChangedEventArgs,
    __out ABI::Windows::UI::Xaml::Data::IPropertyChangedEventArgs **ppEventArgs)
{
    UINT32 propertyId;
    IFR_ASSERT(pPropertyChangedEventArgs->GetProperty(&propertyId));

    // Win8 HSTRING handle reference
    foundation::HStringPtr propertyName;

    IFR(pPropertyChangedEventArgs->GetPropertyName(propertyName.GetAddressOf()));

    // Create the Windows::UI::Xaml::Data::IPropertyChangedEventArgs instance
    foundation::ComPtr<ABI::Windows::UI::Xaml::Data::IPropertyChangedEventArgsFactory> spPropertyChangedEventArgsFactory;

    IFR(Windows::Foundation::GetActivationFactory(
        foundation::HStringRef(RuntimeClass_Windows_UI_Xaml_Data_PropertyChangedEventArgs),
        spPropertyChangedEventArgsFactory.GetAddressOf()));

    IFR(spPropertyChangedEventArgsFactory->CreateInstance(propertyName,nullptr,nullptr,ppEventArgs));
    return S_OK;
}


