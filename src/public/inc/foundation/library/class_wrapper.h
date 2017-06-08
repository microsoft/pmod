/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:class_wrapper.h
****/
#pragma once

#include "class_wrapper_base.h"
#include "class_options.h"
#include <foundation/pv_util.h>
#include <foundation/boxing_util.h>
#include <foundation/library/interfaces/critical_section.h>

namespace foundation {

    template <class T, class TClassInterface>
    class _FoundationClassWrapper : public _ClassWrapperBase<T, TClassInterface>
    {
    public:
        bool IsObserved()
        {
            bool isObserved;
            return FAILED(this->GetClassInterfaceInternal()->GetIsObserved(&isObserved)) ? false : isObserved;
        }

        UINT16 GetFoundationOptions()
        {
            UINT16 options = 0;
            return FAILED(this->GetClassInterfaceInternal()->GetFoundationOptions(&options)) ? 0 : options;
        }

        bool IsFreeThreadApartment()
        {
            return (GetFoundationOptions() &
                foundation::library::FoundationClassOptions_IsFreeThreadApartment) != 0;
        }

        bool IsLocalTypeInfo()
        {
            return (GetFoundationOptions() &
                foundation::library::FoundationClassOptions_IsLocalObjectTypeInfo) != 0;
        }

        UINT32 GetCountEventHandlers()
        {
            UINT32 count = 0;
            this->GetClassInterfaceInternal()->CountEventHandlers(&count);
            return count;
        }

        bool HasEventHandlers()
        {
            return GetCountEventHandlers() > 0;
        }

        HRESULT SetEventDispatcher(foundation::library::IDispatcher *pDispatcher)
        {
            return this->GetClassInterfaceInternal()->SetEventDispatcher(pDispatcher);
        }

        HRESULT GetEventDispatcher(foundation::library::IDispatcher **ppDispatcher)
        {
            return this->GetClassInterfaceInternal()->GetEventDispatcher(ppDispatcher);
        }

		HRESULT GetCriticalSection(foundation::library::ICriticalSection **ppCriticalSection)
		{
			return this->GetClassInterfaceInternal()->GetCriticalSection(ppCriticalSection);
		}

        foundation::library::IDispatcher *GetEventDispatcher()
        {
            foundation::ComPtr<foundation::library::IDispatcher> dispatcher_ptr;
            IGNOREHR(this->GetEventDispatcher(dispatcher_ptr.GetAddressOf()));
            return dispatcher_ptr;
        }

		// access to the internal 'ICriticalSection'
		foundation::library::ICriticalSection *GetCriticalSection()
		{
			foundation::ComPtr<foundation::library::ICriticalSection> critical_section_ptr;
			IGNOREHR(this->GetCriticalSection(critical_section_ptr.GetAddressOf()));
			return critical_section_ptr;
		}

		// lock/unlock methods to be used by derived classes
		HRESULT Lock()
		{
			foundation::library::ICriticalSection *pCriticalSection = this->GetCriticalSection();
			return pCriticalSection == nullptr ? S_OK : pCriticalSection->Lock();
		}
		HRESULT Unlock()
		{
			foundation::library::ICriticalSection *pCriticalSection = this->GetCriticalSection();
			return pCriticalSection == nullptr ? S_OK : pCriticalSection->Unlock();
		}

    };

}

