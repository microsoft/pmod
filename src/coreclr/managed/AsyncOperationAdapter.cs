/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public enum AsyncStatus
    {
        Started = 0,
        Completed = 1,
        Canceled = 2,
        Error = 3
    }

    public class AsyncOperationAdapter : ObjectAdapter
    {
        public static readonly Guid Type = new Guid("235C1CDB-12D2-4005-8D56-81CF21C551F0");




        private Lazy<PInvokeCallbackAdapter<CompletedEventArgsAdapter>> CompletedCallbackAdapter;

        internal AsyncOperationAdapter(IntPtr pAsyncOperation):base(pAsyncOperation)
        {
            this.CompletedCallbackAdapter = new Lazy<PInvokeCallbackAdapter<CompletedEventArgsAdapter>>(
                () => { return new PInvokeCallbackAdapter<CompletedEventArgsAdapter>(); });
        }

        public InspectableAdapter GetResults()
        {
            IntPtr pValue = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.AsyncOperation_GetResults(this.Interface, ref pValue));
            return ObjectStaticsUtil.CreateInspectable(pValue);
        }

        public AsyncStatus Status
        {
            get
            {
                UInt32 status = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.AsyncOperation_GetStatus(this.Interface, ref status));
                return (AsyncStatus)status;
            }
        }
        public void AddCompleted(OnEventHandlerAdapter<CompletedEventArgsAdapter> callback)
        {
            this.CompletedCallbackAdapter.Value.AddCallbackEventHandler(
                this.Interface,
                NativeMethods.AsyncOperation_AddCompleted,
                callback);
        }

        public void RemoveCompleted(OnEventHandlerAdapter<CompletedEventArgsAdapter> callback)
        {
            this.CompletedCallbackAdapter.Value.RemoveCallbackEventHandler(
                this.Interface,
                NativeMethods.AsyncOperation_RemoveCompleted,
                callback);
        }
        public PropertyTypeInfoAdapter GetResultTypeInfo()
        {
            IntPtr pInterface = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.AsyncOperation_GetResultTypeInfo(this.Interface, ref pInterface));
            return new PropertyTypeInfoAdapter(pInterface);
        }

        protected override void Dispose(bool disposing)
        {
            if (this.CompletedCallbackAdapter.IsValueCreated)
            {
                this.CompletedCallbackAdapter.Value.Dispose(
                    this.Interface,
                    NativeMethods.AsyncOperation_RemoveCompleted);
            }
            base.Dispose(disposing);
        }

    }
}
