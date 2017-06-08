/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:event_model_event_args_impl.h
****/
#pragma once

#include <foundation/library/event_args_base.h>
#include <pmod/interfaces/observable_object.h>
#include <pmod/logger_type_info.h>

namespace pmod {

	namespace library {
		//------------------------------------------------------------------------------
		// Class:   EventModelEventArgsImpl
		//
		// Purpose: Implementation of the IEventModelEventArgs interface
		//
		//------------------------------------------------------------------------------
		class EventModelEventArgsImpl :
			public foundation::library::_EventArgsBase<pmod::IEventModelEventArgs>
		{
		public:
			static HRESULT CreateInstance(
				UINT32 eventId,
				_In_ foundation::IInspectable *pEventArgs,
				_Outptr_ pmod::IEventModelEventArgs **ppEventModelEventArgs)
			{
				EventModelEventArgsImpl *pT;
				IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
				// _Initialize the Event Args
				pT->_Initialize(eventId, pEventArgs);
				*ppEventModelEventArgs = pT;

				return S_OK;
			}
		private:
			typedef foundation::library::_EventArgsBase<pmod::IEventModelEventArgs> _TBASE;

			void _Initialize(UINT32 eventId,
				foundation::IInspectable *pEventArgs)
			{
				_event_id = eventId;
				_event_args_ptr = pEventArgs;
			}

		protected:
			STDMETHOD(GetEvent)(_Outptr_ UINT32* pVal)
			{
				*pVal = _event_id;
				return S_OK;
			}

			STDMETHOD(GetEventArgs)(_Outptr_ foundation::IInspectable ** pVal)
			{
				return _event_args_ptr.CopyTo(pVal);
			}

			HRESULT AppendLogParameters(foundation::logger_util::log_parameter_pair_ptr_vector_type& logParameters) override
			{
				IFR_ASSERT(_TBASE::AppendLogParameters(logParameters));

				// event id
				IFR_ASSERT(foundation::logger_util::AppendLogParameter(
					pmod::EventModelEventArgs_LogRecord_EventId_Id,
					foundation::pv_util::CreateValue(_event_id),
					logParameters
					));
				// event args
				IFR_ASSERT(foundation::logger_util::AppendLogParameter(
					pmod::EventModelEventArgs_LogRecord_EventArgs_Id,
					_event_args_ptr,
					logParameters
					));
				return S_OK;
			}

		private:
			foundation::InspectablePtr   _event_args_ptr;
			UINT32 _event_id;
		};
	}
}
