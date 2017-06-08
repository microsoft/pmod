/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectAdapter.cs
****/
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace Microsoft.PropertyModel
{

    public class ObservableObjectAdapter : ObjectDispatchAdapter
    {
        public new static readonly Guid Type = new Guid("8165A381-C2BF-444E-B84B-DF43CA03586E");


        private Lazy<PInvokeCallbackAdapter<PropertyChangedEventArgsAdapter>> PropertyChangedCallbackAdapter;
        private Lazy<PInvokeCallbackAdapter<EventModelEventArgsAdapter>> EventModelCallbackAdapter;

        public ObservableObjectAdapter(IntPtr pPropertyModel):base(pPropertyModel)
        {
            PropertyChangedCallbackAdapter = new Lazy<PInvokeCallbackAdapter<PropertyChangedEventArgsAdapter>>(
                () => { return new PInvokeCallbackAdapter<PropertyChangedEventArgsAdapter>(); });
            EventModelCallbackAdapter = new Lazy<PInvokeCallbackAdapter<EventModelEventArgsAdapter>>(
                () => { return new PInvokeCallbackAdapter<EventModelEventArgsAdapter>(); });
        }

        public void AddNotifyPropertyChanged(OnEventHandlerAdapter<PropertyChangedEventArgsAdapter> callback)
        {
            PropertyChangedCallbackAdapter.Value.AddCallbackEventHandler(
                this.Interface,
                NativeMethods.ObservableObject_AddNotifyPropertyChanged,
                callback);
        }

        public void RemoveNotifyPropertyChanged(OnEventHandlerAdapter<PropertyChangedEventArgsAdapter> callback)
        {
            PropertyChangedCallbackAdapter.Value.RemoveCallbackEventHandler(
                this.Interface,
                NativeMethods.ObservableObject_RemoveNotifyPropertyChanged,
                callback);
        }
        public void AddEventModelEvent(OnEventHandlerAdapter<EventModelEventArgsAdapter> callback)
        {
            EventModelCallbackAdapter.Value.AddCallbackEventHandler(
                this.Interface,
                NativeMethods.ObservableObject_AddEventModelEvent,
                callback);
        }

        public void RemoveEventModelEvent(OnEventHandlerAdapter<EventModelEventArgsAdapter> callback)
        {
            EventModelCallbackAdapter.Value.RemoveCallbackEventHandler(
                this.Interface,
                NativeMethods.ObservableObject_RemoveEventModelEvent,
                callback);
        }

        protected override void Dispose(bool disposing)
        {
            if(this.PropertyChangedCallbackAdapter.IsValueCreated)
            {
                this.PropertyChangedCallbackAdapter.Value.Dispose(
                    this.Interface,
                    NativeMethods.ObservableObject_RemoveNotifyPropertyChanged);
            }
            if (this.EventModelCallbackAdapter.IsValueCreated)
            {
                this.EventModelCallbackAdapter.Value.Dispose(
                    this.Interface,
                    NativeMethods.ObservableObject_RemoveEventModelEvent);
            }
            base.Dispose(disposing);
        }
    }
}
