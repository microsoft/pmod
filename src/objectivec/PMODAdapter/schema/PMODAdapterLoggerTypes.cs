/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterLoggerTypes.cs
****/
using System;

namespace nsadapter
{

    public enum NSAdapter_Category
    {
    }

    public enum NSFoundation_Category
    {
        AddObserver,
        RemoveObserver,
    }

    public enum AddObserver_LogRecord
    {
        UniqueId,
        KeyPath,
        Options
    }

    public enum RemoveObserver_LogRecord
    {
        UniqueId,
        KeyPath,
    }

    public enum NSObjectBase_Category
    {
        FinalRelease,
    }

    public enum FinalRelease_LogRecord
    {
        UniqueId,
    }

    public enum NSPropertyModel_Category
    {
        FireNotifyPropertyChanged,
    }

    public enum NSCollectionModel_Category
    {
        FireNotifyCollectionChanged,
    }

    public enum NSFactory_Category
    {
        AddCache,
        RemoveCache,
        RegisterAdapterClass,
    }

    public enum AddCache_LogRecord
    {
        UniqueId,
        TypeIId,
        Name,
    }

    public enum RemoveCache_LogRecord
    {
        UniqueId,
    }

    public enum RegisterAdapterClass_LogRecord
    {
        ClassName,
        TypeId,
    }
}

