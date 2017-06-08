/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestModelHelper.cs
****/
using System;
using System.Diagnostics;

namespace Microsoft.PropertyModel.Test
{
    internal partial class TestModelHelper
    {
        private static Lazy<bool> _initializer = new Lazy<bool>(() =>
        {
            RegisterTypes();
            return true;
        });

        internal static void EnsureRegisterTypes()
        {
            if (!_initializer.Value)
            {
                Debug.Assert(false, "Failed to RegisterTypes");
            }
        }

    }
}
