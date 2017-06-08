/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyModel.winrt.cs
****/
using System;
using System.Runtime.InteropServices;

namespace CoreInterop
{

    /// <summary>
    /// PropertyModel Class
    /// </summary>
    public partial class PropertyModel
    {
        private _SinkHelper _sinkHelper;

        class _SinkHelper
        {
            private WeakReference _target;

            internal _SinkHelper(object target)
            {
                _target = new WeakReference(target);
            }
            internal void OnPropertyChanged(object sender, Microsoft.PropertyModel.PropertyChangedEventArgs e)
            {
                if (_target.IsAlive)
                {
                    ((PropertyModel)_target.Target).OnPropertyChanged(e);
                }
            }

            internal void OnEventModel(object sender, Microsoft.PropertyModel.EventModelEventArgs e)
            {
                if (_target.IsAlive)
                {
                    ((PropertyModel)_target.Target).OnEventModel(e);
                }
            }
        }

        internal override void AttachEvents()
        {
            if(_sinkHelper == null)
            {
                _sinkHelper = new _SinkHelper(this);
            }
            this._adapterInterface.PropertyChanged += _sinkHelper.OnPropertyChanged;
            this._adapterInterface.EventModel += _sinkHelper.OnEventModel;
        }

        internal override void DetachEvents()
        {
            if (_sinkHelper != null)
            {
                this._adapterInterface.PropertyChanged -= _sinkHelper.OnPropertyChanged;
                this._adapterInterface.EventModel -= _sinkHelper.OnEventModel;
            }
        }
    }
}
