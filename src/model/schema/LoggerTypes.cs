/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Types.cs
****/
using System;
using PropertyModel;

namespace pmod
{

    /// <summary>
    /// Foundation.Object.ObjectNode
    /// </summary>
    public enum ObjectNode_Category
    {
        SetParent,
        ClearParent,
        BeginModelChanges,
        EndModelChanges,
    }

    public enum SetParent_LogRecord
    {
        ParentUniqueId,
        ParentTypeId,
        ChildUniqueId,
        ChildTypeId,
    }

    public enum Model_LogRecord
    {
        UniqueId,
    }

    /// <summary>
    /// Foundation.Object.ObjectNode.Command
    /// </summary>
    public enum Command_Category
    {
        CanExecute,
        Execute
    }

    [EnumBaseType(BaseType = typeof(Model_LogRecord))]
    public enum CommandCanExecute_LogRecord
    {
        Result
    }

    [EnumBaseType(BaseType = typeof(Model_LogRecord))]
    public enum CommandExecute_LogRecord
    {
        Parameters,
        Result
    }

    /// <summary>
    /// Root.Core.Model.Collection
    /// </summary>
    public enum Collection_Category
    {        
        PushChangeAction,
        GetItem,
        SetItem,
        GetItems
    }

    [EnumBaseType(BaseType = typeof(Model_LogRecord))]
    public enum CollectionPushChangeAction_LogRecord
    {
        Action,
        Index,
        NItems
    }

    [EnumBaseType(BaseType = typeof(Model_LogRecord))]
    public enum CollectionGetItem_LogRecord
    {
        Index,
    }

    [EnumBaseType(BaseType = typeof(Model_LogRecord))]
    public enum CollectionSetItem_LogRecord
    {
        Index,
    }
    [EnumBaseType(BaseType = typeof(Model_LogRecord))]
    public enum CollectionGetItems_LogRecord
    {
    }

    /// <summary>
    /// PropertyChanged Event Args Record
    /// </summary>
    public enum PropertyChangedEventArgs_LogRecord
    {
        PropertyId,
        OldValue,
        NewValue,
    }

    /// <summary>
    /// EventModel Event Args Record
    /// </summary>
    public enum EventModelEventArgs_LogRecord
    {
        EventId,
        EventArgs,
    }

    /// <summary>
    /// NotifyCollectionChanged Event Args Record
    /// </summary>
    public enum NotifyCollectionChangedEventArgs_LogRecord
    {
        Action,
        OldStartingIndex,
        NewStartingIndex,
    }
    /// <summary>
    /// CanExecuteChanged Event Args Record
    /// </summary>
    public enum CanExecuteChangedEventArgs_LogRecord
    {
        OldState,
        NewState,
    }

    /// <summary>
    /// Json Proxy Server Category
    /// </summary>
    public enum JsonProxyServer_Category
    {
        Execute,
        SendPluginResult,
    }

    /// <summary>
    /// Json Proxy Server Execute Args Record
    /// </summary>
    public enum JsonProxyServerExecute_LogRecord
    {
        CallbackId,
        Action,
        JsonArgs,
    }

    /// <summary>
    /// Json Proxy Server Send results Record
    /// </summary>
    public enum JsonProxyServerSendResult_LogRecord
    {
        CallbackId,
        KeepCallback,
        HResult,
        JsonEventArgs,
    }
}

