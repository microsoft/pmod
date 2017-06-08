/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FactoryObject.cs
****/
using System;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Linq;

namespace Microsoft.PropertyModel
{
    static class DebugHelpers
    {
        static internal T AssertCast<T>(this object value)
        {
            System.Diagnostics.Debug.Assert(null == value || value is T, "null == value || value is T");

            return (T)value;
        }
    }
    internal interface IFactoryObject
    {
        InspectableAdapter Adapter { get; }
        object ToFactoryObject(InspectableAdapter adapter);
        InspectableAdapter ToAdapter(object o);
    }

    public class FactoryObject<TAdapter> : IFactoryObject
        where TAdapter : InspectableAdapter
    {
        protected TAdapter Adapter;

        internal FactoryObject(TAdapter adapter) :
            this(adapter, null)
        {
        }

        internal FactoryObject(TAdapter adapter, ClassFactory classFactory)
        {
            if (adapter == null)
            {
                throw new ArgumentNullException("adapter");
            }
            this.Adapter = adapter;
            this.ClassFactory = classFactory;
        }

        ~FactoryObject()
        {
            if(this.ClassFactory != null)
            {
                this.ClassFactory.OnFactoryObjectDisposed();
            }
        }

        public ClassFactory ClassFactory { get; private set; }

        public object ToFactoryObject(InspectableAdapter adapter)
        {
            return ClassFactory.ToFactoryObject(this.ClassFactory, adapter);
        }

        internal T[] ToFactoryObjectItems<T>(System.Collections.Generic.IEnumerable<InspectableAdapter> items)
        {
            T[] adapterItems = new T[items.Count()];
            int index = 0;
            foreach (var item in items)
            {
                adapterItems[index] = ToFactoryObject(item).AssertCast<T>();
                ++index;
            }
            return adapterItems;
        }

        public InspectableAdapter ToAdapter(object o)
        {
            return ClassFactory.ToAdapter(o);
        }

        internal void RaiseEvent(Action raiseEventFunc)
        {
            InvokeWithDispatcher(raiseEventFunc, true);
        }

        internal void InvokeWithDispatcher(Action action,bool checkAccess)
        {
            if (this.ClassFactory != null)
            {
                this.ClassFactory.InvokeWithDispatcher(action, checkAccess);
            }
            else
            {
                action();
            }
        }

        InspectableAdapter IFactoryObject.Adapter
        {
            get
            {
                return this.Adapter;
            }
        }
        internal T ConvertTo<T>(object value)
        {
            if (value == null)
            {
                return default(T);
            }
            Type type = typeof(T);
            TypeInfo typeInfo = type.GetTypeInfo();

            // if no conversion is needed
            if (typeInfo.IsSubclassOf(type) || value.GetType() == type)
            {
                return (T)value;
            }

            if(type == typeof(Dictionary))
            {
                ObservableObject observableObject = value as ObservableObject;
                if(observableObject != null)
                {
                    return new Dictionary(
                     (DictionaryAdapter)ObjectStaticsUtil.CreateInspectable(
                        observableObject.Adapter.Detach(),
                        DictionaryAdapter.Type),
                        this.ClassFactory).AssertCast<T>();
                }
            }

            // support for arrays
                if (type.IsArray &&
                !(type.GetElementType().GetTypeInfo().IsPrimitive || type.GetElementType() == typeof(string)))
            {
                Array rawItems = (Array)value;
                int size = rawItems.GetLength(0);
                Array adapterItems = Array.CreateInstance(type.GetElementType(), size);

                for (int index = 0; index < size; ++index)
                {
                    adapterItems.SetValue(ToFactoryObject(rawItems.GetValue(index).AssertCast<InspectableAdapter>()), index);
                }
                return adapterItems.AssertCast<T>();
            }
            return value.AssertCast<T>();
        }
    }
}
