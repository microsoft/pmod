/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeDispatcherAdapter.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <foundation/library/dispatcher_util.h>
#include <foundation/library/async_operation_class.h>

using namespace foundation;
using namespace foundation::library;

typedef HRESULT(STDAPICALLTYPE *_HasThreadAccessCallbackType)(bool *result);

typedef HRESULT(STDAPICALLTYPE *_RunAsyncCallbackType)(
    IDelegateDispatchable *pDelegateDispatchable,
    IAsyncOperation *pAsyncOperation);

class CDispatcherAdapter :
    public ctl::ComInspectableBase,
    public IDispatcher
{
public:
    HRESULT _Initialize(
        _HasThreadAccessCallbackType pHasThreadCallback,
        _RunAsyncCallbackType pInvokeCallback)
    {
        _pHasThreadCallback = pHasThreadCallback;
        _pInvokeCallback = pInvokeCallback;
        return S_OK;
    }
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        HRESULT hr = S_OK;
        if (iid == IDispatcher::GetIID())
        {
            *ppInterface = static_cast<IDispatcher *>(this);
        }
        else
        {
            hr = ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return hr;
    }

    STDMETHOD(HasThreadAccess)(_Always_(_Out_) bool *pValue)
    {
        //return _pHasThreadCallback(pValue);
        *pValue = false;
        return S_OK;
    }

    STDMETHOD(RunAsync)(
        _In_ IDelegateDispatchable *pDelegateDispatchable, 
        _COM_Outptr_opt_result_maybenull_ IAsyncOperation **ppAsyncAction)
    {
        return _pInvokeCallback(pDelegateDispatchable, nullptr);
    }

private:
    _HasThreadAccessCallbackType _pHasThreadCallback;
    _RunAsyncCallbackType _pInvokeCallback;
};

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE DispatcherAdapter_InvokeDispatchable(
    IDelegateDispatchable *pDelegateDispatchable)
{
    return pDelegateDispatchable->Invoke();
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE DispatcherAdapter_CreateDispatcher(
    _HasThreadAccessCallbackType pHasThreadCallback,
    _RunAsyncCallbackType pInvokeCallback,
    IDispatcher **ppDispatcher)
{
    return ctl::inspectable::CreateInstanceWithInitialize<CDispatcherAdapter>(
        ppDispatcher, pHasThreadCallback, pInvokeCallback);
}
