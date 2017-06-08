/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestModelTypeInfoStub.cs
****/
using System;

namespace Microsoft.PropertyModel.Test
{
    internal class TypeInfo
    {
        internal static Guid TypePropertiesType
        {
            get { return TestModel.TypeProperties.IIdType; }
        }
    }
}
