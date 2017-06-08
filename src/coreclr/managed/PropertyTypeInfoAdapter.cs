/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyTypeInfoAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class PropertyTypeInfoAdapter : InspectableAdapter
    {

        internal PropertyTypeInfoAdapter(IntPtr pInterface):base(pInterface)
        {
        }

        public PropertyType Type
        {
            get
            {
                UInt32 value = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.PropertyTypeInfo_GetPropertyType(this.Interface, ref value));
                return (PropertyType)value;
            }
        }

        public ObjectTypeInfoAdapter TypeInfo
        {
            get
            {
                IntPtr pInterface = IntPtr.Zero;
                PInvokeUtils.ThrowIfResult(NativeMethods.PropertyTypeInfo_GetTypeInfo(this.Interface, ref pInterface));
                return pInterface != IntPtr.Zero ? new ObjectTypeInfoAdapter(pInterface) : null;
            }
        }
    }
}
