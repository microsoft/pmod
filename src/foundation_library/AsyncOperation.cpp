/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperation.cpp
****/
#include "pch.h"

#include "AsyncOperation.h"
#include <foundation/library/event_args_base.h>
#include "CreateFoundationHelper.h"
#include "LogCategory.h"
#include "Logger.h"

#include <foundation/library/com_ptr_singleton.h>

#include <foundation/library/logger_macros.h>
#include <pal/library/signal.h>

using namespace foundation;

// {19E9AA54-F6F0-481A-B86A-2CAA555DE6BD}
const GUID library::IID_IAsyncOperationFactory =
{ 0x19e9aa54, 0xf6f0, 0x481a,{ 0xb8, 0x6a, 0x2c, 0xaa, 0x55, 0x5d, 0xe6, 0xbd } };

class CProgressEventArgs : public foundation::library::_EventArgsBase<IProgressEventArgs>
{
public:
    static HRESULT CreateInstance(_In_opt_ IInspectable *pProgressValue, _COM_Outptr_ IProgressEventArgs **ppProgressEventArgs)
    {
        CProgressEventArgs *pProgressEventArgs = nullptr;
        IFR_ASSERT(ctl::ComInspectableObject<CProgressEventArgs>::CreateInstance(&pProgressEventArgs));
        pProgressEventArgs->_progressValue = pProgressValue;
        *ppProgressEventArgs = static_cast<IProgressEventArgs *>(pProgressEventArgs);
        return S_OK;
    }
protected:
    STDMETHOD(GetProgressValue)(IInspectable **ppValue)
    {
        return _progressValue.CopyTo(ppValue);
    }

private:
    foundation::InspectablePtr _progressValue;
};

const CLogCategory*CAsyncOperationLogFactory::GetLogCategory()
{
    static library::_ComPtrSingleton<const CLogCategory, foundation::_NoRefCountPtrImpl> _asyncOperationModelLogCategoryInstance;

    return _asyncOperationModelLogCategoryInstance._GetInstance([]() {
        return CLogger::GetCategory(
            Foundation_CategoryEnum::TypeId,
            Object_CategoryEnum::TypeId,
            AsyncOperation_CategoryEnum::TypeId,
            -1);
    });
}

HRESULT CAsyncOperation::_Initialize(const library::async_operation_create_parameters *modelImpl)
{
    IFR_ASSERT(_AsyncOperation_BaseType::_Initialize(modelImpl));
    m_IsCancelable = IsOptionEnabled(modelImpl->m_options, AsyncOperationOptions::IsCancelable);
    return S_OK;
}

HRESULT CAsyncOperation::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    HRESULT hr = S_OK;
    if (iid == foundation::IAsyncOperationBase::GetIID())
    {
        *ppInterface = static_cast<foundation::IAsyncOperationBase *>(this);
    }
    else
    {
        hr = _AsyncOperation_BaseType::QueryInterfaceImpl(iid, ppInterface);
    }
    return hr;
}

void CAsyncOperation::OnFinalRelease()
{
    _AsyncOperation_BaseType::OnFinalRelease();
}

HRESULT CAsyncOperation::DisposeInternal()
{
    IFR_ASSERT(_AsyncOperation_BaseType::DisposeInternal());
    this->Cleanup();
    return S_OK;
}

HRESULT CAsyncOperation::GetIsCancelable(bool* pVal)
{
    *pVal = m_IsCancelable;
    return S_OK;
}

HRESULT CAsyncOperation::Wait(UINT32 timeout, ResultStatus* status)
{
    return this->_Wait(timeout, status);
}

HRESULT CAsyncOperation::GetResult(UINT32 timeout, foundation::IInspectable **ppResult)
{
    IFCPTR(ppResult);
    return this->_GetResult(timeout, ppResult);
}

HRESULT CAsyncOperation::GetError(HRESULT* hResult)
{
    return this->_GetError(hResult);
}

HRESULT CAsyncOperation::GetStatus(ResultStatus *status)
{
    return this->_GetStatus(status);
}

HRESULT CAsyncOperation::GetCompletedEventSource( ICompletedEventSource** ppEventSource)
{
    IFR_ASSERT(EnsureEventSourceModel());

    *ppEventSource = _AsyncOperation_BaseType::GetEventSourceImpl();
    (*ppEventSource)->AddRef();
    return S_OK;
}

