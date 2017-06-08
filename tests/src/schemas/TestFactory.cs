/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestFactory.cs
****/
using System;
using PropertyModel;

namespace TestModel
{

    public interface ITestFactory
    {
        ITypeProperties CreateTypeProperties();
        IMethodSupport CreateMethodSupport(object workingDispatcher);
        ITypePropertiesTest CreateTypePropertiesTest(object workingDispatcher);
    }
}
