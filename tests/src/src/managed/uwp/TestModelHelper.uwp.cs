/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestModelHelper.uwp.cs
****/
using System;
using System.Diagnostics;

namespace Microsoft.PropertyModel.Test
{
    internal partial class TestModelHelper
    {
        private static void RegisterTypes()
        {
            TestModel.Class.Register();
        }
    }
}
