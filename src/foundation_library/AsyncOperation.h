/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperation.h
****/


#pragma once

#include <foundation/library/async_operation_impl.h>
#include <foundation/library/foundation_library.h>
#include <foundation/async_operation_connection.h>

#include "ObjectBase.h"

class CLogCategory;
class CAsyncOperationLogFactory:
    public _ObjectBaseLogFactory
    <
        foundation::LoggingLevel::Trace,
        foundation::LoggingLevel::Trace,
        foundation::LoggingLevel::Trace,
        foundation::LoggingLevel::Trace
    >
{
public:
    static const CLogCategory*GetLogCategory();
};

typedef _ObjectBase
    <
    foundation::IAsyncOperation,
    foundation::library::IAsyncOperationClass,
    foundation::library::IAsyncOperationDelegate,
    foundation::library::async_operation_create_parameters,
    foundation::ICompletedEventSource,
    foundation::ICompletedEventHandler,
    foundation::IUnknown,
    foundation::ICompletedEventArgs,
    foundation::IAsyncOperationTypeInfo,
    CAsyncOperationLogFactory
    >
    _AsyncOperation_BaseType;

class CAsyncOperation :
    public _AsyncOperation_BaseType,
    protected _EventSourceBase<
        foundation::IProgressEventSource,
        foundation::IProgressEventHandler,
        foundation::IUnknown,
        foundation::IProgressEventArgs,
        CAsyncOperation,
        foundation::library::ICriticalSection
    >,
    public foundation::library::_AsyncOperationBase<CAsyncOperation>
{
private:
    typedef _EventSourceBase<
        foundation::IProgressEventSource,
        foundation::IProgressEventHandler,
        foundation::IUnknown,
        foundation::IProgressEventArgs,
        CAsyncOperation,
        foundation::library::ICriticalSection
    > _ProgressSourceType;

public:
    static HRESULT  CreateInstance(
        _In_ const foundation::library::async_operation_create_parameters *pModelImpl,
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

    HRESULT _Initialize(const foundation::library::async_operation_create_parameters *pParameters);

protected:
    friend class foundation::library::_AsyncOperationBase<CAsyncOperation>;

#if defined(_WINDOWS_RUNTIME)
    INSPECTABLE_CLASS(U("Microsoft.PropertyModel.Library.AsyncOperation"), foundation::TrustLevel::BaseTrust);
#endif

    CAsyncOperation() :
        m_IsCancelable(false)
    {
    }


    #if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override
    {
        DumpObjectBase(U("AsyncOperation"));
    }

    #endif

    // required by _AsyncOperationBase<CAsyncOperation>
    inline foundation::library::ICriticalSection *_GetCriticalSection()
    {
        return this->GetCriticalSectionInterface();
    }
    inline foundation::IAsyncOperation *_GetAsyncOperation()
    {
        return static_cast<foundation::IAsyncOperation *>(this);
    }
    HRESULT _FireCompleted(foundation::library::NoRefCountFoundationCriticalSection& cs);
    HRESULT _OnCancel();

    // ComBase Override
    void OnFinalRelease() override;
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;

    // IAsyncOperationModel Interface
    STDMETHOD(GetResult)(UINT32 timeout,foundation::IInspectable **ppResult) override;
    STDMETHOD(GetError)(HRESULT* hResult) override;
    STDMETHOD(GetStatus)(foundation::ResultStatus* status) override;
    STDMETHOD(GetIsCancelable)(bool* pVal) override;

    STDMETHOD(Cancel)() override;
    STDMETHOD(Wait)(UINT32 timeout, foundation::ResultStatus* status) override;

   // Event methods.
    STDMETHOD(GetCompletedEventSource)(foundation::ICompletedEventSource** ppEventSource) override;
    STDMETHOD(GetProgressEventSource)(foundation::IProgressEventSource** ppEventSource) override;

    // IAsyncOperationClass Interface
    STDMETHOD(GetCancelDelegate)(_COM_Outptr_result_maybenull_ foundation::library::IDelegateDispatchable **ppCancelDelegate) override;
    STDMETHOD(SetCancelDelegate)(_In_opt_ foundation::library::IDelegateDispatchable *pCancelDelegate) override;

    STDMETHOD(SetCompleted)(_In_ HRESULT hResult, _In_ foundation::IInspectable *result) override;
    STDMETHOD(SetCompleted)(_In_ foundation::IAsyncOperation *pAsyncOperationModel) override;
    STDMETHOD(FireProgress)(_In_ foundation::IInspectable *pProgressValue) override;

    // Override from EventSourceModel
    void _OnAddHandler(_In_ foundation::ICompletedEventHandler* pHandler) override;

    HRESULT DisposeInternal() override;

public:
    void _OnAddHandler(_In_ foundation::IProgressEventHandler* pHandler)
    {
        OnAddHandlerInternal();
    }

    void _OnRemoveHandler(_In_ foundation::IProgressEventHandler* pHandler)
    {
        OnRemoveHandlerInternal();
    }

private:
    HRESULT FireCompleted(_FoundationBaseLock& csl);
    HRESULT PerformCompleted(
        _In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
        _In_ foundation::ICompletedEventArgs *pEventArgs);
    HRESULT PerformFireProgress(
        _In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
        _In_ foundation::IProgressEventArgs* pEventArgs);

    // callback for attached IAsyncOperation
    HRESULT OnAsyncOperationCompleted(foundation::IUnknown *pSender, foundation::ICompletedEventArgs *pEventArgs);
    
    HRESULT SetCompletedInternal(foundation::IAsyncOperation *pAsyncOperationModel);

    static HRESULT CreateCompletedEventArgs(foundation::ICompletedEventArgs **ppEventArgs);
private:
    bool                                                        m_IsCancelable;
    foundation::ComPtr<foundation::library::IDelegateDispatchable>   m_CancelDelegate;

    foundation::AsyncOperationCompletedConnection _connection;

};

class CAsyncOperationFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IAsyncOperationFactory
{
public:
    static foundation::library::IAsyncOperationFactory *GetInstance();
    //Interface IModelFactory
    STDMETHOD(CreateAsyncOperation)(
        _In_ const foundation::library::async_operation_create_parameters *pModelImpl,
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IAsyncOperationFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IAsyncOperationFactory *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};
