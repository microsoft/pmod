/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:interface_adapter.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>
#include "fire_event.h"

namespace foundation {
	namespace library
	{

		////////////////////// Interface Adapter support ////////////////////////////
		BEGIN_DECLARE_INTERFACE(IInterfaceAdapterOverrides, IUnknown, FOUNDATION_API)
			STDMETHOD(OnAdapterAddHandler)(_In_ IUnknown* pSender, _In_ IUnknown* pHandler) = 0;
			STDMETHOD(OnAdapterRemoveHandler)(_In_ IUnknown* pSender, _In_ IUnknown* pHandler) = 0;
		END_DECLARE_INTERFACE()

		BEGIN_DECLARE_INTERFACE(IInterfaceAdapter, IUnknown, FOUNDATION_API)
			STDMETHOD(QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface) = 0;
			STDMETHOD(FireAdapterEvent)(
				_In_ library::IFireEventHandlerContainer *pFireEventHandlerContainer,
				_In_ IUnknown* pEventArgs) = 0;
			STDMETHOD(HasEventHandlers)(_Out_ bool *pValue) = 0;
			STDMETHOD(OnHostObservedChanged)(_In_ bool isObserved) = 0;
			STDMETHOD_(IUnknown *,_GetInnerInterface)() = 0;
		END_DECLARE_INTERFACE()

		BEGIN_DECLARE_INTERFACE(IInterfaceAdapterFactory, IUnknown, FOUNDATION_API)
			STDMETHOD(TryCreateInterfaceAdapter)(
				_In_ REFIID riid_outer,
				_In_ REFIID riid,
				_In_ IInspectable *pOuter,
				_In_ IInspectable *pInner,
				_COM_Outptr_ IInspectable **ppInner) = 0;
		END_DECLARE_INTERFACE()

			BEGIN_DECLARE_INTERFACE(IInterfaceAdapterFactoryHost, IUnknown, FOUNDATION_API)
			STDMETHOD(QueryCreateInterfaceAdapter)(
				_In_ IInspectable *pOuter,
				_In_ IInspectable *pInner,
				_In_ REFIID riid_outer,
				_In_ REFIID riid,
				_COM_Outptr_ IInspectable **ppModelAdapterInnerUnk) = 0;
		STDMETHOD(RegisterInterfaceAdapterFactory)(_In_ IInterfaceAdapterFactory *pModelAdapterFactory) = 0;
		STDMETHOD(UnregisterInterfaceAdapterFactory)(_In_ IInterfaceAdapterFactory *pModelAdapterFactory) = 0;
		END_DECLARE_INTERFACE()
	}

}

