/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:property_changed_event_args_impl.h
****/
#pragma once

#include <foundation/type_info_statics_util.h>
#include <foundation/library/event_args_base.h>
#include <pmod/interfaces/observable_object.h>
#include <foundation/library/logger_macros.h>

#include <pmod/logger_type_info.h>

namespace pmod {

	namespace library {

		BEGIN_DECLARE_INTERFACE(IPropertyChangedWithCallbackEventArgs, foundation::IUnknown, PMOD_API)
		END_DECLARE_INTERFACE()

		//------------------------------------------------------------------------------
		// Class:   _PropertyChangedEventArgsBase
		//
		// Purpose: Base Implementation of the IPropertyChangedEventArgs interface
		//
		//------------------------------------------------------------------------------
		class _PropertyChangedEventArgsBase :
			public foundation::library::_EventArgsBase<pmod::IPropertyChangedEventArgs>
		{
		public:
			void _Initialize(
				_In_ pmod::IObservableObject *pSource,
				UINT32 propertyId)
			{
				_source_ptr = pSource;
				_propertyId = propertyId;
			}

		protected:
			typedef foundation::library::_EventArgsBase<pmod::IPropertyChangedEventArgs> _TBASE;


			STDMETHOD(GetProperty)(_Outptr_ UINT32* pVal)
			{
				*pVal = _propertyId;
				return S_OK;
			}

			STDMETHOD(GetPropertyName)(HSTRING* pValue)
			{
				IFCPTR_ASSERT(pValue);

				if (_propertyId != pmod::Property_All)
				{
					foundation::ComPtr<pmod::IObservableObjectInfo> spPropertyModelInfo;
					IFR_ASSERT(foundation::GetObjectTypeInfo(
						_source_ptr,
						spPropertyModelInfo.GetAddressOf()));

					foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
					IFR(spPropertyModelInfo->GetPropertyById(_propertyId, spPropertyInfo.GetAddressOf()));
					IFR_ASSERT(spPropertyInfo->GetName(pValue));
				}
				else
				{
					*pValue = nullptr;
				}
				return S_OK;
			}

			HRESULT AppendLogParameters(foundation::logger_util::log_parameter_pair_ptr_vector_type& logParameters) override
			{
				IFR_ASSERT(_TBASE::AppendLogParameters(logParameters));

				// property id
				IFR_ASSERT(foundation::logger_util::AppendLogParameter(
					pmod::PropertyChangedEventArgs_LogRecord_PropertyId_Id,
					foundation::pv_util::CreateValue(_propertyId),
					logParameters
					));
				return S_OK;
			}

			UINT32 GetProperty()
			{
				return _propertyId;
			}

			pmod::IObservableObject *GetSource()
			{
				return _source_ptr;
			}
		private:
			foundation::ComPtr<pmod::IObservableObject> _source_ptr;
			UINT32 _propertyId;
		};

		//------------------------------------------------------------------------------
		// Class:   PropertyChangedEventArgsImpl
		//
		// Purpose: Implementation of the IPropertyChangedEventArgs interface
		//
		//------------------------------------------------------------------------------
		class PropertyChangedEventArgsImpl :
			public _PropertyChangedEventArgsBase
		{
		public:
			static HRESULT CreateInstance(
				_In_ pmod::IObservableObject *pSource,
				UINT32 propertyId,
				_In_ foundation::IInspectable *oldValue,
				_In_ foundation::IInspectable *newValue,
				_COM_Outptr_ pmod::IPropertyChangedEventArgs **ppPropertyChangedEventArgs)
			{
				PropertyChangedEventArgsImpl *pT;
				IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
				// Iniatialize the Event Args
				pT->_Initialize(pSource, propertyId, oldValue, newValue);
				*ppPropertyChangedEventArgs = pT;
				return S_OK;
			}
		protected:
			friend class BatchPropertyChangedEventArgsImpl;

			void _Initialize(
				_In_ pmod::IObservableObject *pSource,
				UINT32 propertyId,
				foundation::IInspectable *oldValue,
				foundation::IInspectable *newValue)
			{
				_PropertyChangedEventArgsBase::_Initialize(pSource, propertyId);
				_old_value_ptr = oldValue;
				_new_value_ptr = newValue;
			}
		protected:
			STDMETHOD(GetNewValue)(_COM_Outptr_result_maybenull_ foundation::IInspectable ** pVal)
			{
				return _new_value_ptr.CopyTo(pVal);
			}

			STDMETHOD(GetOldValue)(_COM_Outptr_result_maybenull_ foundation::IInspectable ** pVal)
			{
				return _old_value_ptr.CopyTo(pVal);
			}

			HRESULT AppendLogParameters(foundation::logger_util::log_parameter_pair_ptr_vector_type& logParameters) override
			{
				IFR_ASSERT(_PropertyChangedEventArgsBase::AppendLogParameters(logParameters));

				// Old Value
				IFR_ASSERT(foundation::logger_util::AppendLogParameter(
					pmod::PropertyChangedEventArgs_LogRecord_OldValue_Id,
					_old_value_ptr,
					logParameters
					));
				// New Value
				IFR_ASSERT(foundation::logger_util::AppendLogParameter(
					pmod::PropertyChangedEventArgs_LogRecord_NewValue_Id,
					_new_value_ptr,
					logParameters
					));

				return S_OK;
			}
		protected:
			foundation::InspectablePtr   _old_value_ptr;
			foundation::InspectablePtr   _new_value_ptr;
		};

