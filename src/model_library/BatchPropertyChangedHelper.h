/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BatchPropertyChangedHelper.h
****/

#ifndef _BATCH_PROEPRTYCHANGED_HELPER_DEFINED
#define _BATCH_PROEPRTYCHANGED_HELPER_DEFINED

#include "EventHandler.h"
#include <foundation/delegate_library.h>

#include <foundation/array_wrapper.h>
#include <pmod/interfaces/observable_object.h>

//------------------------------------------------------------------------------
// Method:   ForEachPropertyChangedEventArgs
//
// Purpose: Template Helper to iterate into a the array of 'IPropertyChangedEventArgs' inside a 
// a 'IBatchPropertyChangedEventArgs' instance
//
//------------------------------------------------------------------------------
struct CPropertyChangedEventArgsFunc
{
    virtual HRESULT operator()(pmod::IPropertyChangedEventArgs *pPropertyChangedEventArgs) = 0;
};

static inline HRESULT ForEachPropertyChangedEventArgs(
    _In_ pmod::IBatchPropertyChangedEventArgs *pBatchPropertyChangedEventArgs,
    _In_ CPropertyChangedEventArgsFunc *_Func)
{
    foundation_assert(pBatchPropertyChangedEventArgs);
    foundation_assert(_Func);

    // the handler does not support batch event handler
    foundation::ArrayRefCountWrapper<pmod::IPropertyChangedEventArgs> propertyChangesArray;
    IFR_ASSERT(pBatchPropertyChangedEventArgs->GetPropertyChanges(
        propertyChangesArray.GetSizeAddressOf(),
        propertyChangesArray.GetBufferAddressOf()));
    // Iterate on the Array and 'Invoke'
    for(UINT32 i = 0; i < propertyChangesArray.GetSize();++i)
    {
        IFR_ASSERT((*_Func)(propertyChangesArray[i]));
    }
    return S_OK;
}
//------------------------------------------------------------------------------
// Class:   _TInvokePropertyChangedEventHandler
//
//
//------------------------------------------------------------------------------
template <
    class TEventHandler = pmod::IPropertyChangedEventHandler,
    class TSender = foundation::IUnknown,
    class TEventArgs = pmod::IPropertyChangedEventArgs,
    class TBatchEventHandler = pmod::IBatchPropertyChangedEventHandler,
    class TBatchEventArgs = pmod::IBatchPropertyChangedEventArgs
>
class _TInvokePropertyChangedEventHandler
{
protected:
    typedef _TInvokePropertyChangedEventHandler<TEventHandler, TSender, TEventArgs, TBatchEventHandler, TBatchEventArgs> _This_Type;
public:
    HRESULT _Invoke_event_handler(
        _In_ TEventHandler *pEventHandler,
        _In_ TSender *pSender,
        _In_ pmod::IBatchPropertyChangedEventArgs *pBatchPropertyChangedEventArgs)
    {
        // support for Batch notifications
        TBatchEventHandler *pBatchEventHandler = nullptr;
        if(SUCCEEDED(foundation::QueryWeakReference(pEventHandler,&pBatchEventHandler)))
        {
            foundation::ComPtr<TBatchEventArgs> spBatchEventArgs;
            IFR_ASSERT(foundation::QueryInterface(pBatchPropertyChangedEventArgs, spBatchEventArgs.GetAddressOf()));
            IFR(pBatchEventHandler->Invoke(pSender, spBatchEventArgs));
        }
        else
        {
            struct _InvokeFunc : public CPropertyChangedEventArgsFunc {
                _InvokeFunc(
                    _In_ _This_Type *pThis,
                    _In_ TEventHandler *pEventHandler,
                    _In_ TSender *pSender):
                    m_pThis(pThis),
                    m_pEventHandler(pEventHandler),
                    m_pSender(pSender)
                {
                }
                HRESULT operator() (_In_ pmod::IPropertyChangedEventArgs *pPropertyChangedEventArgs) override
                {
                    foundation::ComPtr<TEventArgs> spEventArgs;
                    IFR_ASSERT(m_pThis->ToEventArgs(pPropertyChangedEventArgs, spEventArgs.GetAddressOf()));
                    IFR_ASSERT(m_pEventHandler->Invoke(m_pSender, spEventArgs));
                    return S_OK;
                }
            private:
                _This_Type *m_pThis;
                TEventHandler       *m_pEventHandler;
                TSender             *m_pSender;
            };
            _InvokeFunc invokeFunc(this,pEventHandler,pSender);
            IFR(ForEachPropertyChangedEventArgs(pBatchPropertyChangedEventArgs,&invokeFunc));
        }
        return S_OK;
    }