HRESULT CAsyncOperation::GetProgressEventSource(IProgressEventSource** ppEventSource)
{
    _FoundationBaseLock csl(*this);

    if (this->_ProgressSourceType::GetEventSourceImpl() == nullptr)
    {
        // Initialize our EventModel Source
        IFR_ASSERT(this->_ProgressSourceType::_CreateEventSource(this,this->m_pCritsec));
    }

    *ppEventSource = this->_ProgressSourceType::GetEventSourceImpl();
    (*ppEventSource)->AddRef();
    return S_OK;

}

HRESULT CAsyncOperation::_OnCancel()
{
    HRESULT hr = S_FALSE;
    if (m_pDelegate)
    {
        hr = m_pDelegate->OnCancel();
        IFHR(hr);
    }
    if (hr == S_FALSE && m_CancelDelegate != nullptr)
    {
        hr = m_CancelDelegate->Invoke();
        IFHR(hr);
    }
    return S_OK;
}

HRESULT CAsyncOperation::Cancel()
{
    return this->_Cancel();
}

STDMETHODIMP CAsyncOperation::GetCancelDelegate(_COM_Outptr_result_maybenull_ foundation::library::IDelegateDispatchable **ppCancelDelegate)
{
    _FoundationBaseLock csl(*this);
    return m_CancelDelegate.CopyTo(ppCancelDelegate);
}

STDMETHODIMP CAsyncOperation::SetCancelDelegate(_In_opt_ foundation::library::IDelegateDispatchable *pCancelDelegate)
{
    _FoundationBaseLock csl(*this);
    if (this->_resultStatus == ResultStatus::Started)
    {
        // it only make sense to assign if State == Started
        m_CancelDelegate = pCancelDelegate;
    }
    else if (this->_resultStatus == ResultStatus::Canceled)
    {
        pCancelDelegate->Invoke();
        return E_ASYNC_OPERATION_CANCELED;
    }
    else
    {
        return E_UNEXPECTED;
    }
    return S_OK;
}

STDMETHODIMP CAsyncOperation::FireProgress(
    _In_ foundation::IInspectable *pProgressValue)
{
    foundation::ComPtr<IProgressEventArgs> spProgressEventArgs;
    IFR_ASSERT(CProgressEventArgs::CreateInstance(pProgressValue, spProgressEventArgs.GetAddressOf()));

    IFR(FireWithCallback(
        &CAsyncOperation::PerformFireProgress,
        static_cast<IProgressEventArgs *>(spProgressEventArgs)));
    return S_OK;
}

HRESULT CAsyncOperation::SetCompleted(_In_ HRESULT hResult,_In_ foundation::IInspectable *result)
{
    if (this->IsLevelEnabled(LoggingLevel::Trace,AsyncOperation_Category_SetCompleted_Id))
    {
        logger_util::Log(
            CLogger::GetLogger(),
            this->AsInspectable(),
            LoggingLevel::Trace,
            // Message Enum Value
            AsyncOperation_Category_SetCompleted_Id,
            // Log Category
            CAsyncOperationLogFactory::GetLogCategory()->AsInterface(),
            AsyncOperationSetCompleted_LogRecordEnum::TypeId,
            // UniqueId
            AsyncOperationSetCompleted_LogRecord::UniqueId,
            pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
            // HRESULT
            AsyncOperationSetCompleted_LogRecord::HResult,
            pv_util::CreateValue((INT32)hResult).Get(),
            // Result
            AsyncOperationSetCompleted_LogRecord::Result,
            result,
            -1);
    }
    return this->_SetCompleted(hResult, result);
}
HRESULT CAsyncOperation::_FireCompleted(foundation::library::NoRefCountFoundationCriticalSection& cs)
{
    // detach the lock
    cs.Release();
    // and attach the lock to the _FoundationBaseLock
    _FoundationBaseLock csl(*this, FoundationLockState::AlredyLocked);

    IFR(FireCompleted(csl));
    return S_OK;
}

