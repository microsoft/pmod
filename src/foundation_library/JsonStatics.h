/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JsonStatics.h
****/
#pragma once
#include <foundation/library/base_adapter_host.h>
#include <foundation/interfaces/stream_provider.h>
#include <foundation/library/interfaces/json_statics.h>

//------------------------------------------------------------------------------
// class CJsonStatics
// Singleton to implement the exports Json Statics methods
//------------------------------------------------------------------------------
class CJsonStatics :
    public foundation::library::_DefaultAdapterHostType<foundation::library::IJsonStatics, foundation::ctl::ComInspectableBase, CJsonStatics>,
	public foundation::library::IJsonStatics
{
public:
	static foundation::library::IJsonStatics *GetInstance();
	HRESULT STDMETHODCALLTYPE Parse(
		_In_ const char *json_str,
        /*_COM_Outptr_opt_result_maybenull_*/ foundation::IDictionary **ppJsonError,
		_COM_Outptr_ foundation::IInspectable **ppInspectable) override;

	HRESULT STDMETHODCALLTYPE ParseFromFile(
		_In_ const char *file_path,
        /*_COM_Outptr_opt_result_maybenull_*/ foundation::IDictionary **ppJsonError,
		_COM_Outptr_ foundation::IInspectable **ppInspectable) override;

	HRESULT STDMETHODCALLTYPE ToJson(
		_In_ foundation::IInspectable *pInspectable,
		_Out_ HSTRING *json) override;

	HRESULT STDMETHODCALLTYPE FromJson(
		_In_ HSTRING json,
		_COM_Outptr_ foundation::IInspectable **ppInspectable) override;
	HRESULT STDMETHODCALLTYPE GetStreamProvider(
		_COM_Outptr_ foundation::IStreamProvider **ppStreamProvider) override;

protected:
	typedef foundation::library::_DefaultAdapterHostType<IJsonStatics, foundation::ctl::ComInspectableBase, CJsonStatics> _Base_Type;
	HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;

};

//------------------------------------------------------------------------------
// class CJsonStreamProvider
// Singleton to implement the json stream provider
//------------------------------------------------------------------------------
typedef foundation::library::_DefaultAdapterHost<foundation::IStreamProvider, foundation::ctl::ComInspectableBase> _CJsonStreamProvider_Base;
class CJsonStreamProvider :
    public _CJsonStreamProvider_Base,
    public foundation::IStreamProvider
{
public:
    static foundation::IStreamProvider *GetInstance();

protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;
    STDMETHOD(Serialize)(foundation::IInspectable *pValue, foundation::IInspectable **ppValue) override;
    STDMETHOD(Deserialize)(foundation::IInspectable *pValue, foundation::IInspectable **ppValue) override;
};
