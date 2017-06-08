/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectHelper.uwp.cs
****/
using System;
using Xunit;
using TestModel;

namespace Microsoft.PropertyModel.Test
{
    internal static class ObservableObjectHelper
    {
        internal static void AttachPropertyChanged(
            this IObservableObject observableObject,
            PropertyChangedEventHandler eventHandler)
        {
            observableObject.PropertyChanged += eventHandler;
        }

        internal static void DetachPropertyChanged(
            this IObservableObject observableObject,
            PropertyChangedEventHandler eventHandler)
        {
            observableObject.PropertyChanged -= eventHandler;
        }
    }
}
