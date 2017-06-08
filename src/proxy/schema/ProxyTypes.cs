/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ProxyTypes.cs
****/
using System;
using PropertyModel;

namespace pmod.proxy
{
    public enum Proxy_Category
    { }

    /// <summary>
    /// Proxy.Factory
    /// </summary>
    public enum Factory_Category
    {
        AddToCache,
        RemoveFromCache,
        CreateObjectProxyInstance,
        ViewModelCreated,
        SetRootSource,
    }

    public enum Factory_BaseProxyCache_LogRecord
    {
        UniqueId,
    }
    public enum Factory_CreateObjectProxyInstance_LogRecord
    {
        ProxyType,
        SourceUniqueId
    }
    public enum Factory_Model_LogRecord
    {
        UniqueId,
        TypeId,
    }

    [EnumBaseType(BaseType = typeof(Factory_Model_LogRecord))]
    public enum Factory_ViewModelCreated_LogRecord
    {
    }

    [EnumBaseType(BaseType = typeof(Factory_Model_LogRecord))]
    public enum Factory_SetRootSource_LogRecord
    {
    }

    /// <summary>
    /// Proxy.BaseProxy
    /// </summary>
    public enum BaseProxy_Category
    {
        Initialize,
        FinalRelease,
        SourceEvent,
        AttachToSource,
        DetachFromSource,
        ResyncSource,
        ResyncEvent,
    }

    public enum BaseProxy_Base_LogRecord
    {
        ProxyUniqueId,
        UniqueId,
        TypeId,
    }

    /// <summary>
    /// BaseProxy SourceEvent Record
    /// </summary>
    [EnumBaseType(BaseType = typeof(BaseProxy_Base_LogRecord))]
    public enum BaseProxy_SourceEvent_LogRecord
    {
        EventArgs
    }

    /// <summary>
    /// BaseProxy SourceEvent Record
    /// </summary>
    [EnumBaseType(BaseType = typeof(BaseProxy_Base_LogRecord))]
    public enum BaseProxy_ResyncSource_LogRecord
    {
    }

    /// <summary>
    /// Proxy.BaseProxy.Command
    /// </summary>
    public enum Command_Category
    {
    }

    /// <summary>
    /// Proxy.BaseProxy.Collection
    /// </summary>
    public enum Collection_Category
    {
    }

    /// <summary>
    /// Proxy.BaseProxy.Collection
    /// </summary>
    public enum AsyncOperation_Category
    {
    }

    /// <summary>
    /// Proxy.BaseProxy.PropertyModel
    /// </summary>
    public enum Property_Category
    {
    }

    public enum ProxyType
    {
        None = 0,
        ObservableObject = 1,
        ObservableCollection = 2,
        Command = 3,
        AsyncOperation = 4,
        ViewModel = 5,
    };

    public interface IProxyObject
    {
        uint ProxyUniqueId { get; }
        IProxyObjectFactory Factory { get; }
    }

    public interface IProxyObjectFactory
    {
        IObject ToProxyObject(IObject pSource);
        IProxyObject LookupObjectProxy(IObject pSourceObject);
        IProxyObject[] GetProxies();

        object CreateProxyObject(
            ProxyType proxyType,
            IObject pSource,
            IProxyObjectDelegate pProxyObjectDelegate,
            object pOuter);

        IObject CreateViewModel(Guid iidViewModel, object contextParameter);
        void PurgeProxyObjects(uint maxPurgeSize, ulong time);
        object DisconnectProxySources();
        void ConnectProxySources(object pDisconnectInfo);
    }

    [Flags]
    public enum ProxyObjectFactoryOptions
    {
        None = 0x00,
        PurgeIfAttached = 0x01,
        UseModelTypeInfoProxy = 0x02,
        IsModelCacheEnable = 0x04,
        NoPurgeState = 0x08,
        AutoPurge = 0x20,
        IsCacheValuesEnabled = 0x40,
        IsSourceFreeThreaded = 0x80,
        FromProxyValueDisabled = 0x100,
        ToProxyValueDisabled = 0x200,
        UseCacheStrongRefForProperties = 0x400,
        UseCacheStrongRefForItems = 0x800,
    };

    public interface IProxyObjectFactoryDelegate
    {
        void OnPurgeInvoked(uint maxPurgeSize, ulong timestamp);
        object ToProxyValue(IProxyObject pProxyObject, object value);
        object FromProxyValue(IProxyObject pProxyObject, object value);
        IProxyObject OnCreateProxyObject(ProxyType proxyType, IObject pSource, Guid iidType);
        void OnProxyObjectPurgeReady(IProxyObject pSource);
    }

    public interface IProxyObjectDelegate
    {
        void OnAddEventHandler();
        void OnRemoveEventHandler();
        void OnSourceAttached();
        void OnSourceDetached();
    }

    public interface IObservableObjectProxyDelegate : IProxyObjectDelegate
    {
        void OnSourcePropertyChanged(string propertyName);
    }

    public interface IProxyObjectFactoryFactory
    {
        IProxyObjectFactory Create(
            ProxyObjectFactoryOptions options,
            object workingDispatcher,
            object eventDispatcher);
    }

}

