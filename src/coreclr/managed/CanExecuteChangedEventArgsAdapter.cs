/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CanExecuteChangedEventArgsAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class CanExecuteChangedEventArgsAdapter : InspectableAdapter
    {
        internal CanExecuteChangedEventArgsAdapter(IntPtr pInterface):
            base(pInterface)
        {
        }

    }
}
