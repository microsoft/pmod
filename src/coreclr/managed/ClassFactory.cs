/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ClassFactory.cs
****/
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public partial class ClassFactory
    {
        private IDictionary<ulong, WeakReference<IObject>> CachedObjects = new Dictionary<ulong, WeakReference<IObject>>();
        private IDictionary<Guid, Type> _objectTypes = new Dictionary<Guid, Type>();

        private static IDictionary<Guid, Type> FactoryObjectTypes = new Dictionary<Guid, Type>();

        static ClassFactory()
        {
            FactoryObjectTypes.Add(
                new Guid("81C05E25-DB59-4A54-A7DD-405E9AC97C7B"),
                typeof(Command<,,>).MakeGenericType(typeof(CommandState), typeof(VoidModelType), typeof(VoidModelType)));

            FactoryObjectTypes.Add(
                new Guid("11B0E777-37AA-4DCA-B0D5-68C9A86E9839"),
                typeof(Command<,,>).MakeGenericType(typeof(CommandState), typeof(VoidModelType), typeof(System.Threading.Tasks.Task<>).MakeGenericType(typeof(VoidModelType))));
        }

        public ClassFactory()
        {
        }

        public ClassFactory(IEnumerable<KeyValuePair<Guid, Type>> objectTypes):
            this(objectTypes, null)
        {
        }

        public ClassFactory(IEnumerable<KeyValuePair<Guid, Type>> objectTypes, Action<Action, bool> eventDispatcher)
        {
            this.AddObjectTypes(objectTypes);
            this.Dispatcher = eventDispatcher;
        }

        public Action<Action,bool> Dispatcher { get; set; }

        public void AddObjectTypes(IEnumerable<KeyValuePair<Guid, Type>> iidTypes)
        {
            if (iidTypes == null)
            {
                throw new ArgumentNullException("iidTypes");
            }
            // Copy the Enumerable kvp into our Dictionary
            foreach (KeyValuePair<Guid, Type> kvp in iidTypes)
            {
                _objectTypes.Add(kvp.Key, kvp.Value);
            }
        }

        public IDictionary<Guid, Type> ObjectTypes
        {
            get
            {
                return _objectTypes;
            }
        }

        public T ActivateObject<T>(Guid iidType, object context)
        {
            ObjectAdapter objectAdapter = ObjectStaticsUtil.ActivateObject(iidType, ToAdapter(context));
            return GetOrCreateObject(objectAdapter).AssertCast<T>();
        }

        public T GetActivationFactory<T>(string activationId)
        {
            IntPtr pFactory = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.ActivationFactory_GetFactory(
                activationId,
                ref pFactory));
            return GetOrCreateObject(new ObservableObjectAdapter(pFactory)).AssertCast<T>();
        }

        internal void InvokeWithDispatcher(Action action,bool checkAccess)
        {
            if(this.Dispatcher != null)
            {
                this.Dispatcher(action, checkAccess);
            }
            else
            {
                action();
            }
        }

        public IObject GetOrCreateObject(ObjectAdapter objectAdapter)
        {
            WeakReference<IObject> weakReference;
            lock (this)
            {
                ulong uniqueId = GetUniqueId(objectAdapter);

                IObject target;
                if (CachedObjects.TryGetValue(uniqueId, out weakReference)
                    && weakReference.TryGetTarget(out target))
                {
                    return (IObject)target;
                }
                // cache does not exists or it was garbage collected
                IObject objectFactory;
                Guid iid = objectAdapter.RuntimeType;
                Type type;
                if (_objectTypes.TryGetValue(iid, out type) || FactoryObjectTypes.TryGetValue(iid, out type))
                {
                    objectFactory = ActivateObjectInstance(this,objectAdapter,type);
                }
                else
                {
                    // fallback to generic factory types
                    objectFactory = CreateFactoryObject(this,objectAdapter);
                }
                AddToCache(uniqueId, objectFactory);
                return objectFactory;
            }
        }
        static internal IObject CreateAsyncOperation(ClassFactory classFactory, AsyncOperationAdapter asyncOperationAdapter)
        {
            Type resultType = GetType(classFactory, asyncOperationAdapter.GetResultTypeInfo());
            Type asyncOperationType = typeof(AsyncOperation<>).MakeGenericType(resultType);
            return ActivateObjectInstance(classFactory,asyncOperationAdapter, asyncOperationType);
        }

        static internal IObject CreateCollectionModel(ClassFactory classFactory, ObservableCollectionAdapter collectionModelAdapter)
        {
            Type itemType = GetType(classFactory, collectionModelAdapter.GetItemTypeInfo());
            Type collectionType = typeof(CollectionModel<>).MakeGenericType(itemType);
            return ActivateObjectInstance(classFactory, collectionModelAdapter, collectionType);
        }
        static internal IObject CreateCommandModel(ClassFactory classFactory, CommandAdapter commandModelAdapter)
        {
            Type parameterType = GetType(classFactory, commandModelAdapter.GetParameterTypeInfo());
            Type resultType = GetType(classFactory, commandModelAdapter.GetResultTypeInfo());
            Type commandType = typeof(Command<,,>).MakeGenericType(typeof(CommandState), parameterType, resultType);
            return ActivateObjectInstance(classFactory, commandModelAdapter, commandType);
        }

        static internal IObject CreateFactoryObject(ClassFactory classFactory, ObjectAdapter objectAdapter)
        {
            CommandAdapter commandModelAdapter = objectAdapter as CommandAdapter;
            if (commandModelAdapter != null)
            {
                return CreateCommandModel(classFactory,commandModelAdapter);
            }
            AsyncOperationAdapter asyncOperationAdapter = objectAdapter as AsyncOperationAdapter;
            if (asyncOperationAdapter != null)
            {
                return CreateAsyncOperation(classFactory, asyncOperationAdapter);
            }

            ObservableCollectionAdapter collectionModelAdapter = objectAdapter as ObservableCollectionAdapter;
            if (collectionModelAdapter != null)
            {
                return CreateCollectionModel(classFactory, collectionModelAdapter);
            }

            ObservableObjectAdapter propertyModelAdapter = objectAdapter as ObservableObjectAdapter;
            if (propertyModelAdapter != null)
            {
                return new ObservableObject(propertyModelAdapter, classFactory);
            }

            ObjectDispatchAdapter propertyDataAdapter = objectAdapter as ObjectDispatchAdapter;
            if (propertyDataAdapter != null)
            {
                return new ObjectDispatch<ObjectDispatchAdapter>(propertyDataAdapter, classFactory);
            }

            return new Object<ObjectAdapter>(objectAdapter);
        }

        static internal object ToEnumValue(ClassFactory classFactory, EnumValueAdapter adapter)
        {
            uint value = adapter.EnumValue;
            if (classFactory != null)
            {
                Guid iid = adapter.RuntimeType;
                Type type;
                if (classFactory._objectTypes.TryGetValue(iid, out type))
                {
                    return Enum.ToObject(type, value);
                }
            }
            return value;
        }

        static internal InspectableAdapter ToAdapter(object o)
        {
            if (o == null)
            {
                return null;
            }
            InspectableAdapter inspectableAdapter = o as InspectableAdapter;
            if(inspectableAdapter != null)
            {
                return inspectableAdapter;
            }

            IFactoryObject factoryObject = o as IFactoryObject;

            // If we find an 'IFactoryObject' we will need to return the underlying Interface
            if (factoryObject != null)
            {
                return factoryObject.Adapter;
            }
            if(o.GetType() == typeof(object[]))
            {
                object[] objs = (object[])o;
                IntPtr[] objectAdapters = new IntPtr[objs.Length];
                for(int index = 0;index < objs.Length;++index)
                {
                    inspectableAdapter = ToAdapter(objs[index]);
                    objectAdapters[index] = inspectableAdapter != null ? inspectableAdapter.Interface : IntPtr.Zero;
                }
                return new PropertyValueAdapter(objectAdapters);
            }
            // we assume can be constructed by a PropertyValue
            return PropertyValueAdapter.CreateValue(o);
        }

        static internal object ToFactoryObject(ClassFactory classFactory, InspectableAdapter adapter)
        {
            if (adapter == null)
                return null;
            InspectableAdapter inspectableAdapter;
            EnumValueAdapter enumValueAdapter = adapter as EnumValueAdapter;
            if (enumValueAdapter != null)
            {
                return ToEnumValue(classFactory, enumValueAdapter);
            }
            PropertyValueAdapter pvAdapter = adapter as PropertyValueAdapter;
            if(pvAdapter != null)
            {
                if (pvAdapter.PropertyType == PropertyType.InspectableArray)
                {
                    IntPtr[] arrayObjects = pvAdapter.GetInspectableArray();
                    object[] factoryObjects = new object[arrayObjects.Length];
                    for (int index = 0; index < arrayObjects.Length; ++index)
                    {
                        inspectableAdapter = ObjectStaticsUtil.CreateInspectable(arrayObjects[index]);
                        factoryObjects[index] = ToFactoryObject(classFactory, inspectableAdapter);
                    }
                    return factoryObjects;
                }
                return pvAdapter.ToObject();
            }

            ObjectAdapter objectAdapter = adapter as ObjectAdapter;
            if (objectAdapter != null)
            {
                return ToObject(classFactory, objectAdapter);
            }
            DictionaryAdapter dictionaryAdapter = adapter as DictionaryAdapter;
            if (dictionaryAdapter != null)
            {
                return new Dictionary(dictionaryAdapter, classFactory);
            }
            inspectableAdapter = adapter as InspectableAdapter;
            if (inspectableAdapter != null)
            {
                return new FactoryObject<InspectableAdapter>(inspectableAdapter, classFactory);
            }
            throw new NotImplementedException();
        }

        static internal object ToObject(ClassFactory classFactory, ObjectAdapter objectAdapter)
        {
            if (classFactory != null)
            {
                return classFactory.GetOrCreateObject(objectAdapter);
            }
            return ClassFactory.ToFactoryObject(null, objectAdapter);
        }

        private static ulong GetUniqueId(ObjectAdapter objectAdapter)
        {
            ulong processId = objectAdapter.ProcessId;
            return (processId << 32) + objectAdapter.UniqueId;
        }

        private static IObject ActivateObjectInstance(ClassFactory classFactory, InspectableAdapter adapterInterface, Type type)
        {
            object o = Activator.CreateInstance(type, adapterInterface, classFactory);
            Debug.Assert(o is IObject);
            return (IObject)o;
        }

        private bool PendingCacheDispose { get; set; }
        internal void OnFactoryObjectDisposed()
        {
            if (!this.PendingCacheDispose)
            {
                this.PendingCacheDispose = true;
                this.InvokeWithDispatcher(() =>
                {
                    this.CleanupObjectCache();
                    this.PendingCacheDispose = false;
                    // this could trigger native release of many instances
                    System.GC.Collect();
                },true);
            }
        }

        private void AddToCache(ulong uniqueId, IObject instance)
        {
            this.CachedObjects[uniqueId] = new WeakReference<IObject>(instance);
            
            if(LoggerAdapter.IsAvailable() &&
                LoggerAdapter.FoundationInstance.GetCategoryLevel(null) >= LoggerLevel.Debug)
            {
                string typeName = instance.GetType().ToString();
                IntPtr[] pParameters = new IntPtr[2];
                pParameters[0] = new PropertyValueAdapter(uniqueId).Detach();
                pParameters[1] = new PropertyValueAdapter(typeName).Detach();
                LoggerAdapter.LogMessage(LoggerLevel.Debug, 1, "AddToCache", new PropertyValueAdapter(pParameters));
            }
        }

        private void CleanupObjectCache()
        {
            lock (this)
            {
                List<ulong> diposedKeys = new List<ulong>();

                // Find any Disposed Values
                foreach (KeyValuePair<ulong, WeakReference<IObject>> kvp in this.CachedObjects)
                {
                    IObject target;
                    if (!kvp.Value.TryGetTarget(out target))
                    {
                        diposedKeys.Add(kvp.Key);
                    }
                }
                // Now safely remove from the Dictionary
                foreach (ulong key in diposedKeys)
                {
                    this.CachedObjects.Remove(key);
                    if (LoggerAdapter.IsAvailable() &&
                        LoggerAdapter.FoundationInstance.Interface != IntPtr.Zero &&
                        LoggerAdapter.FoundationInstance.GetCategoryLevel(null) >= LoggerLevel.Debug)
                    {
                        LoggerAdapter.LogMessage(LoggerLevel.Debug, 2, "RemoveFromCache", new PropertyValueAdapter(key));
                    }
                }
            }
        }

        private static Type GetType(ClassFactory classFactory,PropertyTypeInfoAdapter propertyTypeInfoAdapter)
        {
            ObjectTypeInfoAdapter objectTypeInfoAdapter = propertyTypeInfoAdapter.TypeInfo;
            Type objectType;
            if (objectTypeInfoAdapter != null &&
                classFactory != null &&
                classFactory.ObjectTypes.TryGetValue(objectTypeInfoAdapter.Type,out objectType))
            {
                return objectType;
            }
            return GetSystemType(propertyTypeInfoAdapter.Type);
        }

        private static Type GetSystemType(PropertyType propertyType)
        {
            switch (propertyType)
            {
                case PropertyType.Int16:
                    return typeof(short);
                case PropertyType.UInt16:
                    return typeof(ushort);
                case PropertyType.String:
                    return typeof(string);
                case PropertyType.Int32:
                    return typeof(int);
                case PropertyType.UInt32:
                    return typeof(uint);
                case PropertyType.Int64:
                    return typeof(long);
                case PropertyType.UInt64:
                    return typeof(ulong);
                case PropertyType.Boolean:
                    return typeof(bool);
                case PropertyType.DateTime:
                    return typeof(DateTime);
                case PropertyType.UInt8:
                    return typeof(byte);
                case PropertyType.Inspectable:
                    return typeof(object);
                case PropertyType.Empty:
                    return typeof(VoidModelType);
            }
            throw new System.NotImplementedException();

        }
    }
}
