/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:can_execute_changed_event_args_impl.h
****/
#pragma once

#include <pmod/interfaces.h>
#include <pmod/logger_type_info.h>
#include <foundation/library/event_args_base.h>

namespace pmod {

    namespace library {
        typedef foundation::library::_EventArgsBase<ICanExecuteChangedEventArgs> _CanExecuteChangedEventArgsImplBaseType;

        class CanExecuteChangedEventArgsImpl :
            public _CanExecuteChangedEventArgsImplBaseType
        {
        public:
            static HRESULT CreateInstance(
                UINT32 oldState,
                UINT32 newState,
                ICanExecuteChangedEventArgs **ppCanExecuteChangedEventArgs)
            {
                CanExecuteChangedEventArgsImpl *pT;
                IFR_ASSERT(foundation::ctl::inspectable::CreateInstance(&pT));
                // Iniatialize the Event Args
                pT->_Initialize(oldState, newState);
                *ppCanExecuteChangedEventArgs = pT;
                return S_OK;
            }
        private:
            void _Initialize(int oldState, int newState)
            {
                m_OldState = oldState;
                m_NewState = newState;
            }
        protected:
            STDMETHOD(GetOldState)(UINT32* pState)
            {
                *pState = m_OldState;
                return S_OK;
            }
            STDMETHOD(GetNewState)(UINT32* pState)
            {
                *pState = m_NewState;
                return S_OK;
            }
            HRESULT AppendLogParameters(foundation::logger_util::log_parameter_pair_ptr_vector_type& logParameters) override
            {
                IFR_ASSERT(_CanExecuteChangedEventArgsImplBaseType::AppendLogParameters(logParameters));

                // OldState
                IFR_ASSERT(foundation::logger_util::AppendLogParameter(
                    CanExecuteChangedEventArgs_LogRecord_OldState_Id,
                    foundation::pv_util::CreateValue(m_OldState),
                    logParameters
                    ));
                // NewState
                IFR_ASSERT(foundation::logger_util::AppendLogParameter(
                    CanExecuteChangedEventArgs_LogRecord_NewState_Id,
                    foundation::pv_util::CreateValue(m_NewState),
                    logParameters
                    ));
                return S_OK;
            }

        private:
            UINT32 m_OldState;
            UINT32 m_NewState;
        };
    }
}
