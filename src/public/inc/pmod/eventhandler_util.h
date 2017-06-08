/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:eventhandler_util.h
****/
#pragma once

#include <foundation/event_util.h>
#include "interfaces.h"
#include <foundation/macros.h>

namespace pmod
{

//------------------------------------------------------------------------------
// Class:   ObservableObjectListenerBase
//
// Purpose: Class for Property Model Listener
//
//------------------------------------------------------------------------------
class ObservableObjectListenerBase :
    public foundation::BaseListener<
        ObservableObjectListenerBase,
        IObservableObject,
        IPropertyChangedEventHandler,
        IPropertyChangedEventArgs,
        IPropertyChangedEventSource>
{
public:
    static HRESULT GetEventSource(IObservableObject *pSource,IPropertyChangedEventSource **ppEventSource)
    {
        return pSource->GetPropertyChangedEventSource(ppEventSource);
    }
};
//------------------------------------------------------------------------------
// Class:   EventModelListenerBase
//
// Purpose: Class for Event Model Listener
//
//------------------------------------------------------------------------------
class EventModelListenerBase :
    public foundation::BaseListener<
        EventModelListenerBase,
        IObservableObject,
        IEventModelEventHandler,
        IEventModelEventArgs,
        IEventModelEventSource>
{
public:
    static HRESULT GetEventSource(IObservableObject *pSource,IEventModelEventSource **ppEventSource)
    {
        return pSource->GetEventModelEventSource(ppEventSource);
    }
};
//------------------------------------------------------------------------------
// Class:   ObservableCollectionListenerBase
//
// Purpose: Class for Collection Model Listener
//
//------------------------------------------------------------------------------
class ObservableCollectionListenerBase :
    public foundation::BaseListener<
        ObservableCollectionListenerBase,
        IObservableCollection,
        INotifyCollectionChangedEventHandler,
        INotifyCollectionChangedEventArgs,
        INotifyCollectionChangedEventSource>
{
public:
    static HRESULT GetEventSource(IObservableCollection *pSource,INotifyCollectionChangedEventSource **ppEventSource)
    {
        return pSource->GetNotifyCollectionChangedEventSource(ppEventSource);
    }
};

//------------------------------------------------------------------------------
// Class:   CommandListenerBase
//
// Purpose: Class for Command Model Listener
//
//------------------------------------------------------------------------------
class CommandListenerBase :
    public foundation::BaseListener<
        CommandListenerBase,
        ICommand,
        ICanExecuteChangedEventHandler,
        ICanExecuteChangedEventArgs,
        ICanExecuteChangedEventSource>
{
public:
    static HRESULT GetEventSource(ICommand *pSource,ICanExecuteChangedEventSource **ppEventSource)
    {
        return pSource->GetCanExecuteChangedEventSource(ppEventSource);
    }
};

//------------------------------------------------------------------------------
// Class:   PropertyBindingListener
//
// Purpose: Class for Property Binding Listener
//
//------------------------------------------------------------------------------
class PropertyBindingListener :
    public foundation::BaseListener<
        PropertyBindingListener,
        IBindingBase,
        IBindingValueChangedEventHandler,
        IBindingValueChangedEventArgs,
        IBindingValueChangedEventSource>
{
public:
    static HRESULT GetEventSource(IBindingBase *pSource,IBindingValueChangedEventSource **ppEventSource)
    {
        return pSource->GetBindingValueChangedEventSource(ppEventSource);
    }
};

}

