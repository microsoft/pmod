/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventModelEventArgsAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class EventModelEventArgsAdapter : InspectableAdapter
    {
        internal EventModelEventArgsAdapter(IntPtr pInterface):
            base(pInterface)
        {
        }

        public uint Event
        {
            get
            {
                UInt32 eventId = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.EventModelEventArgs_GetEvent(this.Interface, ref eventId));
                return eventId;
            }
        }
        public InspectableAdapter EventArgs
        {
            get
            {
                IntPtr pValue = IntPtr.Zero;
                PInvokeUtils.ThrowIfResult(NativeMethods.EventModelEventArgs_GetEventArgs(this.Interface, ref pValue));
                return ObjectStaticsUtil.CreateInspectable(pValue);
            }
        }
    }
 }
