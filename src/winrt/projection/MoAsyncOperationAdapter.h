/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationAdapter.h
****/

#pragma once
#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"


using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoAsyncOperationAdapter
//
// Purpose: Implementation of IPropertyResultModel Interface
//
//------------------------------------------------------------------------------
class CMoAsyncOperationAdapter :
    public CMoBaseAdapter
        <
        foundation::IAsyncOperation,
        IAsyncOperation,
        IResultCompletedEventHandler,
        IResultCompletedEventArgs
        >
{
public:
    // Interface IAsyncOperation
    HRESULT STDMETHODCALLTYPE get_AsyncStatus(AsyncStatus *value);
    HRESULT STDMETHODCALLTYPE get_AsyncErrorCode(HRESULT *value);

    HRESULT STDMETHODCALLTYPE GetAsyncResults(unsigned int timeout,IInspectable **ppResults);

    HRESULT STDMETHODCALLTYPE add_ResultCompleted( 
        IResultCompletedEventHandler *value,
        EventRegistrationToken *token);
                    
    HRESULT STDMETHODCALLTYPE remove_ResultCompleted( 
        EventRegistrationToken token);
};

//------------------------------------------------------------------------------
// Class:   CMoResultCompletedEventArgsAdapter
//
// Purpose: Implementation of IResultCompletedEventArgs Interface Adapter
//
//------------------------------------------------------------------------------
class CMoResultCompletedEventArgsAdapter :
    public foundation::AdapterBase
    <
        foundation::ICompletedEventArgs,
        IResultCompletedEventArgs,
        &__uuidof(IResultCompletedEventArgs)
    >
{
};
