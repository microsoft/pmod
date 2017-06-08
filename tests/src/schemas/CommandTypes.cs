/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandTypes.cs
****/
using System;
using PropertyModel;

namespace TestModel
{

    public interface ICommandTypes
    {
        [CommandAttribute(Parameter = typeof(void), Result = typeof(string))]
        ICommand GetString { get; }

        [CommandAttribute(Parameter = typeof(void), Result = typeof(string), IsAsync = true)]
        ICommand GetStringAsync { get; }
    }

    
}
