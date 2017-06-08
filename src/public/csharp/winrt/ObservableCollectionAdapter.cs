/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionAdapter.cs
****/
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Microsoft.PropertyModel
{
    class ObservableCollectionAdapter<T> :
       ObservableCollection<T>
    {
        private ICollectionModel _source;
        private int _handlerCount;
        private Func<object, T> _adapterCallback;

        internal ObservableCollectionAdapter(
            ICollectionModel source,
            Func<object, T> adapterCallback)
        {
            this._source = source;
            this._adapterCallback = adapterCallback;
        }

        public override event System.Collections.Specialized.NotifyCollectionChangedEventHandler CollectionChanged
        {
            add
            {
                base.CollectionChanged += value;
                ++_handlerCount;
                if (_handlerCount == 1)
                {
                    EnsureAttach();
                }
            }
            remove
            {
                base.CollectionChanged -= value;
                --_handlerCount;
                if (_handlerCount == 0)
                {
                    EnsureDetach();
                }
            }
        }
        private void SyncItems(System.Collections.IEnumerable items)
        {
            Items.Clear();
            foreach (var item in items)
            {
                Items.Add(_adapterCallback(item));
            }
            OnCollectionChanged(new
                System.Collections.Specialized.NotifyCollectionChangedEventArgs(
                System.Collections.Specialized.NotifyCollectionChangedAction.Reset));
        }

        private void EnsureAttach()
        {
            _source.NotifyCollectionChanged += OnSourceNotifyCollectionChanged;
            SyncItems(_source.ToArray());
        }

        private void EnsureDetach()
        {
            _source.NotifyCollectionChanged += OnSourceNotifyCollectionChanged;
        }

        private void OnSourceNotifyCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (e.Action == NotifyCollectionModelChangedAction.Reset)
            {
                SyncItems(e.NewItems);
            }
            else if (e.Action == NotifyCollectionModelChangedAction.ItemAdded)
            {
                int index = e.NewStartingIndex;
                foreach (var item in e.NewItems)
                {
                    var itemAdapter = _adapterCallback(item);
                    this.InsertItem(index++,itemAdapter);
                }
            }
            else if (e.Action == NotifyCollectionModelChangedAction.ItemRemoved)
            {
                int index = e.OldStartingIndex;
                foreach (var item in e.OldItems)
                {
                    this.RemoveItem(index);
                }
            }
            else if (e.Action == NotifyCollectionModelChangedAction.ItemReplaced)
            {
                int index = e.NewStartingIndex;
                foreach (var item in e.NewItems)
                {
                    var itemAdapter = _adapterCallback(item);
                    this.SetItem(index++, itemAdapter);
                }
            }
        }
    }

}
