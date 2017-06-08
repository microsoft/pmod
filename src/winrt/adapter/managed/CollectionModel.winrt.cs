/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CollectionModel.winrt.cs
****/
using Microsoft.PropertyModel;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Runtime.InteropServices;

namespace CoreInterop
{
    /// <summary>
    /// CollectionModel Class
    /// </summary>
    public partial class CollectionModel<T>
    {
        private _SinkHelper _sinkHelper;
        public CollectionModel(ICollectionModel modelAdapter, ClassFactory classFactory) :
            base(modelAdapter, classFactory)
        {
        }
        class _SinkHelper
        {
            private WeakReference _target;

            internal _SinkHelper(object target)
            {
                _target = new WeakReference(target);
            }
            internal void OnNotifyCollectionChanged(object sender, Microsoft.PropertyModel.NotifyCollectionChangedEventArgs e)
            {
                if (_target.IsAlive)
                {
                    ((CollectionModel<T>)_target.Target).OnNotifyCollectionChanged(e);
                }
            }
        }
        public void Clear()
        {
            this._adapterInterface.Clear();
        }

        public bool IsReadOnly
        {
            get
            {
                return this._adapterInterface.IsReadOnly;
            }
        }

        public bool Remove(T item)
        {
            return this._adapterInterface.Remove(item);
        }

        public void Add(T item)
        {
            this._adapterInterface.Add(item);
        }

        internal System.Collections.IEnumerator GetAdapterEnumerator()
        {
            return this._adapterInterface.GetEnumerator();
        }

        internal override void AttachEvents()
        {
            if (_sinkHelper == null)
            {
                _sinkHelper = new _SinkHelper(this);
            }
            this._adapterInterface.NotifyCollectionChanged += _sinkHelper.OnNotifyCollectionChanged;
        }

        internal override void DetachEvents()
        {
            if (_sinkHelper != null)
            {
                this._adapterInterface.NotifyCollectionChanged -= _sinkHelper.OnNotifyCollectionChanged;
            }
        }


    }

}
