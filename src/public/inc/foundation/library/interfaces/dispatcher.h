/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dispatcher.h
****/
#pragma once

#include <foundation/interfaces/async_operation.h>

namespace foundation
{
    namespace library
    {
        template <class TParameter, const IID& iidType>
        struct IDelegateDispatchable_impl : public foundation::IUnknown
        {
            static const IID& GetIID() { return iidType; }
            typedef TParameter  _TParameter_Type;

            STDMETHOD(Invoke)(_In_ TParameter) = 0;
        };

        template <class TDispatchable, const IID& iidType>
        struct IDispatcher_impl : public foundation::IUnknown
        {
            static const IID& GetIID() { return iidType; }
            typedef TDispatchable  _TDispatchable_Type;

            STDMETHOD(HasThreadAccess)(_Always_(_Out_) bool *pValue) = 0;
            STDMETHOD(RunAsync)(_In_ TDispatchable *pDelegateDispatchable, _COM_Outptr_opt_result_maybenull_ IAsyncOperation **ppAsyncAction) = 0;
        };

        //------------------------------------------------------------------------------
        // Interface:   IDelegateDispatchable
        //
        // Purpose: An Interface defining a generic callback invocation with no arguments
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IDelegateDispatchable, IUnknown, FOUNDATION_API)
            STDMETHOD(Invoke)() = 0;
        END_DECLARE_INTERFACE()

        //------------------------------------------------------------------------------
        // Interface:   IDispatcher
        //
        // Purpose: An Interface defining a Dispatcher pattern
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_IID(IDispatcher, FOUNDATION_API);
        typedef IDispatcher_impl<IDelegateDispatchable, IID_IDispatcher> IDispatcher;

    }
}

