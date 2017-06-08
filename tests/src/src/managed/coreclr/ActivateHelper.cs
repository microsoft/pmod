/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ActivateHelper.cs
****/
using System;

namespace Microsoft.PropertyModel.Test
{
    internal class ActivateHelper
    {
        internal static T Activate<T>(Guid iid,object pContext)
            where T : class
        {
            ClassFactory classFactory = new ClassFactory(TestModel.ClassFactoryTypes.Types);
            return classFactory.ActivateObject<T>(iid, null);
        }
    }
}
