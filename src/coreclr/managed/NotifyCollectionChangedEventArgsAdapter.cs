/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NotifyCollectionChangedEventArgsAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public enum NotifyCollectionModelChangedAction
    {
        ItemAdded = 1,
        ItemRemoved = 2,
        ItemReplaced = 3,
        Reset = 4
    };
    public class NotifyCollectionChangedEventArgsAdapter : InspectableAdapter
    {

        internal NotifyCollectionChangedEventArgsAdapter(IntPtr pInterface):
            base(pInterface)
        {
        }

        public NotifyCollectionModelChangedAction Action
        {
            get
            {
                UInt32 value = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.NotifyCollectionChangedEventArgs_GetAction(this.Interface, ref value));
                return (NotifyCollectionModelChangedAction)value;
            }
        }
        public InspectableAdapter[] NewItems
         {
            get
            {
                var newItems = PInvokeUtils.GetPtrArray(delegate (ref UInt32 size, ref IntPtr pValues)
                {
                    return NativeMethods.NotifyCollectionChangedEventArgs_GetNewItems(this.Interface, ref size, ref pValues);
                });
                return ObjectStaticsUtil.CreateInspectableArray(newItems);
            }
        }
        public InspectableAdapter[] OldItems
        {
            get
            {
                var oldItems = PInvokeUtils.GetPtrArray(delegate (ref UInt32 size, ref IntPtr pValues)
                {
                    return NativeMethods.NotifyCollectionChangedEventArgs_GetOldItems(this.Interface, ref size, ref pValues);
                });
                return ObjectStaticsUtil.CreateInspectableArray(oldItems);
            }
        }

        public uint NewStartingIndex
        {
            get
            {
                UInt32 value = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.NotifyCollectionChangedEventArgs_GetNewStartingIndex(this.Interface, ref value));
                return value;
            }
        }
        public uint OldStartingIndex
        {
            get
            {
                UInt32 value = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.NotifyCollectionChangedEventArgs_GetOldStartingIndex(this.Interface, ref value));
                return value;
            }
        }

    }

}
