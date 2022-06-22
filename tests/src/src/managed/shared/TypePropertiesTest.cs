/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypePropertiesTest.cs
****/
using System;
using Xunit;
using TestModel;

namespace Microsoft.PropertyModel.Test
{
    public partial class TypePropertiesTest
    {
        [Fact]
        public void Test()
        {
            TestModelHelper.EnsureRegisterTypes();

            var typeProperties = ActivateHelper.Activate<
#if WINDOWS_UWP
                ITypeProperties
#else
                TypeProperties
#endif
                >(TypeInfo.TypePropertiesType, null);
            // Enum Property
            typeProperties.TypeFlags = TypeFlags.Value2;
            Assert.Equal(typeProperties.TypeFlags, TypeFlags.Value2);

            // String Property
            typeProperties.StringProperty = "rodrigov";
            Assert.Equal(typeProperties.StringProperty, "rodrigov");

            // Array Objects Property
            typeProperties.ObjectArrayProperty = new object[] { "rodrigov", (uint)300 };
            var objsArrays = typeProperties.ObjectArrayProperty;
            Assert.Equal(objsArrays.Length, 2);
            Assert.Equal(objsArrays[0], "rodrigov");
            Assert.Equal(objsArrays[1], (uint)300);

            PropertyChangedEventHandler eventHandler;
            PropertyChangedEventArgs lastPropertyChangedEventArgs = null;
            eventHandler = (object sender, PropertyChangedEventArgs e) =>
            {
                lastPropertyChangedEventArgs = e;
            };
            typeProperties.AttachPropertyChanged(eventHandler);

            typeProperties.StringProperty = "rocio";
            Assert.NotNull(lastPropertyChangedEventArgs);
            Assert.Equal(lastPropertyChangedEventArgs.PropertyName, "StringProperty");

            typeProperties.DetachPropertyChanged(eventHandler);

        }
    }
}
