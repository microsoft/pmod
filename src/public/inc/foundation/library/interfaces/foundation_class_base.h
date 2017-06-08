/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:foundation_class_base.h
****/
#pragma once

#include <foundation/interfaces/async_operation.h>
#include "dispatcher.h"
#include "disposable.h"
#include "critical_section.h"

namespace foundation
{
    namespace library
    {
        BEGIN_DECLARE_INTERFACE(IFoundationClassBase, IDisposable, FOUNDATION_API)
            STDMETHOD(CountEventHandlers)(_Out_ UINT32 *pValue) = 0;
            STDMETHOD(GetIsObserved)(_Out_ bool *pValue) = 0;
            // If Model has free thread access
            STDMETHOD(GetFoundationOptions)(_Out_ UINT16 *pValue) = 0;
            // Event Dispatcher property
            STDMETHOD(GetEventDispatcher)(_COM_Outptr_result_maybenull_ IDispatcher **ppEventDispatcher) = 0;
            STDMETHOD(SetEventDispatcher)(_In_opt_ IDispatcher *pEventDispatcher) = 0;
			// Critical Section property
			STDMETHOD(GetCriticalSection)(_COM_Outptr_result_maybenull_ ICriticalSection **pp_CriticalSection) = 0;
		END_DECLARE_INTERFACE()
    }
}

