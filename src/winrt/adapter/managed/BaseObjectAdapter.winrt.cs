/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseObjectAdapter.winrt.cs
****/
using System;
using System.Diagnostics;

namespace CoreInterop
{

    public abstract partial class BaseObjectAdapter<T>
    {
        internal abstract void AttachEvents();
        internal abstract void DetachEvents();
    }
}
