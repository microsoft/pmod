/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollection.cs
****/
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace Microsoft.PropertyModel
{
    /// <summary>
    /// CollectionModel Class
    /// </summary>
    public sealed class CollectionModel<T> : BaseNotifyPropertyChanged<ObservableCollectionAdapter>,
        ICollection<T>,
        INotifyCollectionChanged,
        System.Collections.ICollection,
        System.Collections.IList
    {
        private System.Collections.Specialized.NotifyCollectionChangedEventHandler CollectionChangedEventHandler;
        private bool IsCollectionChangedCallback { get; set; }

        private Lazy<ObservableObjectAdapter> LazyPropertyModelAdapter;

        public CollectionModel(ObservableCollectionAdapter adapter, ClassFactory classFactory) :
            base(adapter, classFactory)
        {
            LazyPropertyModelAdapter = new Lazy<ObservableObjectAdapter>(() =>
            {
                return new ObservableObjectAdapter(this.Adapter.CastTo(ObservableObjectAdapter.Type));
            });
        }

        /// <summary>
        /// EventModel Event
        /// </summary>
        public event System.Collections.Specialized.NotifyCollectionChangedEventHandler CollectionChanged
        {
            add
            {
                this.CollectionChangedEventHandler += value;

                // Note: after reviewing the .NET code of ItemsControl found out
                // that during the attachment the Control will not pay attention
                // of event notification during the attachment process and we need
                // the collection stays quiet while the attachement happens. To acomplish
                // this we are going to dispatch the PropertyModel attach to make sure it 
                // after the items control is ready to process events.
                this.InvokeWithDispatcher(() =>
                {
                    OnNotifyCollectionChangedChanged(true);
                },false);

            }
            remove
            {
                this.CollectionChangedEventHandler -= value;
                OnNotifyCollectionChangedChanged(false);
            }
        }

        private void OnNotifyCollectionChangedChanged(bool add)
        {
            int countHandlers = this.CollectionChangedEventHandler != null ?
                this.CollectionChangedEventHandler.GetInvocationList().Length :
                0;
            if (add && countHandlers == 1 && !this.IsCollectionChangedCallback)
            {
                this.Adapter.AddNotifyCollectionChanged(OnNotifyCollectionChanged);
                this.IsCollectionChangedCallback = true;
            }
            else if (!add && countHandlers == 0 && this.IsCollectionChangedCallback)
            {
                this.Adapter.RemoveNotifyCollectionChanged(OnNotifyCollectionChanged);
                this.IsCollectionChangedCallback = false;
            }
        }

        #region IEnumerator Interface

        internal class EnumeratorInterop:
            System.Collections.IEnumerator
        {
            private CollectionModel<T> CollectionModel;
            private System.Collections.IEnumerator AdapterEnumerator;

            internal EnumeratorInterop(
                CollectionModel<T> interopCollectionModel,
                System.Collections.IEnumerator adapterEnumerator)
            {
                this.CollectionModel = interopCollectionModel;
                this.AdapterEnumerator = adapterEnumerator;
            }

            public object Current
            {
                get 
                {
                    return this.CollectionModel.ToFactoryObject(this.AdapterEnumerator.Current.AssertCast<InspectableAdapter>());
                }
            }

            public bool MoveNext()
            {
                return this.AdapterEnumerator.MoveNext();
            }

            public void Reset()
            {
                throw new NotImplementedException();
            }
        }


        internal class EnumeratorAdapter<Item> :
            EnumeratorInterop,
            IEnumerator<Item>
        {
            internal EnumeratorAdapter(
                CollectionModel<T> CollectionModel,
                System.Collections.IEnumerator adapterEnumerator):
                base(CollectionModel, adapterEnumerator)
            {
            }

            new public Item Current
            { 
                get
                {
                    return base.Current.AssertCast<Item>();
                }
            }


            public void Dispose()
            {
            }
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return new EnumeratorInterop(this,this.GetAdapterEnumerator());
        }

        #endregion

        #region ICollection Interface

        bool System.Collections.ICollection.IsSynchronized
        {
            get { return false; }
        }
        object System.Collections.ICollection.SyncRoot
        {
            get { return null; }
        }
        void System.Collections.ICollection.CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }


        public int Count
        {
            get
            {
                return (int)this.Adapter.Count;
            }
        }

        public bool Contains(T item)
        {
            return this.Adapter.IndexOf(this.ToAdapter(item)) != -1;
        }

        public void CopyTo(T[] array, int index)
        {
            int sourceSize = (int)this.Adapter.Count;

            if (array.Length - index < sourceSize)
            {
                throw new ArgumentOutOfRangeException("array", "Array is length is not enough to hold the collection size.");
            }

            //from index to index + sourceSize-1, copy and convert elements into the array
            for (uint i = 0; i < sourceSize; i++)
            {
                array[index + i] = (T)ToFactoryObject(this.Adapter.GetItem(i));
            }
        }

        #endregion

        #region IList Interface
        bool System.Collections.IList.IsFixedSize 
        { 
            get 
            {
                return false;
            }
        }
        //
        bool System.Collections.IList.IsReadOnly
        { 
            get {
                return true;
            }
        }

        public object this[int index]
        { 
            get {
                return this.ToFactoryObject(this.Adapter.GetItem((uint)index));
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        int System.Collections.IList.Add(object value)
        {
                throw new NotImplementedException();
        }
        void System.Collections.IList.Clear()
        {
                throw new NotImplementedException();
        }
        bool System.Collections.IList.Contains(object value)
        {
            return this.Adapter.IndexOf(ToAdapter(value)) != -1;
        }
        int System.Collections.IList.IndexOf(object value)
        {
            return (int)this.Adapter.IndexOf(ToAdapter(value));
        }

        void System.Collections.IList.Insert(int index, object value)
        {
            throw new NotImplementedException();
        }

        void System.Collections.IList.Remove(object value)
        {
            throw new NotImplementedException();
        }
        void System.Collections.IList.RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

#endregion
        public IEnumerator<T> GetEnumerator()
        {
            return new EnumeratorAdapter<T>(this, this.GetAdapterEnumerator());
        }

        public bool IsEmpty
        {
            get
            {
                return this.Adapter.Count == 0;
            }
        }

        public void Clear()
        {
        }

        public bool IsReadOnly
        {
            get
            {
                return false;
            }
        }

        public bool Remove(T item)
        {
            throw new NotImplementedException();
        }

        public void Add(T item)
        {
            throw new NotImplementedException();
        }

        internal System.Collections.IEnumerator GetAdapterEnumerator()
        {
            InspectableAdapter[] items = this.Adapter.GetItems();
            return items.GetEnumerator();
        }

        protected override ObservableObjectAdapter PropertyModelAdapter
        {
            get
            {
                return LazyPropertyModelAdapter.Value;
            }
        }

        void OnNotifyCollectionChanged(NotifyCollectionChangedEventArgsAdapter eventArgsAdapter)
        {
            NotifyCollectionModelChangedAction action = eventArgsAdapter.Action;

            List<object> oldItems = null, newItems = null;

            if (action != NotifyCollectionModelChangedAction.Reset)
            {
                oldItems = new List<object>(ToFactoryObjectItems<object>(eventArgsAdapter.OldItems));
                newItems = new List<object>(ToFactoryObjectItems<object>(eventArgsAdapter.NewItems));
            }

            System.Collections.Specialized.NotifyCollectionChangedEventArgs notifyCollectionChangedEventArgs;
            if (action == NotifyCollectionModelChangedAction.ItemAdded)
            {
                notifyCollectionChangedEventArgs = new System.Collections.Specialized.NotifyCollectionChangedEventArgs(
                    NotifyCollectionChangedAction.Add,
                    newItems,
                    (int)eventArgsAdapter.NewStartingIndex);
            }
            else if (action == NotifyCollectionModelChangedAction.ItemRemoved)
            {
                notifyCollectionChangedEventArgs = new System.Collections.Specialized.NotifyCollectionChangedEventArgs(
                    NotifyCollectionChangedAction.Remove,
                    oldItems,
                    (int)eventArgsAdapter.OldStartingIndex);
            }
            else if (action == NotifyCollectionModelChangedAction.ItemReplaced)
            {
                notifyCollectionChangedEventArgs = new System.Collections.Specialized.NotifyCollectionChangedEventArgs(
                    NotifyCollectionChangedAction.Replace,
                    newItems,
                    oldItems,
                    (int)eventArgsAdapter.NewStartingIndex);
            }
            else
            {
                notifyCollectionChangedEventArgs = new System.Collections.Specialized.NotifyCollectionChangedEventArgs(
                    NotifyCollectionChangedAction.Reset);
            }

            RaiseNotifyCollectionChanged(notifyCollectionChangedEventArgs);
        }

        void RaiseNotifyCollectionChanged(System.Collections.Specialized.NotifyCollectionChangedEventArgs eventArgs)
        {
            RaiseEvent(() =>
            {
                // NotifyCollectionChangedEventHandler
                System.Collections.Specialized.NotifyCollectionChangedEventHandler collectionChanged = this.CollectionChangedEventHandler;
                if (collectionChanged != null)
                {
                    /*
                    System.Diagnostics.Debug.WriteLine(
                        string.Format("OldStartingIndex:{0} NewStartingIndex:{1}",
                        eventArgs.OldStartingIndex,
                        eventArgs.NewStartingIndex));
                     * */

                    // it looks that .NET controls does not like 'batch' add/removes so we will have to
                    // split the notification in single items
                    if ((eventArgs.NewItems != null && eventArgs.NewItems.Count > 1) ||
                        (eventArgs.OldItems != null && eventArgs.OldItems.Count > 1))
                    {
                        System.Collections.IList items = eventArgs.Action == NotifyCollectionChangedAction.Remove ?
                            eventArgs.OldItems :
                            eventArgs.NewItems;
                        int startingIndex = eventArgs.Action == NotifyCollectionChangedAction.Remove ?
                            eventArgs.OldStartingIndex :
                            eventArgs.NewStartingIndex;

                        int nextIndex = 0;
                        foreach(object item in items)
                        {
                            List<object> singleItemList = new List<object>(new object[] { item });

                            System.Collections.Specialized.NotifyCollectionChangedEventArgs singleItemEventArg;
                            if (eventArgs.Action == NotifyCollectionChangedAction.Replace)
                            {
                                singleItemEventArg = new System.Collections.Specialized.NotifyCollectionChangedEventArgs(
                                    NotifyCollectionChangedAction.Replace,
                                    singleItemList,
                                    new List<object>(new object[] { eventArgs.OldItems[nextIndex] }),
                                    startingIndex + nextIndex);
                            }
                            else
                            {
                                singleItemEventArg = new System.Collections.Specialized.NotifyCollectionChangedEventArgs(
                                    eventArgs.Action,
                                    singleItemList,
                                    startingIndex + nextIndex);
                            }
                            // safe notify with just one item
                            collectionChanged(this, singleItemEventArg);
                            ++ nextIndex;
                        }
                    }
                    else
                    {
                        collectionChanged(this, eventArgs);
                    }
                }
            });
        }

    }

}
