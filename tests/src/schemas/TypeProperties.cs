/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeProperties.cs
****/
using System;
using PropertyModel;

namespace TestModel
{

    /// <summary>
    /// Supported properties
    /// </summary>
    public interface ITypeProperties
    {
        TypeFlags TypeFlags { set; get; }
        TypeNonFlags TypeNonFlags { set; get; }

        bool BooleanProperty { get; set; }

        short ShortProperty { get; set; }
        ushort UShortProperty { get; set; }

        int IntProperty { get; set; }
        uint UIntProperty { get; set; }

        long LongProperty { get; set; }
        ulong ULongProperty { get; set; }

        string StringProperty { get; set; }

        byte ByteProperty { get; set; }

        double DoubleProperty { get; set; }

        float FloatProperty { get; set; }

        Guid GuidProperty { get; set; }

        System.Object ObjectProperty { get; set; }

        System.DateTime DateTimeProperty { get; set; }

        string[] StringArrayProperty { get; set; }
        object[] ObjectArrayProperty { get; set; }

        IDictionary DictionaryProperty { get; set; }
    }

    [EnumerationAttribute(ItemType = typeof(ITypeProperties))]
    public interface IEnumerable_ITypeProperties
    {
    }

    [CollectionAttribute(ItemType = typeof(ITypeProperties))]
    public interface ICollection_ITypeProperties
    {
    }

    public interface ITypePropertiesTest
    {
        ITypeProperties TypeProperties { get; }

        [MethodAttribute(IsAsync = true)]
        void ChangePropertyAsync(uint propertyId, object propertyValue);
    }

}
