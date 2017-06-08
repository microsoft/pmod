/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:StructTypes.cs
****/
using System;
using PropertyModel;

namespace TestModel
{
    public struct ITypeStruct
    {
        public string Property1;
        public uint Property2;
    }

    [ModelAttribute(IsObservableObject = false)]
    public interface ITypeStructWithMethods
    {
        string Property1 { get; }
        uint Property2 { get; }

        void VoidEmpty();
        int ReturnInt(int arg);
    }
}
