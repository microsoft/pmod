/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CompletedEventArgsAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class CompletedEventArgsAdapter : InspectableAdapter
    {
        internal CompletedEventArgsAdapter(IntPtr pInterface):
            base(pInterface)
        {
        }
    }
 }
