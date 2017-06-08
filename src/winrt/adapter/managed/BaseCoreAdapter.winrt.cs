/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseCoreAdapter.winrt.cs
****/
using Microsoft.PropertyModel;
using System;
using System.Runtime.InteropServices.ComTypes;

namespace CoreInterop
{
    public partial class BaseCoreAdapter<TAdapterInterface>
    {
        static internal int ToEnumValue(object value)
        {
            if (!value.GetType().IsPrimitive)
            {
                IEnumValue enumValue = value as IEnumValue;
                if (enumValue != null)
                {
                    value = enumValue.Value;
                }
            }
            return Convert.ToInt32(value);
        }
    }
}
