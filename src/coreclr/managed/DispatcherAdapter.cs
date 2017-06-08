/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DispatcherAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class DispatcherAdapter : InspectableAdapter
    {
        private Action<Action, bool> actionDispatcher;

        private NativeMethods.HasThreadAccessCallback hasThreadCallback;
        private NativeMethods.RunAsyncCallback runAsyncCallback;

        public DispatcherAdapter(Action<Action, bool> actionDispatcher)
        {
            if (actionDispatcher == null)
            {
                throw new ArgumentNullException("actionDispatcher");
            }
            this.actionDispatcher = actionDispatcher;
            IntPtr pInterface = IntPtr.Zero;
            this.hasThreadCallback = new NativeMethods.HasThreadAccessCallback(this.HasThreadAccessCallback);
            this.runAsyncCallback = new NativeMethods.RunAsyncCallback(this.RunAsyncCallback);

            PInvokeUtils.ThrowIfResult(NativeMethods.DispatcherAdapter_CreateDispatcher(
                this.hasThreadCallback,
                this.runAsyncCallback,
                ref pInterface));
            this.SetInterface(pInterface);
        }

        public Action<Action, bool> ActionDispatcher
        {
            get
            {
                return this.actionDispatcher;
            }
        }

        private int HasThreadAccessCallback(ref byte result)
        {
            result = 0;
            return 0;
        }
        private int RunAsyncCallback(IntPtr pDelegateDispatchable, IntPtr pAsyncOperation)
        {
            NativeMethods.Inspectable_AddRef(pDelegateDispatchable);
            actionDispatcher(() =>
            {
                NativeMethods.DispatcherAdapter_InvokeDispatchable(pDelegateDispatchable);
                NativeMethods.Inspectable_Release(pDelegateDispatchable);
            }, true);
            return 0;
        }
    }
}
