/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatch.h
****/
#pragma once

#include <foundation/interfaces/object_dispatch.h>
#include <foundation/library/interfaces/properties_container.h>
#include <foundation/library/base_object_impl.h>
#include <foundation/library/interfaces/object_dispatch_parameters.h>
#include "ObjectDispatchDictionaryAdapter.h"

class CObjectDispatch;
class _CObjectDispatchInfoBase;
typedef ObjectDispatchDictionaryAdapter<foundation::IObjectDispatch, CObjectDispatch> CPropertyDataDictionaryAdapter;

//------------------------------------------------------------------------------
// Class:   CObjectDispatch
//
// Purpose: Implementation of IObjectDispatch Interface
//
//------------------------------------------------------------------------------
typedef foundation::library::_ObjectImplBase<
	foundation::IObjectDispatch,
	foundation::IObjectDispatch,
	foundation::ctl::ComInspectableBase,
	foundation::library::_NoFireEventHandlerContainer
> _ObjectDispatchBase_Type;

class CObjectDispatch :
    public _ObjectDispatchBase_Type
#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
	,
	public _FoundationTrackerBase<true>
#endif
	{
 public:
     HRESULT _Initialize(const foundation::library::ObjectDispatchCreateParameters *pParameters);
	 void _BeginInitialize() {}
	 void _EndInitialize() {}

    static HRESULT CreateInstance(
        _In_ const foundation::library::ObjectDispatchCreateParameters *pParameters,
		_In_opt_ foundation::IInspectable *pOuter,
		_COM_Outptr_ foundation::IInspectable **ppNewInstance);

	 static HRESULT _GetPropertyId(
		 _In_ _CObjectDispatchInfoBase *pPropertyDataInfoBase,
		 HSTRING propertyName,
		 _Out_ UINT32 *propertyId);
	 static HRESULT _GetView(
		 _In_ _CObjectDispatchInfoBase *pPropertyDataInfoBase,
		 foundation::IObjectDispatch *pPropertyData,
		 UINT32* size, foundation::IDictionaryPair ***ppProperties);

protected:
	friend CPropertyDataDictionaryAdapter;
	CObjectDispatch();

    STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue) override;

    // IObjectDispatch Interface
    STDMETHOD( GetProperty )(UINT32 propertyId, foundation::IInspectable** value) ;
    STDMETHOD( SetProperty )(UINT32 propertyId, _In_opt_ foundation::IInspectable *value);
    STDMETHOD( GetProperties )(foundation::IInspectable *pProperties,UINT32 *size, foundation::IPropertyIdPair ***ppProperties);
    STDMETHOD(InvokeMethod)(UINT32 methodId, _In_ UINT32 size, _In_opt_ foundation::IInspectable **parameters, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppResult)
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }

    // IDictionary Delegate Interface
    HRESULT _GetView(UINT32* size,foundation::IDictionaryPair ***ppProperties);
	HRESULT _GetSize(_Out_ UINT32* size);
	HRESULT _Lookup(HSTRING propertyName, foundation::IInspectable** value);
	HRESULT _HasKey(HSTRING propertyName, bool* found);

	HRESULT _Insert(HSTRING propertyName, foundation::IInspectable *value,bool* replaced);
	HRESULT _Remove(HSTRING propertyName);
	HRESULT _Clear();
	HRESULT _Keys(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first);

protected:
   
protected:
    #if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override;

    #endif

private:
    // Our Property Container Ptr
    foundation::ComPtr<foundation::library::IPropertiesContainer> m_spProperties;
	// Our Type Info Ptr
	foundation::ComPtr<_CObjectDispatchInfoBase> m_spTypeInfo;
};
