/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "MoBaseDelegateAdapter.h"

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoCommandDelegateAdapter
//
// Purpose: Implementation of ICommandDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoCommandDelegateAdapter :
    public CMoDelegeteBaseAdapter
    <
        pmod::library::ICommandDelegate,
        pmod::ICanExecuteChangedEventHandler
    >
{
public:
    static HRESULT CreateAsyncParameters(
        _In_ IInspectable *parameter,
        _In_ IInspectable *pAsyncOperation,
        _Out_ IInspectable** ppParameter);

protected:
     // ICommandDelegate Implementation
    STDMETHOD (OnCanExecute)(_In_ IInspectable *parameter,_Outptr_ bool *pValue);
    STDMETHOD (OnExecute)(_In_ IInspectable *parameter,_Outptr_ IInspectable **ppResult);
};
//------------------------------------------------------------------------------
// Class:   MoCommandFactory
//
// Purpose: Factory class for a CommandModel
//
//------------------------------------------------------------------------------
class MoCommandFactory :
        public MoModelFactoryStaticsBase
        <
            ICommandFactory,
            ICommandStatics
        >
{
public:
    // ICommandModelFactory
    IFACEMETHOD(CreateInstance)(
        CommandOptions options,
        IInspectable* pOuter,
        IInspectable** ppInner,
        ICommand** ppInstance);
    IFACEMETHOD(CreateInstanceWithType)(
        CommandOptions options,
        GUID iidType,
        IInspectable* pOuter,
        IInspectable** ppInner,
        ICommand** ppInstance);

    //ICommandModelStatics
  
    HRESULT STDMETHODCALLTYPE get_CommandType(GUID *value);
    HRESULT STDMETHODCALLTYPE get_SimpleCommandType(GUID *value);

};
