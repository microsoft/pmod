/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DemoApp.cs
****/
using System;
using PropertyModel;

namespace DemoApp
{
    public interface IItem
    {
        string Name { get; set; }

        [CommandAttribute(Parameter = typeof(string), Result = typeof(void))]
        ICommand ChangeName { get; }
    }

    public interface IAppDemo
    {
        [CollectionAttribute(ItemType = typeof(IItem))]
        ICollection Items { get; }

        // Commands to add/remove stuff from Items Collection
        [CommandAttribute(Parameter = typeof(string), Result = typeof(void), IsUnknownState = true)]
        ICommand AddItem { get; }

        // Add an item without the need to pass any parameter
        [CommandAttribute(Parameter = typeof(void), Result = typeof(void))]
        ICommand AddNextItem { get; }

        [CommandAttribute(Parameter = typeof(IItem), Result = typeof(void), IsUnknownState = true)]
        ICommand RemoveItem { get; }

        [CommandAttribute(Parameter = typeof(void), Result = typeof(void))]
        ICommand ClearItems { get; }

        [MethodAttribute(IsAsync = true)]
        IItem CreateItemAsync(string name);

    }

    public interface IAppDemoFactory
    {
        IAppDemo CreateAppDemo(object workingDisptacher);
    }
}
