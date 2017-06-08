/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCustomPropertyProviderAdapter.h
****/

#pragma once

// Note: classic Windows API define GetCurrentTime macro which trigger a warning 
#undef GetCurrentTime

#if defined(_WINDOWS_PHONE_81)
#include "windows.ui.xaml.data.h"
#else
#include <Windows.UI.Xaml.h>
#endif

#include <foundation/ctl/com_library.h>
#include "MoBaseAdapter.h"

#include "..\..\model_library\BatchPropertyChangedHelper.h"

#include <foundation/hstring_wrapper.h>

typedef PropertyChangedEventHandlerBase
    <
        MoEventHandler
        <
            ABI::Windows::UI::Xaml::Data::IPropertyChangedEventHandler,
            IInspectable,
            ABI::Windows::UI::Xaml::Data::IPropertyChangedEventArgs
        >,
        ABI::Windows::UI::Xaml::Data::IPropertyChangedEventHandler,
        IInspectable,
        ABI::Windows::UI::Xaml::Data::IPropertyChangedEventArgs
    > MoCustomPropertyChangedEventHandler;
//------------------------------------------------------------------------------
// Class:   CMoCustomPropertyProviderAdapter
//
// Purpose: Define a base class for a 'ICustomPropertyProvider' implementation for Direct UI
//
//------------------------------------------------------------------------------

class CMoCustomPropertyProviderAdapter :
    public CMoBaseAdapter
        <
        foundation::IObjectDispatch,
        ABI::Windows::UI::Xaml::Data::ICustomPropertyProvider,
        ABI::Windows::UI::Xaml::Data::IPropertyChangedEventHandler,
        ABI::Windows::UI::Xaml::Data::IPropertyChangedEventArgs,
        MoCustomPropertyChangedEventHandler
        >,
    public ABI::Windows::UI::Xaml::Data::INotifyPropertyChanged
{
public:
    // ICustomPropertyProvider
    IFACEMETHOD(GetCustomProperty)(_In_ HSTRING name, _COM_Outptr_ ABI::Windows::UI::Xaml::Data::ICustomProperty **property);

    IFACEMETHOD(GetIndexedProperty)(
            _In_ HSTRING name, 
            _In_ ABI::Windows::UI::Xaml::Interop::TypeName typeName, 
            _COM_Outptr_ ABI::Windows::UI::Xaml::Data::ICustomProperty **property);

    IFACEMETHODIMP GetStringRepresentation(__out HSTRING *stringRepresentation);

    IFACEMETHODIMP get_Type(__out ABI::Windows::UI::Xaml::Interop::TypeName *value);

    // INotifyPropertyChanged
    IFACEMETHOD(add_PropertyChanged)(_In_ ABI::Windows::UI::Xaml::Data::IPropertyChangedEventHandler *handler, __out EventRegistrationToken *token)
    {
        return this->add_Handler(handler,token);
    }
    IFACEMETHOD(remove_PropertyChanged)(_In_ EventRegistrationToken token)
    {
        return this->remove_Handler(token);
    }

    // Interface IModelAdapter
    STDMETHOD(FireAdapterEvent)(
		_In_ foundation::library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		_In_ IUnknown* pEventArgs) override;

    STDMETHOD (QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface)  override
    {
        if (iid == __uuidof(ABI::Windows::UI::Xaml::Data::INotifyPropertyChanged))
        {
            *ppInterface = static_cast<ABI::Windows::UI::Xaml::Data::INotifyPropertyChanged *>(this);
        }
        else
        {
            return __super::QueryAdapterInterface(iid,ppInterface);
        }
        return S_OK;
    }

protected:

    // PropertyChangedEventHandlerBase Override
    HRESULT ToEventArgs(
        _In_ pmod::IPropertyChangedEventArgs *pPropertyChangedEventArgs,
        __out ABI::Windows::UI::Xaml::Data::IPropertyChangedEventArgs **ppEventArgs) override;

private:
    class HStringLessThan:
        public std::binary_function<foundation::HStringPtr, foundation::HStringPtr, bool>
    {
        public:
            bool operator () (_In_ const foundation::HStringPtr& str1, _In_ const  foundation::HStringPtr& str2) const
            {
                return wcscmp(str1.GetRawBuffer(),str2.GetRawBuffer()) < 0;
            }
    };
    typedef std::map<foundation::HStringPtr,foundation::ComPtr<ABI::Windows::UI::Xaml::Data::ICustomProperty>, HStringLessThan> _CustomPropertyMapType;
    _CustomPropertyMapType m_CustomProperties;
};




