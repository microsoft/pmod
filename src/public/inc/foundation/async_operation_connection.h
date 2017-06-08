/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_connection.h
****/
#pragma once

#include "event_util.h"
#include <foundation/interfaces/async_operation.h>

namespace foundation
{
    //------------------------------------------------------------------------------
    // Class:   AsyncOperationCompletedListenerBase
    //
    // Purpose: base listener for 'completed' event
    //
    //------------------------------------------------------------------------------
    class AsyncOperationCompletedListenerBase :
        public BaseListener
        <
            AsyncOperationCompletedListenerBase,
            IAsyncOperationBase,
            ICompletedEventHandler,
            ICompletedEventArgs,
            ICompletedEventSource
        >
    {
    public:
        static HRESULT GetEventSource(IAsyncOperationBase *pSource, ICompletedEventSource **ppEventSource)
        {
            return pSource->GetCompletedEventSource(ppEventSource);
        }
    };

    typedef SingleSourceEventHandlerConnection
    <
        IAsyncOperationBase,
        ICompletedEventHandler,
        ICompletedEventArgs,
        AsyncOperationCompletedListenerBase
    > AsyncOperationCompletedConnection;

    template<typename TSink>
    class SinkAsyncOperationCompletedEventHandlerConnection :
        public SinkEventHandlerConnection<AsyncOperationCompletedConnection, TSink>
    {
    public:
        SinkAsyncOperationCompletedEventHandlerConnection(
            TSink *pSink, 
            HRESULT(TSink::*pMethod)(foundation::IUnknown *, ICompletedEventArgs *)
            ) :
            SinkEventHandlerConnection<AsyncOperationCompletedConnection, TSink>(pSink, pMethod)
        {
        }
    };
    //------------------------------------------------------------------------------
    // Class:   AsyncOperationProgressListenerBase
    //
    // Purpose: base listener for 'completed' event
    //
    //------------------------------------------------------------------------------
    class AsyncOperationProgressListenerBase :
        public BaseListener
        <
        AsyncOperationProgressListenerBase,
        IAsyncOperationBase,
        IProgressEventHandler,
        IProgressEventArgs,
        IProgressEventSource
        >
    {
    public:
        static HRESULT GetEventSource(IAsyncOperationBase *pSource, IProgressEventSource **ppEventSource)
        {
            return pSource->GetProgressEventSource(ppEventSource);
        }
    };

    typedef SingleSourceEventHandlerConnection
        <
        IAsyncOperationBase,
        IProgressEventHandler,
        IProgressEventArgs,
        AsyncOperationProgressListenerBase
        > AsyncOperationProgressConnection;
}

