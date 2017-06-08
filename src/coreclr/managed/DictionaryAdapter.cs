/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class DictionaryAdapter : InspectableAdapter
    {
        public static readonly Guid Type = new Guid("52F75504-1406-421E-A926-B99A8009901F");
    

        internal DictionaryAdapter(IntPtr pDictionary):base(pDictionary)
        {
        }
        internal DictionaryAdapter()
        {
            IntPtr pInterface = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.Dictionary_CreateInstance(ref pInterface));
            this.SetInterface(pInterface);
        }

        public bool HasKey(string key)
        {
            bool result = false;
            PInvokeUtils.ThrowIfResult(NativeMethods.Dictionary_HasKey(this.Interface, key, ref result));
            return result;
        }

        public InspectableAdapter Lookup(string key)
        {
            IntPtr pValue = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.Dictionary_Lookup(this.Interface, key, ref pValue));
            return ObjectStaticsUtil.CreateInspectable(pValue);
        }

        public bool Insert(string key,InspectableAdapter pValue)
        {
            bool replaced = false;
            PInvokeUtils.ThrowIfResult(NativeMethods.Dictionary_Insert(
                this.Interface,
                key,
                pValue != null ? pValue.Interface : IntPtr.Zero,
                ref replaced));

            return replaced;
        }
        public void Remove(string key)
        {
            PInvokeUtils.ThrowIfResult(NativeMethods.Dictionary_Remove(this.Interface, key));
        }

        public string[] Keys
        {
            get
            {
                var kvPairs = PInvokeUtils.GetPtrArray(delegate (ref UInt32 size, ref IntPtr pValues)
                {
                    return NativeMethods.Dictionary_GetView(this.Interface, ref size, ref pValues);
                });
                var keys = new string[kvPairs.Length];
                int index = 0;
                foreach(var kvp in kvPairs)
                {
                    keys[index++] = PInvokeUtils.GetString(delegate (ref IntPtr pStr)
                    {
                        return NativeMethods.DictionaryPair_GetKey(kvp, ref pStr);
                    });
                    NativeMethods.Inspectable_Release(kvp);
                }
                return keys;
            }
        }
        public InspectableAdapter[] Values
        {
            get
            {
                var kvPairs = PInvokeUtils.GetPtrArray(delegate (ref UInt32 size, ref IntPtr pValues)
                {
                    return NativeMethods.Dictionary_GetView(this.Interface, ref size, ref pValues);
                });
                var values = new IntPtr[kvPairs.Length];
                int index = 0;
                foreach (var kvp in kvPairs)
                {
                    IntPtr value = IntPtr.Zero;
                    NativeMethods.DictionaryPair_GetValue(kvp, ref value);
                    values[index++] = value;
                    NativeMethods.Inspectable_Release(kvp);
                }
                return ObjectStaticsUtil.CreateInspectableArray(values);
            }
        }
    }
}
