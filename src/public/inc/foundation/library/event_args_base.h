/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:event_args_base.h
****/
#pragma once

#include "base_adapter_host.h"
#include "logger_util.h"

#include <foundation/ctl/com_library.h>
#include <foundation/array_wrapper.h>

namespace foundation
{
	namespace library
	{

		template<class TEventArgs>
		class _EventArgsBase :
			public TEventArgs,
			public _DefaultAdapterHost<TEventArgs, foundation::ctl::ComInspectableBase>,
			public ILoggerSerialize
		{
		private:
			typedef _DefaultAdapterHost<TEventArgs, foundation::ctl::ComInspectableBase> _TBASE;
		protected:

			_EventArgsBase()
			{
			}

			HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
			{
				if (iid == TEventArgs::GetIID())
				{
					*ppInterface = static_cast<TEventArgs *>(this);
				}
				else if (iid == ILoggerSerialize::GetIID())
				{
					*ppInterface = static_cast<ILoggerSerialize *>(this);
				}
				else
				{
					return _TBASE::QueryInterfaceImpl(iid, ppInterface);
				}
				return S_OK;
			}

			// Interface library::ILoggerSerialize
			STDMETHOD(CreateLogParameters)(
				UINT32 *size, 
				foundation::ILogParameterPair ***ppLogParameterPair)
			{
				foundation::logger_util::log_parameter_pair_ptr_vector_type logParameters;
				IFR_ASSERT(AppendLogParameters(logParameters));

				return foundation::RefCountCopyTo(
					(UINT32)logParameters.size(),
					foundation::GetVectorBufferPtr(logParameters),
					size,
					ppLogParameterPair);
				return S_OK;
			}

			// Virtual methods to override
			virtual HRESULT AppendLogParameters(foundation::logger_util::log_parameter_pair_ptr_vector_type& logParameters)
			{
				IFR_ASSERT(foundation::logger_util::AppendLogParameter(
					foundation::BaseEventArgs_LogRecord_TypeIID_Id,
					foundation::pv_util::CreateValue(TEventArgs::GetIID()),
					logParameters
					));
				return S_OK;
			}

		};
	}
}

