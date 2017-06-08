/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dispatch_queue_adapter.cpp
****/

#include "dispatch_queue_adapter.h"

#include <foundation/library/logger_macros.h>
#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>

#include <pthread.h>


//+-----------------------------------------------------------------------------
// Class:   CDispatchQueueAdapter
//
// Purpose: Dispatcher Adapter for winRT ICoreDispatcher
//------------------------------------------------------------------------------
class CDispatchQueueAdapter :
    public foundation::ctl::ImplementsInspectable<foundation::library::IDispatcher,&foundation::library::IID_IDispatcher>
{
public:
    static HRESULT 
    CreateInstance(
        dispatch_queue_t queue,
        bool        isWorkingThread,
        foundation::library::IDispatcher** ppDispatcher
        )
    {
        foundation::ComPtr<foundation::ctl::ComInspectableObject<CDispatchQueueAdapter> > spDispatchQueueAdapter;
        IFR(foundation::ctl::ComInspectableObject<CDispatchQueueAdapter>::CreateInstance(spDispatchQueueAdapter.GetAddressOf()));

        spDispatchQueueAdapter->_queue = queue;
        spDispatchQueueAdapter->_self = pthread_self();
        spDispatchQueueAdapter->_isWorkingThread = isWorkingThread;
        *ppDispatcher = spDispatchQueueAdapter.Detach();
        return S_OK;
    }
protected:
    HRESULT STDMETHODCALLTYPE HasThreadAccess(bool *pValue)
    {
        *pValue = _isWorkingThread ? false : (pthread_self() == _self);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE RunAsync( 
        foundation::library::IDelegateDispatchable *pDispatchable,
        foundation::IAsyncOperation **ppAsyncAction) override
    {
        // Add Ref until we release
        pDispatchable->AddRef();
        
        dispatch_async(_queue, ^{
            pDispatchable->Invoke();
            pDispatchable->Release();
        });
        if(ppAsyncAction != nullptr)
        {
            *ppAsyncAction = nullptr;
        }
        return S_OK;
    }
private:
    bool        _isWorkingThread;
    pthread_t   _self;
    dispatch_queue_t _queue;
};

HRESULT foundation::library::create_dispatch_queue_adapter(dispatch_queue_t queue,
                                      foundation::library::IDispatcher **instance)
{
    return CDispatchQueueAdapter::CreateInstance(queue,true,instance);
}

HRESULT foundation::library::create_dispatch_main_queue_adapter(
                                           foundation::library::IDispatcher **instance)
{
    return CDispatchQueueAdapter::CreateInstance(dispatch_get_main_queue(),false,instance);

}

HRESULT foundation::library::create_dispatch_global_queue_adapter(dispatch_queue_priority_t priority,
                                           foundation::library::IDispatcher **instance)
{
    return create_dispatch_queue_adapter(dispatch_get_global_queue(priority,0),instance);
}

