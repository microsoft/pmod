/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryTest.cs
****/
using System;
using System.Linq;
using Xunit;

namespace Microsoft.PropertyModel.Test
{
    public class DictionaryTest
    {
        [Fact]
        public void Test()
        {
            var dictionary = new Dictionary();
            Assert.False(dictionary.ContainsKey("key1"));
            dictionary["key1"] = 100;
            Assert.True(dictionary.ContainsKey("key1"));
            var keys = dictionary.Keys;
            Assert.Equal(keys.Count, 1);
            Assert.Equal(keys.First(), "key1");
            var values = dictionary.Values;
            Assert.Equal(values.Count, 1);
            Assert.Equal(values.First(), 100);

        }
    }
}
