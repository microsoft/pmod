/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class ObservableCollectionAdapter : EnumerableAdapter
    {
        public static readonly Guid Type = new Guid("0F4681D2-5802-42F9-8B0C-96C7546DC78E");



        private Lazy<PInvokeCallbackAdapter<NotifyCollectionChangedEventArgsAdapter>> NotifyCollectionChangedCallbackAdapter;

        internal ObservableCollectionAdapter(IntPtr pCollectionModel):base(pCollectionModel)
        {
            this.NotifyCollectionChangedCallbackAdapter = new Lazy<PInvokeCallbackAdapter<NotifyCollectionChangedEventArgsAdapter>>(
                () => { return new PInvokeCallbackAdapter<NotifyCollectionChangedEventArgsAdapter>(); });
        }

        public void AddNotifyCollectionChanged(OnEventHandlerAdapter<NotifyCollectionChangedEventArgsAdapter> callback)
        {
            this.NotifyCollectionChangedCallbackAdapter.Value.AddCallbackEventHandler(
                this.Interface,
                NativeMethods.ObservableCollection_AddNotifyCollectionChanged,
                callback);
        }

        public void RemoveNotifyCollectionChanged(OnEventHandlerAdapter<NotifyCollectionChangedEventArgsAdapter> callback)
        {
            this.NotifyCollectionChangedCallbackAdapter.Value.RemoveCallbackEventHandler(
                this.Interface,
                NativeMethods.ObservableCollection_RemoveNotifyCollectionChanged,
                callback);
        }

        public uint Count
        {
            get
            {
                UInt32 count = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.ObservableCollection_GetCount(this.Interface, ref count));
                return count;
            }
        }

        public InspectableAdapter GetItem(uint index)
        {
            IntPtr pValue = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.ObservableCollection_GetItem(this.Interface, index, ref pValue));
            return ObjectStaticsUtil.CreateInspectable(pValue);
        }

        public InspectableAdapter[] GetItems()
        {
            var items = PInvokeUtils.GetPtrArray(delegate (ref UInt32 size, ref IntPtr pValues)
            {
                return NativeMethods.ObservableCollection_GetItems(this.Interface, ref size, ref pValues);
            });
            return ObjectStaticsUtil.CreateInspectableArray(items);
        }

        public int IndexOf(InspectableAdapter value)
        {
            Int32 index = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.ObservableCollection_IndexOf(this.Interface, value != null ? value.Interface:IntPtr.Zero, ref index));
            return index;
        }

        protected override void Dispose(bool disposing)
        {
            if (this.NotifyCollectionChangedCallbackAdapter.IsValueCreated)
            {
                this.NotifyCollectionChangedCallbackAdapter.Value.Dispose(
                    this.Interface,
                    NativeMethods.ObservableCollection_RemoveNotifyCollectionChanged);
            }
            base.Dispose(disposing);
        }

    }
}