    // Convert the 'IPropertyChangedEventArgs' -> TEventArgs
    virtual HRESULT ToEventArgs(
        _In_ pmod::IPropertyChangedEventArgs *pPropertyChangedEventArgs,
        _Outptr_ TEventArgs **ppEventArgs)
    {
        HRESULT hr;
        hr = foundation::QueryInterface(pPropertyChangedEventArgs,ppEventArgs);
        foundation_assert(SUCCEEDED(hr));
        IFHR(hr);
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   PropertyChangedEventHandlerBase
//
// Purpose: Help to Adapt the IBatchPropertyChangedEventArgs to different type of handlers
//
//------------------------------------------------------------------------------
template <
    class TBASE,
    class TEventHandler = pmod::IPropertyChangedEventHandler,
    class TSender = foundation::IUnknown,
    class TEventArgs = pmod::IPropertyChangedEventArgs,
    class TBatchEventHandler = pmod::IBatchPropertyChangedEventHandler,
    class TBatchEventArgs = pmod::IBatchPropertyChangedEventArgs
>
class PropertyChangedEventHandlerBase :
    public TBASE,
    public _TInvokePropertyChangedEventHandler<TEventHandler, TSender, TEventArgs, TBatchEventHandler, TBatchEventArgs>
{
};

//------------------------------------------------------------------------------
// Class:   BatchPropertyChangedEventHandlerDelegateBase
//
// Purpose: Enhanced Delegate that will support also the IBatchPropertyChangedEventHandler interface
//
//------------------------------------------------------------------------------
class TBatchPropertyChangedEventInvoker
{
protected:
    template <class TSink, class T>
    static HRESULT InvokeSink(
        T *pThis,
        foundation::IUnknown* pSender, 
        pmod::IBatchPropertyChangedEventArgs* pArgs)
    {
        if (!pThis->HasCallback())
        {
            return S_FALSE;
        }
        foundation::CallbackScope<T> callbackScope(pThis);
        return static_cast<TSink *>(pThis->GetCallback())->InvokeInternal(pSender, pArgs);
    }
};
class TBatchPropertyChangedEventWeakRefInvoker
{
protected:
    template <class TSink, class T>
    static HRESULT InvokeSink(
        T *pThis,
        foundation::IUnknown* pSender,
        pmod::IBatchPropertyChangedEventArgs* pArgs)
    {
        return pThis->_Invoke(pSender, pArgs, &TSink::InvokeInternal);
    }
};

template <class TSink, class TBASE, class TInvokeSink = TBatchPropertyChangedEventInvoker>
class BatchPropertyChangedEventHandlerDelegateBase:
    public TBASE,
    protected TInvokeSink,
    protected pmod::IBatchPropertyChangedEventHandler
{
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::IBatchPropertyChangedEventHandler::GetIID())
        {
            *ppInterface = static_cast<pmod::IBatchPropertyChangedEventHandler *>(this);
        }
        else
        {
            return TBASE::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // IBatchPropertyChangedEventHandler Interface
    STDMETHOD(Invoke)(
        foundation::IUnknown* pSender,
        pmod::IBatchPropertyChangedEventArgs* pArgs)
    {
        return TInvokeSink::template InvokeSink<TSink>(this, pSender, pArgs);
    }

};

#endif
