/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_class_ptr.h
****/
#pragma once

#include <foundation/async_operation_ptr.h>
#include "async_operation_class_wrapper.h"
#include "com_class_ptr.h"

namespace foundation
{
    template <class TResult>
    class _AsyncOperationClassHostWrapper :
        public _IClassInterfaceHost<library::IAsyncOperationClass>,
        public _AsyncOperationClassWrapper<_AsyncOperationClassHostWrapper<TResult>, TResult>
    {};

    typedef _ComClassPtr<
        IAsyncOperation,
        _AsyncOperationClassHostWrapper<foundation::IInspectable *>, AsyncOperationInspectablePtr> AsyncOperationClassPtr;

    typedef _ComClassPtr<
        IAsyncOperation_String,
        _AsyncOperationClassHostWrapper<HSTRING>, AsyncOperationStringPtr> AsyncOperationStringClassPtr;

    typedef _ComClassPtr<
        IAsyncOperation_Int32,
        _AsyncOperationClassHostWrapper<INT32>, AsyncOperationInt32Ptr> AsyncOperationInt32ClassPtr;

    typedef _ComClassPtr<
        IAsyncOperation_UInt32,
        _AsyncOperationClassHostWrapper<UINT32>, AsyncOperationUInt32Ptr> AsyncOperationUInt32ClassPtr;

    typedef _ComClassPtr<
        IAsyncOperation_Boolean,
        _AsyncOperationClassHostWrapper<bool>, AsyncOperationBoolPtr> AsyncOperationBoolClassPtr;

}

