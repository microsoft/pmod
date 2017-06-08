/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Perf.cs
****/
using System;

namespace propertymodelcore
{
    public enum Core_PerfId
    {
        Object_GetContent = 0x300,

        PropertyModel_GetProperty,
        PropertyModel_SetProperty,
        PropertyModel_SetPropertyInternal,
        PropertyModel_Invoke,
        PropertyModel_InvokeCallback,
        PropertyModel_GetProperties,
        PropertyModel_FirePropertyChanged,
        PropertyModel_FireBatchPropertyChanged,
        PropertyModel_FireEventModel,

        CollectionModel_GetCount,
        CollectionModel_GetItem,
        CollectionModel_FireNotifyCollectionChanged,

        CommandModel_Execute,
        CommandModel_ExecuteCallback,
        CommandModel_CanExecute,
        CommandModel_FireCanExecuteChanged,

        BindingBase_GetValue,
        BindingBase_FireValueChanged
    }
    public enum ProxyFactory_PerfId
    {
        Object_GetContent = 0x350,

        PropertyModel_GetProperty,
        PropertyModel_SetProperty,
        PropertyModel_Invoke,
        PropertyModel_GetProperties,

        CollectionModel_GetCount,
        CollectionModel_GetItem,

        CommandModel_Execute,
        CommandModel_CanExecute
    }

    public enum CoreProxyFactory_PerfId
    {
        Object_GetContent = 0x3A0,

        PropertyModel_GetProperty,
        PropertyModel_SetProperty,
        PropertyModel_Invoke,
        PropertyModel_GetProperties,

        CollectionModel_GetCount,
        CollectionModel_GetItem,

        CommandModel_Execute,
        CommandModel_CanExecute
    }
}

