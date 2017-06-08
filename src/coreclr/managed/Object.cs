/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Object.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{

    public interface IObject
    {
        UInt32 UniqueId { get; }
        UInt32 ProcessId { get; }

        Guid RuntimeType { get; }

        IObjectTypeInfo TypeInfo { get; }

        ClassFactory ClassFactory { get; }
    }

    public interface IObjectTypeInfo
    {
        UInt32 TypeId { get; }
        Guid Type { get; }

        string Name { get; }
    }

    public class Object<TAdapter> : FactoryObject<TAdapter>,
        IObject
        where  TAdapter : ObjectAdapter
    {
        internal Object(TAdapter adapter) :
            this(adapter, null)
        {
        }

        internal Object(TAdapter adapter, ClassFactory classFactory):
            base(adapter,classFactory)
        {
        }

        public UInt32 UniqueId
        {
            get
            {
                return this.Adapter.UniqueId;
            }
        }

        public UInt32 ProcessId
        {
            get
            {
                return this.Adapter.ProcessId;
            }
        }

        public Guid RuntimeType
        {
            get
            {
                return this.Adapter.RuntimeType;
            }
        }

        public IObjectTypeInfo TypeInfo
        {
            get
            {
                return this.Adapter.TypeInfo;
            }
        }

    }
}
