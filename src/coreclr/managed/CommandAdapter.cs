/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class CommandAdapter : ObjectAdapter
    {
        public static readonly Guid Type = new Guid("AFAD3B17-330C-4DF2-A194-321DDA117988");



        private Lazy<PInvokeCallbackAdapter<CanExecuteChangedEventArgsAdapter>> CanExecuteChangedCallbackAdapter;

        internal CommandAdapter(IntPtr pCommandModel):base(pCommandModel)
        {
            CanExecuteChangedCallbackAdapter = new Lazy<PInvokeCallbackAdapter<CanExecuteChangedEventArgsAdapter>>(
                () => { return new PInvokeCallbackAdapter<CanExecuteChangedEventArgsAdapter>(); });
        }

        public uint State
        {
            get
            {
                UInt32 value = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.Command_GetState(this.Interface, ref value));
                return value;
            }
        }

        public InspectableAdapter Execute(InspectableAdapter pParameter)
        {
            IntPtr result = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.Command_Execute(
                this.Interface,
                pParameter != null ? pParameter.Interface:IntPtr.Zero,
                ref result));
            return ObjectStaticsUtil.CreateInspectable(result);
        }


        public bool CanExecute(InspectableAdapter pParameter)
        {
            bool canExecute = false;
            PInvokeUtils.ThrowIfResult(NativeMethods.Command_CanExecute(
                this.Interface,
                pParameter != null ? pParameter.Interface : IntPtr.Zero,
                ref canExecute));
            return canExecute;
        }

        public void AddCanExecuteChanged(OnEventHandlerAdapter<CanExecuteChangedEventArgsAdapter> callback)
        {
            this.CanExecuteChangedCallbackAdapter.Value.AddCallbackEventHandler(
                this.Interface,
                NativeMethods.Command_AddCanExecuteChanged,
                callback);
        }

        public void RemoveCanExecuteChanged(OnEventHandlerAdapter<CanExecuteChangedEventArgsAdapter> callback)
        {
            this.CanExecuteChangedCallbackAdapter.Value.RemoveCallbackEventHandler(
                this.Interface,
                NativeMethods.Command_RemoveCanExecuteChanged,
                callback);
        }

        public PropertyTypeInfoAdapter GetParameterTypeInfo()
        {
            IntPtr pInterface = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.Command_GetParameterTypeInfo(this.Interface, ref pInterface));
            return new PropertyTypeInfoAdapter(pInterface);
        }

        public PropertyTypeInfoAdapter GetResultTypeInfo()
        {
            IntPtr pInterface = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.Command_GetResultTypeInfo(this.Interface, ref pInterface));
            return new PropertyTypeInfoAdapter(pInterface);
        }
        
        protected override void Dispose(bool disposing)
        {
            if (this.CanExecuteChangedCallbackAdapter.IsValueCreated)
            {
                this.CanExecuteChangedCallbackAdapter.Value.Dispose(
                    this.Interface,
                    NativeMethods.Command_RemoveCanExecuteChanged);
            }
            base.Dispose(disposing);
        }

    }
}
