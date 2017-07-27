/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationLoggerTypes.cs
****/
using System;
using PropertyModel;

namespace foundation
{

    // TODO: we should prevent generating enum values for this type with an attribute
    public enum LoggingLevel
    {
        Off,
        Fatal,
        Error,
        Warning,
        Info,
        Debug,
        Trace
    }

    public enum Root_Category
    {}

    /// <summary>
    /// Root.Error
    /// </summary>
    public enum Error_Category
    {
        Failure,
        NullPtr,
        Expected
    }

    /// <summary>
    /// A Log Error Record
    /// </summary>
    public enum Error_LogRecord
    {
        HResult,
        File,
        Line,
        Source,
        Parameter
    }

    /// <summary>
    /// Root.Foundation
    /// </summary>
    public enum Foundation_Category
    {
        Message
    }

    /// <summary>
    /// Message Record
    /// </summary>
    public enum Message_LogRecord
    {
        Id,
        Text,
        Parameter
    }

    /// <summary>
    /// Root.Foundation.Object
    /// </summary>
    public enum Object_Category
    {
        Initialize,
        FinalRelease,
        FireEvent,
        IsObserved
    }
    public enum ObjectDispatch_Category
    {
        Invoke,
        GetProperty,
        SetProperty
    }


    /// <summary>
    /// Root.Foundation.AsyncOperation
    /// </summary>
    public enum AsyncOperation_Category
    {
        SetCompleted = 10,
    }

    /// <summary>
    /// Generic Object Record
    /// </summary>
    public enum Object_LogRecord
    {
        UniqueId,
        TypeId,
    }

    /// <summary>
    /// Object Initialize Record
    /// </summary>
    [EnumBaseType(BaseType = typeof(Object_LogRecord))]
    public enum ObjectInitialize_LogRecord
    {
        Options,
    }

    /// <summary>
    /// Object FinalRelease Record
    /// </summary>
    [EnumBaseType(BaseType = typeof(Object_LogRecord))]
    public enum ObjectFinalRelease_LogRecord
    {
    }

    /// <summary>
    /// Object FireEvent Record
    /// </summary>
    [EnumBaseType(BaseType = typeof(Object_LogRecord))]
    public enum ObjectFireEvent_LogRecord
    {
        EventArgs
    }

    /// <summary>
    /// Object FireEvent Record
    /// </summary>
    [EnumBaseType(BaseType = typeof(Object_LogRecord))]
    public enum ObjectIsObserved_LogRecord
    {
        Observed
    }

    /// <summary>
    /// Object Trace Record
    /// </summary>
    public enum ObjectTrace_LogRecord
    {
        ObjectId,
        ParameterName,
        ParameterValue,
    }


    [EnumBaseType(BaseType = typeof(Object_LogRecord))]
    public enum ObjectDispatchInvoke_LogRecord
    {
        Parameters,
        MethodId,
        MethodInfo,
        ParamSize,
        Result
    }

    [EnumBaseType(BaseType = typeof(Object_LogRecord))]
    public enum ObjectDispatchGetProperty_LogRecord
    {
        PropertyId,
    }

    [EnumBaseType(BaseType = typeof(Object_LogRecord))]
    public enum ObjectDispatchSetProperty_LogRecord
    {
        PropertyId,
    }

    /// <summary>
    /// AsyncOperation SetCompleted Record
    /// </summary>
    public enum AsyncOperationSetCompleted_LogRecord
    {
        UniqueId,
        HResult,
        Result,
    }

    /// <summary>
    /// Base Event Args Record
    /// </summary>
    public enum BaseEventArgs_LogRecord
    {
        TypeIID,
    }

    /// <summary>
    /// Root.Foundation.ObjectFactory
    /// </summary>
    public enum ObjectFactory_Category
    {
        RegisterObject,
        CreateObject,
    }

    /// <summary>
    /// ObjectFactory Record
    /// </summary>
    public enum ObjectFactory_LogRecord
    {
        TypeIID,
        Context
    }

}

