/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyChangedEventArgsAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class PropertyChangedEventArgsAdapter : InspectableAdapter
    {

        internal PropertyChangedEventArgsAdapter(IntPtr pInterface):
            base(pInterface)
        {
        }

        public uint Property
        {
            get
            {
                UInt32 propertyId = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.PropertyChangedEventArgs_GetProperty(this.Interface, ref propertyId));
                return propertyId;
            }
        }
        public string PropertyName
        {
            get
            {
                return PInvokeUtils.GetString(delegate (ref IntPtr pStr)
                {
                    return NativeMethods.PropertyChangedEventArgs_GetPropertyName(this.Interface, ref pStr);
                });
           }
        }
        public InspectableAdapter OldValue
        {
            get
            {
                IntPtr pValue = IntPtr.Zero;
                PInvokeUtils.ThrowIfResult(NativeMethods.PropertyChangedEventArgs_GetOldValue(this.Interface, ref pValue));
                return ObjectStaticsUtil.CreateInspectable(pValue);
            }
        }
        public InspectableAdapter NewValue
        {
            get
            {
                IntPtr pValue = IntPtr.Zero;
                PInvokeUtils.ThrowIfResult(NativeMethods.PropertyChangedEventArgs_GetNewValue(this.Interface, ref pValue));
                return ObjectStaticsUtil.CreateInspectable(pValue);
            }
        }

    }
}
