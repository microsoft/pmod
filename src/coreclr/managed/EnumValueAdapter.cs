/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumValueAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class EnumValueAdapter : ObjectAdapter
    {
        public static readonly Guid Type = new Guid("0027FA68-95AB-43F1-A813-580E8C521383");
    

        internal EnumValueAdapter(IntPtr pEnumValue):base(pEnumValue)
        {
        }

        public UInt32 EnumValue
        {
            get
            {
                UInt32 enumValue = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.EnumValue_GetValue(this.Interface, ref enumValue));
                return enumValue;
            }
        }

    }
}
