/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumerableAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class EnumerableAdapter : ObjectAdapter
    {
        internal EnumerableAdapter(IntPtr pEnumerable):base(pEnumerable)
        {
        }

        public PropertyTypeInfoAdapter GetItemTypeInfo()
        {
            IntPtr pInterface = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.Enumerable_GetItemTypeInfo(this.Interface, ref pInterface));
            return new PropertyTypeInfoAdapter(pInterface);
        }
    }
}
