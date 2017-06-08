/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InspectableAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class InspectableAdapter : IDisposable
    {

        private IntPtr PInterface;

        internal InspectableAdapter()
        { }

        internal InspectableAdapter(IntPtr pInterface)
        {
            this.PInterface = pInterface;
        }

        internal IntPtr Interface
        {
            get
            {
                return this.PInterface;
            }
        }

        protected void SetInterface(IntPtr pInterface)
        {
            System.Diagnostics.Debug.Assert(this.PInterface == IntPtr.Zero);
            this.PInterface = pInterface;
        }

        ~InspectableAdapter()
        {
            Dispose(false);
        }


        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        public IntPtr Detach()
        {
            IntPtr pInterface = this.Interface;
            this.PInterface = IntPtr.Zero;
            GC.SuppressFinalize(this);
            return pInterface;
        }

        public bool IsTypeOf(Guid iid)
        {
            bool result = false;
            PInvokeUtils.ThrowIfResult(NativeMethods.Inspectable_IsTypeOf(this.Interface, ref iid, ref result));
            return result;
        }

        public IntPtr CastTo(Guid iid)
        {
            return CastToInternal(this.Interface, iid);
        }

        internal static IntPtr CastToInternal(IntPtr pInterface, Guid iid)
        {
            IntPtr pCastInterface = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.Inspectable_CastTo(pInterface, ref iid, ref pCastInterface));
            return pCastInterface;
        }

        protected virtual void Dispose(bool disposing)
        {
            if(PInterface != IntPtr.Zero)
            {
                NativeMethods.Inspectable_Release(this.PInterface);
                this.PInterface = IntPtr.Zero;
            }
        }

    }
}
