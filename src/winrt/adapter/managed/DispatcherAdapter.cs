/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DispatcherAdapter.cs
****/

using Microsoft.PropertyModel;
namespace CoreInterop
{
    public class DispatcherAdapter : IDispatcher
    {
        private System.Windows.Threading.Dispatcher _dispatcher;

        public DispatcherAdapter(System.Windows.Threading.Dispatcher dispatacher)
        {
            _dispatcher = dispatacher;
        }

        public bool HasThreadAccess
        {
            get { return _dispatcher.CheckAccess(); }
        }

        public IAsyncOperation RunAsync(Microsoft.PropertyModel.Dispatchable pDispatchable)
        {
            _dispatcher.BeginInvoke(() =>
            {
                pDispatchable.Invoke();
            });
            return null;
        }
    }

}
