/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_ptr.h
****/
#pragma once

#include <foundation/object_ptr.h>
#include <foundation/async_operation_connection.h>
#include <foundation/boxing_util.h>

namespace foundation
{
    template <class T>
    inline HRESULT GetResult(
        IAsyncOperation *pAsyncOperation, 
        UINT32 timeout,
        T *pT)
    {
        InspectablePtr valuePtr;
        _IFR_(pAsyncOperation->GetResult(timeout,valuePtr.GetAddressOf()));
        return foundation::pv_util::GetValue(valuePtr, pT);
    }

    template <class T>
    inline HRESULT GetResult(
        IAsyncOperation *pAsyncOperation, 
        UINT32 timeout,
        T& value)
    {
        InspectablePtr valuePtr;
        _IFR_(pAsyncOperation->GetResult(timeout, valuePtr.GetAddressOf()));
        return foundation::GetValue(valuePtr, value);
    }

    template <class IAsyncOperation_t, class T>
    inline HRESULT GetResultType(
        IAsyncOperation_t *pAsyncOperation_t,
        UINT32 timeout,
        T& value)
    {
        IFCPTR_ASSERT(pAsyncOperation_t);
        IAsyncOperation *pAsyncOperation;
        IFR_ASSERT(QueryWeakReference(pAsyncOperation_t, &pAsyncOperation));
        return GetResult(pAsyncOperation, timeout, value);
    }

    template <class T>
    inline HRESULT GetResult(IAsyncOperation *pAsyncOperation, T *pT)
    {
        return GetResult(pAsyncOperation, _InfiniteTimeout, pT);
    }

    template <class T>
    inline HRESULT GetResult(IAsyncOperation *pAsyncOperation, T& value)
    {
        return GetResult(pAsyncOperation, _InfiniteTimeout,value);
    }

    template <class IAsyncOperation_t, class T>
    inline HRESULT GetResult(IAsyncOperation_t *pAsyncOperation, T& value)
    {
        return GetResultType(pAsyncOperation, _InfiniteTimeout, value);
    }

    template <class T,class TPublicInterface>
    class _AsyncOperationBasePtrWrapper : public _ObjectPtrWrapper<T, TPublicInterface>
    {
    public:
        HRESULT GetStatus(ResultStatus *resultStatus) const
        {
            return this->GetClassInterface()->GetStatus(resultStatus);
        }

        ResultStatus GetStatus() const
        {
            ResultStatus resultStatus;
            IGNOREHR(this->GetStatus(&resultStatus));
            return resultStatus;
        }

        ResultStatus Wait(UINT32 timeout = _InfiniteTimeout) const
        {
            ResultStatus resultStatus;
            IGNOREHR(this->GetClassInterface()->Wait(timeout,&resultStatus));
            return resultStatus;
        }

        HRESULT GetError() const
        {
            HRESULT hr_error;
            IGNOREHR(this->GetClassInterface()->GetError(&hr_error));
            return hr_error;
        }

        HRESULT Cancel() const
        {
            return this->GetClassInterface()->Cancel();
        }

#if _HAS_CPP0X

        template<typename _Function>
        HRESULT OnCompleted(const _Function& _Func) const
        {
            return AsyncOperationCompletedConnection::SetSinkOnce(
                this->GetClassInterface(),
                [=](foundation::ICompletedEventArgs *pEventArgs) {
                    return _Func();
            });
        }

        template<typename async_class_ptr_t,typename _Function>
        HRESULT OnCompleted(
            const async_class_ptr_t& async_class_ptr,
            const _Function& _Func) const
        {
            ComPtr<TPublicInterface> async_ptr(this->GetClassInterface());
            return OnCompleted([async_ptr, async_class_ptr, _Func] () -> HRESULT {
                HRESULT hr_error;
                ResultStatus status;
                async_ptr->GetStatus(&status);

                if (status == ResultStatus::Completed)
                {
                    hr_error = _Func();
                }
                else if (status == ResultStatus::Canceled)
                {
                    hr_error = foundation::E_ASYNC_OPERATION_CANCELED;
                }
                else
                {
                    async_ptr->GetError(&hr_error);
                }
                if (FAILED(hr_error))
                {
                    return async_class_ptr.SetCompleted(hr_error, nullptr);
                }
                return S_OK;
            });
        }
#endif
    };

    template <class T, class TPublicInterface>
    class _AsyncOperationPtrWrapper : public _AsyncOperationBasePtrWrapper<T, TPublicInterface>
    {
    public:
        template <class TResult>
        inline HRESULT GetResult(TResult *pT) const
        {
            return foundation::GetResult(this->GetClassInterface(), pT);
        }

        template <class TResult>
        inline HRESULT GetResult(TResult &result) const
        {
            return foundation::GetResult(this->GetClassInterface(), result);
        }

        template <class TResult>
        inline HRESULT GetResult(UINT32 timeout,TResult *pT) const
        {
            return foundation::GetResult(this->GetClassInterface(), timeout, pT);
        }

        template <class TResult>
        inline HRESULT GetResult(UINT32 timeout, TResult &result) const
        {
            return foundation::GetResult(this->GetClassInterface(), timeout, result);
        }
    };


    template <class T>
    class _AsyncOperationHostWrapper :
        public _IInterfaceHost<T>,
        public _AsyncOperationPtrWrapper<_AsyncOperationHostWrapper<T>,T>
    {};

    template <class T>
    class _AsyncOperationPtr:
        public _ObjectPtr<_AsyncOperationHostWrapper<T>>
    {};

    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation>> AsyncOperationInspectablePtr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_String>> AsyncOperationStringPtr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_Int32>> AsyncOperationInt32Ptr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_UInt32>> AsyncOperationUInt32Ptr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_Boolean>> AsyncOperationBoolPtr;

    // array ptr's
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_InspectableArray>> AsyncOperationInspectableArrayPtr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_StringArray>> AsyncOperationStringArrayPtr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_Int32Array>> AsyncOperationInt32ArrayPtr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_UInt32Array>> AsyncOperationUInt32ArrayPtr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_BooleanArray>> AsyncOperationBoolArrayPtr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_UInt8Array>> AsyncOperationUInt8ArrayPtr;

    class _AsyncOperationBaseHostWrapper :
        public _IInterfaceHost<IAsyncOperationBase>,
        public _AsyncOperationBasePtrWrapper<_AsyncOperationBaseHostWrapper, IAsyncOperationBase>
    {};

    typedef _ObjectPtr<_AsyncOperationBaseHostWrapper> AsyncOperationBasePtr;

    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_IObject>> AsyncOperationObjectPtr;
    typedef _ObjectPtr<_AsyncOperationHostWrapper<IAsyncOperation_IDictionary>> AsyncOperationDictionaryPtr;

}

