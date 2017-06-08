/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumTypes.cs
****/
using System;
using PropertyModel;

namespace TestModel
{

    public enum TypeEnum
    {
        Type1 = 1,
        Type2 = 2,
        Type3 = 3,
    }

    [EnumBaseType(BaseType = typeof(TypeEnum))]
    public enum TypeEnum2
    {
        Type4
    }

    [Flags]
    public enum TypeFlags
    {
        None = 0,
        Value1 = 1,
        Value2 = 2,
        Value3 = 4,
    }

    public enum TypeNonFlags
    {
        Value1 = 1,
        Value2 = 2,
        Value3 = 3,
    }
    
}