		//------------------------------------------------------------------------------
		// Class:   PropertyChangedEventArgsWIthCallbackImpl
		//
		// Purpose: Implementation of the IPropertyChangedEventArgs interface with callback
		//
		//------------------------------------------------------------------------------
		class PropertyChangedEventArgsWIthCallbackImpl :
			public _PropertyChangedEventArgsBase,
			public IPropertyChangedWithCallbackEventArgs
		{
		public:
			static HRESULT CreateInstance(
				_In_ pmod::IObservableObject *pSource,
				UINT32 propertyId,
				_Outptr_ pmod::IPropertyChangedEventArgs **ppPropertyChangedEventArgs)
			{
				PropertyChangedEventArgsWIthCallbackImpl *pT;
				IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
				// Iniatialize the Event Args
				pT->_Initialize(pSource, propertyId);
				*ppPropertyChangedEventArgs = pT;
				return S_OK;
			}

		protected:
			HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
			{
				if (iid == pmod::library::IPropertyChangedWithCallbackEventArgs::GetIID())
				{
					*ppInterface = static_cast<IPropertyChangedWithCallbackEventArgs *>(this);
				}
				else
				{
					return pmod::library::_PropertyChangedEventArgsBase::QueryInterfaceImpl(iid, ppInterface);
				}
				return S_OK;
			}

			STDMETHOD(GetNewValue)(_Outptr_ foundation::IInspectable ** ppVal)
			{
				return _Get_property_with_callback(ppVal);
			}

			STDMETHOD(GetOldValue)(_Outptr_ foundation::IInspectable ** ppVal)
			{
				return _Get_property_with_callback(ppVal);
			}
		private:
			HRESULT _Get_property_with_callback(foundation::IInspectable ** ppVal)
			{
				if (this->GetProperty() == pmod::Property_All)
				{
					return foundation::pv_util::CreateEmptyValue(ppVal);
				}
				return GetSource()->GetProperty(this->GetProperty(), ppVal);
			}
		};

		//------------------------------------------------------------------------------
		// Class:   BatchPropertyChangedEventArgsImpl
		//
		// Purpose: Implementation of the IBatchPropertyChangedEventArgs interface
		//
		//------------------------------------------------------------------------------
		typedef std::vector<foundation::ComPtr<pmod::IPropertyChangedEventArgs> > _PropertyChangedEventArgs_Vector_Type;

		class BatchPropertyChangedEventArgsImpl :
			public foundation::library::_EventArgsBase<pmod::IBatchPropertyChangedEventArgs>
		{
		public:
			HRESULT add_property_changed_event_args(pmod::IPropertyChangedEventArgs *pPropertyChangedEventArgs)
			{
				_property_changes_container.push_back(pPropertyChangedEventArgs);
				return S_OK;
			}
			const _PropertyChangedEventArgs_Vector_Type& GetPropertyChanges() { return _property_changes_container; }
		protected:
			typedef foundation::library::_EventArgsBase<pmod::IBatchPropertyChangedEventArgs> _TBASE;


			HRESULT AppendLogParameters(foundation::logger_util::log_parameter_pair_ptr_vector_type& logParameters) override
			{
				IFR_ASSERT(_TBASE::AppendLogParameters(logParameters));

				for (_PropertyChangedEventArgs_Vector_Type::const_iterator
					iter = _property_changes_container.begin();
					iter != _property_changes_container.end();
					++iter)
				{
					pmod::library::PropertyChangedEventArgsImpl *pT = static_cast<pmod::library::PropertyChangedEventArgsImpl *>((*iter).Get());
					pT->AppendLogParameters(logParameters);
				}
				return S_OK;
			}

			// Interface IBatchPropertyChangedEventArgs
			HRESULT STDMETHODCALLTYPE GetPropertyChanges(
				UINT32* size,
				pmod::IPropertyChangedEventArgs ***ppPropertyChangedEventArgs)
			{
				return foundation::RefCountCopyTo(_property_changes_container, size, ppPropertyChangedEventArgs);
			}

		private:
			_PropertyChangedEventArgs_Vector_Type _property_changes_container;
		};
	}
}