HRESULT CAsyncOperation::SetCompleted(_In_ IAsyncOperation *pAsyncOperation)
{
    foundation_assert(pAsyncOperation);
    ResultStatus status;
    pAsyncOperation->GetStatus(&status);

    if(status != ResultStatus::Started)
    {
        return SetCompletedInternal(pAsyncOperation);
    }
    else
    {
        // async operation in progress so we need to monitor the 'Completed' event
        _connection.SetSink(this, &CAsyncOperation::OnAsyncOperationCompleted);
        _connection.Attach(pAsyncOperation);
    }
    return S_OK;
}

HRESULT CAsyncOperation::OnAsyncOperationCompleted(foundation::IUnknown *pSender,ICompletedEventArgs *pEventArgs)
{
    IFR_ASSERT(SetCompletedInternal(_connection.GetSourceAs<IAsyncOperation>()));
    _connection.Detach();
    return S_OK;
}

HRESULT CAsyncOperation::SetCompletedInternal(IAsyncOperation *pAsyncOperation)
{
    foundation_assert(pAsyncOperation);

    foundation::InspectablePtr spResult;
    pAsyncOperation->GetResult(_InfiniteTimeout,spResult.GetAddressOf());
    HRESULT herror;
    pAsyncOperation->GetError(&herror);
    return SetCompleted(herror,spResult);
}

HRESULT CAsyncOperation::CreateCompletedEventArgs(ICompletedEventArgs **ppEventArgs)
{
    foundation::ComPtr<library::CompletedEventArgs> spCompletedEventArgs;
    IFR_ASSERT(ctl::inspectable::CreateInstance(spCompletedEventArgs.GetAddressOf()));
    *ppEventArgs = static_cast<ICompletedEventArgs *>(spCompletedEventArgs.Detach());
    return S_OK;
}

HRESULT CAsyncOperation::FireCompleted(_FoundationBaseLock& csl)
{
    foundation::ComPtr<ICompletedEventArgs> spCompletedEventArgs;
    IFR_ASSERT(CreateCompletedEventArgs(spCompletedEventArgs.GetAddressOf()));

    IFR(FireWithCallback(
        csl,
        &CAsyncOperation::PerformCompleted,
        static_cast<ICompletedEventArgs *>(spCompletedEventArgs)));
    return S_OK;
}

HRESULT CAsyncOperation::PerformFireProgress(
    _In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
    _In_ IProgressEventArgs* pEventArgs)
{
    if (this->_ProgressSourceType::GetEventSourceImpl() != nullptr)
    {
        this->_ProgressSourceType::GetEventSourceImpl()->FireEvent(
            pEventHandlerContainer,
            ctl::inspectable::CastToInspectable(this),
            pEventArgs);
    }
    return S_OK;
}

HRESULT CAsyncOperation::PerformCompleted(
    _In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
    _In_ ICompletedEventArgs *pEventArgs)
{
    IFR(FireEvent(pEventHandlerContainer,pEventArgs));
    return S_OK;
}

void CAsyncOperation::_OnAddHandler(_In_ ICompletedEventHandler* pHandler)
{
    {
        // Note: there is no need to Lock/Unlock since this callback is executed 
        // during a Lock/Unlock context
        if(this->_resultStatus != ResultStatus::Started)
        {
            foundation::ComPtr<ICompletedEventArgs> spCompletedEventArgs;
            IGNOREHR(CreateCompletedEventArgs(spCompletedEventArgs.GetAddressOf()));
            IGNOREHR(pHandler->Invoke(ctl::inspectable::CastToInspectable(this), spCompletedEventArgs));
        }
    }
    _AsyncOperation_BaseType::_OnAddHandler(pHandler);
}

HRESULT CAsyncOperation::CreateInstance(
    _In_ const library::async_operation_create_parameters *pModelImpl,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CAsyncOperation>(pModelImpl, pOuter, ppNewInstance);
}

STDMETHODIMP CAsyncOperationFactory::CreateAsyncOperation(
    _In_ const library::async_operation_create_parameters *pParameters,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CAsyncOperation::CreateInstance(pParameters, pOuter, ppNewInstance);
}

foundation::library::IAsyncOperationFactory *CAsyncOperationFactory::GetInstance()
{
    return library::_GetFactoryInstance<CAsyncOperationFactory, foundation::library::IAsyncOperationFactory>();
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.AsyncOperation"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CAsyncOperationFactory::GetInstance)
    );
