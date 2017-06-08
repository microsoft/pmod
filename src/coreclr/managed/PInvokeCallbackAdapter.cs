/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeCallbackAdapter.cs
****/
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Linq;

namespace Microsoft.PropertyModel
{

    public delegate void OnEventHandlerAdapter<T>(T eventArgsAdapter);

    internal class PInvokeCallbackAdapter<T>
        where T :class,IDisposable
    {

        internal delegate int AttachCallbackEventHandlerDelegate(
            IntPtr pInterface,
            NativeMethods.OnCallbackEventHandler callback,
            ref IntPtr pHandler);
        internal delegate int DetachCallbackEventHandlerDelegate(
            IntPtr pInterface,
            IntPtr pHandler);

        private Lazy<Dictionary<OnEventHandlerAdapter<T>, Tuple<IntPtr, NativeMethods.OnCallbackEventHandler>>> Callbacks;

        private IntPtr PInterface { get; set; }

        internal PInvokeCallbackAdapter()
        {
            Callbacks = new Lazy<Dictionary<OnEventHandlerAdapter<T>, Tuple<IntPtr, NativeMethods.OnCallbackEventHandler>>>(() =>
            {
                return new Dictionary<OnEventHandlerAdapter<T>, Tuple<IntPtr, NativeMethods.OnCallbackEventHandler>>();
            });
        }

        internal void AddCallbackEventHandler(
            IntPtr pInterface,
            AttachCallbackEventHandlerDelegate pInvokeAttach,
            OnEventHandlerAdapter<T> callback)
        {
            NativeMethods.OnCallbackEventHandler callbackAdapter = (IntPtr pEventArgs) =>
            {
                try
                {
                    var ctorInfo = typeof(T).GetTypeInfo().DeclaredConstructors.Single(
                        ci => ci.GetParameters().Length > 0 &&
                        ci.GetParameters()[0].ParameterType == typeof(IntPtr));
                    var eventArgsAdapter = (T)ctorInfo.Invoke(new object[] { pEventArgs });
                    callback(eventArgsAdapter);
                }
                catch(Exception )
                {
                    return -1;
                }
                return 0;
            };

            IntPtr pEventHandler = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(pInvokeAttach(pInterface, callbackAdapter, ref pEventHandler));
            this.Callbacks.Value.Add(callback, new Tuple<IntPtr, NativeMethods.OnCallbackEventHandler>(pEventHandler, callbackAdapter));
        }

        internal void RemoveCallbackEventHandler(
            IntPtr pInterface,
            DetachCallbackEventHandlerDelegate pInvokeDetach,
            OnEventHandlerAdapter<T> callback)
        {
            Tuple<IntPtr, NativeMethods.OnCallbackEventHandler> tuple;

            if (!this.Callbacks.Value.TryGetValue(callback, out tuple))
            {
                throw new InvalidOperationException("Callback not registered");
            }
            PInvokeUtils.ThrowIfResult(pInvokeDetach(pInterface, tuple.Item1));
            NativeMethods.Inspectable_Release(tuple.Item1);
            this.Callbacks.Value.Remove(callback);
        }

        internal void Dispose(
            IntPtr pInterface,
            DetachCallbackEventHandlerDelegate pInvokeDetach)
        {
            if (this.Callbacks.IsValueCreated)
            {
                foreach (var tuple in this.Callbacks.Value.Values)
                {
                    pInvokeDetach(pInterface, tuple.Item1);
                    NativeMethods.Inspectable_Release(tuple.Item1);
                }
                this.Callbacks.Value.Clear();
            }
        }
    }
}
