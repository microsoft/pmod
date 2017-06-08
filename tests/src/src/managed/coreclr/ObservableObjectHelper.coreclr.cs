/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectHelper.coreclr.cs
****/
using System;
using Xunit;
using TestModel;

namespace Microsoft.PropertyModel.Test
{
    internal delegate void PropertyChangedEventHandler(object sender, PropertyChangedEventArgs e);

    internal static class ObservableObjectHelper
    {
        private static System.Collections.Generic.Dictionary<
            PropertyChangedEventHandler,
            System.ComponentModel.PropertyChangedEventHandler
            > dictionaryAdapters = new System.Collections.Generic.Dictionary<PropertyChangedEventHandler, System.ComponentModel.PropertyChangedEventHandler>();

        internal static void AttachPropertyChanged(
            this System.ComponentModel.INotifyPropertyChanged observableObject,
            PropertyChangedEventHandler eventHandler)
        {
            System.ComponentModel.PropertyChangedEventHandler eventHandlerAdapter;
            eventHandlerAdapter = (object sender, System.ComponentModel.PropertyChangedEventArgs e) =>
            {
                eventHandler.Invoke(sender, (PropertyChangedEventArgs)e);
            };
            dictionaryAdapters[eventHandler] = eventHandlerAdapter;
            observableObject.PropertyChanged += eventHandlerAdapter;
        }

        internal static void DetachPropertyChanged(
            this System.ComponentModel.INotifyPropertyChanged observableObject,
            PropertyChangedEventHandler eventHandler)
        {
            System.ComponentModel.PropertyChangedEventHandler eventHandlerAdapter;
            if (dictionaryAdapters.TryGetValue(eventHandler, out eventHandlerAdapter))
            {
                observableObject.PropertyChanged -= eventHandlerAdapter;
            }
        }
    }
}
