/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValueTest.cs
****/
using System;
using Xunit;

namespace Microsoft.PropertyModel.Net.Test
{
    public class PropertyValueTest
    {
        [Fact]
        public void TestCreateValues()
        {
            PropertyValueAdapter value;

            value = new PropertyValueAdapter("rodrigov");
            Assert.True(value.IsTypeOf(PropertyValueAdapter.Type));
            Assert.Equal(value.PropertyType, PropertyType.String);
            Assert.Equal(value.GetString(), "rodrigov");

            DateTime dt = DateTime.Now;
            value = new PropertyValueAdapter(dt);
            Assert.Equal(value.PropertyType, PropertyType.DateTime);
            DateTime dtValue = value.GetDateTime();
            Assert.Equal(dtValue, dt);

            value = new PropertyValueAdapter((byte)100);
            Assert.Equal(value.PropertyType,PropertyType.UInt8);
            Assert.Equal(value.GetByte(), 100);

            value = new PropertyValueAdapter(new byte[] { 100, 200 });
            Assert.Equal(value.PropertyType, PropertyType.UInt8Array);
            byte[] byteValues = value.GetBytes();
            Assert.Equal(byteValues.Length, 2);
            Assert.Equal(byteValues[0], 100);
            Assert.Equal(byteValues[1], 200);
        }
    }
}
